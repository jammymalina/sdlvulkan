#include "./gpu_info.h"

#include "./functions/functions.h"
#include "./tools/tools.h"
#include "../utils/heap.h"
#include "../string/string.h"

void init_gpu_info(gpu_info *gpu) {
	gpu->device = VK_NULL_HANDLE;

	gpu->queue_family_props = NULL;
	gpu->queue_family_props_size = 0;

	gpu->surface_formats = NULL;
	gpu->surface_formats_size = 0;

	gpu->extension_props = NULL;
	gpu->extension_props_size = 0;

	gpu->present_modes = NULL;
	gpu->present_modes_size = 0;	
}

bool init_gpu_info_props(gpu_info *gpu, VkPhysicalDevice device, VkSurfaceKHR surface) {
	gpu->device = device;
	
	// Queue family props
	uint32_t num_queues = 0;
	vk_GetPhysicalDeviceQueueFamilyProperties(gpu->device, &num_queues, NULL);
	CHECK_VK_VAL(num_queues > 0, "No queue family props");
	
	gpu->queue_family_props = mem_alloc(num_queues * sizeof(VkQueueFamilyProperties));
	CHECK_ALLOC(gpu->queue_family_props, "Allocation fail");
	
	vk_GetPhysicalDeviceQueueFamilyProperties(gpu->device, &num_queues, 
		gpu->queue_family_props);
	CHECK_VK_VAL(num_queues > 0, "No queue family props");
	gpu->queue_family_props_size = num_queues;
	
	// Extensions
	uint32_t num_extensions = 0; 
	CHECK_VK(vk_EnumerateDeviceExtensionProperties(gpu->device, NULL, &num_extensions, NULL));
	CHECK_VK_VAL(num_extensions > 0, "No device extensions");

	gpu->extension_props = mem_alloc(num_extensions * sizeof(VkExtensionProperties));
	CHECK_ALLOC(gpu->extension_props, "Allocation fail");

	CHECK_VK(vk_EnumerateDeviceExtensionProperties(gpu->device, NULL, &num_extensions,
		gpu->extension_props));
	CHECK_VK_VAL(num_extensions > 0, "No device extensions");
	gpu->extension_props_size = num_extensions;

	// Surface formats
	uint32_t num_formats = 0;
	CHECK_VK(vk_GetPhysicalDeviceSurfaceFormatsKHR(gpu->device, surface,
		&num_formats, NULL));
	CHECK_VK_VAL(num_formats > 0, "No surface formats");

	gpu->surface_formats = mem_alloc(num_formats * sizeof(VkSurfaceFormatKHR));
	CHECK_ALLOC(gpu->surface_formats, "Allocation fail");

	CHECK_VK(vk_GetPhysicalDeviceSurfaceFormatsKHR(gpu->device, surface,
		&num_formats, gpu->surface_formats));
	CHECK_VK_VAL(num_formats > 0, "No surface formats");
	gpu->surface_formats_size = num_formats;
	
	uint32_t num_present_modes = 0;
	CHECK_VK(vk_GetPhysicalDeviceSurfacePresentModesKHR(gpu->device, surface, 
		&num_present_modes, NULL));
	CHECK_VK_VAL(num_present_modes > 0, "No surface present modes");

	gpu->present_modes = mem_alloc(num_present_modes * sizeof(VkPresentModeKHR));
	CHECK_ALLOC(gpu->present_modes, "Allocation fail");
	
	CHECK_VK(vk_GetPhysicalDeviceSurfacePresentModesKHR(gpu->device, surface, 
		&num_present_modes, gpu->present_modes));
	CHECK_VK_VAL(num_present_modes > 0, "No surface present modes");
	gpu->present_modes_size = num_present_modes;

	CHECK_VK(vk_GetPhysicalDeviceSurfaceCapabilitiesKHR(gpu->device, surface, &gpu->surface_caps));
	vk_GetPhysicalDeviceMemoryProperties(gpu->device, &gpu->mem_props);
	vk_GetPhysicalDeviceProperties(gpu->device, &gpu->props);
	vk_GetPhysicalDeviceFeatures(gpu->device, &gpu->features);
	
	return true;
}

bool check_desired_extensions(gpu_info *gpu, const char *const desired_extensions[], size_t desired_extensions_size) {
	size_t required = desired_extensions_size;
	size_t available = 0;

	for (size_t i = 0; i < desired_extensions_size; i++) {
		for (size_t j = 0; j < gpu->extension_props_size; j++) {
			if (string_compare(desired_extensions[i], gpu->extension_props[j].extensionName)) {
				available++;
				break;
			}
		}
	}

	return available == required;
}

bool is_gpu_suitable_for_graphics(gpu_info *gpu, VkSurfaceKHR surface,
	uint32_t *graphics_index, uint32_t *present_index) 
{
	bool graphics_index_found = false;
	bool present_index_found = false;

	if (gpu->surface_formats_size == 0) {
		return false;
	}

	if (gpu->present_modes_size == 0) {
		return false;
	}

	if (!gpu->features.geometryShader) {
		return false;
	}

	if (!check_desired_extensions(gpu, GRAPHICS_DEVICE_EXTENSIONS, GRAPHICS_DEVICE_EXTENSIONS_SIZE)) {
		return false;
	}

	for (uint32_t i = 0; i < gpu->queue_family_props_size; i++) {
		VkQueueFamilyProperties *props = &gpu->queue_family_props[i];
		if (props->queueCount == 0) {
			continue;
		}

		if (props->queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			*graphics_index = i;
			graphics_index_found = true;
		}

		VkBool32 support_present = VK_FALSE;
		CHECK_VK(vk_GetPhysicalDeviceSurfaceSupportKHR(gpu->device, i, surface, &support_present));
		if (support_present) {
			*present_index = i;
			present_index_found = true;
		}
	}

	return graphics_index_found && present_index_found;
}

int rate_gpu(gpu_info *gpu) {
	int score = 0;

	if (gpu->props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		score += 1000;
	}
	score += gpu->props.limits.maxImageDimension2D;

	return score;
}

void free_gpu_info(gpu_info *gpu) {
	if (gpu->queue_family_props) {
		mem_free(gpu->queue_family_props);
		gpu->queue_family_props = NULL;
		gpu->queue_family_props_size = 0;
	}
	if (gpu->extension_props) {
		mem_free(gpu->extension_props);
		gpu->extension_props = NULL;
		gpu->extension_props_size = 0;
	}
	if (gpu->surface_formats) {
		mem_free(gpu->surface_formats);
		gpu->surface_formats = NULL;
		gpu->surface_formats_size = 0;
	}
	if (gpu->present_modes) {
		mem_free(gpu->present_modes);
		gpu->present_modes = NULL;
		gpu->present_modes_size = 0;
	}
}
