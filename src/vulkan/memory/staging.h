#ifndef STAGING_H
#define STAGING_H

#include <vulkan/vulkan.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "./memory.h"
#include "../config.h"

typedef struct vk_staging_buffer {
    bool submitted;
    VkCommandBuffer command_buffer;
    VkBuffer buffer;
    VkFence fence;
    VkDeviceSize offset;
    byte *data;
} vk_staging_buffer;

typedef struct vk_staging_manager {
    VkDeviceSize max_buffer_size;
    uint32_t current_buffer;
    byte *mapped_data;
    VkDeviceMemory memory;
    VkCommandPool command_pool;
    vk_staging_buffer buffers[NUM_FRAME_DATA];
} vk_staging_manager;

void init_vk_staging_buffer(vk_staging_buffer *buffer);

void init_vk_staging_manager(vk_staging_manager *manager);
bool init_buffers_vk_staging_manager(vk_staging_manager *manager);
byte* stage_vk_staging_manager(vk_staging_manager *manager, VkDeviceSize size, VkDeviceSize alignment,
    VkCommandBuffer *command_buffer, VkBuffer *buffer, VkDeviceSize *buffer_offset);
void flush_vk_staging_manager(vk_staging_manager *manager);
void destroy_vk_staging_manager(vk_staging_manager *manager);

extern vk_staging_manager staging_manager;

bool vk_init_stage_manager();
void vk_flush_stage();
byte* vk_stage(VkDeviceSize size, VkDeviceSize alignment, VkCommandBuffer *command_buffer, VkBuffer *buffer,
    VkDeviceSize *buffer_offset);
void vk_destroy_stage_manager();

#endif
