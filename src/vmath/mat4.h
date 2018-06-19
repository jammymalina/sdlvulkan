#ifndef VMATH_MAT4_H
#define VMATH_MAT4_H

#include "./mat.h"
#include "./vec4.h"
#include "./mat3.h"

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

static inline void mat3_to_mat4(mat4 dest, const mat3 m) {
    identity_mat4(dest);

    set_mat4(dest, 0, 0, get_mat3(m, 0, 0));
    set_mat4(dest, 0, 1, get_mat3(m, 0, 1));
    set_mat4(dest, 0, 2, get_mat3(m, 0, 2));

    set_mat4(dest, 1, 0, get_mat3(m, 1, 0));
    set_mat4(dest, 1, 1, get_mat3(m, 1, 1));
    set_mat4(dest, 1, 2, get_mat3(m, 1, 2));

    set_mat4(dest, 2, 0, get_mat3(m, 2, 0));
    set_mat4(dest, 2, 1, get_mat3(m, 2, 1));
    set_mat4(dest, 2, 2, get_mat3(m, 2, 2));
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

static inline void mulmat4(mat4 dest, mat4 a, mat4 b) {
    float a11 = get_mat4(a, 0, 0), a12 = get_mat4(a, 0, 1), a13 = get_mat4(a, 0, 2), a14 = get_mat4(a, 0, 3);
    float a21 = get_mat4(a, 1, 0), a22 = get_mat4(a, 1, 1), a23 = get_mat4(a, 1, 2), a24 = get_mat4(a, 1, 3);
    float a31 = get_mat4(a, 2, 0), a32 = get_mat4(a, 2, 1), a33 = get_mat4(a, 2, 2), a34 = get_mat4(a, 2, 3);
    float a41 = get_mat4(a, 3, 0), a42 = get_mat4(a, 3, 1), a43 = get_mat4(a, 3, 2), a44 = get_mat4(a, 3, 3);

    float b11 = get_mat4(b, 0, 0), b12 = get_mat4(b, 0, 1), b13 = get_mat4(b, 0, 2), b14 = get_mat4(b, 0, 3);
    float b21 = get_mat4(b, 1, 0), b22 = get_mat4(b, 1, 1), b23 = get_mat4(b, 1, 2), b24 = get_mat4(b, 1, 3);
    float b31 = get_mat4(b, 2, 0), b32 = get_mat4(b, 2, 1), b33 = get_mat4(b, 2, 2), b34 = get_mat4(b, 2, 3);
    float b41 = get_mat4(b, 3, 0), b42 = get_mat4(b, 3, 1), b43 = get_mat4(b, 3, 2), b44 = get_mat4(b, 3, 3);

    set_mat4(dest, 0, 0, a11 * b11 + a12 * b21 + a13 * b31 + a14 * b41);
	set_mat4(dest, 0, 1, a11 * b12 + a12 * b22 + a13 * b32 + a14 * b42);
    set_mat4(dest, 0, 2, a11 * b13 + a12 * b23 + a13 * b33 + a14 * b43);
	set_mat4(dest, 0, 3, a11 * b14 + a12 * b24 + a13 * b34 + a14 * b44);

	set_mat4(dest, 1, 0, a21 * b11 + a22 * b21 + a23 * b31 + a24 * b41);
	set_mat4(dest, 1, 1, a21 * b12 + a22 * b22 + a23 * b32 + a24 * b42);
	set_mat4(dest, 1, 2, a21 * b13 + a22 * b23 + a23 * b33 + a24 * b43);
	set_mat4(dest, 1, 3, a21 * b14 + a22 * b24 + a23 * b34 + a24 * b44);

	set_mat4(dest, 2, 0, a31 * b11 + a32 * b21 + a33 * b31 + a34 * b41);
	set_mat4(dest, 2, 1, a31 * b12 + a32 * b22 + a33 * b32 + a34 * b42);
	set_mat4(dest, 2, 2, a31 * b13 + a32 * b23 + a33 * b33 + a34 * b43);
	set_mat4(dest, 2, 3, a31 * b14 + a32 * b24 + a33 * b34 + a34 * b44);

	set_mat4(dest, 3, 0, a41 * b11 + a42 * b21 + a43 * b31 + a44 * b41);
	set_mat4(dest, 3, 1, a41 * b12 + a42 * b22 + a43 * b32 + a44 * b42);
	set_mat4(dest, 3, 2, a41 * b13 + a42 * b23 + a43 * b33 + a44 * b43);
	set_mat4(dest, 3, 3, a41 * b14 + a42 * b24 + a43 * b34 + a44 * b44);
}

static inline float determinant_mat4(const mat4 m) {
    float m11 = get_mat4(m, 0, 0), m12 = get_mat4(m, 0, 1), m13 = get_mat4(m, 0, 2), m14 = get_mat4(m, 0, 3);
    float m21 = get_mat4(m, 1, 0), m22 = get_mat4(m, 1, 1), m23 = get_mat4(m, 1, 2), m24 = get_mat4(m, 1, 3);
    float m31 = get_mat4(m, 2, 0), m32 = get_mat4(m, 2, 1), m33 = get_mat4(m, 2, 2), m34 = get_mat4(m, 2, 3);
    float m41 = get_mat4(m, 3, 0), m42 = get_mat4(m, 3, 1), m43 = get_mat4(m, 3, 2), m44 = get_mat4(m, 3, 3);

    return m41 * (
            + m14 * m23 * m32
            - m13 * m24 * m32
            - m14 * m22 * m33
            + m12 * m24 * m33
            + m13 * m22 * m34
            - m12 * m23 * m34
        ) + m42 * (
            + m11 * m23 * m34
            - m11 * m24 * m33
            + m14 * m21 * m33
            - m13 * m21 * m34
            + m13 * m24 * m31
            - m14 * m23 * m31
        ) + m43 * (
            + m11 * m24 * m32
            - m11 * m22 * m34
            - m14 * m21 * m32
            + m12 * m21 * m34
            + m14 * m22 * m31
            - m12 * m24 * m31
        ) + m44 * (
            - m13 * m22 * m31
            - m11 * m23 * m32
            + m11 * m22 * m33
            + m13 * m21 * m32
            - m12 * m21 * m33
            + m12 * m23 * m31
        );
}

#endif // VMATH_MAT4_H
