#ifndef GPU_INFO_H
#define GPU_INFO_H

#include <stddef.h>
#include <stdbool.h>
#include <vulkan/vulkan.h>
#include <stdint.h>

#define GRAPHICS_DEVICE_EXTENSIONS_SIZE 1
static const char *const GRAPHICS_DEVICE_EXTENSIONS[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

typedef struct gpu_info {
    VkPhysicalDevice device;
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceProperties props;
    VkPhysicalDeviceMemoryProperties mem_props;
    VkSurfaceCapabilitiesKHR surface_caps;
    
    VkSurfaceFormatKHR *surface_formats;
    uint32_t surface_formats_size;
    
    VkQueueFamilyProperties *queue_family_props;
    uint32_t queue_family_props_size; 
    
    VkExtensionProperties *extension_props;
    uint32_t extension_props_size;

    VkPresentModeKHR *present_modes;
    uint32_t present_modes_size;
} gpu_info;

void init_gpu_info(gpu_info *gpu);
bool init_gpu_info_props(gpu_info *gpu, VkPhysicalDevice device, VkSurfaceKHR surface);
bool check_desired_extensions(gpu_info *gpu, const char *const desired_extensions[], size_t desired_extensions_size);
bool is_gpu_suitable_for_graphics(gpu_info *gpu, VkSurfaceKHR surface,
    uint32_t *graphics_index, uint32_t *present_index);

bool choose_surface_format(gpu_info *gpu, VkSurfaceFormatKHR *result);
bool choose_present_mode(gpu_info *gpu, VkPresentModeKHR *result);
bool choose_extent(gpu_info *gpu, VkExtent2D *result, VkExtent2D *window_size);
bool choose_supported_format(gpu_info *gpu, VkFormat *result, VkFormat *formats, size_t num_formats, VkImageTiling tiling,
    VkFormatFeatureFlags features);

int rate_gpu(gpu_info *gpu);
void free_gpu_info(gpu_info *gpu);

#endif // GPU_INFO_H
