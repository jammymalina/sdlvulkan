#ifndef VULKAN_IMAGE_H
#define VULKAN_IMAGE_H

#include <stdbool.h>
#include <vulkan/vulkan.h>

typedef enum texture_type {
    TT_DISABLED,
    TT_2D,
    TT_CUBIC
} texture_type;

typedef enum texture_format {
    FMT_NONE,
    FMT_RGBA8,
    FMT_XRGB8,
    FMT_ALPHA,
    FMT_L8A8,
    FMT_LUM8,
    FMT_INT8,
    FMT_DXT1,
    FMT_DXT5,
    FMT_X16,
    FMT_Y16_X16,
    FMT_RGB565,
    FMT_DEPTH
} texture_format;

typedef struct vk_image_props {
    texture_type type;
    texture_format format;
    int width;
    int height;
    int num_levels;
    bool gamma_mips;
} vk_image_props;

typedef struct vk_image {
    vk_image_props props;
} vk_image;

void init_image(vk_image *image);
void create_from_swapchain_image(vk_image *result, VkImage image, VkImageView imageView, VkFormat format,
    VkExtent2D *extent);
bool alloc_image(vk_image *result);
void destroy_image(vk_image *image);

#endif // VULKAN_IMAGE_H
