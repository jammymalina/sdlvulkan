#include "function_loader.h"

#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include "./functions.h"
#include "../../logger/logger.h"

#define EXPORTED_VULKAN_FUNCTION(name) PFN_vk##name vk_##name = NULL;
#define GLOBAL_LEVEL_VULKAN_FUNCTION(name) PFN_vk##name vk_##name = NULL; 
#define INSTANCE_LEVEL_VULKAN_FUNCTION(name) PFN_vk##name vk_##name = NULL; 
#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name) PFN_vk##name vk_##name = NULL; 
#define DEVICE_LEVEL_VULKAN_FUNCTION(name) PFN_vk##name vk_##name = NULL; 
#define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) PFN_vk##name vk_##name = NULL;

#include "list.inl"

bool load_external_function(PFN_vkGetInstanceProcAddr vk_get_proc) {
	if (!vk_get_proc) {
		return false;
	}
	vk_GetInstanceProcAddr = vk_get_proc;
	return true;
}

bool load_global_functions() {
	#define GLOBAL_LEVEL_VULKAN_FUNCTION(name)                                           \
		vk_##name = (PFN_vk##name) vk_GetInstanceProcAddr(NULL, "vk" #name);             \
		if(vk_##name == NULL) {                                                          \
			log_error("Could not load global level function: vk" #name);                 \
			return false;                                                                \
		} else {                                                                         \
			log_debug("Successfully loaded global Vulkan function: vk" #name);           \
		}

	#include "list.inl"

	return true;
}

bool load_instance_vulkan_functions(VkInstance instance)
{
	#define INSTANCE_LEVEL_VULKAN_FUNCTION(name)                                 \
		vk_##name = (PFN_vk##name) vk_GetInstanceProcAddr(instance, "vk" #name); \
		if (vk_##name == NULL) {                                                 \
			log_error("Could not load instance level function: vk" #name);       \
			return false;                                                        \
		} else {                                                                 \
			log_debug("Successfully loaded instance level function: vk" #name);  \
		}

	#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name)                  \
		vk_##name = (PFN_vk##name) vk_GetInstanceProcAddr(instance, "vk" #name); \
		if (vk_##name == NULL) {                                                 \
			log_error("Could not load instance level function: vk" #name);       \
			return false;                                                        \
		} else {                                                                 \
			log_debug("Successfully loaded instance level function: vk" #name);  \
		}

	#include "list.inl"

	return true;
}

// bool load_device_level_functions(vk_functions *vk, VkDevice device,
// 	const char loaded_extensions[MAX_VULKAN_EXTENSIONS][VK_MAX_EXTENSION_NAME_SIZE],
// 	uint32_t extensions_count)
// {
// 	#define DEVICE_LEVEL_VULKAN_FUNCTION(name)                                           \
// 		vk->name = (PFN_vk##name) vk->GetDeviceProcAddr(device, "vk" #name);             \
// 		if (vk->name == NULL) {                                                          \
// 			log_error("Could not load device level function: vk" #name);                 \
// 			return false;                                                                \
// 		} else {                                                                         \
// 			log_debug("Successfully loaded device level function: vk" #name);            \
// 		}

// 	#define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension)                 \
// 		for (size_t i = 0; i < extensions_count; i++) {                                  \
// 			if (strcmp(extension, loaded_extensions[i]) == 0) {                          \
// 				vk->name = (PFN_vk##name) vk->GetDeviceProcAddr(device, "vk" #name);     \
// 				if (vk->name == NULL) {                                                  \
// 					log_error("Could not load device level function: vk" #name);         \
// 					return false;                                                        \
// 				} else {                                                                 \
// 					log_debug("Successfully loaded device level function: vk" #name);    \
// 				}                                                                        \
// 			}                                                                            \
// 		}

// 	#include "list.inl"
	
// 	return true;	
// }

