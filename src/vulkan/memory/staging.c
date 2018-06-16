#include "./staging.h"

#include "../functions/functions.h"
#include "../context.h"
#include "../tools/tools.h"
#include "./config.h"
#include "../../logger/logger.h"

vk_staging_manager staging_manager;

void init_vk_staging_buffer(vk_staging_buffer *buffer) {
    buffer->submitted = false;
    buffer->command_buffer = VK_NULL_HANDLE;
    buffer->buffer = VK_NULL_HANDLE;
    buffer->fence = VK_NULL_HANDLE;
    buffer->offset = 0;
    buffer->data = NULL;
}

void init_vk_staging_manager(vk_staging_manager *manager) {
    manager->max_buffer_size = 0;
    manager->current_buffer = 0;
    manager->mapped_data = NULL;
    manager->memory = VK_NULL_HANDLE;
    manager->command_pool = VK_NULL_HANDLE;
    for (size_t i = 0; i < NUM_FRAME_DATA; i++) {
        init_vk_staging_buffer(&manager->buffers[i]);
    }
}

bool init_buffers_vk_staging_manager(vk_staging_manager *manager) {
    manager->max_buffer_size = vk_mem_config.upload_buffer_size_MB * 1024 * 1024;

    VkBufferCreateInfo buffer_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .size = manager->max_buffer_size,
        .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = NULL
    };

    for (size_t i = 0; i < NUM_FRAME_DATA; i++) {
        manager->buffers[i].offset = 0;
        CHECK_VK(vkCreateBuffer(context.device, &buffer_info, NULL, &manager->buffers[i].buffer));
    }

    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(context.device, manager->buffers[0].buffer, &mem_requirements);

    VkDeviceSize align_mod = mem_requirements.size % mem_requirements.alignment;
    VkDeviceSize aligned_size = align_mod == 0 ?
        mem_requirements.size : (mem_requirements.size + mem_requirements.alignment - align_mod);

    uint32_t memory_type_index = find_memory_type_index(mem_requirements.memoryTypeBits, VULKAN_MEMORY_USAGE_CPU_TO_GPU);

    VkMemoryAllocateInfo mem_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        .allocationSize = aligned_size * NUM_FRAME_DATA,
        .memoryTypeIndex = memory_type_index
    };

    CHECK_VK(vkAllocateMemory(context.device, &mem_info, NULL, &manager->memory));

    for (size_t i = 0; i < NUM_FRAME_DATA; i++) {
        VkMemoryRequirements current_mem_requirements;
        vkGetBufferMemoryRequirements(context.device, manager->buffers[i].buffer, &current_mem_requirements);
        if (current_mem_requirements.size != mem_requirements.size ||
            current_mem_requirements.alignment != mem_requirements.alignment ||
            current_mem_requirements.memoryTypeBits != mem_requirements.memoryTypeBits)
        {
            log_error("Different memory requirements for stage buffers");
            return false;
        }
        CHECK_VK(vkBindBufferMemory(context.device, manager->buffers[i].buffer, manager->memory, i * aligned_size));
    }

    CHECK_VK(vkMapMemory(context.device, manager->memory, 0, aligned_size * NUM_FRAME_DATA, 0,
        (void**) &manager->mapped_data));

    VkCommandPoolCreateInfo pool_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = context.graphics_family_index
    };

    CHECK_VK(vkCreateCommandPool(context.device, &pool_info, NULL, &manager->command_pool));

    VkCommandBufferAllocateInfo command_buffer_alloc_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .commandPool = manager->command_pool,
        .level = 0,
        .commandBufferCount = 1
    };

    VkFenceCreateInfo fence_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0
    };

    VkCommandBufferBeginInfo command_buffer_begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = 0,
        .pInheritanceInfo = NULL
    };

    for (size_t i = 0; i < NUM_FRAME_DATA; i++) {
        CHECK_VK(vkAllocateCommandBuffers(context.device, &command_buffer_alloc_info,
            &manager->buffers[i].command_buffer));
        CHECK_VK(vkCreateFence(context.device, &fence_info, NULL, &manager->buffers[i].fence));
        CHECK_VK(vkBeginCommandBuffer(manager->buffers[i].command_buffer, &command_buffer_begin_info));

        manager->buffers[i].data = (byte*) manager->mapped_data + (i * aligned_size);
    }

    return true;
}

static bool wait_stage(vk_staging_buffer *stage) {
    if (!stage->submitted) {
        return true;
    }

    CHECK_VK(vkWaitForFences(context.device, 1, &stage->fence, VK_TRUE, UINT64_MAX));
    CHECK_VK(vkResetFences(context.device, 1, &stage->fence));

    stage->offset = 0;
    stage->submitted = false;

    VkCommandBufferBeginInfo command_buffer_begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = 0,
        .pInheritanceInfo = NULL
    };

    CHECK_VK(vkBeginCommandBuffer(stage->command_buffer, &command_buffer_begin_info));

    return true;
}

byte* stage_vk_staging_manager(vk_staging_manager *manager, VkDeviceSize size, VkDeviceSize alignment,
    VkCommandBuffer *command_buffer, VkBuffer *buffer, VkDeviceSize *buffer_offset)
{
    if (size > manager->max_buffer_size) {
        log_error("Unable to allocate %d MB in gpu transfer buffer", (int) (size / (1024 * 1024)));
        return NULL;
    }

    vk_staging_buffer *stage = &manager->buffers[manager->current_buffer];

    VkDeviceSize align_mod = stage->offset % alignment;
    stage->offset = stage->offset % alignment == 0 ? stage->offset : stage->offset + alignment - align_mod;

    if (stage->offset + size >= manager->max_buffer_size && !stage->submitted) {
        flush_vk_staging_manager(manager);
    }

    stage = &manager->buffers[manager->current_buffer];
    if (stage->submitted) {
        if (!wait_stage(stage)) {
            log_error("Error while waiting in staging manager");
            return NULL;
        }
    }

    *command_buffer = stage->command_buffer;
    *buffer = stage->buffer;
    *buffer_offset = stage->offset;

    byte *data = stage->data + stage->offset;
    stage->offset += size;

    return data;
}

void flush_vk_staging_manager(vk_staging_manager *manager) {
    vk_staging_buffer *stage = &manager->buffers[manager->current_buffer];
    if (stage->submitted || stage->offset == 0) {
        return;
    }

    VkMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER,
        .pNext = NULL,
        .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
        .dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_INDEX_READ_BIT
    };
    vkCmdPipelineBarrier(stage->command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
        0, 1, &barrier, 0, NULL, 0, NULL);

    vkEndCommandBuffer(stage->command_buffer);

    VkMappedMemoryRange memory_range = {
        .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
        .pNext = NULL,
        .memory = manager->memory,
        .offset = stage->offset,
        .size = VK_WHOLE_SIZE
    };
    vkFlushMappedMemoryRanges(context.device, 1, &memory_range);

    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = NULL,
        .waitSemaphoreCount = 0,
        .pWaitSemaphores = NULL,
        .pWaitDstStageMask = NULL,
        .commandBufferCount = 1,
        .pCommandBuffers = &stage->command_buffer,
        .signalSemaphoreCount = 0,
        .pSignalSemaphores = NULL
    };
    vkQueueSubmit(context.graphics_queue, 1, &submit_info, stage->fence);
    stage->submitted = true;

    manager->current_buffer = (manager->current_buffer + 1) % NUM_FRAME_DATA;
}

void destroy_vk_staging_manager(vk_staging_manager *manager) {
    if (manager->memory) {
        vkUnmapMemory(context.device, manager->memory);
        vkFreeMemory(context.device, manager->memory, NULL);
        manager->memory = VK_NULL_HANDLE;
        manager->mapped_data = NULL;
    }

    for (size_t i = 0; i < NUM_FRAME_DATA; i++) {
        if (manager->buffers[i].fence)
            vkDestroyFence(context.device, manager->buffers[i].fence, NULL);
        if (manager->buffers[i].buffer)
            vkDestroyBuffer(context.device, manager->buffers[i].buffer, NULL);
        if (manager->buffers[i].command_buffer)
            vkFreeCommandBuffers(context.device, manager->command_pool, 1, &manager->buffers[i].command_buffer);
        init_vk_staging_buffer(&manager->buffers[i]);
    }

    if (manager->command_pool) {
        vkDestroyCommandPool(context.device, manager->command_pool, NULL);
        manager->command_pool = VK_NULL_HANDLE;
    }

    manager->max_buffer_size = 0;
    manager->current_buffer = 0;
}

bool vk_init_stage_manager() {
    init_vk_staging_manager(&staging_manager);
    return init_buffers_vk_staging_manager(&staging_manager);
}

void vk_flush_stage() {
    flush_vk_staging_manager(&staging_manager);
}

byte* vk_stage(VkDeviceSize size, VkDeviceSize alignment, VkCommandBuffer *command_buffer, VkBuffer *buffer,
    VkDeviceSize *buffer_offset)
{
    return stage_vk_staging_manager(&staging_manager, size, alignment, command_buffer, buffer, buffer_offset);
}

void vk_destroy_stage_manager() {
    destroy_vk_staging_manager(&staging_manager);
}
