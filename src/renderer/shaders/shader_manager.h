#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include <vulkan/vulkan.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "./shader.h"
#include "../../vulkan/config.h"

#define SHADER_TYPES_COUNT 6
#define MAX_SHADERS 64
#define MAX_RENDER_PROGRAMS 32

typedef struct pipeline_state {
    uint64_t state_bits;
    VkPipeline pipeline;
} pipeline_state;

typedef enum render_program_instance_type {
    RENDER_PROGRAM_INTANCE_UNDEFINED = -1,
    RENDER_PROGRAM_INTANCE_TEST,
    RENDER_PROGRAM_INTANCES_TOTAL
} render_program_instance_type;

typedef enum vertex_layout_type {
	VERTEX_LAYOUT_UNKNOWN = -1,
	VERTEX_LAYOUT_DRAW_VERT,
	VERTEX_LAYOUTS_TOTAL
} vertex_layout_type;

typedef struct render_program_config {
    char *name;
    render_program_instance_type instance;
    struct {
        shader_instance_type vert;
        shader_instance_type frag;
        shader_instance_type geom;
        shader_instance_type tesc;
        shader_instance_type tese;
        shader_instance_type comp;
    } shader_instances;
    vertex_layout_type vertex_layout;
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

    VkDescriptorPool descriptor_pools[NUM_FRAME_DATA];
    size_t current_frame;
    size_t current_descriptor_set;
    size_t current_parameter_buffer_offset;
} render_program_manager;

static inline void init_pipeline_state(pipeline_state *pipe_state) {
    pipe_state->state_bits = 0;
    pipe_state->pipeline = VK_NULL_HANDLE;
}

void init_render_program(render_program *prog);
uint32_t get_shader_type_bits_render_program(render_program *prog);
void destroy_render_program(render_program *prog);

bool init_render_program_manager(render_program_manager *m);
int find_shader_instance_program_manager(render_program_manager *m, shader_instance_type instance_type,
    shader_type type);
bool start_frame_render_program_manager(render_program_manager *m);
void destroy_render_program_manager(render_program_manager *m);

extern render_program_manager ren_pm;

bool init_ren_pm();
bool start_frame_ren_pm();
void destroy_ren_pm();

#endif // SHADER_MANAGER_H
