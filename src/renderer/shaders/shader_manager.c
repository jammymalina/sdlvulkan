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

const static shader_type shader_types[SHADER_TYPES_COUNT] = { SHADER_TYPE_VERTEX, SHADER_TYPE_FRAGMENT,
    SHADER_TYPE_TESS_CTRL, SHADER_TYPE_TESS_EVAL, SHADER_TYPE_GEOMETRY, SHADER_TYPE_COMPUTE };

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

uint32_t get_shader_type_bits_render_program(render_program *prog) {
    uint32_t result = 0;
    if (prog->shader_indices.vert != -1)
        result += SHADER_TYPE_VERTEX;
    if (prog->shader_indices.frag != -1)
        result += SHADER_TYPE_FRAGMENT;
    if (prog->shader_indices.tesc != -1)
        result += SHADER_TYPE_TESS_CTRL;
    if (prog->shader_indices.tese != -1)
        result += SHADER_TYPE_TESS_EVAL;
    if (prog->shader_indices.geom != -1)
        result += SHADER_TYPE_GEOMETRY;
    if (prog->shader_indices.comp != -1)
        result += SHADER_TYPE_COMPUTE;

    return result;
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

static bool init_shaders(render_program_manager *m) {
    m->shaders_size = 0;
    m->shaders = mem_alloc(MAX_SHADERS * sizeof(shader));
    CHECK_ALLOC(m->shaders, "Allocation fail");
    for (size_t i = 0; i < MAX_SHADERS; i++) {
        init_shader(&m->shaders[i]);
    }

    const static shader_config shader_list[] = SHADER_LIST;
    size_t n = sizeof(shader_list) / sizeof(shader_config);
    size_t shader_types_size = sizeof(shader_types) / sizeof(shader_type);

    bool success = true;
    for (size_t i = 0; i < n && success; i++) {
        const shader_config *sc = &shader_list[i];
        char shader_path[MAX_PATH_LENGTH];
        for (size_t j = 0; j < shader_types_size && success; j++) {
            if (sc->type_bits & shader_types[j]) {
                success &= shader_resolve_path(shader_path, sc->name, sc->directory, shader_types[j]) &&
                    add_shader_to_render_program_manager(m, sc->instance, sc->name, shader_path);
            }
        }
    }

    return success;
}

static bool create_descriptor_set_layout(render_program_manager *m, render_program *rp) {
    static VkDescriptorSetLayoutBinding layout_bindings[SHADER_TYPES_COUNT * MAX_SHADER_BINDINGS_SIZE];
    struct { int index; shader_type type; } shader_array[SHADER_TYPES_COUNT] ={
        { rp->shader_indices.vert, SHADER_TYPE_VERTEX }, { rp->shader_indices.frag, SHADER_TYPE_FRAGMENT },
        { rp->shader_indices.tesc, SHADER_TYPE_TESS_CTRL }, { rp->shader_indices.tese, SHADER_TYPE_TESS_EVAL },
        { rp->shader_indices.geom, SHADER_TYPE_GEOMETRY }, { rp->shader_indices.comp, SHADER_TYPE_COMPUTE }
    };

    uint32_t bindings_count = 0;
    for (size_t i = 0; i < SHADER_TYPES_COUNT; i++) {
        int index = shader_array[i].index;
        shader_type type = shader_array[i].type;

        if (index == -1)
            continue;

        shader *shader = &m->shaders[index];
        for (size_t j = 0; j < shader->bindings_size; i++) {
            VkDescriptorSetLayoutBinding binding = {
                .binding = bindings_count,
                .descriptorType = get_descriptor_type(shader->bindings[j]),
                .descriptorCount = 1,
                .stageFlags = shader_type_to_shader_stage(type),
                .pImmutableSamplers = NULL
            };
            layout_bindings[bindings_count] = binding;
            bindings_count++;
        }
    }

    VkDescriptorSetLayoutCreateInfo descriptor_set_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .bindingCount = bindings_count,
        .pBindings = bindings_count == 0 ? NULL : layout_bindings
    };
    CHECK_VK(vk_CreateDescriptorSetLayout(context.device, &descriptor_set_info, NULL, &rp->descriptor_set_layout));
    return true;
}

static bool add_program_to_render_program_manager(render_program_manager *m) {
    return true;
}

static bool init_render_programs(render_program_manager *m) {
    m->programs_size = 0;
    m->programs = mem_alloc(MAX_RENDER_PROGRAMS * sizeof(render_program));
    CHECK_ALLOC(m->programs, "Allocation fail");
    for (size_t i = 0; i < MAX_RENDER_PROGRAMS; i++) {
        init_render_program(&m->programs[i]);
    }

    const static render_program_config render_program_list[] = RENDER_PROGRAM_LIST;
    size_t n = sizeof(render_program_list) / sizeof(render_program_config);

    bool success = true;
    for (size_t i = 0; i < n && success; i++) {
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
