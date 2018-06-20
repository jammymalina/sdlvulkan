#ifndef VMATH_MAT3_H
#define VMATH_MAT3_H

#include "./mat.h"
#include "./vec3.h"

typedef float mat3[9];

#define get_mat3(dest, row, column)    get_mat(dest, row, column, 3, 3)
#define set_mat3(dest, row, column, x) set_mat(dest, row, column, x, 3, 3)

void scalar_mat3(mat3 m, float x);
void diagonal_mat3(mat3 m, float x);
void identity_mat3(mat3 m);

void get_row_mat3(vec3 dest, const mat3 m, size_t row);
void get_column_mat3(vec3 dest, const mat3 m, size_t column);
void set_row_mat3(mat3 m, size_t row, const vec3 v);
void set_column_mat3(mat3 m, size_t column, const vec3 v);

#endif // VMATH_MAT3_H
