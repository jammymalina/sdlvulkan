#ifndef BASIC_DYNAMIC_LIST_H
#define BASIC_DYNAMIC_LIST_H

#include <stddef.h>
#include <stdbool.h>

#define GENERATE_BASIC_DYNAMIC_LIST_HEADER(name, function_prefix, type)                    \
    typedef struct {                                                                       \
        type *elements;                                                                    \
        size_t size;                                                                       \
		size_t max_size;                                                                   \
    } name;                                                                                \
                                                                                           \
    bool function_prefix##_add(name *c, type element);                                     \
    bool function_prefix##_add_all(name *dest, const name *c);                             \
    bool function_prefix##_get(name *c, size_t index, type *value);                        \
    bool function_prefix##_set(name *c, size_t index, type element);                       \
    void function_prefix##_clear(name *c);                                                 \
    bool function_prefix##_remove(name *c, size_t index);

#define GENERATE_BASIC_DYNAMIC_LIST_SOURCE(name, function_prefix, type)                    \
	bool function_prefix##_init(name *c, size_t max_size) {                                \
		c->elements = mem_alloc(sizeof(type) * max_size);                                  \
		if (!c->elements) {                                                                \
			c->max_size = 0;                                                               \
			c->size = 0;                                                                   \
			return false;                                                                  \
		}                                                                                  \
		c->max_size = max_size;                                                            \
		c->size = 0;                                                                       \
		return true;                                                                       \
	}                                                                                      \
		                                                                                   \
    bool function_prefix##_add(name *c, type element) {                                    \
        if (c->size >= c->max_size)                                                        \
            return false;                                                                  \
        c->elements[c->size] = element;                                                    \
        c->size++;                                                                         \
        return true;                                                                       \
    }                                                                                      \
                                                                                           \
    bool function_prefix##_add_all(name *dest, const name *c) {                            \
        if (dest->size + c->size > c->max_size)                                            \
            return false;                                                                  \
        for (size_t i = 0; i < c->size; i++) {                                             \
            dest->elements[dest->size + i] = c->elements[i];                               \
            dest->size++;                                                                  \
        }                                                                                  \
        return true;                                                                       \
    }                                                                                      \
                                                                                           \
    bool function_prefix##_get(name *c, size_t index, type *value) {                       \
        if (index >= c->size)                                                          \
            return false;                                                                  \
        *value = c->elements[index];                                                       \
        return true;                                                                       \
    }                                                                                      \
                                                                                           \
    bool function_prefix##_remove(name *c, size_t index) {                                 \
        if (index >= c->size)                                                          \
            return false;                                                                  \
        for (size_t i = index; i < c->size - 1; i++) {                                     \
            c->elements[i] = c->elements[i + 1];                                           \
        }                                                                                  \
        c->size--;                                                                         \
        return true;                                                                       \
    }                                                                                      \
                                                                                           \
    bool function_prefix##_set(name *c, size_t index, type element) {                      \
        if (index >= c->size)                                                          \
            return false;                                                                  \
        c->elements[index] = element;                                                      \
        return true;                                                                       \
    }                                                                                      \
                                                                                           \
    void function_prefix##_clear(name *c) {                                                \
        c->size = 0;                                                                       \
    }


#endif // BASIC_DYNAMIC_LIST_H
