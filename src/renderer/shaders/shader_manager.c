#include "./shader_manager.h"

#include "../../logger/logger.h"
#include "../../string/string.h"
#include "../../utils/heap.h"
#include "../../vulkan/context.h"
#include "../../vulkan/tools/tools.h"
#include "../../vulkan/functions/functions.h"
#include "../render_state.h"

render_program_manager ren_pm;

static bool add_shader_to_render_program_manager(render_program_manager *m, const char *name, const char *filepath) {
    if (m->shaders_size >= MAX_SHADERS) {
        log_error("Not enough space for another shader");
        return false;
    }

    shader s;
    if (!init_shader_from_file(&s, name, filepath)) {
        log_error("Error while creating shader: %s", name);
        return false;
    }

    m->shaders[m->shaders_size] = s;
    m->shaders_size++;

    return true;
}

bool init_render_program_manager(render_program_manager *m) {
    m->shaders_size = 0;
    m->shaders = mem_alloc(MAX_SHADERS * sizeof(shader));
    CHECK_ALLOC(m->shaders, "Allocation fail");
    for (size_t i = 0; i < MAX_SHADERS; i++) {
        init_shader(&m->shaders[i]);
    }

    bool success = add_shader_to_render_program_manager(m, "test.vert", "shaders/basic/test.vert.svm") &&
        add_shader_to_render_program_manager(m, "test.frag", "shaders/basic/test.vert.svm");

    return success;
}

void destroy_render_program_manager(render_program_manager *m) {
    for (size_t i = 0; i < m->shaders_size; i++) {
        destroy_shader(&m->shaders[i]);
    }

    if (m->shaders) {
        mem_free(m->shaders);
        m->shaders = NULL;
    }
}

bool init_ren_pm() {
    return init_render_program_manager(&ren_pm);
}

void destroy_ren_pm() {
    destroy_render_program_manager(&ren_pm);
}
