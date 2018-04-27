#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_PATH_LENGTH 512

size_t get_file_size_bytes(FILE *file);
size_t read_binary_file(const char *filepath, void **data);

extern char dirname[MAX_PATH_LENGTH];
bool set_dirname(const char *src);


#endif // FILE_UTILS_H
