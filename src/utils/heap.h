#ifndef HEAP_UTILS_H
#define HEAP_UTILS_H

#include <stddef.h>
#include "../logger/logger.h"

#define CHECK_ALLOC(x, s)                          \
    do {                                           \
        if ((x) == NULL) {                         \
            SDL_OutOfMemory();                     \
            log_error("%s: %s", s, #x " == NULL"); \
            return false;                          \
        }                                          \
    } while (0)

#define ALIGN(x, a) (((x) + ((a) - 1) ) & ~((a) - 1))

void *mem_alloc(size_t size);
void mem_free(void *data);

#endif

