#include "./context.h"

#include <SDL2/SDL_vulkan.h>
#include "./functions/functions.h"
#include "./functions/function_loader.h"
#include "../logger/logger.h"
#include "../utils/heap.h"
#include "./tools/tools.h"

#define MAX_VULKAN_PHYSICAL_DEVICES 8

static void init_vk_context(vk_context *ctx) {
	ctx->instance = VK_NULL_HANDLE;
	ctx->surface = VK_NULL_HANDLE;
	ctx->device = VK_NULL_HANDLE;	
}

static bool create_instance(vk_context *ctx, SDL_Window *window) {
	const char **extensions = NULL;
    unsigned extension_count = 0;

	 if(!SDL_Vulkan_GetInstanceExtensions(window, &extension_count, NULL)) {
        log_error("SDL_Vulkan_GetInstanceExtensions(): %s", SDL_GetError());
        return false;
    }
    extensions = mem_alloc(sizeof(const char*) * extension_count);
    if(!extensions) {
        SDL_OutOfMemory();
        return false;
    }
    if(!SDL_Vulkan_GetInstanceExtensions(window, &extension_count, extensions)) {
        mem_free(extensions);
        log_error("SDL_Vulkan_GetInstanceExtensions(): %s", SDL_GetError());
        return false;
    }

	VkApplicationInfo vk_application_info = {
		.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pNext              = NULL,
		.pApplicationName   = "SDL test app",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName        = "Custom engine",
		.engineVersion      = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion         = VK_MAKE_VERSION(1, 0, VK_HEADER_VERSION)
	};

	VkInstanceCreateInfo vk_instance_create_info = {
		.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext                   = NULL,
		.flags                   = 0,
		.pApplicationInfo        = &vk_application_info,
		.enabledLayerCount       = 0,
		.ppEnabledLayerNames     = NULL,
		.enabledExtensionCount   = extension_count,
		.ppEnabledExtensionNames = extensions
	};

	VkResult result = vk_CreateInstance(&vk_instance_create_info, NULL, &ctx->instance);
	if (result != VK_SUCCESS || ctx->instance == VK_NULL_HANDLE) {
		ctx->instance = VK_NULL_HANDLE;
		log_error("Could not create Vulkan instance: %s", vulkan_result_to_string(result));
        return false;
	}

	return true;
}

static bool create_surface(vk_context *ctx, SDL_Window *window) {
	if (!SDL_Vulkan_CreateSurface(window, ctx->instance, &ctx->surface)) {
		log_error("SDL_Vulkan_CreateSurface(): %s", SDL_GetError());
	    return false;
	}
	return true;
}

static bool init_vulkan_function_loader() {
	PFN_vkGetInstanceProcAddr vk_get_proc = SDL_Vulkan_GetVkGetInstanceProcAddr();
	if(!vk_get_proc) {
        log_error("SDL_Vulkan_GetVkGetInstanceProcAddr error: %s", SDL_GetError());
		return false;
    }
	return load_external_function(vk_get_proc);
}

static bool enumerate_physical_devices(vk_context *ctx) {
	uint32_t physical_device_count = 0;
	VkPhysicalDevice physical_devices[MAX_VULKAN_PHYSICAL_DEVICES];
	VkQueueFamilyProperties *queue_family_props = NULL; 
	uint32_t queue_props_allocated_size = 0;
	VkExtensionProperties *device_extensions = NULL;
	uint32_t device_ext_allocated_size = 0;	
	uint32_t physical_device_index;

	VkResult result = vk_EnumeratePhysicalDevices(ctx->instance, &physical_device_count, NULL);
	if (result != VK_SUCCESS) {
		log_error("vkEnumeratePhysicalDevices error: %s", vulkan_result_to_string(result));
	    return false;	
	}

	if (physical_device_count == 0) {
		log_error("No physical devices");
	    return false;
	}

	if (physical_device_count > MAX_VULKAN_PHYSICAL_DEVICES) {
		log_error("Not enough space for all physical_devices");
	    return false;
	}

	result = vk_EnumeratePhysicalDevices(ctx->instance, &physical_device_count, physical_devices);

	if (result != VK_SUCCESS) {
		SDL_free(physical_devices);
		log_error("vkEnumeratePhysicalDevices error: %s", vulkan_result_to_string(result));
		return false;
	}

	for (size_t i = 0; i < physical_device_count; i++) {
		uint32_t queue_family_count = 0;
		uint32_t queue_family_index;
		uint32_t device_ext_count = 0;
	}

	return true;
}

bool init_vulkan(vk_context *ctx, SDL_Window *window) {
	init_vk_context(ctx);
	return init_vulkan_function_loader() &&
		load_global_functions() &&
		create_instance(ctx, window) && 
		load_instance_vulkan_functions(ctx->instance) &&
		create_surface(ctx, window);
}

void shutdown_vulkan(vk_context *ctx) {
	if (ctx->surface && vk_DestroySurfaceKHR) {	
		vk_DestroySurfaceKHR(ctx->instance, ctx->surface, NULL);
	}
	if (ctx->instance && vk_DestroyInstance) {	
		vk_DestroyInstance(ctx->instance, NULL);
	}
	init_vk_context(ctx);
	SDL_Vulkan_UnloadLibrary();
}