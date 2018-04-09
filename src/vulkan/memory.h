#ifndef VULKAN_MEMORY_H
#define VULKAN_MEMORY_H

#include <vulkan/vulkan.h>
#include <stdint.h>
#include "./config.h"
#include "../collections/basic_dynamic_list.h"
#include "../collections/common.h"

typedef unsigned char byte;

typedef enum vk_memory_usage_type {
	VULKAN_MEMORY_USAGE_UNKNOWN,
	VULKAN_MEMORY_USAGE_GPU_ONLY,
	VULKAN_MEMORY_USAGE_CPU_ONLY,
	VULKAN_MEMORY_USAGE_CPU_TO_GPU,
	VULKAN_MEMORY_USAGE_GPU_TO_CPU,
	VULKAN_MEMORY_USAGES_SIZE
} vk_memory_usage_type;

typedef enum vk_allocation_type {
	VULKAN_ALLOCATION_TYPE_FREE,
	VULKAN_ALLOCATION_TYPE_BUFFER,
	VULKAN_ALLOCATION_TYPE_IMAGE,
	VULKAN_ALLOCATION_TYPE_IMAGE_LINEAR,
	VULKAN_ALLOCATION_TYPE_IMAGE_OPTIMAL,
	VULKAN_ALLOCATION_TYPES_SIZE
} vk_allocation_type;

typedef struct vk_chunk {
	uint32_t id;
	VkDeviceSize size;
	VkDeviceSize offset;
	struct vk_chunk *prev;
	struct vk_chunk *next;
	vk_allocation_type type;
} vk_chunk;

typedef struct vk_block {
	vk_chunk *head;
	uint32_t next_block_id;
	uint32_t memory_type_index;
	vk_memory_usage_type usage;
	VkDeviceMemory device_memory;
	VkDeviceSize size; 
	VkDeviceSize allocated;
	byte *data;
} vk_block;

typedef struct vk_allocation {
	vk_block *block;
	uint32_t pool_id;
	uint32_t block_id;
	VkDeviceMemory device_memory;
	VkDeviceSize offset;
	VkDeviceSize size;
	byte *data;
} vk_allocation;

GENERATE_BASIC_DYNAMIC_LIST_HEADER(vk_block_list, vk_block_list, vk_block*)

typedef struct vk_allocator {
	int garbage_index;
	int device_local_memory_bytes;
	int host_visible_memory_bytes;
	VkDeviceSize buffer_image_granularity;
	vk_block_list blocks[VK_MAX_MEMORY_TYPES];
	vk_allocation garbage[NUM_FRAME_DATA];
} vk_allocator;

void init_vk_allocation(vk_allocation *a);

#endif // VULKAN_MEMORY_H
