#ifndef LIB_UTILS_H
#define LIB_UTILS_H

#include <stdint.h>

#define BIT(n) (UINT32_C(1) << (n))
 
#define min(x, y) ((x) < (y) ? : (x) : (y))
#define max(x, y) ((x) > (y) ? : (x) : (y))

#endif // LIB_UTILS_H
