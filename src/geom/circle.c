#include "./circle.h"

void generate_circle_geometry(vertex_float radius, vertex_float theta_start, vertex_float theta_length,
    uint32_t segments, uint32_t geom_config_flag_bits, uint32_t *vertex_count, vertex *vertices,
    uint32_t *index_count, uint32_t *indices)
{
    radius = radius == 0 ? 1.0 : radius;

    segments = segments < 3 ? 3 : segments;

    uint32_t v_count = 0;
    uint32_t i_count = 0;

    if (vertex_count) {
        *vertex_count = v_count;
    }

    if (index_count) {
        *index_count = i_count;
    }
}
