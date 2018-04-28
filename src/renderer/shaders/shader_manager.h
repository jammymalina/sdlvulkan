#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include <vulkan/vulkan.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_SHADER_NAME_SIZE 256
#define MAX_SHADER_RENDER_PARAMS 32
#define MAX_SHADER_BINDINGS_SIZE 32
#define MAX_SHADERS 256

typedef enum render_param {
    RENDER_PARAMS_TOTAL
} render_param;

typedef enum shader_binding {
    BINDING_TYPE_UNIFORM_BUFFER,
    BINDING_TYPE_SAMPLER,
    BINDINGS_TOTAL
} shader_binding;

typedef enum shader_type {
    SHADER_TYPE_VERTEX    =  1,
    SHADER_TYPE_FRAGMENT  =  2,
    SHADER_TYPE_GEOMETRY  =  4,
    SHADER_TYPE_COMPUTE   =  8,
    SHADER_TYPE_TESS_CTRL = 16,
    SHADER_TYPE_TESS_EVAL = 32,
    SHADER_TYPE_UNDEFINED = 64
} shader_type;

typedef struct pipeline_state {
    uint64_t state_bits;
    VkPipeline pipeline;
} pipeline_state;

typedef struct shader {
    char name[MAX_SHADER_NAME_SIZE];
    shader_type type;

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

typedef struct render_program_manager {
    int current_render_program;

    shader *shaders;
    size_t shaders_size;
} render_program_manager;

shader_type extension_to_shader_type(const char *extension);
VkShaderStageFlagBits shader_type_to_shader_stage(shader_type type);

static inline void init_pipeline_state(pipeline_state *pipe_state) {
    pipe_state->state_bits = 0;
    pipe_state->pipeline = VK_NULL_HANDLE;
}

void init_shader(shader *s);
bool init_shader_from_file(shader *s, const char *name, const char *filepath);
void destroy_shader(shader *s);

bool init_render_program_manager(render_program_manager *m);
void destroy_render_program_manager(render_program_manager *m);

extern render_program_manager ren_pm;

bool init_ren_pm();
void destroy_ren_pm();

#endif // SHADER_MANAGER_H
