#include "./gpu_info.h"

#include <stddef.h>
#include "./functions/functions.h"
#include "./tools/tools.h"
#include "../utils/heap.h"

void init_gpu_info(gpu_info *gi) {
	gi->device = VK_NULL_HANDLE;

	gi->queue_family_props = NULL;
	gi->queue_family_props_size = 0;

	gi->surface_formats = NULL;
	gi->surface_formats_size = 0;

	gi->extension_props = NULL;
	gi->extension_props_size = 0;

	gi->present_modes = NULL;
	gi->present_modes_size = 0;	
}

bool init_gpu_info_props(gpu_info *gi, VkPhysicalDevice device, VkSurfaceKHR surface) {
		gi->device = device;
		
		// Queue family props
		uint32_t num_queues = 0;
		vk_GetPhysicalDeviceQueueFamilyProperties(gi->device, &num_queues, NULL);
		CHECK_VK_VAL(num_queues > 0, "No queue family props");
		
		gi->queue_family_props = mem_alloc(num_queues * sizeof(VkQueueFamilyProperties));
		CHECK_ALLOC(gi->queue_family_props, "Allocation fail");
		
		vk_GetPhysicalDeviceQueueFamilyProperties(gi->device, &num_queues, 
			gi->queue_family_props);
		CHECK_VK_VAL(num_queues > 0, "No queue family props");
		gi->queue_family_props_size = num_queues;
		
		// Extensions
		uint32_t num_extensions = 0; 
		CHECK_VK(vk_EnumerateDeviceExtensionProperties(gi->device, NULL, &num_extensions, NULL));
		CHECK_VK_VAL(num_extensions > 0, "No device extensions");

		gi->extension_props = mem_alloc(num_extensions * sizeof(VkExtensionProperties));
		CHECK_ALLOC(gi->extension_props, "Allocation fail");

		CHECK_VK(vk_EnumerateDeviceExtensionProperties(gi->device, NULL, &num_extensions,
			gi->extension_props));
		CHECK_VK_VAL(num_extensions > 0, "No device extensions");
		gi->extension_props_size = num_extensions;
	
		// Surface formats
		uint32_t num_formats = 0;
		CHECK_VK(vk_GetPhysicalDeviceSurfaceFormatsKHR(gi->device, surface,
			&num_formats, NULL));
		CHECK_VK_VAL(num_formats > 0, "No surface formats");

		gi->surface_formats = mem_alloc(num_formats * sizeof(VkSurfaceFormatKHR));
		CHECK_ALLOC(gi->surface_formats, "Allocation fail");

		CHECK_VK(vk_GetPhysicalDeviceSurfaceFormatsKHR(gi->device, surface,
			&num_formats, gi->surface_formats));
		CHECK_VK_VAL(num_formats > 0, "No surface formats");
		gi->surface_formats_size = num_formats;
		
		uint32_t num_present_modes = 0;
		CHECK_VK(vk_GetPhysicalDeviceSurfacePresentModesKHR(gi->device, surface, 
			&num_present_modes, NULL));
		CHECK_VK_VAL(num_present_modes > 0, "No surface present modes");

		gi->present_modes = mem_alloc(num_present_modes * sizeof(VkPresentModeKHR));
		CHECK_ALLOC(gi->present_modes, "Allocation fail");
		
		CHECK_VK(vk_GetPhysicalDeviceSurfacePresentModesKHR(gi->device, surface, 
			&num_present_modes, gi->present_modes));
		CHECK_VK_VAL(num_present_modes > 0, "No surface present modes");
		gi->present_modes_size = num_present_modes;

		vk_GetPhysicalDeviceMemoryProperties(gi->device, &gi->mem_props);
		vk_GetPhysicalDeviceProperties(gi->device, &gi->props);
		vk_GetPhysicalDeviceFeatures(gi->device, &gi->features);
		
		return true;
}

void free_gpu_info(gpu_info *gi) {
	if (gi->queue_family_props_size > 0) {
		mem_free(gi->queue_family_props);
		gi->queue_family_props = NULL;
		gi->queue_family_props_size = 0;
	}
	if (gi->extension_props_size > 0) {
		mem_free(gi->extension_props);
		gi->extension_props = NULL;
		gi->extension_props_size = 0;
	}
	if (gi->surface_formats_size > 0) {
		mem_free(gi->surface_formats);
		gi->surface_formats = NULL;
		gi->surface_formats_size = 0;
	}
	if (gi->present_modes_size > 0) {
		mem_free(gi->present_modes);
		gi->present_modes = NULL;
		gi->present_modes_size = 0;
	}
}
