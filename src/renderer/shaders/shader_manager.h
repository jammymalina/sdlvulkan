#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#define MAX_SHADER_NAME_SIZE 256
#define MAX_SHADER_RENDER_PARAMS 32
#define MAX_SHADER_BINDINGS_SIZE 32

#include <vulkan/vulkan.h>
#include <stddef.h>
#include <stdbool.h>

typedef enum render_param {
    RENDER_PARAMS_TOTAL
} render_param;

typedef enum shader_binding {
    BINDING_TYPE_UNIFORM_BUFFER,
    BINDING_TYPE_SAMPLER,
    BINDINGS_TOTAL
} shader_binding;

typedef struct shader {
    VkShaderModule module;

    shader_binding bindings[MAX_SHADER_BINDINGS_SIZE];
    size_t bindings_size;

    render_param render_params[MAX_SHADER_RENDER_PARAMS];
    size_t render_params_size;
} shader;

typedef struct render_program {
    char name[MAX_SHADER_NAME_SIZE];
    int vertex_shader_index;
    int fragment_shader_index;
    VkPipelineLayout pipeline_layout;
    VkDescriptorSetLayout descriptor_set_layout;

} render_program;

void init_shader(shader *s);

#endif // SHADER_MANAGER_H
