#ifndef GEOM_POLYHEDRON_H
#define GEOM_POLYHEDRON_H

#include <stdint.h>
#include "./geom.h"

void generate_polyhedron_geometry(vertex_float radius, uint32_t detail, const float *project_vertices,
    uint32_t project_vertices_count, const uint32_t *project_indices, uint32_t project_indices_count,
    uint32_t geom_config_flag_bits, uint32_t *vertex_count, vertex *vertices, uint32_t *index_count, uint32_t *indices);

#endif // GEOM_POLYHEDRON_H
