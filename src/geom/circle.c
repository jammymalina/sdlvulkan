#include "./circle.h"

#include <math.h>
#include "common.h"

void generate_circle_geometry(vertex_float radius, vertex_float theta_start, vertex_float theta_length,
    uint32_t segments, uint32_t geom_config_flag_bits, uint32_t *vertex_count, vertex *vertices,
    uint32_t *index_count, uint32_t *indices)
{
    radius = radius == 0 ? 1.0 : radius;

    segments = segments < 3 ? 3 : segments;

    vertex center = {
        .position = { 0.0, 0.0, 0.0 },
        .normal = { 0.0, 0.0, 1.0 },
        .uv = { 0.5, 0.5 }
    };

    uint32_t v_count = 1;
    uint32_t i_count = 0;

    push_triangle_vertex(vertices, &center);

    for (uint32_t s = 0, i = 0; s <= segments; s++, i += 3) {
        vertex_float segment = theta_start + ((vertex_float) s) / segments * theta_length;

        vertex_float x = radius * cos(segment);
        vertex_float y = radius * sin(segment);

        vertex v = {
            .position = { x, y, center.position[3] },
            .normal = { 0.0, 0.0, 1.0 },
            .uv = { (x / radius + 1) * 0.5, ( y / radius + 1) * 0.5 }
        };

        push_triangle_vertex(vertices + v_count, &v);
        v_count++;
    }

    for (uint32_t i = 1; i <= segments; i++) {
        uint32_t triangle_indices[] = { i, i + 1, 0 };
        push_triangle_indices(indices + i_count, triangle_indices, 6,
            (geom_config_flag_bits & GEOM_CLOCKWISE_BIT) != 0);
        i_count += 3;
    }

    if (vertex_count) {
        *vertex_count = v_count;
    }

    if (index_count) {
        *index_count = i_count;
    }
}
