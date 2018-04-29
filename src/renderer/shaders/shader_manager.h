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
} render_program_manager;

static inline void init_pipeline_state(pipeline_state *pipe_state) {
    pipe_state->state_bits = 0;
    pipe_state->pipeline = VK_NULL_HANDLE;
}

bool init_render_program_manager(render_program_manager *m);
void destroy_render_program_manager(render_program_manager *m);

extern render_program_manager ren_pm;

bool init_ren_pm();
void destroy_ren_pm();

#endif // SHADER_MANAGER_H
