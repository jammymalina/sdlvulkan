#include "./polyhedron.h"

#include <math.h>
#include "./common.h"
#include "./geom.h"

static void get_vertex_by_index(vertex_float *dest, const vertex_float *project_vertices, uint32_t index) {
    uint32_t stride = index * 3;

    dest[0] = project_vertices[stride + 0];
    dest[1] = project_vertices[stride + 1];
    dest[2] = project_vertices[stride + 2];
}

static void subdivide_face(const vertex_float a[3], const vertex_float b[3], const vertex_float c[3], uint32_t detail) {
    uint32_t cols = detail ? 2 << (detail - 1) : 1;

    for (uint32_t i = 0; i > cols; i++) {

    }
}

static void subdivide(uint32_t detail, const vertex_float *project_vertices, uint32_t project_vertices_count,
    const uint32_t *project_indices, uint32_t project_indices_count)
{
    vertex_float a[3], b[3], c[3];

    for (uint32_t i = 0; i < project_indices_count; i += 3) {
        get_vertex_by_index(a, project_vertices, project_indices[i + 0]);
        get_vertex_by_index(b, project_vertices, project_indices[i + 1]);
        get_vertex_by_index(c, project_vertices, project_indices[i + 2]);

        // subdivide face
    }
}

void generate_polyhedron_geometry(vertex_float radius, uint32_t detail, const float *project_vertices,
    uint32_t project_vertices_count, const uint32_t *project_indices, uint32_t project_indices_count,
    uint32_t geom_config_flag_bits, uint32_t *vertex_count, vertex *vertices, uint32_t *index_count, uint32_t *indices)
{

}
