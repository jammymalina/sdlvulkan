#ifndef VMATH_MAT_H
#define VMATH_MAT_H

#include <stddef.h>

#if !defined(VMATH_MATRIX_COLUMN_MAJOR_ORDER) && !defined(VMATH_MATRIX_ROW_MAJOR_ORDER)
    #define VMATH_MATRIX_COLUMN_MAJOR_ORDER
#endif

#ifdef VMATH_MATRIX_COLUMN_MAJOR_ORDER
    #define get_mat(dest, row, column, height, width) (dest[(column) * (height) + (row)])
    #define set_mat(dest, row, column, value, height, width)                                    \
        do {                                                                                    \
            dest[(column) * (height) + (row)] = value;                                          \
        } while(0)
#else
    #define get_mat(dest, row, column, value, height, width) (dest[(row) * (width) + (column)])
    #define set_mat(dest, row, column, value, height, width)                                    \
        do {                                                                                    \
            dest[(row) * (width) + (column)] = value;                                           \
        } while(0)
#endif

#endif // VMATH_MAT_H
