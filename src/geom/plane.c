#include "./plane.h"

#include <stdbool.h>
#include "./geom.h"
#include "./common.h"

// default winding order is counter-clockwise
void generate_plane_geometry(vertex_float width, vertex_float height,
    uint32_t width_segments, uint32_t height_segments, uint32_t geom_config_flag_bits,
    uint32_t *vertex_count, vertex *vertices,
    uint32_t *index_count, uint32_t *indices)
{
    width  = width  == 0 ? 1 : width;
    height = height == 0 ? 1 : height;

    vertex_float width_half  = 0.5 * width;
    vertex_float height_half = 0.5 * height;

    width_segments = width_segments  == 0 ? 1 : width_segments;
    height_segments = height_segments == 0 ? 1 : height_segments;

    vertex_float segment_width  = width  / width_segments;
    vertex_float segment_height = height / height_segments;

    uint32_t v_count = (width_segments + 1) * (height_segments + 1);
    uint32_t i_count = width_segments * height_segments * 6;

    vertex *v_iter = vertices;
    uint32_t *i_iter = indices;

    for (uint32_t iy = 0; iy < height_segments + 1; iy++) {
        vertex_float y = iy * segment_height - height_half;
        for (uint32_t ix = 0; ix < width_segments + 1; ix++) {
            vertex_float x = ix * segment_width - width_half;
            if (vertices) {
                vertex ver = {
                    .position = { x, -y, 0.0 },
                    .normal = { 0.0, 0.0, 1.0 },
                    .uv = { ((vertex_float) ix) / width_segments, ((vertex_float) iy) / height_segments }
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
            if (indices) {
                uint32_t a = ix + (width_segments + 1) * iy;
                uint32_t b = ix + (width_segments + 1) * (iy + 1);
                uint32_t c = (ix + 1) + (width_segments + 1) * (iy + 1);
                uint32_t d = (ix + 1) + (width_segments + 1) * iy;

                uint32_t triangle_indices[6] = {
                    a, b, d,
                    b, c, d
                };
                push_triangle_indices(i_iter, triangle_indices, 6,
                    (geom_config_flag_bits & GEOM_CLOCKWISE_BIT) != 0);
                i_iter += 6;
            }
        }
    }

    if (vertex_count) {
        *vertex_count = v_count;
    }

    if (index_count) {
        *index_count = i_count;
    }
}
