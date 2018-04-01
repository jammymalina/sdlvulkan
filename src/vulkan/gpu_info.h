#ifndef GPU_INFO_H
#define GPU_INFO_H

#include <stdbool.h>
#include <vulkan/vulkan.h>
#include <stdint.h>

typedef struct gpu_info {
	VkPhysicalDevice device;
	VkPhysicalDeviceFeatures features;
	VkPhysicalDeviceProperties props;
	VkPhysicalDeviceMemoryProperties mem_props;
	VkSurfaceCapabilitiesKHR suface_caps;
	
	VkSurfaceFormatKHR *surface_formats;
	uint32_t surface_formats_size;
	
	VkQueueFamilyProperties *queue_family_props;
	uint32_t queue_family_props_size; 
	
	VkExtensionProperties *extension_props;
	uint32_t extension_props_size;

	VkPresentModeKHR *present_modes;
	uint32_t present_modes_size;
} gpu_info;

void init_gpu_info(gpu_info *gi);
bool init_gpu_info_props(gpu_info *gi, VkPhysicalDevice device, VkSurfaceKHR surface);
void free_gpu_info(gpu_info *gi);

#endif // GPU_INFO_H
