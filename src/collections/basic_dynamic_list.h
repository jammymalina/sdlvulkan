#ifndef BASIC_DYNAMIC_LIST_H
#define BASIC_DYNAMIC_LIST_H

#include <stddef.h>
#include <stdbool.h>

#define GENERATE_BASIC_DYNAMIC_LIST_HEADER(name, function_postfix, type)                  \
    typedef struct {                                                                      \
        type *elements;                                                                   \
        size_t size;                                                                      \
        size_t max_size;                                                                  \
    } name;                                                                               \
                                                                                          \
    bool init_##function_postfix(name *c, size_t max_size);                               \
    bool add_##function_postfix(name *c, type element);                                   \
    bool add_all_##function_postfix(name *dest, const name *c);                           \
    bool get_##function_postfix(name *c, size_t index, type *value);                      \
    bool set_##function_postfix(name *c, size_t index, type element);                     \
    void clear_##function_postfix(name *c);                                               \
    bool remove_##function_postfix(name *c, size_t index);

#define GENERATE_BASIC_DYNAMIC_LIST_SOURCE(name, function_postfix, type)                  \
    bool init_##function_postfix(name *c, size_t max_size) {                              \
        c->elements = mem_alloc(sizeof(type) * max_size);                                 \
        if (!c->elements) {                                                               \
            c->max_size = 0;                                                              \
            c->size = 0;                                                                  \
            return false;                                                                 \
        }                                                                                 \
        c->max_size = max_size;                                                           \
        c->size = 0;                                                                      \
        return true;                                                                      \
    }                                                                                     \
                                                                                          \
    bool add_##function_postfix(name *c, type element) {                                  \
        if (c->size >= c->max_size)                                                       \
            return false;                                                                 \
        c->elements[c->size] = element;                                                   \
        c->size++;                                                                        \
        return true;                                                                      \
    }                                                                                     \
                                                                                          \
    bool add_all_##function_postfix(name *dest, const name *c) {                          \
        if (dest->size + c->size > c->max_size)                                           \
            return false;                                                                 \
        for (size_t i = 0; i < c->size; i++) {                                            \
            dest->elements[dest->size + i] = c->elements[i];                              \
            dest->size++;                                                                 \
        }                                                                                 \
        return true;                                                                      \
    }                                                                                     \
                                                                                          \
    bool get_##function_postfix(name *c, size_t index, type *value) {                     \
        if (index >= c->size)                                                             \
            return false;                                                                 \
        *value = c->elements[index];                                                      \
        return true;                                                                      \
    }                                                                                     \
                                                                                          \
    bool remove_##function_postfix(name *c, size_t index) {                               \
        if (index >= c->size)                                                             \
            return false;                                                                 \
        for (size_t i = index; i < c->size - 1; i++) {                                    \
            c->elements[i] = c->elements[i + 1];                                          \
        }                                                                                 \
        c->size--;                                                                        \
        return true;                                                                      \
    }                                                                                     \
                                                                                          \
    bool remove_element_##function_postfix(name *c, type element, compare_function f) {   \
        size_t index = c->max_size;                                                       \
        for (size_t i = 0; i < c->size; i++) {                                            \
            if (f(&c->elements[i], &element) == 0) {                                      \
                index = i;                                                                \
                break;                                                                    \
            }                                                                             \
        }                                                                                 \
        if (index == c->max_size) {                                                       \
            return false;                                                                 \
        }                                                                                 \
        return remove_##function_postfix(c, index);                                       \
    }                                                                                     \
                                                                                          \
    bool set_##function_postfix(name *c, size_t index, type element) {                    \
        if (index >= c->size)                                                             \
            return false;                                                                 \
        c->elements[index] = element;                                                     \
        return true;                                                                      \
    }                                                                                     \
                                                                                          \
    void clear_##function_postfix(name *c) {                                              \
        c->size = 0;                                                                      \
    }                                                                                     \
                                                                                          \
    void destroy_##function_postfix(name *c) {                                            \
        if (c->max_size == 0)                                                             \
            return;                                                                       \
        c->size = 0;                                                                      \
        c->max_size = 0;                                                                  \
        mem_free(c->elements);                                                            \
    }


#endif // BASIC_DYNAMIC_LIST_H
