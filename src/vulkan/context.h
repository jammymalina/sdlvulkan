#ifndef VULKAN_CONTEXT_H
#define VULKAN_CONTEXT_H

#include <vulkan/vulkan.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "./gpu_info.h"
#include "./config.h"
#include "./image.h"

typedef struct vk_context {
    VkInstance instance;
    VkSurfaceKHR surface;
    VkDevice device;

    gpu_info *gpus;
    uint32_t gpus_size;
    uint32_t selected_gpu;

    uint32_t graphics_family_index;
    uint32_t present_family_index;

    VkQueue graphics_queue;
    VkQueue present_queue;

    VkSemaphore acquire_semaphores[NUM_FRAME_DATA];
    VkSemaphore render_complete_semaphores[NUM_FRAME_DATA];

    VkQueryPool query_pools[NUM_FRAME_DATA];

    VkCommandPool command_pool;
    VkCommandBuffer command_buffers[NUM_FRAME_DATA];
    VkFence command_buffer_fences[NUM_FRAME_DATA];

    VkSwapchainKHR swapchain;
    VkImage swapchain_images[NUM_FRAME_DATA];
    VkImageView swapchain_views[NUM_FRAME_DATA];

    bool supersampling;
    VkSampleCountFlagBits sample_count;

    vk_image depth_image;

    VkSurfaceFormatKHR surface_format;
    VkPresentModeKHR present_mode;
    VkExtent2D extent;

    VkFormat depth_format;
    VkRenderPass render_pass;

    VkFramebuffer framebuffers[NUM_FRAME_DATA];

    VkPipelineCache pipeline_cache;

    #ifdef DEBUG
        VkDebugReportCallbackEXT debug_callback;
    #endif
} vk_context;

extern vk_context context;

void init_vk_context(vk_context *ctx);
bool init_vulkan(vk_context *ctx, SDL_Window *window);
void shutdown_vulkan(vk_context *ctx);

#endif // VULKAN_CONTEXT_H
