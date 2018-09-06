#ifndef GEOM_ICOSAHEDRON_H
#define GEOM_ICOSAHEDRON_H

#include <stdint.h>
#include "./geom.h"

void generate_icosahedron_geometry(vertex_float radius, vertex_float detail, uint32_t geom_config_flag_bits,
    uint32_t *vertex_count, vertex *vertices, uint32_t *index_count, uint32_t *indices);

#endif // GEOM_ICOSAHEDRON_H
