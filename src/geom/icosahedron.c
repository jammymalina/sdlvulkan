#include "./icosahedron.h"

#include <math.h>
#include "./common.h"
#include "./geom.h"

void generate_icosahedron_geometry(vertex_float radius, vertex_float detail, uint32_t geom_config_flag_bits,
    uint32_t *vertex_count, vertex *vertices, uint32_t *index_count, uint32_t *indices)
{
    vertex_float t = 0.5 * (1.0 + sqrt(5.0));

    const vertex_float v[] = {
        -1,  t,  0,   1, t, 0,   -1, -t,  0,    1, -t,  0,
         0, -1,  t,   0, 1, t,	  0, -1, -t,    0,  1, -t,
         t,  0, -1,   t, 0, 1,   -t,  0, -1,   -t,  0,  1
    };

    const uint32_t i[] = {
        0, 11, 5,   0,  5,  1,    0,  1,  7,    0, 7, 10,   0, 10, 11,
		1,  5, 9,   5, 11,  4,   11, 10,  2,   10, 7,  6,   7,  1,  8,
        3,  9, 4,   3,  4,  2,	  3,  2,  6,    3, 6,  8,   3,  8,  9,
        4,  9, 5,   2,  4, 11,	  6,  2, 10,    8, 6,  7,   9,  8,  1
    };
}
