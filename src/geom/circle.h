#ifndef GEOM_CIRCLE_H
#define GEOM_CIRCLE_H

#include <stdint.h>
#include <stddef.h>
#include "./geom.h"

void generate_circle_geometry(vertex_float radius, vertex_float theta_start, vertex_float theta_length,
    uint32_t segments, uint32_t geom_config_flag_bits, uint32_t *vertex_count, vertex *vertices,
    uint32_t *index_count, uint32_t *indices);

#endif // GEOM_CIRCLE_H
