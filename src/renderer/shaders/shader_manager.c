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

void init_render_program(render_program *prog) {
    string_copy(prog->name, MAX_SHADER_NAME_SIZE, "");

    prog->shader_indices.vert = -1;
    prog->shader_indices.frag = -1;
    prog->shader_indices.geom = -1;
    prog->shader_indices.tesc = -1;
    prog->shader_indices.tese = -1;
    prog->shader_indices.comp = -1;

    prog->pipeline_layout = VK_NULL_HANDLE;
    prog->descriptor_set_layout = VK_NULL_HANDLE;
}

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
    VkDescriptorSetLayoutCreateInfo descriptor_set_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .bindingCount = 0,
        .pBindings = NULL
    };
    vk_CreateDescriptorSetLayout(context.device, &descriptor_set_info, NULL, &rp->descriptor_set_layout);
    return true;
}

static bool init_shaders(render_program_manager *m) {
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

static bool init_render_programs(render_program_manager *m) {
    bool success = true;

    const static render_program_config render_program_list[] = RENDER_PROGRAM_LIST;
    size_t n = sizeof(render_program_list) / sizeof(render_program_config);

    for (size_t i = 0; i < n && success; i++) {
        render_program prog;
        init_render_program(&prog);
    }

    return success;
}

bool init_render_program_manager(render_program_manager *m) {
    bool success = init_shaders(m) &&
        init_render_programs(m);

    return success;
}

int find_shader_instance_program_manager(render_program_manager *m, shader_instance_type instance_type,
    shader_type type)
{
    if (instance_type == SHADER_INSTANCE_UNDEFINED || type == SHADER_TYPE_UNDEFINED) {
        log_warning("Searching for undefined shader, instance: %d, type: %d", instance_type, type);
        return -1;
    }

    for (int i = 0; i < m->shaders_size; i++) {
        const shader *s = &m->shaders[i];
        if (s->instance == instance_type && s->type == type) {
            return i;
        }
    }

    log_warning("Unable to find a shader instance: %d, type: %d", instance_type, type);
    return -1;
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
