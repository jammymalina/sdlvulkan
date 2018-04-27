#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <vulkan/vulkan.h>
#include "./logger/logger.h"
#include "./vulkan/context.h"
#include "./window/config.h"
#include "./renderer/config.h"
#include "./vulkan/memory/memory.h"
#include "./renderer/backend.h"
#include "./utils/file.h"

void quit(int rc);

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
    shutdown_vulkan(&context);
    shutdown_SDL();
    exit(rc);
}

int main(int argc, char* args[]) {
    set_dirname(args[0]);
    log_info("Binary path: %s, directory: %s", args[0], dirname);

    init_vk_context(&context);

    if (!init_SDL()) {
        quit(EXIT_FAILURE);
    }

    SDL_DisplayMode mode;
    SDL_Window *window = NULL;

    if (window_config.fullscreen) {
        window = SDL_CreateWindow("Vulkan sample", 0, 0, 0, 0,
            SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_VULKAN);
        SDL_GetWindowSize(window, &window_config.width, &window_config.height);
    } else {
        window = SDL_CreateWindow(
            "Vulkan sample", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_config.width, window_config.height,
            SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN
        );
    }

    if (!window) {
        log_error("Unable to create window");
        quit(EXIT_FAILURE);
    }

    SDL_GetCurrentDisplayMode(0, &mode);
    SDL_Vulkan_GetDrawableSize(window, &render_config.width, &render_config.height);
    log_info("Window size: %d %d", window_config.width, window_config.height);
    log_info("Draw Size: %d, %d", render_config.width, render_config.height);
    log_info("Screen BPP: %d", SDL_BITSPERPIXEL(mode.format));

    if (!init_vulkan(&context, window)) {
        quit(EXIT_FAILURE);
    }
    init_renderer();

    bool is_running = true;
    while (is_running) {
        if (!render()) {
            is_running = false;
        }
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
