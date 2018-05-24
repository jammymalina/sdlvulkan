#ifndef VMATH_VEC4_H
#define VMATH_VEC4_H

#include "./vmath.h"

typedef float vec4[4];

static inline void scalar_vec4(vec4 v, float x) {
    v[0] = x, v[1] = x, v[2] = x, v[3] = x;
}

static inline void addv4(vec4 dest, const vec4 u, const vec4 v) {
    dest[0] = u[0] + v[0];
    dest[1] = u[1] + v[1];
    dest[2] = u[2] + v[2];
    dest[3] = u[3] + v[3];
}

static inline void subv4(vec4 dest, const vec4 u, const vec4 v) {
    dest[0] = u[0] - v[0];
    dest[1] = u[1] - v[1];
    dest[2] = u[2] - v[2];
    dest[3] = u[3] - v[3];
}

static inline void mulv4(vec4 dest, const vec4 u, const vec4 v) {
    dest[0] = u[0] * v[0];
    dest[1] = u[1] * v[1];
    dest[2] = u[2] * v[2];
    dest[3] = u[3] * v[3];
}

static inline void divv4(vec4 dest, const vec4 u, const vec4 v) {
    dest[0] = u[0] / v[0];
    dest[1] = u[1] / v[1];
    dest[2] = u[2] / v[2];
    dest[3] = u[3] / v[3];
}

static inline void addv4s(vec4 dest, const vec4 u, float x) {
    dest[0] = u[0] + x;
    dest[1] = u[1] + x;
    dest[2] = u[2] + x;
    dest[3] = u[3] + x;
}

static inline void subv4s(vec4 dest, const vec4 u, float x) {
    dest[0] = u[0] - x;
    dest[1] = u[1] - x;
    dest[2] = u[2] - x;
    dest[3] = u[3] - x;
}

static inline void mulv4s(vec4 dest, const vec4 u, float x) {
    dest[0] = u[0] * x;
    dest[1] = u[1] * x;
    dest[2] = u[2] * x;
    dest[3] = u[3] * x;
}

static inline void divv4s(vec4 dest, const vec4 u, const float x) {
    dest[0] = u[0] / x;
    dest[1] = u[1] / x;
    dest[2] = u[2] / x;
    dest[3] = u[3] / x;
}

static inline float dotv4(const vec4 u, const vec4 v) {
    return u[0] * v[0] + u[1] * v[1] + u[2] * v[2] + u[3] * v[3];
}

static inline float lengthv4(const vec4 v) {
    return sqrt(dotv4(v, v));
}

static inline void normalizev4(vec4 dest, const vec4 u) {
    const float len = lengthv4(u);
    if (len <= EPSILON) {
        return;
    }
    divv4s(dest, u, len);
}

#endif // VMATH_VEC4_H
