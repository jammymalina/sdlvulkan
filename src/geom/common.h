#ifndef GEOM_COMMON_H
#define GEOM_COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include "./vertex.h"

static inline void push_triangle_indices(uint32_t *index_head, uint32_t *indices, uint32_t index_count, bool clockwise) {
    if (!index_head) {
        return;
    }
    // default winding order is counterclockwise - reverse indices
    if (clockwise) {
        uint32_t triangle_count = index_count / 3;
        for (uint32_t i = 0; i < triangle_count; i++) {
            uint32_t tmp = indices[i * 3];
            indices[i * 3] = indices[i * 3 + 2];
            indices[i * 3 + 2] = tmp;
        }
    }

    uint32_t *iter = index_head;
    for (uint32_t i = 0; i < index_count; i++) {
        *iter = indices[i];
        iter++;
    }
}

static inline void push_triangle_vertex(vertex *vertex_head, vertex *v) {
    if (!vertex_head) {
        return;
    }
    *vertex_head = *v;
}

static inline void push_triangle_vertices(vertex *vertex_head, vertex *vertices, uint32_t vertex_count) {
    vertex *iter = vertex_head;
    for (uint32_t i = 0; i < vertex_count; i++) {
        push_triangle_vertex(iter, &vertices[i]);
        iter++;
    }
}

#endif // GEOM_COMMON_H
