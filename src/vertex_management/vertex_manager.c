#include "./vertex_manager.h"

#include "../utils/heap.h"
#include "../geom/vertex.h"
#include "../geom/geom.h"
#include "../geom/sphere.h"
#include "../logger/logger.h"

vertex_cache_manager vertex_cache;

bool init_vertex_manager(vertex_cache_manager *vc) {
    uint32_t vertex_count = 0;
    uint32_t index_count = 0;

    vertex vertices[600];
    float packed_vertex_data[5000];
    uint32_t indices[3000];

    generate_sphere_geometry(1.0, 0, 2 * GEOM_PI, 0, GEOM_PI, 32, 16, 0, &vertex_count, vertices, &index_count, indices);

    log_info("%u %u", index_count, vertex_count);

    vc->static_data = mem_alloc(1 * 1024 * 1024);

    log_info("%d", vc->static_data != NULL);

    size_t j = 0;
    for (size_t i = 0; i < vertex_count; i++) {
        vertex *v = &vertices[i];
        vc->static_data[j + 0] = v->position[0];
        vc->static_data[j + 1]  = v->position[1];
        vc->static_data[j + 2]  = v->position[2];

        vc->static_data[j + 3]  = v->normal[0];
        vc->static_data[j + 4]  = v->normal[1];
        vc->static_data[j + 5]  = v->normal[2];

        vc->static_data[j + 6]  = v->uv[0];
        vc->static_data[j + 7]  = v->uv[1];

        j += 8;
    }

    log_info("%zu", sizeof(vertex) * vertex_count + sizeof(uint32_t) * index_count);

    mem_copy(vc->static_data, packed_vertex_data, sizeof(vertex) * vertex_count);
    // mem_copy(vc->static_data + sizeof(vertex) * vertex_count, indices, sizeof(uint32_t) * index_count);
    init_vk_buffer(&vc->static_buffer, VERTEX_BUFFER);
    log_info("%d", alloc_vk_buffer(&vc->static_buffer, vc->static_data, 1 * 1024 * 1024, BU_DYNAMIC));

    return true;
}

void destroy_vertex_manager(vertex_cache_manager *vc) {
    free_vk_buffer(&vc->static_buffer);
    mem_free(vc->static_data);
}

bool init_vertex_cache() {
    return init_vertex_manager(&vertex_cache);
}

void destroy_vertex_cache() {
    destroy_vertex_manager(&vertex_cache);
}
