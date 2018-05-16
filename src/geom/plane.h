#ifndef GEOM_PLANE_H
#define GEOM_PLANE_H

#include <stdint.h>
#include "./geom.h"

void generate_plane_geometry(vertex_float width, vertex_float height,
    uint32_t width_segments, uint32_t height_segments, uint32_t geom_config_flag_bits,
    uint32_t *vertex_count, vertex *vertices,
    uint32_t *index_count, uint32_t *indices);

#endif // GEOM_PLANE_H
