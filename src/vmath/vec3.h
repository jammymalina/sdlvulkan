#ifndef VMATH_VEC3_H
#define VMATH_VEC3_H

#include "./vmath.h"

typedef float vec3[3];

#define VEC3_COPY(v) { v[0], v[1], v[2] }

static inline void scalar_vec3(vec3 v, float x) {
    v[0] = x, v[1] = x, v[2] = x;
}

static inline void addv3(vec3 dest, const vec3 u, const vec3 v) {
    dest[0] = u[0] + v[0];
    dest[1] = u[1] + v[1];
    dest[2] = u[2] + v[2];
}

static inline void subv3(vec3 dest, const vec3 u, const vec3 v) {
    dest[0] = u[0] - v[0];
    dest[1] = u[1] - v[1];
    dest[2] = u[2] - v[2];
}

static inline void mulv3(vec3 dest, const vec3 u, const vec3 v) {
    dest[0] = u[0] * v[0];
    dest[1] = u[1] * v[1];
    dest[2] = u[2] * v[2];
}

static inline void divv3(vec3 dest, const vec3 u, const vec3 v) {
    dest[0] = u[0] / v[0];
    dest[1] = u[1] / v[1];
    dest[2] = u[2] / v[2];
}

static inline void addv3s(vec3 dest, const vec3 u, float x) {
    dest[0] = u[0] + x;
    dest[1] = u[1] + x;
    dest[2] = u[2] + x;
}

static inline void subv3s(vec3 dest, const vec3 u, float x) {
    dest[0] = u[0] - x;
    dest[1] = u[1] - x;
    dest[2] = u[2] - x;
}

static inline void mulv3s(vec3 dest, const vec3 u, float x) {
    dest[0] = u[0] * x;
    dest[1] = u[1] * x;
    dest[2] = u[2] * x;
}

static inline void divv3s(vec3 dest, const vec3 u, const float x) {
    dest[0] = u[0] / x;
    dest[1] = u[1] / x;
    dest[2] = u[2] / x;
}

static inline float dotv3(const vec3 u, const vec3 v) {
    return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}

static inline float lengthv3(const vec3 v) {
    return sqrt(dotv3(v, v));
}

static inline void normalizev3(vec3 dest, const vec3 u) {
    const float len = lengthv3(u);
    if (len <= EPSILON) {
        dest[0] = 0, dest[1] = 0, dest[2] = 0;
        return;
    }
    divv3s(dest, u, len);
}

static inline void crossv3(vec3 dest, const vec3 u, const vec3 v) {
    const float u0 = u[0], u1 = u[1], u2 = u[2];
    const float v0 = v[0], v1 = v[1], v2 = v[2];
    dest[0] = u1 * v2 - v1 * u2;
    dest[1] = u2 * v0 - v2 * u0;
    dest[2] = u0 * v1 - v0 * u1;
}

#endif // VMATH_VEC3_H
