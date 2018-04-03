#include "./string.h"

#include <stddef.h>

bool string_compare(const char *str1, const char *str2) {
    size_t i;
    for (i = 0; str1[i] == str2[i] && str1[i] != '\0' && str2[i] != '\0'; i++);
    return str1[i] == str2[i];
}