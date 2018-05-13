#ifndef GEOM_VERTEX_H
#define GEOM_VERTEX_H

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

#endif // GEOM_VERTEX_H
