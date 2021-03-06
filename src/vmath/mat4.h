#ifndef VMATH_MAT4_H
#define VMATH_MAT4_H

#include <stdbool.h>
#include "./mat.h"
#include "./vec4.h"
#include "./vec3.h"
#include "./mat3.h"
#include "./quat.h"

typedef float mat4[16];

#define MAT4_COPY(m) {          \
    m[0],  m[1],  m[2],  m[3],  \
    m[4],  m[5],  m[6],  m[7],  \
    m[8],  m[9],  m[10], m[11], \
    m[12], m[13], m[14], m[15]  \
}

#define get_mat4(dest, row, column)    get_mat(dest, row, column, 4, 4)
#define set_mat4(dest, row, column, x) set_mat(dest, row, column, x, 4, 4)

void scalar_mat4(mat4 m, float x);
void diagonal_mat4(mat4 m, float x);
void identity_mat4(mat4 m);

void mat3_to_mat4(mat4 dest, const mat3 m);
void get_row_mat4(vec4 dest, const mat4 m, size_t row);
void get_column_mat4(vec4 dest, const mat4 m, size_t column);
void set_row_mat4(mat4 m, size_t row, const vec4 v);
void set_column_mat4(mat4 m, size_t column, const vec4 v);

void mulmat4(mat4 dest, mat4 a, mat4 b);

void transpose_mat4(mat4 dest, const mat4 m);
bool inverse_mat4(mat4 dest, const mat4 m);
void compose_mat4(mat4 dest, const vec3 position, const quat quaternion, const vec3 scale);
void decompose_mat4(const mat4 m, vec3 position, quat quaternion, vec3 scale);
void rotation_matrix_to_quat(quat dest, const mat4 m);

float determinant_mat4(const mat4 m);

#define perspective(dest, fov, aspect, z_near, z_far) perspective_RH_ZO(dest, fov, aspect, z_near, z_far)
#define look_at(dest, eye, center, up) look_at_RH(dest, eye, center, up)

void ortho(mat4 dest, float left, float right, float bottom, float top);
void perspective_RH_ZO(mat4 dest, float fov, float aspect, float z_near, float z_far);
void look_at_RH(mat4 dest, const vec3 eye, const vec3 center, const vec3 up);

#endif // VMATH_MAT4_H
