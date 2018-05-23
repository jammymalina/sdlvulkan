#ifndef VMATH_MAT4_H
#define VMATH_MAT4_H

#include "./mat.h"

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

#endif // VMATH_MAT4_H
