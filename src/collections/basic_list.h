#ifndef BASIC_LIST_H
#define BASIC_LIST_H

#include <stddef.h>

#define GENERATE_BASIC_LIST_HEADER(name, function_prefix, type, max_size)           \
    typedef struct {                                                                \
        type elements[max_size];                                                    \
        size_t size;                                                                \
    } name;                                                                         \
                                                                                    \
    bool function_prefix##_add(name *c, type element);                              \
    bool function_prefix##_add_all(name *dest, const name *c);                      \
    bool function_prefix##_get(name *c, size_t index, type *value);                 \
    bool function_prefix##_set(name *c, size_t index, type element);                \
    void function_prefix##_clear(name *c);                                          \
    bool function_prefix##_remove(name *c, size_t index);

#define GENERATE_BASIC_LIST_SOURCE(name, function_prefix, type, max_size)           \
    bool function_prefix##_add(name *c, type element) {                             \
        if (c->size >= max_size)                                                    \
            return false;                                                           \
        c->elements[c->size] = element;                                             \
        c->size++;                                                                  \
        return true;                                                                \
    }                                                                               \
                                                                                    \
    bool function_prefix##_add_all(name *dest, const name *c) {                     \
        if (dest->size + c->size > max_size)                                        \
            return false;                                                           \
        for (size_t i = 0; i < c->size; i++) {                                      \
            dest->elements[dest->size + i] = c->elements[i];                        \
            dest->size++;                                                           \
        }                                                                           \
        return true;                                                                \
    }                                                                               \
                                                                                    \
    bool function_prefix##_get(name *c, size_t index, type *value) {                \
        if (index >= max_size)                                                      \
            return false;                                                           \
        *value = c->elements[index];                                                \
        return true;                                                                \
    }                                                                               \
                                                                                    \
    bool function_prefix##_remove(name *c, size_t index) {                          \
        if (index >= max_size)                                                      \
            return false;                                                           \
        for (size_t i = index; i < c->size - 1; i++) {                              \
            c->elements[i] = c->elements[i + 1];                                    \
        }                                                                           \
        c->size--;                                                                  \
        return true;                                                                \
    }                                                                               \
                                                                                    \
    bool function_prefix##_set(name *c, size_t index, type element) {               \
        if (index >= max_size)                                                      \
            return false;                                                           \
        c->elements[index] = element;                                               \
        return true;                                                                \
    }                                                                               \
                                                                                    \
    void function_prefix##_clear(name *c) {                                         \
        c->size = 0;                                                                \
    }


#endif // BASIC_LIST_H
