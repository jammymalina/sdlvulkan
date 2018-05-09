#ifndef PIPELINE_STATE_H
#define PIPELINE_STATE_H

#include <vulkan/vulkan.h>
#include <stdint.h>
#include <stddef.h>

#define MAX_PIPELINE_DYNAMIC_STATES_SIZE 12

typedef struct pipeline_state {
    uint64_t state_bits;
    VkPipeline pipeline;
    size_t counter;
} pipeline_state;

static inline void init_pipeline_state(pipeline_state *pipe_state) {
    pipe_state->state_bits = 0;
    pipe_state->pipeline = VK_NULL_HANDLE;
    pipe_state->counter = 0;
}

VkPipelineRasterizationStateCreateInfo get_rasterization_state_from_pipeline_bits(uint64_t state_bits);
VkPipelineColorBlendAttachmentState get_color_blend_attachment_from_pipeline_bits(uint64_t state_bits);
VkPipelineDepthStencilStateCreateInfo get_depth_stencil_info_from_pipeline_bits(uint64_t state_bits);
VkPipelineDynamicStateCreateInfo get_dynamic_states_from_pipeline_bits(VkDynamicState dest[MAX_PIPELINE_DYNAMIC_STATES_SIZE],
    uint64_t state_bits);

#endif // PIPELINE_STATE_H
