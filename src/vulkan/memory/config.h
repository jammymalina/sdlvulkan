#ifndef VULKAN_MEMORY_CONFIG_H
#define VULKAN_MEMORY_CONFIG_H

#include <vulkan/vulkan.h>
#include <stddef.h>

typedef struct renderer_configuration {
	int device_local_memory_MB;
	int host_visible_memory_MB;
	size_t max_block_count_per_memory_type;
	size_t max_garbage_allocations_size;
} vulkan_memory_configuration;

extern vulkan_memory_configuration vk_mem_config;

#endif //  VULKAN_MEMORY_CONFIG_H
