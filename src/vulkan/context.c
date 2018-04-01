#include "./context.h"

#include <SDL2/SDL_vulkan.h>
#include <stdint.h>
#include "./functions/functions.h"
#include "./functions/function_loader.h"
#include "../logger/logger.h"
#include "../utils/heap.h"
#include "./tools/tools.h"

static void init_vk_context(vk_context *ctx) {
	ctx->instance = VK_NULL_HANDLE;
	ctx->surface = VK_NULL_HANDLE;
	ctx->device = VK_NULL_HANDLE;
	ctx->gpus = NULL;
	ctx->gpus_size = 0;
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
		mem_free(extensions);
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
	uint32_t num_physical_devices = 0;
	VkPhysicalDevice physical_devices[MAX_PHYSICAL_DEVICES];	

	CHECK_VK(vk_EnumeratePhysicalDevices(ctx->instance, &num_physical_devices, NULL));
	CHECK_VK_VAL(num_physical_devices > 0, "No physical_devices");
	CHECK_VK_VAL(num_physical_devices <= MAX_PHYSICAL_DEVICES, "Not enough space for physical devices");	

	CHECK_VK(vk_EnumeratePhysicalDevices(ctx->instance, &num_physical_devices, physical_devices));
	CHECK_VK_VAL(num_physical_devices > 0, "No physical devices");

	ctx->gpus = mem_alloc(num_physical_devices * sizeof(gpu_info));
	CHECK_ALLOC(ctx->gpus, "Allocation fail");

	for (size_t i = 0; i < num_physical_devices; i++) {
		init_gpu_info(&ctx->gpus[i]);
		bool ret = init_gpu_info_props(&ctx->gpus[i], physical_devices[i], ctx->surface);
		if (!ret) {
			log_error("Could not init gpu %lu properties", i);
			return false;
		}
	}

	ctx->gpus_size = num_physical_devices;
	log_info("Number of gpus found: %d", ctx->gpus_size);

	return true;
}

bool init_vulkan(vk_context *ctx, SDL_Window *window) {
	init_vk_context(ctx);
	return init_vulkan_function_loader() &&
		load_global_functions() &&
		create_instance(ctx, window) && 
		load_instance_vulkan_functions(ctx->instance) &&
		create_surface(ctx, window) &&
		enumerate_physical_devices(ctx);
}

void shutdown_vulkan(vk_context *ctx) {
	if (ctx->gpus_size > 0) {
		for (size_t i = 0; i < ctx->gpus_size; i++) {
			free_gpu_info(&ctx->gpus[i]);
		}
		mem_free(ctx->gpus);
	}
	if (ctx->surface && vk_DestroySurfaceKHR) {	
		vk_DestroySurfaceKHR(ctx->instance, ctx->surface, NULL);
	}
	if (ctx->instance && vk_DestroyInstance) {	
		vk_DestroyInstance(ctx->instance, NULL);
	}
	init_vk_context(ctx);
	SDL_Vulkan_UnloadLibrary();
}
