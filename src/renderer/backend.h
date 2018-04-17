#ifndef RENDERER_BACKEND_H
#define RENDERER_BACKEND_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "../vulkan/config.h"

typedef struct backend_counters {
    uint64_t gpu_microsec;
} backend_counters;

typedef struct renderer {
    uint32_t current_frame;
    uint32_t current_swap_index;
    uint64_t query_results[NUM_FRAME_DATA][NUM_TIMESTAMP_QUERIES];
    uint32_t query_index[NUM_FRAME_DATA];
    backend_counters pc;
} renderer;

void init_backend_counters(backend_counters *b);

#endif // RENDERER_BACKEND_H
