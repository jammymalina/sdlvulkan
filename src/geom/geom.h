#ifndef GEOM_H
#define GEOM_H

#ifdef GEOM_FLOAT_PRECISION
    typedef GEOM_FLOAT_PRECISION geom_float;
#else
    typedef float geom_float;
#endif

#include <stdbool.h>
#include <stddef.h>

#endif // GEOM_H
