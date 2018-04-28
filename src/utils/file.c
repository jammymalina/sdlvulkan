#include "./file.h"

#include "../logger/logger.h"
#include "../string/string.h"
#include "./heap.h"

char dirname[MAX_PATH_LENGTH];

size_t get_file_size_bytes(FILE *file) {
    if (!file) {
        return 0;
    }

    size_t file_size = 0;
    size_t current_pos = ftell(file);

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, current_pos);

    return file_size;
}

size_t read_binary_file(const char *filepath, void **data) {
    FILE *file = fopen(filepath, "rb");

    if (!file) {
        log_error("Unable to open file: %s", filepath);
        return 0;
    }

    size_t file_size = get_file_size_bytes(file);
    if (file_size == 0) {
        log_error("The file seems to be empty: %s", filepath);
        fclose(file);
        return 0;
    }

    *data = mem_alloc(file_size);
    if (*data == NULL) {
        log_error("Allocation error during reading the file: %s", filepath);
        fclose(file);
        return 0;
    }

    fseek(file, 0, SEEK_SET);
    fread(*data, file_size, 1, file);
    fclose(file);

    return file_size;
}

bool path_resolve(char dest[MAX_PATH_LENGTH], const char *directory, const char *filepath) {
    char directory_separator[4];
    if (is_empty_string(directory)) {
        directory_separator[0] = '.';
        directory_separator[1] = PATH_SEPARATOR;
        directory_separator[2] = '\0';
    } else {
        directory_separator[0] = PATH_SEPARATOR;
        directory_separator[1] = '\0';
    }
    bool success = string_copy(dest, MAX_PATH_LENGTH, directory) &&
        string_append(dest, MAX_PATH_LENGTH, directory_separator) &&
        string_append(dest, MAX_PATH_LENGTH, filepath);
    if (!success) {
        log_error("Unable to resolve path, path is too long");
        string_copy(dest, MAX_PATH_LENGTH, "");
        return false;
    }

    return true;
}

bool extract_extension(char dest[MAX_PATH_LENGTH], const char *filepath, int n) {
    string_copy(dest, MAX_PATH_LENGTH, "");
    size_t num_extensions = 0;
    size_t i = 0;
    for (i = 0; filepath[i] != '\0'; i++) {
        if (filepath[i] == '.') {
            num_extensions++;
        }
    }
    if (num_extensions < n) {
        log_error("The filepath %s has only %zu extensions", filepath, num_extensions);
        return false;
    }

    size_t filepath_len = i;
    size_t j = 0;
    int original_n = n;
    for (i = filepath_len; i >= 0 && n >= 0; i--) {
        if (filepath[i] == '.') {
            n--;
            continue;
        }
        if (n == 0) {
            if (j < MAX_PATH_LENGTH - 1) {
                dest[j] = filepath[i];
                j++;
            } else {
                log_error("Extension no. %d in path %s is too long", original_n, filepath);
                string_copy(dest, MAX_PATH_LENGTH, "");
                return false;
            }
        }
    }
    string_reverse(dest, 0, j - 1);
    dest[j] = '\0';

    return true;
}

bool set_dirname(const char *src) {
    const size_t NO_PATH_SEPARATOR = 10 * MAX_PATH_LENGTH;
    size_t last_file_separator = NO_PATH_SEPARATOR;
    for (size_t i = 0; i < MAX_PATH_LENGTH - 1 && src[i] != '\0'; i++) {
        if (is_path_separator(src[i])) {
            last_file_separator = i;
        }
    }

    if (last_file_separator == NO_PATH_SEPARATOR) {
        return string_copy(dirname, MAX_PATH_LENGTH, "");
    }
    if (last_file_separator >= MAX_PATH_LENGTH - 1) {
        log_error("Current directory path is too long");
        return false;
    }

    size_t i = 0;
    for (i = 0; i < last_file_separator; i++) {
        dirname[i] = src[i];
    }
    dirname[i] = '\0';
    return true;
}


