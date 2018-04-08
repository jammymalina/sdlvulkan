#ifndef VULKAN_IMAGE_H
#define VULKAN_IMAGE_H

#include <stdbool.h>
#include <vulkan/vulkan.h>

enum texture_type {
	TT_DISABLED,
	TT_2D,
	TT_CUBIC
};

enum texture_format {
	FMT_NONE,
	FMT_RGBA8,
	FMT_DEPTH
};

typedef struct vk_image {

} vk_image;

void init_image(vk_image *image);
void create_from_swapchain_image(vk_image *result, VkImage image, VkImageView imageView, VkFormat format,
	VkExtent2D *extent);
bool alloc_image(vk_image *result);
void destroy_image(vk_image *image);

#endif // VULKAN_IMAGE_H
