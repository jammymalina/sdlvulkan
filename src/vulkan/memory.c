#include "./memory.h"

#include "./functions/functions.h"

GENERATE_BASIC_DYNAMIC_LIST_SOURCE(vk_block_list, vk_block_list, vk_block*)

void init_vk_allocation(vk_allocation *a) {
	a->block = NULL;
	a->pool_id = 0;
	a->device_memory = VK_NULL_HANDLE;
	a->offset = 0;
	a->size = 0;
	a->data = NULL;
}

bool allocate_vk_block(vk_block *block, uint32_t size, uint32_t align, VkDeviceSize granularity, 
	vk_allocation_type alloc_type, vk_allocation *allocation)
{
	return true;
}

void destroy_vk_block(vk_block *block) {
	if (is_host_visible(block->usage)) {
		// vk_UnmapMemory();
	}
}