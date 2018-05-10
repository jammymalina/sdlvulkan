#ifndef RENDERER_BACKEND_H
#define RENDERER_BACKEND_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "../vulkan/config.h"

typedef struct backend_counters {
    uint64_t gpu_microsec;
} backend_counters;

typedef struct render_backend {
    uint32_t current_frame;
    uint64_t query_results[NUM_FRAME_DATA][NUM_TIMESTAMP_QUERIES];
    uint32_t query_index[NUM_FRAME_DATA];
    bool command_buffer_recorded[NUM_FRAME_DATA];
    backend_counters pc;
} render_backend;

extern render_backend renderer;

void init_backend_counters(backend_counters *b);

void init_render_backend(render_backend *r);
bool execute_render_backend(render_backend *r);
bool block_swap_buffers_render_backend(render_backend *r);

void init_renderer();
bool render();

#endif // RENDERER_BACKEND_H
