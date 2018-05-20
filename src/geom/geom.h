#ifndef GEOM_H
#define GEOM_H

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#define GEOM_PI  3.14159265358979323846
#define GEOM_2PI (2 * GEOM_PI)
#define GEOM_BIAS 0.00005

#define GEOM_CLOCKWISE_BIT        (UINT32_C(1) << 0)
#define GEOM_TEX_COORD_FLIP_Y_BIT (UINT32_C(1) << 1)
#define GEOM_Y_AXIS_FLIP_BIT      (UINT32_C(1) << 2) // default y axis points upwards

#ifdef VERTEX_FLOAT_PRECISION
    typedef VERTEX_FLOAT_PRECISION vertex_float;
#else
    typedef float vertex_float;
#endif

typedef struct vertex {
    vertex_float position[3];
    vertex_float normal[3];
    vertex_float uv[2];
} vertex;

static inline uint32_t vertex_data_to_float_data(float *dest, vertex *vertices, uint32_t vertex_count) {
    uint32_t j = 0;
    for (uint32_t i = 0; i < vertex_count; i++) {
        dest[j++] = vertices[i].position[0];
        dest[j++] = vertices[i].position[1];
        dest[j++] = vertices[i].position[2];

        dest[j++] = vertices[i].normal[0];
        dest[j++] = vertices[i].normal[1];
        dest[j++] = vertices[i].normal[2];

        dest[j++] = vertices[i].uv[0];
        dest[j++] = vertices[i].uv[1];

    }

    return j;
}

#endif // GEOM_H
