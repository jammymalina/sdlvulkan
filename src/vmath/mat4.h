#ifndef VMATH_MAT4_H
#define VMATH_MAT4_H

#include "./mat.h"
#include "./vec4.h"

typedef float mat4[16];

#define get_mat4(dest, row, column)    get_mat(dest, row, column, 4, 4)
#define set_mat4(dest, row, column, x) set_mat(dest, row, column, x, 4, 4)

static inline void scalar_mat4(mat4 m, float x) {
    m[ 0] = x, m[ 1] = x, m[ 2] = x, m[ 3] = x,
    m[ 4] = x, m[ 5] = x, m[ 6] = x, m[ 7] = x,
    m[ 8] = x, m[ 9] = x, m[10] = x, m[11] = x,
    m[12] = x, m[13] = x, m[14] = x, m[15] = x;
}

static inline void diagonal_mat4(mat4 m, float x) {
    m[ 0] = x, m[ 1] = 0, m[ 2] = 0, m[ 3] = 0,
    m[ 4] = 0, m[ 5] = x, m[ 6] = 0, m[ 7] = 0,
    m[ 8] = 0, m[ 9] = 0, m[10] = x, m[11] = 0,
    m[12] = 0, m[13] = 0, m[14] = 0, m[15] = x;
}

static inline void identity_mat4(mat4 m) {
    diagonal_mat4(m, 1.0);
}

static inline void get_row_mat4(vec4 dest, const mat4 m, size_t row) {
    dest[0] = get_mat4(m, row, 0);
    dest[1] = get_mat4(m, row, 1);
    dest[2] = get_mat4(m, row, 2);
    dest[3] = get_mat4(m, row, 3);
}

static inline void get_column_mat4(vec4 dest, const mat4 m, size_t column) {
    dest[0] = get_mat4(m, 0, column);
    dest[1] = get_mat4(m, 1, column);
    dest[2] = get_mat4(m, 2, column);
    dest[3] = get_mat4(m, 3, column);
}

static inline void set_row_mat4(mat4 m, size_t row, const vec4 v) {
    set_mat4(m, row, 0, v[0]);
    set_mat4(m, row, 1, v[1]);
    set_mat4(m, row, 2, v[2]);
    set_mat4(m, row, 3, v[3]);
}

static inline void set_column_mat4(mat4 m, size_t column, const vec4 v) {
    set_mat4(m, 0, column, v[0]);
    set_mat4(m, 1, column, v[1]);
    set_mat4(m, 2, column, v[2]);
    set_mat4(m, 3, column, v[3]);
}

#endif // VMATH_MAT4_H
