#ifndef MESH_LOADER_H
#define MESH_LOADER_H

#include <stdint.h>
#include <stdbool.h>
#include "./config.h"
#include "./vertex_manager.h"
#include "../geom/geom.h"

#define MESH_VERTEX_COUNT(x) ((x) >> UINT64_C(32))
#define MESH_INDEX_COUNT(x)  ((x) & ((1 << 32) - 1))

typedef enum mesh_geometry_type {
    PLANE_GEOMETRY,
    CIRCLE_GEOMETRY,
    SPHERE_GEOMETRY,
    GEOMETRY_TYPE_COUNT
} mesh_geometry_type;

typedef struct mesh_geometry_config {
    mesh_geometry_type type;
    uint32_t geom_config_flag_bits;
    union {
        float width;
        float radius;
    };
    union {
        float height;
        float phi_length;
    };
    union {
        float depth;
        float theta_length;
    };
    float phi_start;
    float theta_start;
    union {
        uint32_t width_segments;
        uint32_t segments;
    };
    uint32_t height_segments;
    uint32_t depth_segments;
} mesh_geometry_config;

typedef struct mesh_loader_tool {
    vertex *vertex_buffer;
    uint32_t vertex_buffer_size;

    uint32_t *index_buffer;
    uint32_t index_buffer_size;
} mesh_loader_tool;

typedef void (*generate_mesh_geometry_function)(const mesh_geometry_config*, uint32_t*, vertex*, uint32_t*, uint32_t*);

bool init_mesh_loader_tool(mesh_loader_tool *mesh_tool, uint32_t vertex_buffer_size, uint32_t index_buffer_size);
uint64_t load_mesh_geometry_mesh_loader_tool(mesh_loader_tool *mesh_tool, const mesh_geometry_config *conf);
void destroy_mesh_loader_tool(mesh_loader_tool *mesh_tool);

extern mesh_loader_tool mesh_loader;

bool init_mesh_loader(uint32_t vertex_buffer_size, uint32_t index_buffer_size);
uint64_t load_mesh_geometry_mesh_loader(const mesh_geometry_config *conf);
void destroy_mesh_loader();

#endif // MESH_LOADER_H
