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

typedef struct pipeline_state {
    uint64_t state_bits;
    VkPipeline pipeline;
} pipeline_state;

static inline void init_pipeline_state(pipeline_state *pipe_state) {
    pipe_state->state_bits = 0;
    pipe_state->pipeline = VK_NULL_HANDLE;
}

typedef struct shader {
    char name[MAX_SHADER_NAME_SIZE];
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

void init_shader(shader *s);
bool init_shader_from_file(shader *s, const char *name, const char *filepath);
void destroy_shader(shader *s);

bool init_render_program_manager(render_program_manager *m);
bool load_shader(render_program_manager *m, const char *name, const char *filepath);
void destroy_render_program_manager(render_program_manager *m);


#endif // SHADER_MANAGER_H
