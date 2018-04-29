#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_PATH_LENGTH 512

#if defined(WIN32) || defined(_WIN32)
    #define PATH_SEPARATOR '\\'
    #define is_path_separator(x) \
        ((x) == '\\' || (x) == '/')
#else
    #define PATH_SEPARATOR '/'
    #define is_path_separator(x) \
        ((x) == '/')
#endif

size_t get_file_size_bytes(FILE *file);
size_t read_binary_file(const char *filepath, void **data);
bool path_resolve(char dest[MAX_PATH_LENGTH], const char *directory, ...);
bool extract_extension(char dest[MAX_PATH_LENGTH], const char *filepath, int n);

extern char dirname[MAX_PATH_LENGTH];
bool set_dirname(const char *src);

#endif // FILE_UTILS_H
