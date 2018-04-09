#include "./memory.h"

GENERATE_BASIC_DYNAMIC_LIST_SOURCE(vk_block_list, vk_block_list, vk_block*)

void init_vk_allocation(vk_allocation *a) {
	a->block = NULL;
	a->pool_id = 0;
	a->device_memory = VK_NULL_HANDLE;
	a->offset = 0;
	a->size = 0;
	a->data = NULL;
}