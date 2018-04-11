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

bool choose_surface_format(gpu_info *gpu, VkSurfaceFormatKHR *result) {
    if (gpu->surface_formats_size == 0) {
        return false;
    }

    // If Vulkan returned an unknown format, then just force what we want.
    if (gpu->surface_formats_size == 1 && gpu->surface_formats[0].format == VK_FORMAT_UNDEFINED) {
        result->format = VK_FORMAT_B8G8R8A8_UNORM;
        result->colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        return true;
    }

    // Favor 32 bit rgba and srgb nonlinear colorspace
    for (size_t i = 0; i < gpu->surface_formats_size; i++) {
        VkSurfaceFormatKHR  *fmt = &gpu->surface_formats[i];
        if (fmt->format == VK_FORMAT_B8G8R8A8_UNORM && fmt->colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            result->format = fmt->format;
            result->colorSpace = fmt->colorSpace;
            return true;
        }
    }

    // If all else fails, just return what's available
    result->format = gpu->surface_formats[0].format;
    result->colorSpace = gpu->surface_formats[0].colorSpace; 	 
    return true;
}

bool choose_present_mode(gpu_info *gpu, VkPresentModeKHR *result) {
    if (gpu->present_modes_size == 0) {
        return false;
    }

    const VkPresentModeKHR desired_mode = VK_PRESENT_MODE_MAILBOX_KHR;

    // Favor looking for mailbox mode.
    for (size_t i = 0; i < gpu->present_modes_size; i++) {
        if (gpu->present_modes[i] == desired_mode) {
            *result = desired_mode;
            return true;
        }
    }

    // If we couldn't find mailbox, then default to FIFO which is always available.
    *result = VK_PRESENT_MODE_FIFO_KHR;
    return true;
}

bool choose_extent(gpu_info *gpu, VkExtent2D *result, VkExtent2D *window_size) {
    // The extent is typically the size of the window we created the surface from.
    // However if Vulkan returns -1 then simply substitute the window size.
    if (gpu->surface_caps.currentExtent.width == -1) {
        result->width = window_size->width;
        result->height = window_size->height;
    } else {
        result->width = gpu->surface_caps.currentExtent.width;
        result->height = gpu->surface_caps.currentExtent.height;		
    }

    return true;
}

bool choose_supported_format(gpu_info *gpu, VkFormat *result, VkFormat *formats, size_t num_formats, VkImageTiling tiling,
    VkFormatFeatureFlags features) 
{
    for (size_t i = 0; i < num_formats; i++) {
        VkFormat format = formats[i];

        VkFormatProperties props;
        vk_GetPhysicalDeviceFormatProperties(gpu->device, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            *result = format;
            return true;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            *result = format;
            return true;
        }
    }

    *result = VK_FORMAT_UNDEFINED;

    return false;
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

    if (!(gpu->surface_caps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)) {
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
