#ifndef VERTEX_MANAGEMENT_CONFIG_H
#define VERTEX_MANAGEMENT_CONFIG_H

#include <stdint.h>

typedef struct vertex_management_configuration {
    struct {
        uint32_t max_vertex_buffer_size;
        uint32_t max_index_buffer_size;
    } mesh_loader_config;
    uint32_t static_buffer_size;
} vertex_management_configuration;

extern vertex_management_configuration vertex_management_config;

#endif // VERTEX_MANAGEMENT_CONFIG_H
