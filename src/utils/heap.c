#include "./heap.h"

#include <SDL2/SDL.h>

void *mem_alloc(size_t size) {
    return SDL_malloc(size);
}

void mem_free(void *data) {
    SDL_free(data);
}

