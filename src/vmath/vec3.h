#ifndef VMATH_VEC3_H
#define VMATH_VEC3_H

#include "./vmath.h"

typedef float vec3[3];

#define VEC3_COPY(v) { v[0], v[1], v[2] }

void scalar_vec3(vec3 v, float x);

void addv3(vec3 dest, const vec3 u, const vec3 v);
void subv3(vec3 dest, const vec3 u, const vec3 v);
void mulv3(vec3 dest, const vec3 u, const vec3 v);
void divv3(vec3 dest, const vec3 u, const vec3 v);

void addv3s(vec3 dest, const vec3 u, float x);
void subv3s(vec3 dest, const vec3 u, float x);
void mulv3s(vec3 dest, const vec3 u, float x);
void divv3s(vec3 dest, const vec3 u, const float x);

float dotv3(const vec3 u, const vec3 v);
float lengthv3(const vec3 v);

void normalizev3(vec3 dest, const vec3 u);
void crossv3(vec3 dest, const vec3 u, const vec3 v);

#endif // VMATH_VEC3_H
