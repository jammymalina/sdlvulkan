#ifndef VERTEX_MANAGER_H
#define VERTEX_MANAGER_H

#define VERTEX_CACHE_MEMORY_SIZE_PER_FRAME (10 * 1024 * 1024) // 10 MB
#define VERTEX_CACHE_STATIC_MEMORY_SIZE    (10 * 1024 * 1024) // 10 MB

#include <stddef.h>
#include <stdbool.h>
#include "../vulkan/buffers/buffers.h"

typedef struct vertex_cache_manager {
    vk_buffer static_buffer;
    byte *static_data;
} vertex_cache_manager;

bool init_vertex_manager(vertex_cache_manager *vc);
void destroy_vertex_manager(vertex_cache_manager *vc);

bool init_vertex_cache();
void destroy_vertex_cache();

extern vertex_cache_manager vertex_cache;

#endif // VERTEX_MANAGER_H
