#include "./file.h"

#include "../logger/logger.h"
#include "../string/string.h"
#include "./heap.h"

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

bool set_dirname(const char *src) {
    size_t last_file_separator = MAX_PATH_LENGTH + 1;
    for (size_t i = 0; i < MAX_PATH_LENGTH - 1 && src[i] != '\0'; i++) {

    }
    if (last_file_separator == MAX_PATH_LENGTH + 1) {
        return string_copy(dirname, MAX_PATH_LENGTH, "");
    }
    return true;
}


