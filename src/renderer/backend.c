#include "./backend.h"

#include <vulkan/vulkan.h>
#include "../vulkan/functions/functions.h"
#include "../vulkan/tools/tools.h"
#include "../vulkan/context.h"
#include "../vulkan/gpu_info.h"
#include "../vulkan/memory/memory.h"
#include "../vulkan/memory/staging.h"
#include "../logger/logger.h"
#include "./shaders/shader_manager.h"
#include "../vertex_management/vertex_manager.h"

#include "./render_state.h"
#include "./config.h"

render_backend renderer;

void init_backend_counters(backend_counters *b) {
    b->gpu_microsec = 0;
}

void init_render_backend(render_backend *r) {
    r->current_frame = 0;
    r->current_swap_index = 0;

    for (size_t i = 0; i < NUM_FRAME_DATA; i++) {
        r->query_index[i] = 0;
        r->command_buffer_recorded[i] = false;
        for (size_t j = 0; j < NUM_TIMESTAMP_QUERIES; j++) {
            r->query_results[i][j] = 0;
        }
    }

    init_backend_counters(&r->pc);
}

static void clear_frame(render_backend *r, uint32_t clear_bits, float rgba[4], uint32_t stencil_value) {
    uint32_t num_attachments = 0;
    VkClearAttachment attachments[2] = {
        { 0 },
        { 0 }
    };

    if (clear_bits & CLEAR_COLOR_BUFFER) {
        VkClearAttachment *attachment = &attachments[num_attachments++];
        attachment->aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        attachment->colorAttachment = 0;
        VkClearColorValue *color = &attachment->clearValue.color;
        color->float32[0] = rgba[0];
        color->float32[1] = rgba[1];
        color->float32[2] = rgba[2];
        color->float32[3] = rgba[3];
    }

    if (clear_bits & (CLEAR_DEPTH_BUFFER | CLEAR_STENCIL_BUFFER)) {
        VkClearAttachment *attachment = &attachments[num_attachments++];
        if (clear_bits & CLEAR_DEPTH_BUFFER) {
            attachment->aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
        }
        if (clear_bits & CLEAR_STENCIL_BUFFER) {
            attachment->aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
        attachment->clearValue.depthStencil.depth = 1.0;
        attachment->clearValue.depthStencil.stencil = stencil_value;
    }

    VkClearRect clear_rect = {
        .rect = {
            .offset = {
                .x = 0,
                .y = 0
            },
            .extent = context.extent
        },
        .baseArrayLayer = 0,
        .layerCount = 1
    };
    vkCmdClearAttachments(context.command_buffers[r->current_frame], num_attachments, attachments, 1, &clear_rect);
}

static bool start_frame(render_backend *r) {
    CHECK_VK(vkAcquireNextImageKHR(context.device, context.swapchain, UINT64_MAX,
        context.acquire_semaphores[r->current_frame], VK_NULL_HANDLE, &r->current_swap_index));
    vk_empty_garbage();
    vk_flush_stage();

    if (!start_frame_ren_pm()) {
        log_error("Unable to start render manager");
        return false;
    }

    VkQueryPool query_pool = context.query_pools[r->current_frame];
    uint64_t *results = r->query_results[r->current_frame];
    size_t results_byte_size = sizeof(uint64_t) * NUM_TIMESTAMP_QUERIES;

    if (r->query_index[r->current_frame] > 0) {
        vkGetQueryPoolResults(context.device, query_pool, 0, 2, results_byte_size,
            results, sizeof(uint64_t), VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT);

        gpu_info *gpu = &context.gpus[context.selected_gpu];

        uint64_t gpu_start = results[0];
        uint64_t gpu_end = results[1];
        uint64_t gpu_tick = (1000 * 1000 * 1000) / gpu->props.limits.timestampPeriod;
        r->pc.gpu_microsec = ((gpu_end - gpu_start) * 1000 * 1000) / gpu_tick;

        r->query_index[r->current_frame] = 0;
    }

    VkCommandBuffer command_buffer = context.command_buffers[r->current_frame];

    VkCommandBufferBeginInfo command_buffer_begin_info = {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext            = NULL,
        .flags            = 0,
        .pInheritanceInfo = NULL
    };

    CHECK_VK(vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info));

    VkViewport viewport = {
        x: 0,
        y: 0,
        width: render_config.width,
        height: render_config.height,
        minDepth: 0.0f,
        maxDepth: 1.0f
    };
    vkCmdSetViewport(command_buffer, 0, 1, &viewport);

    VkRect2D scissor = {
        offset: {
            x: 0,
            y: 0,
        },
        extent: {
            width: viewport.width,
            height: viewport.height
        }
    };
    vkCmdSetScissor(command_buffer, 0, 1, &scissor);

    vkCmdResetQueryPool(command_buffer, query_pool, 0, NUM_TIMESTAMP_QUERIES);

    VkRenderPassBeginInfo render_pass_begin_info = {
        .sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext       = NULL,
        .renderPass  = context.render_pass,
        .framebuffer = context.framebuffers[r->current_swap_index],
        .renderArea  = {
            .offset  = {
                .x = 0,
                .y = 0
            },
            .extent = context.extent
        },
        .clearValueCount = 0,
        .pClearValues    = NULL
    };

    vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdWriteTimestamp(command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, query_pool, r->query_index[r->current_frame]);

    float clear_color[4] = { 0.0, 0.0, 0.0, 0.0 };
    clear_frame(r, CLEAR_COLOR_BUFFER | CLEAR_DEPTH_BUFFER, clear_color, 0);

    r->query_index[r->current_frame]++;

    return true;
}

static bool end_frame(render_backend *r) {
    VkCommandBuffer command_buffer = context.command_buffers[r->current_frame];

    vkCmdWriteTimestamp(command_buffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, context.query_pools[r->current_frame],
        r->query_index[r->current_frame]);

    vkCmdEndRenderPass(command_buffer);

    r->query_index[r->current_frame]++;

    CHECK_VK(vkEndCommandBuffer(command_buffer));
    r->command_buffer_recorded[r->current_frame] = true;

    VkSemaphore *acquire_semaphore = &context.acquire_semaphores[r->current_frame];
    VkSemaphore *render_complete_semaphore = &context.render_complete_semaphores[r->current_frame];

    VkPipelineStageFlags dst_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = acquire_semaphore,
        .pWaitDstStageMask = &dst_stage_mask,
        .commandBufferCount = 1,
        .pCommandBuffers = &command_buffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = render_complete_semaphore
    };

    CHECK_VK(vkQueueSubmit(context.graphics_queue, 1, &submit_info, context.command_buffer_fences[r->current_frame]));
    if (!block_swap_buffers_render_backend(&renderer)) {
        log_error("Unable to swap buffers");
        return false;
    }

    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = render_complete_semaphore,
        .swapchainCount = 1,
        .pSwapchains = &context.swapchain,
        .pImageIndices = &r->current_swap_index,
        .pResults = NULL
    };

    CHECK_VK(vkQueuePresentKHR(context.present_queue, &present_info));

    r->current_frame = (r->current_frame + 1) % NUM_FRAME_DATA;

    return true;
}

static bool draw(render_backend *r) {
    VkCommandBuffer command_buffer = context.command_buffers[r->current_frame];
    bool success = bind_program_instance(RENDER_PROGRAM_INSTANCE_LAMBERT_DIFFUSE) &&
        commit_current_program(RST_BASIC_3D, command_buffer);
    if (!success) {
        return false;
    }

    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(command_buffer, 0, 1, &vertex_cache.static_buffer.buffer, &offset);
    vkCmdBindIndexBuffer(command_buffer, vertex_cache.static_buffer.buffer, 2080, VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(command_buffer, 192, 1, 0, 0, 0);

    return true;
}

bool execute_render_backend(render_backend *r) {
    bool success = start_frame(r);
    if (!success) {
        log_error("Unable to start a frame");
        return false;
    }

    success = draw(r);
    if (!success) {
        log_error("Unable to draw stuff");
        return false;
    }

    success = end_frame(r);
    if (!success) {
        log_error("Unable to finish a frame");
        return false;
    }

    return true;
}

bool block_swap_buffers_render_backend(render_backend *r) {
    if (!r->command_buffer_recorded[r->current_frame]) {
        return true;
    }
    CHECK_VK(vkWaitForFences(context.device, 1, &context.command_buffer_fences[r->current_frame], VK_TRUE, UINT64_MAX));
    CHECK_VK(vkResetFences(context.device, 1, &context.command_buffer_fences[r->current_frame]));
    r->command_buffer_recorded[r->current_frame] = false;

    return true;
}

void init_renderer() {
    init_render_backend(&renderer);
}

bool render() {
    return execute_render_backend(&renderer);
}

