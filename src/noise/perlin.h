#ifndef PERLIN_NOISE_H
#define PERLIN_NOISE_H

#include <stdint.h>

#define PERLIN_PERMUTATION_TABLE_LENGTH 256

typedef struct perlin_gradient {
    double x, y, z;
} perlin_gradient;

void perlin_seed(double seed, uint16_t perm[2 * PERLIN_PERMUTATION_TABLE_LENGTH],
    perlin_gradient grad_p[2 * PERLIN_PERMUTATION_TABLE_LENGTH]);

double simplex_noise_2d(double xin, double yin);
double simplex_noise_3d(double xin, double yin, double zin);

#endif
