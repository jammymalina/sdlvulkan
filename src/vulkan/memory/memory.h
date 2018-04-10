#ifndef VULKAN_MEMORY_H
#define VULKAN_MEMORY_H

#include <vulkan/vulkan.h>
#include <stdint.h>
#include "../config.h"
#include "../../collections/basic_dynamic_list.h"
#include "../../collections/common.h"

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
	uint32_t id;
	vk_block *block;
	VkDeviceMemory device_memory;
	VkDeviceSize offset;
	VkDeviceSize size;
	byte *data;
} vk_allocation;

GENERATE_BASIC_DYNAMIC_LIST_HEADER(vk_block_list, vk_block_list, vk_block*)
GENERATE_BASIC_DYNAMIC_LIST_HEADER(vk_alloc_list, vk_alloc_list, vk_allocation)

typedef struct vk_mem_allocator {
	int garbage_index;
	int device_local_memory_bytes;
	int host_visible_memory_bytes;
	VkDeviceSize buffer_image_granularity;
	vk_block_list blocks[VK_MAX_MEMORY_TYPES];
	vk_alloc_list garbage[NUM_FRAME_DATA];
} vk_mem_allocator;

static inline bool is_host_visible(vk_memory_usage_type t) {
	return t != VULKAN_MEMORY_USAGE_GPU_ONLY;
}

// ALLOCATION

void init_vk_allocation(vk_allocation *a);

// BLOCK

void init_vk_block(vk_block *block, uint32_t memory_type_index, VkDeviceSize size, vk_memory_usage_type usage);
bool init_vk_block_memory(vk_block *block);
bool allocate_vk_block(vk_block *block, uint32_t size, uint32_t align, VkDeviceSize granularity, 
	vk_allocation_type alloc_type, vk_allocation *allocation);
void free_allocation_vk_block(vk_block *block, vk_allocation *allocation);
void destroy_vk_block(vk_block *block);
void print_vk_block(vk_block *block);

// ALLOCATOR

bool init_vk_allocator(vk_mem_allocator *allocator);
bool vk_allocate(vk_mem_allocator *allocator, vk_allocation *result, 
	uint32_t size, uint32_t align, uint32_t memory_type_bits, 
	vk_memory_usage_type usage, vk_allocation_type alloc_type);
void empty_garbage_vk_allocator(vk_mem_allocator *allocator);
bool free_allocation_vk_allocator(vk_mem_allocator *allocator, vk_allocation *allocation);
void destroy_vk_allocator(vk_mem_allocator *allocator);
void print_vk_allocator(vk_mem_allocator *allocator);

extern vk_mem_allocator vk_allocator;

#endif // VULKAN_MEMORY_H
