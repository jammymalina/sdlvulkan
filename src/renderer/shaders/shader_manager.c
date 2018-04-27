#include "./shader_manager.h"

#include "../../logger/logger.h"
#include "../../string/string.h"
#include "../../utils/heap.h"
#include "../../utils/file.h"
#include "../../vulkan/context.h"
#include "../../vulkan/tools/tools.h"
#include "../../vulkan/functions/functions.h"

void init_shader(shader *s) {
    string_copy(s->name, MAX_SHADER_NAME_SIZE, "");
    s->module = VK_NULL_HANDLE;
    s->bindings_size = 0;
    s->render_params_size = 0;
}

bool init_shader_from_file(shader *s, const char *name, const char *filepath) {
    bool success = string_copy(s->name, MAX_SHADER_NAME_SIZE, name);
    if (!success) {
        log_error("Name too long for a shader: %s", name);
        return false;
    }

    void *shader_code = NULL;
    size_t shader_code_byte_size = read_binary_file(filepath, &shader_code);
    if (shader_code_byte_size == 0) {
        log_error("File seems to be empty or does not exist: %s", filepath);
        return false;
    }
    // Shader data must be aligned to 32 bits = 4 bytes
    if (is_4_byte_aligned(shader_code)) {
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

bool init_render_program_manager(render_program_manager *m) {
    m->shaders_size = 0;
    m->shaders = mem_alloc(MAX_SHADERS * sizeof(shader));
    CHECK_ALLOC(m->shaders, "Allocation fail");
    for (size_t i = 0; i < MAX_SHADERS; i++) {
        init_shader(&m->shaders[i]);
    }

    return true;
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
