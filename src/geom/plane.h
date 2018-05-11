#ifndef GEOM_PLANE_H
#define GEOM_PLANE_H

#include <stdint.h>
#include "./geom.h"

void generate_plane_geometry(geom_float width, geom_float height,
    uint32_t width_segments, uint32_t height_segments, const char *spec, uint32_t *vertex_count, geom_float *data,
    uint32_t *index_count, uint32_t *indices);

#endif
