#include "./shader_manager.h"

#include "../../logger/logger.h"
#include "../../string/string.h"
#include "../../utils/heap.h"
#include "../../vulkan/context.h"
#include "../../vulkan/tools/tools.h"
#include "../../vulkan/functions/functions.h"
#include "../render_state.h"
#include "./list.inl"

render_program_manager ren_pm;

static bool add_shader_to_render_program_manager(render_program_manager *m, shader_instance_type instance,
    const char *name, const char *filepath)
{
    if (m->shaders_size >= MAX_SHADERS) {
        log_error("Not enough space for another shader");
        return false;
    }

    shader s;
    if (!init_shader_from_file(&s, instance, name, filepath)) {
        log_error("Error while creating shader: %s", name);
        destroy_shader(&s);
        return false;
    }

    m->shaders[m->shaders_size] = s;
    m->shaders_size++;

    return true;
}

static bool create_descriptor_set_layout(render_program *rp) {
    return true;
}

bool init_render_program_manager(render_program_manager *m) {
    m->shaders_size = 0;
    m->shaders = mem_alloc(MAX_SHADERS * sizeof(shader));
    CHECK_ALLOC(m->shaders, "Allocation fail");
    for (size_t i = 0; i < MAX_SHADERS; i++) {
        init_shader(&m->shaders[i]);
    }

    const static shader_config shader_list[] = SHADER_LIST;
    size_t n = sizeof(shader_list) / sizeof(shader_config);

    bool success = true;
    for (size_t i = 0; i < n && success; i++) {
        const shader_config *sc = &shader_list[i];
        char shader_path[MAX_PATH_LENGTH];
        if (sc->type_bits & SHADER_TYPE_VERTEX) {
            success &= shader_resolve_path(shader_path, sc->name, sc->directory, SHADER_TYPE_VERTEX) &&
                add_shader_to_render_program_manager(m, sc->instance, sc->name, shader_path);
            if (!success)
                break;
        }
        if (sc->type_bits & SHADER_TYPE_FRAGMENT) {
            success &= shader_resolve_path(shader_path, sc->name, sc->directory, SHADER_TYPE_FRAGMENT);
                add_shader_to_render_program_manager(m, sc->instance, sc->name, shader_path);
            if (!success)
                break;
        }
        if (sc->type_bits & SHADER_TYPE_TESS_CTRL) {
            success &= shader_resolve_path(shader_path, sc->name, sc->directory, SHADER_TYPE_TESS_CTRL);
                add_shader_to_render_program_manager(m, sc->instance, sc->name, shader_path);
            if (!success)
                break;
        }
        if (sc->type_bits & SHADER_TYPE_TESS_EVAL) {
            success &= shader_resolve_path(shader_path, sc->name, sc->directory, SHADER_TYPE_TESS_EVAL);
                add_shader_to_render_program_manager(m, sc->instance, sc->name, shader_path);
            if (!success)
                break;
        }
        if (sc->type_bits & SHADER_TYPE_GEOMETRY) {
            success &= shader_resolve_path(shader_path, sc->name, sc->directory, SHADER_TYPE_GEOMETRY);
                add_shader_to_render_program_manager(m, sc->instance, sc->name, shader_path);
            if (!success)
                break;
        }
        if (sc->type_bits & SHADER_TYPE_COMPUTE) {
            success &= shader_resolve_path(shader_path, sc->name, sc->directory, SHADER_TYPE_GEOMETRY);
                add_shader_to_render_program_manager(m, sc->instance, sc->name, shader_path);
            if (!success)
                break;
        }
    }

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
