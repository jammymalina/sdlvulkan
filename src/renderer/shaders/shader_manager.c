#include "./shader_manager.h"

#include "../../logger/logger.h"
#include "../../string/string.h"
#include "../../utils/heap.h"
#include "../../vulkan/gpu_info.h"
#include "../../vulkan/context.h"
#include "../../vulkan/tools/tools.h"
#include "../../vulkan/functions/functions.h"
#include "../config.h"
#include "../render_state.h"
#include "./list.inl"

render_program_manager ren_pm;

const static shader_type shader_types[SHADER_TYPES_COUNT] = { SHADER_TYPE_VERTEX, SHADER_TYPE_FRAGMENT,
    SHADER_TYPE_TESS_CTRL, SHADER_TYPE_TESS_EVAL, SHADER_TYPE_GEOMETRY, SHADER_TYPE_COMPUTE };

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

static bool create_pipeline_layout(render_program *prog) {
    VkPipelineLayoutCreateInfo pipeline_layout_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .setLayoutCount = 1,
        .pSetLayouts = &prog->descriptor_set_layout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL
    };
    CHECK_VK(vk_CreatePipelineLayout(context.device, &pipeline_layout_info, NULL, &prog->pipeline_layout));

    return true;
}

static bool init_render_program_from_config(render_program *prog, const render_program_config *rp_conf,
    render_program_manager *m)
{
    prog->shader_indices.vert = rp_conf->shader_instances.vert != SHADER_INSTANCE_UNDEFINED ?
        find_shader_instance_program_manager(m, rp_conf->shader_instances.vert, SHADER_TYPE_VERTEX) : -1;
    prog->shader_indices.frag = rp_conf->shader_instances.frag != SHADER_INSTANCE_UNDEFINED ?
        find_shader_instance_program_manager(m, rp_conf->shader_instances.frag, SHADER_TYPE_FRAGMENT) : -1;
    prog->shader_indices.tesc = rp_conf->shader_instances.tesc != SHADER_INSTANCE_UNDEFINED ?
        find_shader_instance_program_manager(m, rp_conf->shader_instances.tesc, SHADER_TYPE_TESS_CTRL) : -1;
    prog->shader_indices.tese = rp_conf->shader_instances.tese != SHADER_INSTANCE_UNDEFINED ?
        find_shader_instance_program_manager(m, rp_conf->shader_instances.tese, SHADER_TYPE_TESS_EVAL) : -1;
    prog->shader_indices.geom = rp_conf->shader_instances.geom != SHADER_INSTANCE_UNDEFINED ?
        find_shader_instance_program_manager(m, rp_conf->shader_instances.geom, SHADER_TYPE_GEOMETRY) : -1;
    prog->shader_indices.comp = rp_conf->shader_instances.comp != SHADER_INSTANCE_UNDEFINED ?
        find_shader_instance_program_manager(m, rp_conf->shader_instances.comp, SHADER_TYPE_COMPUTE) : -1;

    bool success = string_copy(prog->name, MAX_SHADER_NAME_SIZE, rp_conf->name) &&
        create_descriptor_set_layout(m, prog) &&
        create_pipeline_layout(prog);

    return success;
}

static bool add_program_to_render_program_manager(render_program_manager *m, const render_program_config *rp_conf) {
    if (m->programs_size >= MAX_RENDER_PROGRAMS) {
        log_error("Not enough space for another shader program");
        return false;
    }

    render_program p;
    if (!init_render_program_from_config(&p, rp_conf, m)) {
        log_error("Error while creating render program: %s", rp_conf->name);
        destroy_render_program(&p);
        return false;
    }

    m->programs[m->programs_size] = p;
    m->programs_size++;

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
        success &= add_program_to_render_program_manager(m, &render_program_list[i]);
    }

    return success;
}

static bool create_vertex_descriptions(render_program_manager *m) {
    VkPipelineVertexInputStateCreateInfo vertex_input_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = NULL,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = NULL
    };

    return true;
}

static bool create_descriptor_pools(render_program_manager *m) {
    const gpu_info *gpu = &context.gpus[context.selected_gpu];
    uint32_t max_uniform_descriptors = gpu->props.limits.maxDescriptorSetUniformBuffers;
    uint32_t max_sample_descriptors = gpu->props.limits.maxDescriptorSetSampledImages;
    VkDescriptorPoolSize pool_sizes[] = {
        {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = max_uniform_descriptors
        },
        {
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = max_sample_descriptors
        }
    };
    const size_t num_sizes = sizeof(pool_sizes) / sizeof(VkDescriptorPoolSize);

    VkDescriptorPoolCreateInfo pool_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .maxSets = MAX_DECRIPTOR_SETS,
        .poolSizeCount = num_sizes,
        .pPoolSizes = pool_sizes
    };

    for (size_t i = 0; i < NUM_FRAME_DATA; i++) {
        CHECK_VK(vk_CreateDescriptorPool(context.device, &pool_info, NULL, &m->descriptor_pools[i]));
    }

    return true;
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

void destroy_render_program(render_program *prog) {
    if (prog->pipeline_layout) {
        vk_DestroyPipelineLayout(context.device, prog->pipeline_layout, NULL);
        prog->pipeline_layout = VK_NULL_HANDLE;
    }
    if (prog->descriptor_set_layout) {
        vk_DestroyDescriptorSetLayout(context.device, prog->descriptor_set_layout, NULL);
        prog->descriptor_set_layout = VK_NULL_HANDLE;
    }
}

bool init_render_program_manager(render_program_manager *m) {
    m->current_frame = 0;
    m->current_descriptor_set = 0;
    m->current_parameter_buffer_offset = 0;
    bool success = init_shaders(m) &&
        init_render_programs(m) &&
        create_descriptor_pools(m);

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

bool start_frame_render_program_manager(render_program_manager *m) {
    m->current_frame = (m->current_frame + 1) % NUM_FRAME_DATA;
    m->current_descriptor_set = 0;

    CHECK_VK(vk_ResetDescriptorPool(context.device, m->descriptor_pools[m->current_frame], 0));

    return true;
}

bool end_frame_render_program_manager(render_program_manager *m) {
    return true;
}

void destroy_render_program_manager(render_program_manager *m) {
    for (size_t i = 0; i < m->programs_size; i++) {
        destroy_render_program(&m->programs[i]);
    }
    for (size_t i = 0; i < m->shaders_size; i++) {
        destroy_shader(&m->shaders[i]);
    }

    if (m->programs) {
        mem_free(m->programs);
        m->programs = NULL;
    }

    if (m->shaders) {
        mem_free(m->shaders);
        m->shaders = NULL;
    }
}

bool init_ren_pm() {
    return init_render_program_manager(&ren_pm);
}

bool start_frame_ren_pm() {
    return start_frame_render_program_manager(&ren_pm);
}

bool end_frame_ren_pm() {
    return end_frame_render_program_manager(&ren_pm);
}

void destroy_ren_pm() {
    destroy_render_program_manager(&ren_pm);
}
