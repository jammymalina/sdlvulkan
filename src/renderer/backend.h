#ifndef RENDERER_BACKEND_H
#define RENDERER_BACKEND_H

#include <stdint.h>
#include <stdbool.h>

typedef struct renderer {
    uint32_t current_frame;
    uint32_t current_swap_index;
} renderer;

#endif // RENDERER_BACKEND_H
