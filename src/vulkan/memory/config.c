#include "./config.h"

vulkan_memory_configuration vk_mem_config = {
	.device_local_memory_MB = 128,
	.host_visible_memory_MB = 64,
	.max_block_count_per_memory_type = 20,
	.max_garbage_allocations_size = 400
};
