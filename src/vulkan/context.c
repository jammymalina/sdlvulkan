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
	ctx->graphics_queue = VK_NULL_HANDLE;
	ctx->present_queue = VK_NULL_HANDLE;
	ctx->command_pool = VK_NULL_HANDLE;
	for (size_t i = 0; i < NUM_FRAME_DATA; i++) {
		ctx->acquire_semaphores[i] = VK_NULL_HANDLE;
		ctx->render_complete_semaphores[i] = VK_NULL_HANDLE;
		ctx->command_buffer_fences[i] = VK_NULL_HANDLE;
	}
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
    CHECK_ALLOC(extensions, "Allocation fail");
    if (!SDL_Vulkan_GetInstanceExtensions(window, &extension_count, extensions)) {
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

static bool choose_suitable_graphics_gpu(vk_context *ctx) {
	int maxScore = -1;
	for (uint32_t i = 0; i < ctx->gpus_size; i++) {
		if (is_gpu_suitable_for_graphics(&ctx->gpus[i], ctx->surface,
			&ctx->graphics_family_index, &ctx->present_family_index)) 
		{
			int currentScore = rate_gpu(&ctx->gpus[i]);
			log_info("Found suitable gpu, score: %d", currentScore);
			if (currentScore > maxScore) {
				ctx->selected_gpu = i;
				maxScore = currentScore;
			}
		}
	}

	return maxScore != -1;
}

static bool create_device(vk_context *ctx) {
	uint32_t indices[] = { ctx->graphics_family_index, ctx->present_family_index };
	
	VkDeviceQueueCreateInfo devq_info[2];
	uint32_t queue_count = indices[0] == indices[1] ? 1 : 2;
	const float priority = 1.0f;
	for (uint32_t i = 0; i < queue_count; i++) {
		VkDeviceQueueCreateInfo qinfo = {
			.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.pNext            = NULL,
			.queueFamilyIndex = indices[i],
			.queueCount       = 1,
			.pQueuePriorities = &priority
		};
		devq_info[i] = qinfo;
	}

	gpu_info *gpu = &ctx->gpus[ctx->selected_gpu];

	VkPhysicalDeviceFeatures device_features = {};
	device_features.textureCompressionBC = VK_TRUE;
	device_features.imageCubeArray = VK_TRUE;
	device_features.depthClamp = VK_TRUE;
	device_features.depthBiasClamp = VK_TRUE;
	device_features.depthBounds = gpu->features.depthBounds;
	device_features.fillModeNonSolid = VK_TRUE;

	VkDeviceCreateInfo info = {
		.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext                   = NULL,
		.flags                   = 0,
		.queueCreateInfoCount    = queue_count,
		.pQueueCreateInfos       = devq_info,
		.enabledLayerCount       = 0,
		.ppEnabledLayerNames     = NULL,
		.enabledExtensionCount   = GRAPHICS_DEVICE_EXTENSIONS_SIZE,
		.ppEnabledExtensionNames = GRAPHICS_DEVICE_EXTENSIONS,
		.pEnabledFeatures        = &device_features
	};

	CHECK_VK(vk_CreateDevice(gpu->device, &info, NULL, &ctx->device));
	
	return true;
}

static bool init_queues(vk_context *ctx) {
	vk_GetDeviceQueue(ctx->device, ctx->graphics_family_index, 0, &ctx->graphics_queue);
	if (ctx->graphics_family_index == ctx->present_family_index) {
		ctx->present_queue = ctx->graphics_queue;
	} else {
		vk_GetDeviceQueue(ctx->device, ctx->present_family_index, 0, &ctx->present_queue);
	}

	return true;
}

static bool create_semaphores(vk_context *ctx) {
	VkSemaphoreCreateInfo sempahore_info = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0
	};
	for (size_t i = 0; i < NUM_FRAME_DATA; i++) {
		CHECK_VK(vk_CreateSemaphore(ctx->device, &sempahore_info, NULL, &ctx->acquire_semaphores[i]));
		CHECK_VK(vk_CreateSemaphore(ctx->device, &sempahore_info, NULL, &ctx->render_complete_semaphores[i]));		
	}
	return true;
}

static bool create_command_pool(vk_context *ctx) {
	VkCommandPoolCreateInfo pool_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = NULL,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = ctx->graphics_family_index 
	};

	CHECK_VK(vk_CreateCommandPool(ctx->device, &pool_info, NULL, &ctx->command_pool));

	return true;
}

static bool create_command_buffers(vk_context *ctx) {
	VkCommandBufferAllocateInfo allocate_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = NULL,
		.commandPool = ctx->command_pool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = NUM_FRAME_DATA
	};

	CHECK_VK(vk_AllocateCommandBuffers(ctx->device, &allocate_info, ctx->command_buffers));

	VkFenceCreateInfo fence_info = {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0
	};

	for (size_t i = 0; i < NUM_FRAME_DATA; i++) {
		CHECK_VK(vk_CreateFence(ctx->device, &fence_info, NULL, &ctx->command_buffer_fences[i]));
	}

	return true;
}

bool init_vulkan(vk_context *ctx, SDL_Window *window) {
	init_vk_context(ctx);
	return init_vulkan_function_loader() &&
		load_global_functions() &&
		create_instance(ctx, window) && 
		load_instance_vulkan_functions(ctx->instance) &&
		create_surface(ctx, window) &&
		enumerate_physical_devices(ctx) &&
		choose_suitable_graphics_gpu(ctx) &&
		create_device(ctx) &&
		load_device_level_functions(ctx->device) &&
		init_queues(ctx) &&
		create_semaphores(ctx) &&
		create_command_pool(ctx) && 
		create_command_buffers(ctx);
}

void shutdown_vulkan(vk_context *ctx) {
	if (vk_DestroyFence) {
		for (size_t i = 0; i < NUM_FRAME_DATA; i++) {
			if (ctx->command_buffer_fences[i]) {
				vk_DestroyFence(ctx->device, ctx->command_buffer_fences[i], NULL);
			}
		}
	}
	if (vk_FreeCommandBuffers && ctx->command_buffers[0]) {
		vk_FreeCommandBuffers(ctx->device, ctx->command_pool, NUM_FRAME_DATA, ctx->command_buffers);
	}
	if (vk_DestroyCommandPool && ctx->command_pool) {
		vk_DestroyCommandPool(ctx->device, ctx->command_pool, NULL);
	}
	if (vk_DestroySemaphore) {
		for (size_t i = 0; i < NUM_FRAME_DATA; i++) {
			if (ctx->acquire_semaphores[i]) {
				vk_DestroySemaphore(ctx->device, ctx->acquire_semaphores[i], NULL);
			}
			if (ctx->render_complete_semaphores[i]) {
				vk_DestroySemaphore(ctx->device, ctx->render_complete_semaphores[i], NULL);
			}
		}
	}
	if (ctx->gpus_size > 0) {
		for (size_t i = 0; i < ctx->gpus_size; i++) {
			free_gpu_info(&ctx->gpus[i]);
		}
		mem_free(ctx->gpus);
	}
	if (ctx->device && vk_DestroyDevice) {
        vk_DestroyDevice(ctx->device, NULL);
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
