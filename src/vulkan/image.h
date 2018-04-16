#ifndef VULKAN_IMAGE_H
#define VULKAN_IMAGE_H

#include <stdbool.h>
#include <vulkan/vulkan.h>
#include "./memory/memory.h"
#include "./config.h"
#include "../renderer/materials/material.h"

typedef struct vk_image_props {
    texture_type type;
    texture_format format;
    texture_samples samples;
    texture_filter filter;
    texture_repeat repeat;
    size_t width;
    size_t height;
    size_t num_levels;
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
bool sub_image_upload(vk_image *image, size_t mip_level, size_t x, size_t y, size_t z,
    size_t width, size_t height, void *picture, size_t pixel_pitch);
void destroy_image(vk_image *image);

#endif // VULKAN_IMAGE_H
