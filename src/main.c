#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <vulkan/vulkan.h>
#include "./vulkan/functions/functions.h"
#include "./vulkan/functions/function_loader.h"
#include "./logger/logger.h"
#include "./vulkan/tools/tools.h"
#include "./utils/heap.h"

#define MAX_VULKAN_PHYSICAL_DEVICES 8

void quit(int rc);

typedef struct vk_context {
	VkInstance instance;
	VkSurfaceKHR surface;
	VkDevice device;
} vk_context;

void init_vk_context(vk_context *ctx) {
	ctx->instance = VK_NULL_HANDLE;
	ctx->surface = VK_NULL_HANDLE;
	ctx->device = VK_NULL_HANDLE;	
}

typedef struct gpu_info {
	VkPhysicalDevice device;
	VkPhysicalDeviceProperties props;
	VkPhysicalDeviceMemoryProperties mem_props;
	VkSurfaceCapabilitiesKHR suface_caps;
	VkSurfaceFormatKHR *surface_formats;
	VkQueueFamilyProperties *queue_family_props;
	VkExtensionProperties *extensions_props;
} gpu_info;

vk_context vk_ctx;

bool create_instance(vk_context *ctx, SDL_Window *window) {
	const char **extensions = NULL;
    unsigned extension_count = 0;

	 if(!SDL_Vulkan_GetInstanceExtensions(window, &extension_count, NULL)) {
        log_error("SDL_Vulkan_GetInstanceExtensions(): %s", SDL_GetError());
        quit(EXIT_FAILURE);
    }
    extensions = mem_alloc(sizeof(const char*) * extension_count);
    if(!extensions) {
        SDL_OutOfMemory();
        quit(EXIT_FAILURE);
    }
    if(!SDL_Vulkan_GetInstanceExtensions(window, &extension_count, extensions)) {
        mem_free(extensions);
        log_error("SDL_Vulkan_GetInstanceExtensions(): %s", SDL_GetError());
        quit(EXIT_FAILURE);
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
        quit(EXIT_FAILURE);
	}

	return true;
}

bool create_surface(vk_context *ctx, SDL_Window *window) {
	if (!SDL_Vulkan_CreateSurface(window, ctx->instance, &ctx->surface)) {
		log_error("SDL_Vulkan_CreateSurface(): %s", SDL_GetError());
		quit(EXIT_FAILURE);
	}
	return true;
}

bool init_vulkan_function_loader() {
	PFN_vkGetInstanceProcAddr vk_get_proc = SDL_Vulkan_GetVkGetInstanceProcAddr();
	if(!vk_get_proc) {
        log_error("SDL_Vulkan_GetVkGetInstanceProcAddr error: %s", SDL_GetError());
		return false;
    }
	return load_external_function(vk_get_proc);
}

bool enumerate_physical_devices(vk_context *ctx) {
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
	   quit(EXIT_FAILURE);	
	}

	if (physical_device_count == 0) {
		log_error("No physical devices");
		quit(EXIT_FAILURE);
	}

	if (physical_device_count > MAX_VULKAN_PHYSICAL_DEVICES) {
		log_error("Not enough space for all physical_devices");
		quit(EXIT_FAILURE);
	}

	result = vk_EnumeratePhysicalDevices(ctx->instance, &physical_device_count, physical_devices);

	if (result != VK_SUCCESS) {
		SDL_free(physical_devices);
		log_error("vkEnumeratePhysicalDevices error: %s", vulkan_result_to_string(result));
		quit(EXIT_FAILURE);
	}

	for (size_t i = 0; i < physical_device_count; i++) {
		uint32_t queue_family_count = 0;
		uint32_t queue_family_index;
		uint32_t device_ext_count = 0;
	}

	return true;
}

bool init_SDL() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
		log_error("Error while initializing SDL: %s", SDL_GetError());
		return false;
	}

	if (SDL_Vulkan_LoadLibrary(NULL) != 0) {
		log_error("Error while loading Vulkan library: %s", SDL_GetError());
		return false;
	}

	return true;
}

void shutdown_SDL() {
	SDL_Quit();
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

void quit(int rc) {
    shutdown_vulkan(&vk_ctx); 
	shutdown_SDL();  
    exit(rc);
}

int main(int argc, char* args[]) {
	if (!init_SDL()) {
		quit(EXIT_FAILURE);
	}

	SDL_DisplayMode mode;
	SDL_Window *window = NULL;

	window = SDL_CreateWindow("Vulkan sample", 0, 0, 1920, 1080, 
		SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_VULKAN);
	SDL_GetCurrentDisplayMode(0, &mode);
    log_info("Screen BPP: %d", SDL_BITSPERPIXEL(mode.format));
	int dw, dh;
	SDL_Vulkan_GetDrawableSize(window, &dw, &dh);
	log_info("Draw Size: %d, %d", dw, dh);

	if (!init_vulkan(&vk_ctx, window)) {
		quit(EXIT_FAILURE);
	}

	bool is_running = true;
	while (is_running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					is_running = false;
					break;
				case SDL_KEYUP:
					if (event.key.keysym.sym == SDLK_ESCAPE) {
						is_running = false;
					}
					break;
			}

		}
    }

	if (window == NULL) {
		log_error("Error while creating window: %s", SDL_GetError());
		return 1;
	}

	SDL_DestroyWindow(window);

	quit(EXIT_SUCCESS);
}
