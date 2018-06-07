#include "./mesh_loader.h"

#include "../utils/heap.h"

mesh_loader_tool mesh_loader;

bool init_mesh_loader_tool(mesh_loader_tool *mesh_tool, uint32_t vertex_buffer_size, uint32_t index_buffer_size) {
    mesh_tool->vertex_buffer_size = 0;
    mesh_tool->index_buffer_size = 0;
    mesh_tool->vertex_buffer = NULL;
    mesh_tool->index_buffer = NULL;

    mesh_tool->vertex_buffer = mem_alloc(vertex_buffer_size);
    mesh_tool->index_buffer = mem_alloc(index_buffer_size);

    CHECK_ALLOC(mesh_tool->vertex_buffer, "Unable to create mesh tool, vertex buffer allocation problem");
    CHECK_ALLOC(mesh_tool->index_buffer, "Unable to create mesh tool, index buffer allocation problem");

    mesh_tool->vertex_buffer_size = vertex_buffer_size;
    mesh_tool->index_buffer_size = index_buffer_size;

    return true;
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

void destroy_mesh_loader() {
    destroy_mesh_loader_tool(&mesh_loader);
}
