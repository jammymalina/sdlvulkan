#include "./vertex_manager.h"

#include "../utils/heap.h"
#include "../logger/logger.h"
#include "../geom/circle.h"

vertex_cache_manager vertex_cache;

bool init_vertex_manager(vertex_cache_manager *vc) {
    vc->static_data = mem_alloc(1600);
    for (uint32_t i = 0; i < 1600; i++) vc->static_data[i] = 0;

    uint32_t circle_vertex_count = 0;
    uint32_t circe_index_count = 0;

    generate_circle_geometry(0.6, 0, GEOM_2PI, 5, GEOM_Y_AXIS_FLIP_BIT, &circle_vertex_count,
        NULL, &circe_index_count, NULL);

    vertex circle_vertices[6];
    uint32_t indices[15];
    generate_circle_geometry(0.6, 0, GEOM_2PI, 5, GEOM_Y_AXIS_FLIP_BIT, &circle_vertex_count,
        circle_vertices, &circe_index_count, indices);

    float vertex_data[48];
    vertex_data_to_float_data(vertex_data, circle_vertices, 6);
    uint32_t offset = ALIGN(sizeof(vertex_data), 16);

    mem_copy(vc->static_data, vertex_data, sizeof(vertex_data));
    mem_copy(vc->static_data + offset, indices, sizeof(indices));
    init_vk_buffer(&vc->static_buffer, VERTEX_INDEX_BUFFER);
    alloc_vk_buffer(&vc->static_buffer, vc->static_data, 1600, BU_STATIC);
    // update_data_vk_buffer(&vc->static_buffer, vc->static_data, sizeof(vertex_data) + sizeof(indices), 0);

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
