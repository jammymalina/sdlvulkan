#ifndef VULKAN_CONTEXT_H
#define VULKAN_CONTEXT_H

#include <vulkan/vulkan.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct vk_context {
	VkInstance instance;
	VkSurfaceKHR surface;
	VkDevice device;
} vk_context;

typedef struct gpu_info {
	VkPhysicalDevice device;
	VkPhysicalDeviceProperties props;
	VkPhysicalDeviceMemoryProperties mem_props;
	VkSurfaceCapabilitiesKHR suface_caps;
	VkSurfaceFormatKHR *surface_formats;
	VkQueueFamilyProperties *queue_family_props;
	VkExtensionProperties *extensions_props;
} gpu_info;

bool init_vulkan(vk_context *ctx, SDL_Window *window);
void shutdown_vulkan(vk_context *ctx);

#endif // VULKAN_CONTEXT_H
