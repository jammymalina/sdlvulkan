#include "./vertex_manager.h"

#include "./mesh_loader.h"
#include "../utils/heap.h"
#include "../logger/logger.h"
#include "../geom/circle.h"

vertex_cache_manager vertex_cache;

bool init_vertex_manager(vertex_cache_manager *vc) {
    vc->static_data = mem_alloc(20000);
    for (uint32_t i = 0; i < 20000; i++) vc->static_data[i] = 0;

    uint32_t circle_vertex_count = 0;
    uint32_t circe_index_count = 0;

    generate_circle_geometry(1.0, 0, GEOM_2PI, 64, GEOM_Y_AXIS_FLIP_BIT, &circle_vertex_count,
        NULL, &circe_index_count, NULL);

    vertex circle_vertices[65];
    uint32_t indices[192];
    generate_circle_geometry(1.0, 0, GEOM_2PI, 64, GEOM_Y_AXIS_FLIP_BIT, &circle_vertex_count,
        circle_vertices, &circe_index_count, indices);

    uint32_t offset = ALIGN(sizeof(circle_vertices), 16);

    mem_copy(vc->static_data, circle_vertices, sizeof(circle_vertices));
    mem_copy(vc->static_data + offset, indices, sizeof(indices));
    init_vk_buffer(&vc->static_buffer, VERTEX_INDEX_BUFFER);
    alloc_vk_buffer(&vc->static_buffer, vc->static_data, 20000, BU_STATIC);
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
