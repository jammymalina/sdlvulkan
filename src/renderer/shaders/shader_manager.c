#include "./shader_manager.h"

#include "../../logger/logger.h"
#include "../../string/string.h"
#include "../../utils/heap.h"
#include "../../utils/file.h"
#include "../../vulkan/context.h"
#include "../../vulkan/tools/tools.h"
#include "../../vulkan/functions/functions.h"

render_program_manager ren_pm;

shader_type extension_to_shader_type(const char *extension) {
    if (string_equal(extension, "vert"))
        return SHADER_TYPE_VERTEX;
    if (string_equal(extension, "frag"))
        return SHADER_TYPE_FRAGMENT;
    if (string_equal(extension, "tesc"))
        return SHADER_TYPE_TESS_CTRL;
    if (string_equal(extension, "tese"))
        return SHADER_TYPE_TESS_EVAL;
    if (string_equal(extension, "geom"))
        return SHADER_TYPE_GEOMETRY;
    if (string_equal(extension, "comp"))
        return SHADER_TYPE_COMPUTE;

    return SHADER_TYPE_UNDEFINED;
}

VkShaderStageFlagBits shader_type_to_shader_stage(shader_type type) {
    switch (type) {
        case SHADER_TYPE_VERTEX:
            return VK_SHADER_STAGE_VERTEX_BIT;
        case SHADER_TYPE_FRAGMENT:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case SHADER_TYPE_TESS_CTRL:
            return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        case SHADER_TYPE_TESS_EVAL:
            return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        case SHADER_TYPE_GEOMETRY:
            return VK_SHADER_STAGE_GEOMETRY_BIT;
        case SHADER_TYPE_COMPUTE:
            return VK_SHADER_STAGE_COMPUTE_BIT;
        default:
            return VK_SHADER_STAGE_ALL;
    }
}

void init_shader(shader *s) {
    string_copy(s->name, MAX_SHADER_NAME_SIZE, "");
    s->module = VK_NULL_HANDLE;
    s->type = SHADER_TYPE_UNDEFINED;
    s->bindings_size = 0;
    s->render_params_size = 0;
}

bool init_shader_from_file(shader *s, const char *name, const char *filepath) {
    init_shader(s);

    bool success = string_copy(s->name, MAX_SHADER_NAME_SIZE, name);
    if (!success) {
        log_error("Name too long for a shader: %s", name);
        return false;
    }

    char shader_extension[MAX_PATH_LENGTH];
    success = extract_extension(shader_extension, filepath, 1);
    if (!success) {
        log_error("Unable to extract shader extension from path: %s", filepath);
        return false;
    }
    shader_type t = extension_to_shader_type(shader_extension);
    if (t == SHADER_TYPE_UNDEFINED) {
        log_error("Invalid shader extension: %s", shader_extension);
        return false;
    }
    s->type = t;

    void *shader_code = NULL;
    size_t shader_code_byte_size = read_binary_file(filepath, &shader_code);
    if (shader_code_byte_size == 0) {
        log_error("File seems to be empty or does not exist: %s", filepath);
        return false;
    }
    // Shader data must be aligned to 32 bits = 4 bytes
    if (!is_4_byte_aligned(shader_code)) {
        log_error("Shader code is incorrectly aligned: %s", filepath);
        mem_free(shader_code);
        return false;
    }

    VkShaderModuleCreateInfo shader_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .codeSize = shader_code_byte_size,
        .pCode = shader_code
    };

    VkResult result = vk_CreateShaderModule(context.device, &shader_info, NULL, &s->module);
    if (result != VK_SUCCESS) {
        log_error("VK error: %s - %s", "vk_CreateShaderModule", vulkan_result_to_string(result));
        mem_free(shader_code);
        return true;
    }

    mem_free(shader_code);

    return true;
}

void destroy_shader(shader *s) {
    if (s->module) {
        vk_DestroyShaderModule(context.device, s->module, NULL);
        s->module = VK_NULL_HANDLE;
    }
}

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
