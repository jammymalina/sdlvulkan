#ifndef VMATH_VEC4_H
#define VMATH_VEC4_H

#include "./vmath.h"

typedef float vec4[4];

#define VEC4_COPY(v) { v[0], v[1], v[2], v[3] }

void scalar_vec4(vec4 v, float x);

void addv4(vec4 dest, const vec4 u, const vec4 v);
void subv4(vec4 dest, const vec4 u, const vec4 v);
void mulv4(vec4 dest, const vec4 u, const vec4 v);
void divv4(vec4 dest, const vec4 u, const vec4 v);

void addv4s(vec4 dest, const vec4 u, float x);
void subv4s(vec4 dest, const vec4 u, float x);
void mulv4s(vec4 dest, const vec4 u, float x);
void divv4s(vec4 dest, const vec4 u, float x);

float dotv4(const vec4 u, const vec4 v);
float lengthv4(const vec4 v);

void normalizev4(vec4 dest, const vec4 u);

#endif // VMATH_VEC4_H
