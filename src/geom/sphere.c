#include "./sphere.h"

#include <math.h>
#include "./common.h"
#include "./geom.h"

void generate_sphere_geometry(vertex_float radius, vertex_float phi_start, vertex_float phi_length,
    vertex_float theta_start, vertex_float theta_length, uint32_t width_segments, uint32_t height_segments,
    uint32_t geom_config_flag_bits, uint32_t *vertex_count, vertex *vertices, uint32_t *index_count, uint32_t *indices)
{
    radius = radius <= GEOM_BIAS ? 1.0 : radius;

    width_segments  = width_segments  < 3 ? 3 : width_segments;
    height_segments = height_segments < 2 ? 2 : height_segments;

    uint32_t vx_count = phi_length >= GEOM_2PI - GEOM_BIAS ? width_segments : width_segments + 1;
    uint32_t vy_count = theta_length >= GEOM_PI - GEOM_BIAS ? height_segments : height_segments + 1;

    vertex_float theta_end = theta_start + theta_length;

    uint32_t v_count = vx_count * vy_count;
    uint32_t i_count = 0;

    vertex *v_iter = vertices;
    uint32_t *i_iter = indices;

    for (uint32_t iy = 0; iy < vy_count; iy++) {
        for (uint32_t ix = 0; ix < vx_count; ix++) {
            vertex_float v = ((vertex_float) iy) / height_segments;
            if (vertices) {
                vertex_float u = ((vertex_float) ix) / width_segments;

                vertex_float x = -radius * cos(phi_start + u * phi_length) * sin(theta_start + v * theta_length);
                vertex_float y =  radius * cos(theta_start + v * theta_length);
                vertex_float z =  radius * sin(phi_start + u * phi_length) * sin(theta_start + v * theta_length);

                vertex ver = {
                    .position = { x, y, z },
                    .normal = { x, y, z },
                    .uv = { u, v }
                };

                if (geom_config_flag_bits & GEOM_TEX_COORD_FLIP_Y_BIT) {
                    ver.uv[1] = 1 - ver.uv[1];
                }

                push_triangle_vertex(v_iter, &ver);
                v_iter++;
            }
        }
    }

    for (uint32_t iy = 0; iy < height_segments; iy++) {
        for (uint32_t ix = 0; ix < width_segments; ix++) {
            uint32_t ix1 = (ix + 1) % vx_count;
            uint32_t iy1 = (iy + 1) % vy_count;

            uint32_t a = iy  * vx_count + ix1;
            uint32_t b = iy  * vx_count + ix;
            uint32_t c = iy1 * vx_count + ix;
            uint32_t d = iy1 * vx_count + ix1;

            uint32_t triangle_indices[6];
            uint32_t triangle_index_count = 0;
            if (iy != 0 || theta_start > GEOM_BIAS) {
                triangle_indices[0] = a;
                triangle_indices[1] = b;
                triangle_indices[2] = d;
                triangle_index_count += 3;
                i_iter += 3;
            }
            if (iy != height_segments - 1 || fabs(GEOM_PI - theta_end) > GEOM_BIAS) {
                triangle_indices[triangle_index_count + 0] = b;
                triangle_indices[triangle_index_count + 1] = c;
                triangle_indices[triangle_index_count + 2] = d;
                triangle_index_count += 3;
                i_iter += 3;
            }

            if (indices) {
                push_triangle_indices(i_iter, triangle_indices, triangle_index_count,
                    (geom_config_flag_bits & GEOM_CLOCKWISE_BIT) != 0);
            }
            i_count += triangle_index_count;
        }
    }

    if (vertex_count) {
        *vertex_count = v_count;
    }

    if (index_count) {
        *index_count = i_count;
    }
}
