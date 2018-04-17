#include "./backend.h"

#include <vulkan/vulkan.h>
#include "../vulkan/functions/functions.h"
#include "../vulkan/tools/tools.h"
#include "../vulkan/context.h"
#include "../vulkan/gpu_info.h"
#include "../vulkan/memory/memory.h"
#include "../vulkan/memory/staging.h"

void init_backend_counters(backend_counters *b) {
    b->gpu_microsec = 0;
}

void init_renderer(renderer *r) {
    r->current_frame = 0;
    r->current_swap_index = 0;

    for (size_t i = 0; i < NUM_FRAME_DATA; i++) {
        r->query_index[i] = 0;
        for (size_t j = 0; j < NUM_TIMESTAMP_QUERIES; j++) {
            r->query_results[i][j] = 0;
        }
    }

    init_backend_counters(&r->pc);
}

static bool start_frame(renderer *r) {
    CHECK_VK(vk_AcquireNextImageKHR(context.device, context.swapchain, UINT64_MAX,
        context.acquire_semaphores[r->current_frame], VK_NULL_HANDLE, &r->current_swap_index));
    vk_empty_garbage();
    vk_flush_stage();

    VkQueryPool query_pool = context.query_pools[r->current_frame];
    uint64_t *results = r->query_results[r->current_frame];
    size_t results_byte_size = sizeof(uint64_t) * NUM_TIMESTAMP_QUERIES;

    if (r->query_index[r->current_frame] > 0) {
        vk_GetQueryPoolResults(context.device, query_pool, 0, 2, results_byte_size,
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
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = 0,
        .pInheritanceInfo = NULL
    };

    CHECK_VK(vk_BeginCommandBuffer(command_buffer, &command_buffer_begin_info));

    vk_CmdResetQueryPool(command_buffer, query_pool, 0, NUM_TIMESTAMP_QUERIES);

    VkRenderPassBeginInfo render_pass_begin_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = NULL,
        .renderPass = context.render_pass,
        .framebuffer = context.framebuffers[r->current_swap_index],
        .renderArea = {
            .offset = {
                .x = 0,
                .y = 0
            },
            .extent = context.extent
        },
        .clearValueCount = 0,
        .pClearValues = NULL,
    };

    vk_CmdBeginRenderPass(command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
    vk_CmdWriteTimestamp(command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, query_pool, r->query_index[r->current_frame]);
    r->query_index[r->current_frame]++;

    return true;
}
