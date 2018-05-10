#ifndef BUFFER_OBJECT_H
#define BUFFER_OBJECT_H

#include <vulkan/vulkan.h>
#include <stdbool.h>
#include "../memory/memory.h"

#define MAPPED_FLAG (((VkDeviceSize) 1) << (sizeof(VkDeviceSize) * 8 - 1 ))
#define OWNS_BUFFER_FLAG (((VkDeviceSize) 1) << (sizeof(VkDeviceSize) * 8 - 1 ))

typedef enum buffer_map_type {
    BM_READ,
    BM_WRITE
} buffer_map_type;

typedef enum buffer_usage_type {
    BU_STATIC,
    BU_DYNAMIC
} buffer_usage_type;

typedef enum buffer_type {
    UNDEFINED_BUFFER,
    VERTEX_BUFFER,
    INDEX_BUFFER,
    UNIFORM_BUFFER
} buffer_type;

typedef struct vk_buffer {
    buffer_type type;
    VkDeviceSize size;
    VkDeviceSize offset_in_other_buffer;
    buffer_usage_type usage;
    vk_allocation allocation;
    VkBuffer buffer;
} vk_buffer;

bool copy_buffer_data(byte *dest, const byte *src, VkDeviceSize num_bytes);

void init_vk_buffer(vk_buffer *buffer, buffer_type type);
bool alloc_vk_buffer(vk_buffer *buffer, void *data, VkDeviceSize alloc_size, buffer_usage_type usage);
bool reference_vk_buffer(vk_buffer *dest, const vk_buffer *src);
bool reference_vk_buffer_part(vk_buffer *dest, const vk_buffer *src, VkDeviceSize ref_offset, VkDeviceSize ref_size);
void free_vk_buffer(vk_buffer *buffer);
bool update_data_vk_buffer(vk_buffer *buffer, void *data, VkDeviceSize size, VkDeviceSize offset);
bool map_vk_buffer(vk_buffer *buffer, void **dest, buffer_map_type map_type);
bool unmap_vk_buffer(vk_buffer *buffer);

static inline VkDeviceSize get_allocated_buffer_size(const vk_buffer *buffer) {
    return ((buffer->size & ~MAPPED_FLAG) + 15) & ~15;
}

static inline VkDeviceSize get_buffer_offset(const vk_buffer *buffer) {
    return buffer->offset_in_other_buffer & ~OWNS_BUFFER_FLAG;
}

static inline bool owns_buffer(const vk_buffer *buffer) {
    return (buffer->offset_in_other_buffer & OWNS_BUFFER_FLAG) != 0;
}

static inline bool is_buffer_mapped(const vk_buffer *buffer) {
    return (buffer->size & MAPPED_FLAG) != 0;
}

#endif // BUFFER_OBJECT_H
