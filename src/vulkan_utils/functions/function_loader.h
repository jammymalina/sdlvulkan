#ifndef VULKAN_FUNCTION_LOADER_H
#define VULKAN_FUNCTION_LOADER_H

#include <vulkan/vulkan.h>
#include <stdbool.h>
#include <stdint.h>
#include "./functions.h"

bool load_global_functions(const PFN_vkGetInstanceProcAddr vk_GetInstanceProcAddr);
// bool load_instance_vulkan_functions(vk_functions *vk, VkInstance instance, 
// 	const char loaded_extensions[MAX_VULKAN_EXTENSIONS][VK_MAX_EXTENSION_NAME_SIZE],
// 	uint32_t extensions_count);
// bool load_device_level_functions(vk_functions *vk, VkDevice device, 
// 	const char loaded_extensions[MAX_VULKAN_EXTENSIONS][VK_MAX_EXTENSION_NAME_SIZE],
// 	uint32_t extensions_count);

#endif // VULKAN_FUNCTION_LOADER_H

