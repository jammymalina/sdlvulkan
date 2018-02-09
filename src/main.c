#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <vulkan/vulkan.h>
#include "./vulkan/functions/functions.h"
#include "./vulkan/functions/function_loader.h"
#include "./logger/logger.h"

void quit(int rc);

VkInstance instance = NULL;

bool create_instance(SDL_Window *window) {
	const char **extensions = NULL;
    unsigned extension_count = 0;

	 if(!SDL_Vulkan_GetInstanceExtensions(window, &extension_count, NULL)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "SDL_Vulkan_GetInstanceExtensions(): %s\n", SDL_GetError());
        quit(EXIT_FAILURE);
    }
    extensions = SDL_malloc(sizeof(const char*) * extension_count);
    if(!extensions) {
        SDL_OutOfMemory();
        quit(EXIT_FAILURE);
    }
    if(!SDL_Vulkan_GetInstanceExtensions(window, &extension_count, extensions)) {
        SDL_free(extensions);
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "SDL_Vulkan_GetInstanceExtensions(): %s\n", SDL_GetError());
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

	VkResult result = vk_CreateInstance(&vk_instance_create_info, NULL, &instance);
	if (result != VK_SUCCESS || instance == VK_NULL_HANDLE) {
		instance = NULL;
		error_log("Could not create Vulkan instance");
        quit(EXIT_FAILURE);
	}

	return load_instance_vulkan_functions(instance, extensions, extension_count);
}

bool init_vulkan_function_loader() {
	PFN_vkGetInstanceProcAddr vk_get_proc = SDL_Vulkan_GetVkGetInstanceProcAddr();
	if(!vk_get_proc) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Vulkan_GetVkGetInstanceProcAddr error: %s\n", 
			SDL_GetError());
		return false;
    }
	return load_external_function(vk_get_proc);
}

bool init_SDL() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error while initializing SDL: %s\n", SDL_GetError());
		return false;
	}

	if (SDL_Vulkan_LoadLibrary(NULL) != 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error while loading Vulkan library: %s\n", SDL_GetError());
		return false;
	}

	return true;
}

void shutdown_SDL() {
	SDL_Quit();
}

bool init_vulkan(SDL_Window *window) {
	return init_vulkan_function_loader() &&
		load_global_functions() &&
		create_instance(window);
}

void shutdown_vulkan() {
	SDL_Vulkan_UnloadLibrary();
}

void quit(int rc) {
    shutdown_vulkan(); 
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
    SDL_Log("Screen BPP: %d\n", SDL_BITSPERPIXEL(mode.format));
	int dw, dh;
	SDL_Vulkan_GetDrawableSize(window, &dw, &dh);
	SDL_Log("Draw Size: %d, %d\n", dw, dh);

	if (!init_vulkan(window)) {
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
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error while creating window: %s", SDL_GetError());
		return 1;
	}

	SDL_DestroyWindow(window);

	quit(EXIT_SUCCESS);
}
