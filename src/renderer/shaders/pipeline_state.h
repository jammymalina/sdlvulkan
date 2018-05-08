#ifndef PIPELINE_STATE_H
#define PIPELINE_STATE_H

#include <vulkan/vulkan.h>
#include <stdint.h>
#include <stddef.h>

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



#endif // PIPELINE_STATE_H
