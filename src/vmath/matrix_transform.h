#ifndef VKMATH_MATRIX_TRANSFORM_H
#define VKMATH_MATRIX_TRANSFORM_H

#include "./mat4.h"
#include "./vec3.h"

void translate_mat4(mat4 dest, const vec3 v);
void rotate_mat4(mat4 dest, float angle, const vec3 v);
void scale_mat4(mat4 dest, const vec3 v);

#define perspective(dest, fov, aspect, z_near, z_far) perspective_RH_ZO(dest, fov, aspect, z_near, z_far)
#define look_at(dest, eye, center, up) look_at_RH(dest, eye, center, up)

void ortho(mat4 dest, float left, float right, float bottom, float top);
void perspective_RH_ZO(mat4 dest, float fov, float aspect, float z_near, float z_far);
void look_at_RH(mat4 dest, const vec3 eye, const vec3 center, const vec3 up);

#endif // VKMATH_MATRIX_TRANSFORM_H
