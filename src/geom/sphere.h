#ifndef GEOM_SPHERE_H
#define GEOM_SPHERE_H

#include <stdint.h>
#include "./geom.h"

void generate_sphere_geometry(vertex_float radius, vertex_float phi_start, vertex_float phi_length,
    vertex_float theta_start, vertex_float theta_length, uint32_t width_segments, uint32_t height_segments,
    uint32_t geom_config_flag_bits, uint32_t *vertex_count, vertex *vertices, uint32_t *index_count, uint32_t *indices);

#endif // GEOM_SPHERE_H
