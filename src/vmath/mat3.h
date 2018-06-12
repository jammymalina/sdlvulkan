#ifndef VMATH_MAT3_H
#define VMATH_MAT3_H

#include "./mat.h"
#include "./vec3.h"

typedef float mat3[9];

#define get_mat3(dest, row, column)    get_mat(dest, row, column, 3, 3)
#define set_mat3(dest, row, column, x) set_mat(dest, row, column, x, 3, 3)

static inline void scalar_mat3(mat3 m, float x) {
    m[0] = x, m[1] = x, m[2] = x;
    m[3] = x, m[4] = x, m[5] = x;
    m[6] = x, m[7] = x, m[8] = x;
}

static inline void diagonal_mat3(mat3 m, float x) {
    m[0] = x, m[1] = 0, m[2] = 0;
    m[3] = 0, m[4] = x, m[5] = 0;
    m[6] = 0, m[7] = 0, m[8] = x;
}

static inline void identity_mat3(mat3 m) {
    diagonal_mat3(m, 1.0);
}

static inline void get_row_mat3(vec3 dest, const mat3 m, size_t row) {
    dest[0] = get_mat3(m, row, 0);
    dest[1] = get_mat3(m, row, 1);
    dest[2] = get_mat3(m, row, 2);
    dest[3] = get_mat3(m, row, 3);
}

static inline void get_column_mat3(vec3 dest, const mat3 m, size_t column) {
    dest[0] = get_mat3(m, 0, column);
    dest[1] = get_mat3(m, 1, column);
    dest[2] = get_mat3(m, 2, column);
}

static inline void set_row_mat3(mat3 m, size_t row, const vec3 v) {
    set_mat3(m, row, 0, v[0]);
    set_mat3(m, row, 1, v[1]);
    set_mat3(m, row, 2, v[2]);
}

static inline void set_column_mat3(mat3 m, size_t column, const vec3 v) {
    set_mat3(m, 0, column, v[0]);
    set_mat3(m, 1, column, v[1]);
    set_mat3(m, 2, column, v[2]);
}

#endif // VMATH_MAT3_H
