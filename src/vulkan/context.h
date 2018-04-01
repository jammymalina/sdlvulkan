#ifndef VULKAN_CONTEXT_H
#define VULKAN_CONTEXT_H

#include <vulkan/vulkan.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "./gpu_info.h"

#define MAX_PHYSICAL_DEVICES 32

typedef struct vk_context {
	VkInstance instance;
	VkSurfaceKHR surface;
	VkDevice device;

	gpu_info *gpus;
	uint32_t gpus_size;
} vk_context;

bool init_vulkan(vk_context *ctx, SDL_Window *window);
void shutdown_vulkan(vk_context *ctx);

#endif // VULKAN_CONTEXT_H
