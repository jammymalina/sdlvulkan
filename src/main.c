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
#include "./input/input.h"
#include "./vertex_management/mesh_loader.h"

#define MS_PER_UPDATE 16

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

bool init(vk_context *ctx, SDL_Window *window) {
    return init_vulkan(ctx, window) &&
        init_mesh_loader(
            vertex_management_config.mesh_loader_config.max_vertex_buffer_size,
            vertex_management_config.mesh_loader_config.max_index_buffer_size
        ) &&
        init_input();
}

void shutdown_SDL() {
    SDL_Quit();
}

void quit(int rc) {
    shutdown_vulkan(&context);
    destroy_mesh_loader();
    shutdown_SDL();
    exit(rc);
}

int main(int argc, char* args[]) {
    if (!set_dirname(args[0])) {
        exit(EXIT_FAILURE);
    }
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

    if (!init(&context, window)) {
        quit(EXIT_FAILURE);
    }
    init_renderer();

    bool is_running = true;

    uint32_t previous_time = SDL_GetTicks();
    double lag = 0.0;
    while (is_running) {
        uint32_t current_time = SDL_GetTicks();
        uint32_t elapsed_time = current_time - previous_time;

        previous_time = current_time;
        lag += elapsed_time;

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
            update_input(&event);
        }

        while (lag >= MS_PER_UPDATE) {
            double delta = lag / MS_PER_UPDATE;
            lag -= MS_PER_UPDATE;
        }

        bool success = render();
        if (!success) {
            is_running = false;
        }
    }

    if (window == NULL) {
        log_error("Error while creating window: %s", SDL_GetError());
        return 1;
    }

    SDL_DestroyWindow(window);

    quit(EXIT_SUCCESS);
}
