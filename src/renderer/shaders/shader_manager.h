#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include <vulkan/vulkan.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "./shader.h"

#define MAX_SHADERS 64

typedef struct pipeline_state {
    uint64_t state_bits;
    VkPipeline pipeline;
} pipeline_state;

typedef enum render_program_instance_type {
    RENDER_PROGRAM_INTANCE_UNDEFINED = -1,
    RENDER_PROGRAM_INTANCE_TEST,
    RENDER_PROGRAM_INTANCES_TOTAL
} render_program_instance_type;

typedef struct render_program_config {
    render_program_instance_type instance;
    struct {
        shader_instance_type vert;
        shader_instance_type frag;
        shader_instance_type geom;
        shader_instance_type tesc;
        shader_instance_type tese;
        shader_instance_type comp;
    } shader_instances;
} render_program_config;

typedef struct render_program {
    char name[MAX_SHADER_NAME_SIZE];
    struct {
        int vert;
        int frag;
        int geom;
        int tesc;
        int tese;
        int comp;
    } shader_indices;
    VkPipelineLayout pipeline_layout;
    VkDescriptorSetLayout descriptor_set_layout;
} render_program;

typedef struct render_program_manager {
    int current_render_program;

    shader *shaders;
    size_t shaders_size;

    render_program *programs;
    size_t programs_size;
} render_program_manager;

static inline void init_pipeline_state(pipeline_state *pipe_state) {
    pipe_state->state_bits = 0;
    pipe_state->pipeline = VK_NULL_HANDLE;
}

void init_render_program(render_program *prog);

bool init_render_program_manager(render_program_manager *m);
int find_shader_instance_program_manager(render_program_manager *m, shader_instance_type instance_type,
    shader_type type);
void destroy_render_program_manager(render_program_manager *m);

extern render_program_manager ren_pm;

bool init_ren_pm();
void destroy_ren_pm();

#endif // SHADER_MANAGER_H
