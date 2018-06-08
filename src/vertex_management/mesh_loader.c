#include "./mesh_loader.h"

#include "../utils/heap.h"
#include "../geom/plane.h"
#include "../geom/circle.h"
#include "../geom/sphere.h"

mesh_loader_tool mesh_loader;

static void generate_plane_mesh_geometry(const mesh_geometry_config *conf, uint32_t *vertex_count, vertex *vertices,
    uint32_t *index_count, uint32_t *indices)
{
    generate_plane_geometry(conf->width, conf->height, conf->width_segments, conf->height_segments,
        conf->geom_config_flag_bits, vertex_count, vertices, index_count, indices);
}

static void generate_circle_mesh_geometry(const mesh_geometry_config *conf, uint32_t *vertex_count, vertex *vertices,
    uint32_t *index_count, uint32_t *indices)
{
    generate_circle_geometry(conf->radius, conf->theta_start, conf->theta_length, conf->segments,
        conf->geom_config_flag_bits, vertex_count, vertices, index_count, indices);
}

static void generate_sphere_mesh_geometry(const mesh_geometry_config *conf, uint32_t *vertex_count, vertex *vertices,
    uint32_t *index_count, uint32_t *indices)
{
    generate_sphere_geometry(conf->radius, conf->phi_start, conf->phi_length, conf->theta_start, conf->theta_length,
        conf->width_segments, conf->height_segments, conf->geom_config_flag_bits, vertex_count, vertices, index_count,
        indices);
}

static generate_mesh_geometry_function gen_geom_functions[GEOMETRY_TYPE_COUNT] = {
    [PLANE_GEOMETRY]  = &generate_plane_mesh_geometry,
    [CIRCLE_GEOMETRY] = &generate_circle_mesh_geometry,
    [SPHERE_GEOMETRY] = &generate_sphere_mesh_geometry
};

bool init_mesh_loader_tool(mesh_loader_tool *mesh_tool, uint32_t vertex_buffer_size, uint32_t index_buffer_size) {
    mesh_tool->vertex_buffer_size = 0;
    mesh_tool->index_buffer_size = 0;
    mesh_tool->vertex_buffer = NULL;
    mesh_tool->index_buffer = NULL;

    mesh_tool->vertex_buffer = mem_alloc(vertex_buffer_size * sizeof(vertex));
    mesh_tool->index_buffer = mem_alloc(index_buffer_size * sizeof(uint32_t));

    CHECK_ALLOC(mesh_tool->vertex_buffer, "Unable to create mesh tool, vertex buffer allocation problem");
    CHECK_ALLOC(mesh_tool->index_buffer, "Unable to create mesh tool, index buffer allocation problem");

    mesh_tool->vertex_buffer_size = vertex_buffer_size;
    mesh_tool->index_buffer_size = index_buffer_size;

    return true;
}

uint64_t load_mesh_geometry_mesh_loader_tool(mesh_loader_tool *mesh_tool, const mesh_geometry_config *conf) {
    generate_mesh_geometry_function f = gen_geom_functions[conf->type];
    uint32_t vertex_count = 0, index_count = 0;
    f(conf, &vertex_count, NULL, &index_count, NULL);

    if (vertex_count == 0 || index_count == 0) {
        return 0;
    }
    if (mesh_tool->vertex_buffer_size < vertex_count || mesh_tool->index_buffer_size < index_count) {
        return 0;
    }

    f(conf, &vertex_count, mesh_tool->vertex_buffer, &index_count, mesh_tool->index_buffer);

    uint64_t result = ((uint64_t) vertex_count) << UINT64_C(32);
    result |= index_count;

    return result;
}

void destroy_mesh_loader_tool(mesh_loader_tool *mesh_tool) {
    if (mesh_tool->vertex_buffer) {
        mem_free(mesh_tool->vertex_buffer);
        mesh_tool->vertex_buffer = NULL;
    }
    if (mesh_tool->index_buffer) {
        mem_free(mesh_tool->index_buffer);
        mesh_tool->index_buffer = NULL;
    }
}

bool init_mesh_loader(uint32_t vertex_buffer_size, uint32_t index_buffer_size) {
    return init_mesh_loader_tool(&mesh_loader, vertex_buffer_size, index_buffer_size);
}

uint64_t load_mesh_geometry_mesh_loader(const mesh_geometry_config *conf) {
    return load_mesh_geometry_mesh_loader_tool(&mesh_loader, conf);
}

void destroy_mesh_loader() {
    destroy_mesh_loader_tool(&mesh_loader);
}
