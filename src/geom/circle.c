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

    uint32_t v_count = theta_length >= GEOM_2PI - GEOM_BIAS ? segments + 1 : segments + 1 + 1;
    uint32_t i_count = 3 * segments;

    vertex *v_iter = vertices;
    uint32_t *i_iter = indices;

    push_triangle_vertex(v_iter, &center);
    v_iter++;

    for (uint32_t s = 0; s < v_count - 1; s++) {
        if (vertices) {
            vertex_float segment = theta_start + (((vertex_float) s) / segments) * theta_length;

            vertex_float x = radius * cos(segment);
            vertex_float y = radius * sin(segment);

            vertex v = {
                .position = { x, y, center.position[3] },
                .normal = { 0.0, 0.0, 1.0 },
                .uv = { (x / radius + 1) * 0.5, ( y / radius + 1) * 0.5 }
            };
            push_triangle_vertex(v_iter, &v);
            v_iter++;
        }
    }

    for (uint32_t i = 1; i <= segments; i++) {
        if (indices) {
            uint32_t triangle_indices[] = { 0, i + 1 < v_count ? i + 1 : 1, i };
            push_triangle_indices(i_iter, triangle_indices, 3, (geom_config_flag_bits & GEOM_CLOCKWISE_BIT) != 0);
            i_iter += 3;
        }
    }

    if (vertex_count) {
        *vertex_count = v_count;
    }

    if (index_count) {
        *index_count = i_count;
    }
}
