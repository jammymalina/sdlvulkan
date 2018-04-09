#ifndef BASIC_LIST_H
#define BASIC_LIST_H

#include <stddef.h>
#include <stdbool.h>

#define GENERATE_BASIC_LIST_HEADER(name, function_postfix, type, max_size)            \
    typedef struct {                                                                  \
        type elements[max_size];                                                      \
        size_t size;                                                                  \
    } name;                                                                           \
                                                                                      \
    bool add_##function_postfix(name *c, type element);                               \
    bool add_all_##function_postfix(name *dest, const name *c);                       \
    bool get_##function_postfix(name *c, size_t index, type *value);                  \
    bool set_##function_postfix(name *c, size_t index, type element);                 \
    void clear_##function_postfix(name *c);                                           \
    bool remove_##function_postfix(name *c, size_t index);

#define GENERATE_BASIC_LIST_SOURCE(name, function_postfix, type, max_size)            \
    bool add_##function_postfix(name *c, type element) {                              \
        if (c->size >= max_size)                                                      \
            return false;                                                             \
        c->elements[c->size] = element;                                               \
        c->size++;                                                                    \
        return true;                                                                  \
    }                                                                                 \
                                                                                      \
    bool add_all_##function_postfix(name *dest, const name *c) {                      \
        if (dest->size + c->size > max_size)                                          \
            return false;                                                             \
        for (size_t i = 0; i < c->size; i++) {                                        \
            dest->elements[dest->size + i] = c->elements[i];                          \
            dest->size++;                                                             \
        }                                                                             \
        return true;                                                                  \
    }                                                                                 \
                                                                                      \
    bool get_##function_postfix(name *c, size_t index, type *value) {                 \
        if (index >= size)                                                            \
            return false;                                                             \
        *value = c->elements[index];                                                  \
        return true;                                                                  \
    }                                                                                 \
                                                                                      \
    bool remove_##function_postfix(name *c, size_t index) {                           \
        if (index >= size)                                                            \
            return false;                                                             \
        for (size_t i = index; i < c->size - 1; i++) {                                \
            c->elements[i] = c->elements[i + 1];                                      \
        }                                                                             \
        c->size--;                                                                    \
        return true;                                                                  \
    }                                                                                 \
                                                                                      \
    bool set_##function_postfix(name *c, size_t index, type element) {                \
        if (index >= size)                                                            \
            return false;                                                             \
        c->elements[index] = element;                                                 \
        return true;                                                                  \
    }                                                                                 \
                                                                                      \
    void clear_##function_postfix(name *c) {                                          \
        c->size = 0;                                                                  \
    }


#endif // BASIC_LIST_H
