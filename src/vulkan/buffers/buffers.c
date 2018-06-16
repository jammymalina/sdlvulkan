#include "./buffers.h"

#include "../functions/functions.h"
#include "../tools/tools.h"
#include "../context.h"
#include "../memory/staging.h"
#include "../../utils/heap.h"
#include "../../logger/logger.h"

bool copy_buffer_data(buffer_type type, byte *dest, const byte *src, VkDeviceSize num_bytes) {
    mem_copy(dest, src, num_bytes);
    return true;
}

static void set_buffer_mapped(vk_buffer *buffer) {
    buffer->size |= MAPPED_FLAG;
}

static void set_buffer_unmapped(vk_buffer *buffer) {
    buffer->size &= ~MAPPED_FLAG;
}

void init_vk_buffer(vk_buffer *buffer, buffer_type type) {
    buffer->type = type;
    buffer->buffer = VK_NULL_HANDLE;
    buffer->size = 0;
    buffer->offset_in_other_buffer = OWNS_BUFFER_FLAG;
    set_buffer_unmapped(buffer);
    init_vk_allocation(&buffer->allocation);
}

static VkBufferUsageFlags buffer_type_to_vulkan_buffer_usage(buffer_type type) {
    switch (type) {
        case VERTEX_BUFFER:
            return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        case INDEX_BUFFER:
            return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        case UNIFORM_BUFFER:
            return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        case VERTEX_INDEX_BUFFER:
            return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        default:
            return 0;
    }
}

bool alloc_vk_buffer(vk_buffer *buffer, void *data, VkDeviceSize alloc_size, buffer_usage_type usage) {
    if (buffer->buffer) {
        log_error("Buffer already allocated");
        return false;
    }
    if (alloc_size == 0) {
        log_error("Cannot allocate buffer of size 0");
        return false;
    }

    buffer->size = alloc_size;
    buffer->usage = usage;

    VkDeviceSize num_bytes = get_allocated_buffer_size(buffer);

    VkBufferCreateInfo buffer_info = {
        .sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext                 = NULL,
        .flags                 = 0,
        .size                  = num_bytes,
        .usage                 = buffer_type_to_vulkan_buffer_usage(buffer->type),
        .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices   = NULL
    };

    if (buffer->usage == BU_STATIC) {
        buffer_info.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }

    CHECK_VK(vkCreateBuffer(context.device, &buffer_info, NULL, &buffer->buffer));

    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(context.device, buffer->buffer, &mem_requirements);

    vk_memory_usage_type mem_usage = buffer->usage == BU_STATIC ?
        VULKAN_MEMORY_USAGE_GPU_ONLY : VULKAN_MEMORY_USAGE_CPU_TO_GPU;

    bool success = vk_allocate(&buffer->allocation, mem_requirements.size, mem_requirements.alignment,
        mem_requirements.memoryTypeBits, mem_usage, VULKAN_ALLOCATION_TYPE_BUFFER);

    if (!success) {
        return false;
    }

    CHECK_VK(vkBindBufferMemory(context.device, buffer->buffer, buffer->allocation.device_memory,
        buffer->allocation.offset));

    if (data != NULL) {
        update_data_vk_buffer(buffer, data, alloc_size, 0);
    }

    return true;
}

bool reference_vk_buffer(vk_buffer *dest, const vk_buffer *src) {
    if (is_buffer_mapped(dest)) {
        log_error("Buffer is mapped, cannot create reference");
        return false;
    }

    free_vk_buffer(dest);
    dest->size = src->size;
    dest->offset_in_other_buffer = get_buffer_offset(src);
    dest->usage = src->usage;
    dest->buffer = src->buffer;
    dest->allocation = src->allocation;

    if (owns_buffer(dest)) {
        log_error("Buffer should be a reference and not own itself");
        return false;
    }

    return true;
}

bool reference_vk_buffer_part(vk_buffer *dest, const vk_buffer *src, VkDeviceSize ref_offset, VkDeviceSize ref_size) {
    if (is_buffer_mapped(dest)) {
        log_error("Buffer is mapped, cannot create reference");
        return false;
    }
    if (ref_offset + ref_size > src->size) {
        log_error("Unable to reference buffer, the reference part is out of bounds of the source buffer");
        return false;
    }

    free_vk_buffer(dest);
    dest->size = ref_size;
    dest->offset_in_other_buffer = get_buffer_offset(src) + ref_offset;
    dest->usage = src->usage;
    dest->buffer = src->buffer;
    dest->allocation = src->allocation;

    if (owns_buffer(dest)) {
        log_error("Buffer should be a reference and not own itself");
        return false;
    }

    return true;
}

static void clear_buffer(vk_buffer *buffer) {
    buffer->size = 0;
    buffer->offset_in_other_buffer = OWNS_BUFFER_FLAG;
    buffer->buffer = VK_NULL_HANDLE;
    buffer->allocation.device_memory = VK_NULL_HANDLE;
}

void free_vk_buffer(vk_buffer *buffer) {
    if (is_buffer_mapped(buffer)) {
        unmap_vk_buffer(buffer);
    }

    if (!owns_buffer(buffer)) {
        clear_buffer(buffer);
        return;
    }

    if (!buffer->buffer) {
        return;
    }

    vk_free_allocation(&buffer->allocation);
    vkDestroyBuffer(context.device, buffer->buffer, NULL);
    buffer->buffer = VK_NULL_HANDLE;
    init_vk_allocation(&buffer->allocation);

    clear_buffer(buffer);
}

bool update_data_vk_buffer(vk_buffer *buffer, void *data, VkDeviceSize size, VkDeviceSize offset) {
    if (!buffer->buffer) {
        log_error("Buffer is not allocated (vulkan handle is null)");
        return false;
    }
    if ((get_buffer_offset(buffer) & 15) != 0) {
        log_error("Invalid offset");
        return false;
    }

    if (buffer->usage == BU_DYNAMIC) {
        return copy_buffer_data(buffer->type, buffer->allocation.data + get_buffer_offset(buffer) + offset,
            (const byte*) data, size);
    }

    VkBuffer stage_buffer;
    VkCommandBuffer command_buffer;
    VkDeviceSize stage_offset = 0;
    byte *stage_data = vk_stage(size, 1, &command_buffer, &stage_buffer, &stage_offset);

    if (!stage_data) {
        log_error("Could not stage data");
        return false;
    }

    mem_copy(stage_data, data, size);

    VkBufferCopy buffer_copy = {
        .srcOffset = stage_offset,
        .dstOffset = get_buffer_offset(buffer) + offset,
        .size      = size
    };
    vkCmdCopyBuffer(command_buffer, stage_buffer, buffer->buffer, 1, &buffer_copy);

    return true;
}

bool map_vk_buffer(vk_buffer *buffer, void **dest, buffer_map_type map_type) {
    if (!buffer->buffer) {
        log_error("Buffer is not allocated (vulkan handle is null)");
        return false;
    }
    if (buffer->usage == BU_STATIC) {
        log_error("Cannot map a static buffer");
        return false;
    }
    if (!buffer->allocation.data) {
        log_error("Buffer is not allocated (allocation data pointer is null)");
        return false;
    }

    void *data = buffer->allocation.data + get_allocated_buffer_size(buffer);

    set_buffer_mapped(buffer);

    if (data == NULL) {
        log_error("Mapping of data failed");
    }

    *dest = data;

    return true;
}

bool unmap_vk_buffer(vk_buffer *buffer) {
    if (buffer->buffer) {
        log_error("Buffer already allocated");
        return false;
    }
    if (buffer->usage == BU_STATIC) {
        log_error("Cannot unmap a static buffer");
        return false;
    }

    set_buffer_unmapped(buffer);

    return true;
}
