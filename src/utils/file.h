#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stdio.h>
#include <stddef.h>

size_t get_file_size_bytes(FILE *file);
size_t read_binary_file(const char *filepath, void **data);

#endif // FILE_UTILS_H
