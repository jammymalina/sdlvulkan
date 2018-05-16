#include "./vertex_manager.h"

#include "../utils/heap.h"
#include "../geom/vertex.h"
#include "../logger/logger.h"

vertex_cache_manager vertex_cache;

bool init_vertex_manager(vertex_cache_manager *vc) {
    vc->static_data = mem_alloc(1600);
    float positions[] = {
        -0.5,  0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
         0.5,  0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
         0.0, -0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
    };
    uint32_t indices[] = {0, 1, 2};

    mem_copy(vc->static_data, positions, sizeof(positions));
    mem_copy(vc->static_data + sizeof(positions), indices, sizeof(indices));
    init_vk_buffer(&vc->static_buffer, VERTEX_INDEX_BUFFER);
    alloc_vk_buffer(&vc->static_buffer, vc->static_data, sizeof(positions) + sizeof(indices), BU_STATIC);
    // update_data_vk_buffer(&vc->static_buffer, vc->static_data, sizeof(positions) + sizeof(indices), 0);

    return true;
}

void destroy_vertex_manager(vertex_cache_manager *vc) {
    free_vk_buffer(&vc->static_buffer);
    mem_free(vc->static_data);
    vc->static_data = NULL;
}

bool init_vertex_cache() {
    return init_vertex_manager(&vertex_cache);
}

void destroy_vertex_cache() {
    destroy_vertex_manager(&vertex_cache);
}
