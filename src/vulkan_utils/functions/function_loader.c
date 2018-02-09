#include "function_loader.h"

#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include "./functions.h"
#include "../../logger/logger.h"

bool load_global_functions(const PFN_vkGetInstanceProcAddr vk_GetInstanceProcAddr) {
	#define GLOBAL_LEVEL_VULKAN_FUNCTION(name)                                           \
		PFN_vk##name vk_##name = (PFN_vk##name) vk_GetInstanceProcAddr(NULL, "vk" #name);             \
		if(vk_##name == NULL) {                                                          \
			error_log("Could not load global level function: vk" #name);                 \
			return false;                                                                \
		} else {                                                                         \
			debug_log("Successfully loaded global Vulkan function: vk" #name);           \
		}

	#include "list.inl"

	return true;
}

// bool load_instance_vulkan_functions(vk_functions *vk, VkInstance instance,
// 	const char loaded_extensions[MAX_VULKAN_EXTENSIONS][VK_MAX_EXTENSION_NAME_SIZE],
// 	uint32_t extensions_count)
// {
// 	#define INSTANCE_LEVEL_VULKAN_FUNCTION(name)                                         \
// 		vk->name = (PFN_vk##name) vk->GetInstanceProcAddr(instance, "vk" #name);         \
// 		if (vk->name == NULL) {                                                          \
// 			error_log("Could not load instance level function: vk" #name);               \
// 			return false;                                                                \
// 		} else {                                                                         \
// 			debug_log("Successfully loaded instance level function: vk" #name);          \
// 		}

// 	#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension)               \
// 		for (size_t i = 0; i < extensions_count; i++) {                                  \
// 			if (strcmp(extension, loaded_extensions[i]) == 0) {                          \
// 				vk->name = (PFN_vk##name) vk->GetInstanceProcAddr(instance, "vk" #name); \
// 				if (vk->name == NULL) {                                                  \
// 					error_log("Could not load instance level function: vk" #name);       \
// 					return false;                                                        \
// 				} else {                                                                 \
// 					debug_log("Successfully loaded instance level function: vk" #name);  \
// 				}                                                                        \
// 			}                                                                            \
// 		}

// 	#include "list.inl"

// 	return true;
// }

// bool load_device_level_functions(vk_functions *vk, VkDevice device,
// 	const char loaded_extensions[MAX_VULKAN_EXTENSIONS][VK_MAX_EXTENSION_NAME_SIZE],
// 	uint32_t extensions_count)
// {
// 	#define DEVICE_LEVEL_VULKAN_FUNCTION(name)                                           \
// 		vk->name = (PFN_vk##name) vk->GetDeviceProcAddr(device, "vk" #name);             \
// 		if (vk->name == NULL) {                                                          \
// 			error_log("Could not load device level function: vk" #name);                 \
// 			return false;                                                                \
// 		} else {                                                                         \
// 			debug_log("Successfully loaded device level function: vk" #name);            \
// 		}

// 	#define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension)                 \
// 		for (size_t i = 0; i < extensions_count; i++) {                                  \
// 			if (strcmp(extension, loaded_extensions[i]) == 0) {                          \
// 				vk->name = (PFN_vk##name) vk->GetDeviceProcAddr(device, "vk" #name);     \
// 				if (vk->name == NULL) {                                                  \
// 					error_log("Could not load device level function: vk" #name);         \
// 					return false;                                                        \
// 				} else {                                                                 \
// 					debug_log("Successfully loaded device level function: vk" #name);    \
// 				}                                                                        \
// 			}                                                                            \
// 		}

// 	#include "list.inl"
	
// 	return true;	
// }

