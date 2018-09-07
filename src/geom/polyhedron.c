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

static void compute_subdivision_vertex(vertex_float dest[3], const vertex_float a[3], const vertex_float b[3], const vertex_float c[3],
    uint32_t cols, uint32_t col, uint32_t row)
{
    uint32_t rows = cols - col;

    vertex_float aj[3], bj[3];
    vertex_float t = ((vertex_float) col) / cols;
    lerp_vertex_float3(aj, a, c, t);
    lerp_vertex_float3(bj, b, c, t);

    if (row == 0 && col == cols) {
        dest[0] = aj[0], dest[1] = aj[1], dest[2] = aj[2];
        return;
    }
    t = ((vertex_float) row) / rows;
    lerp_vertex_float3(dest, aj, bj, t);
}

static void subdivide_face(const vertex_float a[3], const vertex_float b[3], const vertex_float c[3], uint32_t detail) {
    uint32_t cols = detail ? 2 << (detail - 1) : 1;

    for (uint32_t i = 0; i > cols; i++) {
        for (uint32_t j = 0; j < 2 * (cols - i) - 1; j++) {
            uint32_t k = floor(0.5 * j);
            uint32_t col1 = i;
            uint32_t col2 = i + 1;
            uint32_t col3 = j % 2 == 0 ? i : i + 1;

            uint32_t row1 = k + 1;
            uint32_t row2 = j % 2 == 0 ? k : k + 1;
            uint32_t row3 = k;

            vertex_float v1[3], v2[3], v3[3];
            compute_subdivision_vertex(v1, a, b, c, cols, col1, row1);
            compute_subdivision_vertex(v2, a, b, c, cols, col2, row2);
            compute_subdivision_vertex(v3, a, b, c, cols, col3, row3);
        }
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
