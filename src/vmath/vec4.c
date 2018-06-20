#include "./vec4.h"

void scalar_vec4(vec4 v, float x) {
    v[0] = x, v[1] = x, v[2] = x, v[3] = x;
}

void addv4(vec4 dest, const vec4 u, const vec4 v) {
    dest[0] = u[0] + v[0];
    dest[1] = u[1] + v[1];
    dest[2] = u[2] + v[2];
    dest[3] = u[3] + v[3];
}

void subv4(vec4 dest, const vec4 u, const vec4 v) {
    dest[0] = u[0] - v[0];
    dest[1] = u[1] - v[1];
    dest[2] = u[2] - v[2];
    dest[3] = u[3] - v[3];
}

void mulv4(vec4 dest, const vec4 u, const vec4 v) {
    dest[0] = u[0] * v[0];
    dest[1] = u[1] * v[1];
    dest[2] = u[2] * v[2];
    dest[3] = u[3] * v[3];
}

void divv4(vec4 dest, const vec4 u, const vec4 v) {
    dest[0] = u[0] / v[0];
    dest[1] = u[1] / v[1];
    dest[2] = u[2] / v[2];
    dest[3] = u[3] / v[3];
}

void addv4s(vec4 dest, const vec4 u, float x) {
    dest[0] = u[0] + x;
    dest[1] = u[1] + x;
    dest[2] = u[2] + x;
    dest[3] = u[3] + x;
}

void subv4s(vec4 dest, const vec4 u, float x) {
    dest[0] = u[0] - x;
    dest[1] = u[1] - x;
    dest[2] = u[2] - x;
    dest[3] = u[3] - x;
}

void mulv4s(vec4 dest, const vec4 u, float x) {
    dest[0] = u[0] * x;
    dest[1] = u[1] * x;
    dest[2] = u[2] * x;
    dest[3] = u[3] * x;
}

void divv4s(vec4 dest, const vec4 u, float x) {
    dest[0] = u[0] / x;
    dest[1] = u[1] / x;
    dest[2] = u[2] / x;
    dest[3] = u[3] / x;
}

float dotv4(const vec4 u, const vec4 v) {
    return u[0] * v[0] + u[1] * v[1] + u[2] * v[2] + u[3] * v[3];
}

float lengthv4(const vec4 v) {
    return sqrt(dotv4(v, v));
}

void normalizev4(vec4 dest, const vec4 u) {
    const float len = lengthv4(u);
    if (len <= EPSILON) {
        dest[0] = 0, dest[1] = 0, dest[2] = 0, dest[3] = 0;
        return;
    }
    divv4s(dest, u, len);
}
