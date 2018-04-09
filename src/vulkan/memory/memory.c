#include "./memory.h"

#include "./config.h"
#include "../functions/functions.h"
#include "../context.h"
#include "../tools/tools.h"
#include "../../utils/heap.h"
#include "../../logger/logger.h"
#include "../gpu_info.h"

GENERATE_BASIC_DYNAMIC_LIST_SOURCE(vk_block_list, vk_block_list, vk_block*)
GENERATE_BASIC_DYNAMIC_LIST_SOURCE(vk_alloc_list, vk_alloc_list, vk_allocation)

static const char* memory_usage_strings[VULKAN_MEMORY_USAGES_SIZE] = {
	"VULKAN_MEMORY_USAGE_UNKNOWN",
	"VULKAN_MEMORY_USAGE_GPU_ONLY",
	"VULKAN_MEMORY_USAGE_CPU_ONLY",
	"VULKAN_MEMORY_USAGE_CPU_TO_GPU",
	"VULKAN_MEMORY_USAGE_GPU_TO_CPU",
};

static const char* allocation_type_strings[VULKAN_ALLOCATION_TYPES_SIZE] = {
	"VULKAN_ALLOCATION_TYPE_FREE",
	"VULKAN_ALLOCATION_TYPE_BUFFER",
	"VULKAN_ALLOCATION_TYPE_IMAGE",
	"VULKAN_ALLOCATION_TYPE_IMAGE_LINEAR",
	"VULKAN_ALLOCATION_TYPE_IMAGE_OPTIMAL",
};

// ALLOCATION

void init_vk_allocation(vk_allocation *a) {
	a->id = 0;
	a->block = NULL;
	a->device_memory = VK_NULL_HANDLE;
	a->offset = 0;
	a->size = 0;
	a->data = NULL;
}

// BLOCK

void init_vk_block(vk_block *block, uint32_t memory_type_index, VkDeviceSize size, vk_memory_usage_type usage) {
	block->next_block_id = 0;
	block->size = size;
	block->allocated = 0;
	block->memory_type_index = memory_type_index;
	block->usage = usage;
	block->device_memory = VK_NULL_HANDLE;
}

bool init_vk_block_memory(vk_block *block) {
	if (block->memory_type_index == UINT32_MAX) {
		return false;
	}

	VkMemoryAllocateInfo allocate_info = {
		.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext           = NULL,
		.allocationSize  = block->size,
		.memoryTypeIndex = block->memory_type_index
	};

	CHECK_VK(vk_AllocateMemory(context.device, &allocate_info, NULL, &block->device_memory));

	if (block->device_memory == VK_NULL_HANDLE) {
		return false;
	}

	if (is_host_visible(block->usage)) {
		CHECK_VK(vk_MapMemory(context.device, block->device_memory, 0, block->size, 0, (void**) &block->data));
	}

	block->head = mem_alloc(sizeof(vk_chunk*));
	CHECK_ALLOC(block->head, "Allocation fail");

	block->head->id = block->next_block_id++;
	block->head->size = block->size;
	block->head->offset = 0;
	block->head->prev = NULL;
	block->head->type = VULKAN_ALLOCATION_TYPE_FREE;

	return true;
}

static bool is_on_same_page(VkDeviceSize rA_offset, VkDeviceSize rA_size,
	VkDeviceSize rB_offset, VkDeviceSize page_size) 
{
	if (!(rA_offset + rA_size <= rB_offset && rA_size > 0 && page_size > 0)) {
		return false;
	}

	VkDeviceSize rA_end = rA_offset + rA_size - 1;
	VkDeviceSize rA_end_page = rA_end & ~(page_size - 1);
	VkDeviceSize rB_start = rB_offset;
	VkDeviceSize rB_start_page = rB_start & ~(page_size - 1);

	return rA_end_page == rB_start_page;
}

static bool has_granularity_conflict(vk_allocation_type a, vk_allocation_type b) {
	if (a > b) {
		vk_allocation_type tmp = a;
		a = b;
		b = tmp;
	}

	switch (a) {
		case VULKAN_ALLOCATION_TYPE_FREE:
			return false;
		case VULKAN_ALLOCATION_TYPE_BUFFER:
			return b == VULKAN_ALLOCATION_TYPE_IMAGE || b == VULKAN_ALLOCATION_TYPE_IMAGE_OPTIMAL;
		case VULKAN_ALLOCATION_TYPE_IMAGE:
			return b == VULKAN_ALLOCATION_TYPE_IMAGE ||
				b == VULKAN_ALLOCATION_TYPE_IMAGE_LINEAR ||
				b == VULKAN_ALLOCATION_TYPE_IMAGE_OPTIMAL;
		case VULKAN_ALLOCATION_TYPE_IMAGE_LINEAR:
			return b == VULKAN_ALLOCATION_TYPE_IMAGE_OPTIMAL;
		case VULKAN_ALLOCATION_TYPE_IMAGE_OPTIMAL:
			return false;
		default:
			return false;
	}
}

bool allocate_vk_block(vk_block *block, uint32_t size, uint32_t align, VkDeviceSize granularity, 
	vk_allocation_type alloc_type, vk_allocation *allocation)
{
	VkDeviceSize free_size = block->size - block->allocated;
	if (free_size < size) {
		return false;
	}

	vk_chunk *current = NULL;
	vk_chunk *best_fit = NULL;
	vk_chunk *prev = NULL;

	VkDeviceSize padding = 0;
	VkDeviceSize offset = 0;
	VkDeviceSize aligned_size = 0;

	for (current = block->head; current != NULL; prev = current, current = current->next) {
		if (current->type != VULKAN_ALLOCATION_TYPE_FREE) {
			continue;
		}

		if (size > current->size) {
			continue;
		}

		offset = ALIGN(current->offset, align);

		if (prev != NULL && granularity > 1) {
			if (
				is_on_same_page(prev->offset, prev->size, offset, granularity) &&
				has_granularity_conflict(prev->type, alloc_type)
			) {
				offset = ALIGN(offset, granularity);
			}
		}

		padding = offset - current->offset;
		aligned_size = padding + size;

		if (aligned_size > current->size) {
			continue;
		}

		if (aligned_size + block->allocated >= block->size) {
			return false;
		}

		if (granularity > 1 && current->next != NULL) {
			vk_chunk *next = current->next;
			if (
				is_on_same_page(offset, size, next->offset, granularity) &&
				has_granularity_conflict(alloc_type, next->type)
			) {
				continue;
			}
		}

		best_fit = current;
		break;
	}

	if (best_fit == NULL) {
		return false;
	}

	if (best_fit->size > size) {
		vk_chunk *chunk = mem_alloc(sizeof(vk_chunk*));
		CHECK_ALLOC(chunk, "Allocation fail");

		vk_chunk *next = best_fit->next;

		chunk->id = block->next_block_id++;
		chunk->prev = best_fit;
		best_fit->next = chunk;

		chunk->next = next;
		if (next) {
			next->prev = chunk;
		}

		chunk->size = best_fit->size - aligned_size;
		chunk->offset = offset + size;
		chunk->type = VULKAN_ALLOCATION_TYPE_FREE;
	}

	best_fit->type = alloc_type;
	best_fit->size = size;

	block->allocated += aligned_size;

	allocation->size = best_fit->size;
	allocation->id = best_fit->id;
	allocation->device_memory = block->device_memory;
	if (is_host_visible(block->usage)) {
		allocation->data = block->data + offset;
	}
	allocation->offset = offset;
	allocation->block = block;

	return true;
}

void free_allocation_vk_block(vk_block *block, vk_allocation *allocation) {
	vk_chunk *current = NULL;

	for (current = block->head; current != NULL; current = current->next) {
		if (current->id == allocation->id) {
			break;
		}
	}

	if (!current) {
		log_warning("Failed to free the allocation from block: %u", allocation->id);
		return;
	}

	current->type = VULKAN_ALLOCATION_TYPE_FREE;

	while (current->prev && current->prev->type == VULKAN_ALLOCATION_TYPE_FREE) {
		vk_chunk *prev = current->prev;
		
		prev->next = current->next;
		if (current->next) {
			current->next->prev = prev;
		}

		prev->size += current->size;

		mem_free(current);
		current = prev;
	}

	while (current->next && current->next->type == VULKAN_ALLOCATION_TYPE_FREE) {
		vk_chunk *next = current->next;

		if (next->next) {
			next->next->prev = current;
		}
		current->next = next->next; 

		current->size += next->size;

		mem_free(next);
	}

	block->allocated -= allocation->size;
}

void destroy_vk_block(vk_block *block) {
	if (is_host_visible(block->usage)) {
		vk_UnmapMemory(context.device, block->device_memory);
	}

	vk_FreeMemory(context.device, block->device_memory, NULL);
	block->device_memory = VK_NULL_HANDLE;

	vk_chunk *prev = NULL;
	vk_chunk *current = block->head;

	while (true) {
		if (current->next == NULL) {
			mem_free(current);
			break;
		} else {
			prev = current;
			current = current->next;
			mem_free(prev);
		}
	}

	block->head = NULL;
}

void print_vk_block(vk_block *block) {
	size_t count = 0; 
	for (vk_chunk *current = block->head; current != NULL; current = current->next) {
		count++;
	}

	log_info("Type Index: %u",  block->memory_type_index);
	log_info("Usage:      %s",  memory_usage_strings[block->usage]);
	log_info("Count:      %zu", count);
	log_info("Size:       %lu", block->size);
	log_info("Allocated:  %lu", block->allocated);
	log_info("Next Block: %u",  block->next_block_id);
	log_info("----------------------------------------");

	for (vk_chunk *current = block->head; current != NULL; current = current->next) {
		log_info("{");
		log_info("\tId:     %u",  current->id);
		log_info("\tSize:   %lu", current->size);
		log_info("\tOffset: %lu", current->offset);
		log_info("\tType:   %s",  allocation_type_strings[current->type]);
		log_info("}");
	}

	log_info("");	
}

// ALLOCATOR

vk_mem_allocator vk_allocator = {
	.garbage_index = 0,
	.device_local_memory_bytes = 0,
	.host_visible_memory_bytes = 0,
	.buffer_image_granularity  = 0
};

bool init_vk_allocator(vk_mem_allocator *allocator) {
	gpu_info *gpu = &context.gpus[context.selected_gpu];	
	
	allocator->device_local_memory_bytes = vk_mem_config.device_local_memory_MB * 1024 * 1024;
	allocator->host_visible_memory_bytes = vk_mem_config.host_visible_memory_MB * 1024 * 1024;
	allocator->buffer_image_granularity = gpu->props.limits.bufferImageGranularity;

	for (size_t i = 0; i < VK_MAX_MEMORY_TYPES; i++) {
		if (!init_vk_block_list(&allocator->blocks[i], vk_mem_config.max_block_count_per_memory_type)) {
			return false;
		}
	}

	return true;
}

void empty_garbage_vk_allocator(vk_mem_allocator *allocator) {
	allocator->garbage_index = (allocator->garbage_index + 1) % NUM_FRAME_DATA;


}
