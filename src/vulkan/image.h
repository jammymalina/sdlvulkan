#ifndef VULKAN_IMAGE_H
#define VULKAN_IMAGE_H

#include <stdbool.h>
#include <vulkan/vulkan.h>
#include "./memory/memory.h"
#include "./config.h"
#include "./material.h"

typedef struct vk_image_props {
    texture_type type;
    texture_format format;
    texture_samples samples;
    texture_filter filter; 
    texture_repeat repeat;
    int width;
    int height;
    int num_levels;
    bool gamma_mips;
} vk_image_props;

typedef struct vk_image {
    vk_image_props props;

    bool is_swapchain_image;
    VkFormat internal_format;
    VkImage image;
    VkImageView view;
    VkImageLayout layout;
    VkSampler sampler;

    vk_allocation allocation;
} vk_image;

void init_image_props(vk_image_props *props);

void init_image(vk_image *image);
void create_from_swapchain_image(vk_image *result, VkImage image, VkImageView image_view, VkFormat format,
    VkExtent2D *extent);
bool alloc_image(vk_image *image);
void destroy_image(vk_image *image);

#endif // VULKAN_IMAGE_H
