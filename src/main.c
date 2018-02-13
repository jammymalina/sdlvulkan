#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <vulkan/vulkan.h>
#include "./logger/logger.h"
#include "./vulkan/context.h"

void quit(int rc);

vk_context vk_ctx;

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
