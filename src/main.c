#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <vulkan/vulkan.h>
#include "./vulkan_utils/functions/functions.h"
#include "./vulkan_utils/functions/function_loader.h"


void quit(int rc);

static PFN_vkGetInstanceProcAddr vk_GetInstanceProcAddr = NULL;

bool load() {
	vk_GetInstanceProcAddr = SDL_Vulkan_GetVkGetInstanceProcAddr();
	if(!vk_GetInstanceProcAddr) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Vulkan_GetVkGetInstanceProcAddr error: %s\n", 
			SDL_GetError());
		return false;
    }
	return load_global_functions(vk_GetInstanceProcAddr);
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

bool init_vulkan() {
	return load();
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

	if (!init_vulkan()) {
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
