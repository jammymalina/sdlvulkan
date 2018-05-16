#ifndef GEOM_H
#define GEOM_H

#include <stdint.h>

#define GEOM_PI 3.14159265358979323846
#define GEOM_BIAS 0.00005

#define GEOM_CLOCKWISE_BIT        (UINT32_C(1) << 0)
#define GEOM_TEX_COORD_FLIP_Y_BIT (UINT32_C(1) << 1)

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

#endif // GEOM_H
