#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stdbool.h>
#include <stddef.h>

bool string_copy(char *dest, size_t max_dest_length, const char *src);
bool string_compare(const char *str1, const char *str2);
void string_reverse(char *str, size_t start_index, size_t end_index);
bool string_add_number_postfix(char *dest, size_t max_dest_length, const char *str, int num, int base);

#endif // STRING_UTILS_H
