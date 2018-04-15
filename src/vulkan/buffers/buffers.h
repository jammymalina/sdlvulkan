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

#define BUFFER_BASE                      \
    VkDeviceSize size;                   \
    VkDeviceSize offset_in_other_buffer; \
    buffer_usage_type usage;             \
    vk_allocation allocation;            \
    VkBuffer buffer;

typedef struct vertex_buffer {
    BUFFER_BASE
} vertex_buffer;

typedef struct index_buffer {
    BUFFER_BASE
} index_buffer;

typedef struct uniform_buffer {
    BUFFER_BASE
} uniform_buffer;

#define init_buffer(buffer)                       \
        do {                                      \
            (buffer)->buffer = VK_NULL_HANDLE;    \
            (buffer)->size = 0;                   \
            (buffer)->offset_in_other_buffer = 0; \
            set_buffer_unmapped((buffer));        \
        } while (0)

#define get_allocated_buffer_size(buffer) ((((buffer)->size & ~MAPPED_FLAG) + 15) & ~15)          

#define get_buffer_offset(buffer) ((buffer)->offset_in_other_buffer & ~OWNS_BUFFER_FLAG)

#define is_buffer_mapped(buffer) ((buffer)->size & MAPPED_FLAG != 0)                     

#define set_buffer_mapped(buffer)       \
    do {                                \
        (buffer)->size |= MAPPED_FLAG;  \
    } while(0)

#define set_buffer_unmapped(buffer)     \
    do {                                \
        (buffer)->size &= ~MAPPED_FLAG; \
    } while (0)

#define GENERATE_BUFFER_FUNCTIONS_HEADER(buffer_type)                                                            \
    bool alloc_##buffer_type(buffer_type *buffer, void *data, VkDeviceSize alloc_size, buffer_usage_type usage); \
    void free_##buffer_type(buffer_type *buffer);                                                                \
    bool update_data_##buffer_type(buffer_type *buffer, void *data, VkDeviceSize size, VkDeviceSize offset);     \
    bool map_##buffer_type(buffer_type *buffer, buffer_map_type);                                                \
    bool unmap_##buffer_type(buffer_type *buffer);

GENERATE_BUFFER_FUNCTIONS_HEADER(vertex_buffer)
GENERATE_BUFFER_FUNCTIONS_HEADER(index_buffer)
GENERATE_BUFFER_FUNCTIONS_HEADER(uniform_buffer)

#define GENERATE_BUFFER_FUNCTIONS_SOURCE(buffer_type, buffer_usage) \
    bool alloc_##buffer_type(buffer_type *buffer, void *data, VkDeviceSize alloc_size, buffer_usage_type usage) { \
        if ((buffer)->buffer != VK_NULL_HANDLE) {                                                                 \
            log_error("Buffer already allocated");                                                                \
            return false;                                                                                         \
        }                                                                                                         \
        if (!is_16_byte_aligned(data)) {                                                                          \
            log_error("Buffer data is not properly aligned (16 bytes");                                           \
            return false;                                                                                         \
        }                                                                                                         \
        if (alloc_size == 0) {                                                                                    \
            log_error("Cannot allocate buffer of size 0");                                                        \
            return false;                                                                                         \
        }                                                                                                         \
                                                                                                                  \
        (buffer)->size = alloc_size;                                                                              \
        (buffer)->usage = usage;                                                                                  \
                                                                                                                  \
        VkDeviceSize num_bytes =  get_allocated_buffer_size((buffer));                                            \
                                                                                                                  \
        VkBufferCreateInfo buffer_info = {                                                                        \
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,                                                        \
            .pNext = NULL,                                                                                        \
            .flags = 0,                                                                                           \
            .size = num_bytes,                                                                                    \
            .usage = buffer_usage,                                                                                \
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,                                                             \
            .queueFamilyIndexCount = 0,                                                                           \
            .pQueueFamilyIndices = NULL                                                                           \
        };                                                                                                        \
                                                                                                                  \
        if ((buffer)->usage == BU_STATIC) {                                                                       \
            buffer_info.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;                                                \
        }                                                                                                         \
                                                                                                                  \
        CHECK_VK(vk_CreateBuffer(context.device, &buffer_info, NULL, &(buffer)->buffer));                         \
                                                                                                                  \
        VkMemoryRequirements mem_requirements;                                                                    \
        vk_GetBufferMemoryRequirements(context.device, (buffer)->buffer, &mem_requirements);                      \
                                                                                                                  \
        vk_memory_usage_type mem_usage = (buffer)->usage == BU_STATIC ?                                           \
            VULKAN_MEMORY_USAGE_GPU_ONLY : VULKAN_MEMORY_USAGE_CPU_TO_GPU;                                        \
                                                                                                                  \
        bool success = vk_allocate(&(buffer)->allocation, mem_requirements.size, mem_requirements.alignment,      \
            mem_requirements.memoryTypeBits, mem_usage, VULKAN_ALLOCATION_TYPE_BUFFER);                           \
                                                                                                                  \
        if (!success) {                                                                                           \
            return false;                                                                                         \
        }                                                                                                         \
                                                                                                                  \
        CHECK_VK(vk_BindBufferMemory(context.device, (buffer)->buffer, (buffer)->allocation.device_memory,        \
            (buffer)->allocation.offset));                                                                        \
                                                                                                                  \
        if (data != NULL) {                                                                                       \
                                                                                                                  \
        }                                                                                                         \
                                                                                                                  \
        return true;                                                                                              \
    }

#endif // BUFFER_OBJECT_H
