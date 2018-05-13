#include "./image.h"

#include "./functions/functions.h"
#include "../logger/logger.h"
#include "./tools/tools.h"
#include "./context.h"
#include "./memory/staging.h"
#include "../utils/heap.h"

static inline VkFormat texture_format_to_vk_format(texture_format format) {
    switch (format) {
        case FMT_RGBA8:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case FMT_XRGB8:
            return VK_FORMAT_R8G8B8_UNORM;
        case FMT_ALPHA:
            return VK_FORMAT_R8_UNORM;
        case FMT_L8A8:
            return VK_FORMAT_R8G8_UNORM;
        case FMT_LUM8:
            return VK_FORMAT_R8_UNORM;
        case FMT_INT8:
            return VK_FORMAT_R8_UNORM;
        case FMT_DXT1:
            return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
        case FMT_DXT5:
            return VK_FORMAT_BC3_UNORM_BLOCK;
        case FMT_DEPTH:
            return context.depth_format;
        case FMT_X16:
            return VK_FORMAT_R16_UNORM;
        case FMT_Y16_X16:
            return VK_FORMAT_R16G16_UNORM;
        case FMT_RGB565:
            return VK_FORMAT_R5G6B5_UNORM_PACK16;
        default:
            return VK_FORMAT_UNDEFINED;
    }
}


static inline VkComponentMapping texture_format_to_component_mapping(texture_format format) {
    VkComponentMapping component_mapping = {
        .r = VK_COMPONENT_SWIZZLE_ZERO,
        .g = VK_COMPONENT_SWIZZLE_ZERO,
        .b = VK_COMPONENT_SWIZZLE_ZERO,
        .a = VK_COMPONENT_SWIZZLE_ZERO
    };

    switch (format) {
        case FMT_LUM8:
            component_mapping.r = VK_COMPONENT_SWIZZLE_R;
            component_mapping.g = VK_COMPONENT_SWIZZLE_R;
            component_mapping.b = VK_COMPONENT_SWIZZLE_R;
            component_mapping.a = VK_COMPONENT_SWIZZLE_ONE;
            break;
        case FMT_L8A8:
            component_mapping.r = VK_COMPONENT_SWIZZLE_R;
            component_mapping.g = VK_COMPONENT_SWIZZLE_R;
            component_mapping.b = VK_COMPONENT_SWIZZLE_R;
            component_mapping.a = VK_COMPONENT_SWIZZLE_G;
            break;
        case FMT_ALPHA:
            component_mapping.r = VK_COMPONENT_SWIZZLE_ONE;
            component_mapping.g = VK_COMPONENT_SWIZZLE_ONE;
            component_mapping.b = VK_COMPONENT_SWIZZLE_ONE;
            component_mapping.a = VK_COMPONENT_SWIZZLE_R;
            break;
        case FMT_INT8:
            component_mapping.r = VK_COMPONENT_SWIZZLE_R;
            component_mapping.g = VK_COMPONENT_SWIZZLE_R;
            component_mapping.b = VK_COMPONENT_SWIZZLE_R;
            component_mapping.a = VK_COMPONENT_SWIZZLE_R;
            break;
        default:
            component_mapping.r = VK_COMPONENT_SWIZZLE_R;
            component_mapping.g = VK_COMPONENT_SWIZZLE_G;
            component_mapping.b = VK_COMPONENT_SWIZZLE_B;
            component_mapping.a = VK_COMPONENT_SWIZZLE_A;
            break;
    }

    return component_mapping;
}

void init_image_props(vk_image_props *props) {
    props->format = FMT_NONE;
    props->samples = SAMPLE_1;
    props->width = 0;
    props->height = 0;
    props->num_levels = 0;
    props->type = TT_2D;
    props->gamma_mips = false;
    props->filter = TF_DEFAULT;
    props->repeat = TR_REPEAT;
}

void init_image(vk_image *image) {
    image->is_swapchain_image = false;
    image->internal_format = VK_FORMAT_UNDEFINED;
    image->image = VK_NULL_HANDLE;
    image->view = VK_NULL_HANDLE;
    image->layout = VK_IMAGE_LAYOUT_UNDEFINED;
    image->sampler = VK_NULL_HANDLE;
    init_image_props(&image->props);
}

void create_from_swapchain_image(vk_image *result, VkImage image, VkImageView image_view, VkFormat format,
    VkExtent2D *extent)
{
    result->image = image;
    result->view = image_view;
    result->internal_format = format;

    result->props.type = TT_2D;
    result->props.format = FMT_RGBA8;
    result->props.num_levels = 1;
    result->props.width = extent->width;
    result->props.height = extent->height;

    result->is_swapchain_image = true;
}

static bool create_vk_sampler(vk_image *image) {
    VkSamplerCreateInfo sampler_info = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .mipLodBias = 0,
        .anisotropyEnable = VK_FALSE,
        .maxAnisotropy = 1.0,
        .compareEnable = image->props.format == FMT_DEPTH,
        .compareOp = image->props.format == FMT_DEPTH ? VK_COMPARE_OP_LESS_OR_EQUAL : VK_COMPARE_OP_NEVER,
        .minLod = 0,
        .maxLod = 0,
        .borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
        .unnormalizedCoordinates = VK_FALSE
    };

    switch (image->props.filter) {
        case TF_DEFAULT:
        case TF_LINEAR:
            sampler_info.minFilter = VK_FILTER_LINEAR;
            sampler_info.magFilter = VK_FILTER_LINEAR;
            sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            break;
        case TF_NEAREST:
            sampler_info.minFilter = VK_FILTER_NEAREST;
            sampler_info.magFilter = VK_FILTER_NEAREST;
            sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
            break;
    }

    switch (image->props.repeat) {
        case TR_REPEAT:
            sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            break;
        case TR_CLAMP:
            sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            break;
        case TR_CLAMP_TO_ZERO_ALPHA:
            sampler_info.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
            sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
            sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
            sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
            break;
        case TR_CLAMP_TO_ZERO:
            sampler_info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
            sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
            sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
            sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
            break;
    }

    CHECK_VK(vk_CreateSampler(context.device, &sampler_info, NULL, &image->sampler));

    return true;
}

bool alloc_image(vk_image *image) {
    destroy_image(image);

    image->internal_format = texture_format_to_vk_format(image->props.format);

    if (!create_vk_sampler(image)) {
        log_error("Unable to create sampler");
        return false;
    }

    VkImageUsageFlags usage_flags = VK_IMAGE_USAGE_SAMPLED_BIT;
    if (image->props.format == FMT_DEPTH) {
        usage_flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    } else {
        usage_flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }

    VkImageCreateInfo image_info = {
        .sType     = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext     = NULL,
        .flags     = (image->props.type == TT_CUBIC ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0),
        .imageType = VK_IMAGE_TYPE_2D,
        .format    = image->internal_format,
        .extent    = {
            .width  = image->props.width,
            .height = image->props.height,
            .depth  = 1
        },
        .mipLevels             = image->props.num_levels,
        .arrayLayers           = image->props.type == TT_CUBIC ? 6 : 1,
        .samples               = (VkSampleCountFlagBits) image->props.samples,
        .tiling                = VK_IMAGE_TILING_OPTIMAL,
        .usage                 = usage_flags,
        .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices   = NULL,
        .initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED
    };

    CHECK_VK(vk_CreateImage(context.device, &image_info, NULL, &image->image));

    VkMemoryRequirements memory_requirements;
    vk_GetImageMemoryRequirements(context.device, image->image, &memory_requirements);

    bool success = vk_allocate(&image->allocation, memory_requirements.size, memory_requirements.alignment,
        memory_requirements.memoryTypeBits, VULKAN_MEMORY_USAGE_GPU_ONLY, VULKAN_ALLOCATION_TYPE_IMAGE_OPTIMAL);
    if (!success) {
        log_error("Unable to allocate image");
        return false;
    }

    CHECK_VK(vk_BindImageMemory(context.device, image->image, image->allocation.device_memory,
        image->allocation.offset));

    VkImageViewCreateInfo view_info = {
        .sType      = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext      = NULL,
        .flags      = 0,
        .image      = image->image,
        .viewType   = image->props.type == TT_CUBIC ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_2D,
        .format     = image->internal_format,
        .components = texture_format_to_component_mapping(image->props.format),
        .subresourceRange = {
            .aspectMask     = image->props.format == FMT_DEPTH ?
                VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT : VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel   = 0,
            .levelCount     = image->props.num_levels,
            .baseArrayLayer = 0,
            .layerCount     = image->props.type == TT_CUBIC ? 6 : 1
        }
    };

    CHECK_VK(vk_CreateImageView(context.device, &view_info, NULL, &image->view));

    return true;
}

bool sub_image_upload(vk_image *image, size_t mip_level, size_t x, size_t y, size_t z,
    size_t width, size_t height, void *picture, size_t pixel_pitch)
{
    if (pixel_pitch >= image->props.num_levels) {
        log_error("Invalid pixel pitch, it must be < than the number of levels");
        return false;
    }

    size_t size = width * height * (bit_count_image_format(image->props.format) / 8);

    VkBuffer buffer;
    VkCommandBuffer command_buffer;
    VkDeviceSize offset = 0;
    byte *data = vk_stage(size, 16, &command_buffer, &buffer, &offset);
    if (image->props.format == FMT_RGB565) {
        byte *img_data = (byte*) picture;
        for (size_t i = 0; i < size; i += 2) {
            data[i] = img_data[i + 1];
            data[i + 1] = img_data[i];
        }
    } else {
        mem_copy(data, picture, size);
    }

    VkBufferImageCopy img_copy = {
        .bufferOffset = offset,
        .bufferRowLength = pixel_pitch,
        .bufferImageHeight = height,
        .imageSubresource = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = mip_level,
            .baseArrayLayer = z,
            .layerCount = 1
        },
        .imageOffset = {
            .x = x,
            .y = y,
            .z = 0
        },
        .imageExtent = {
            .width = width,
            .height = height,
            .depth = 1
        }
    };

    VkImageMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext = NULL,
        .srcAccessMask = 0,
        .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image->image,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = image->props.num_levels,
            .baseArrayLayer = z,
            .layerCount = 1
        }
    };

    vk_CmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0,
        NULL, 0, NULL, 1, &barrier);
    vk_CmdCopyBufferToImage(command_buffer, buffer, image->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &img_copy);

    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    vk_CmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 0, 0,
        NULL, 0, NULL, 1, &barrier);

    image->layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    return true;
}

void destroy_image(vk_image *image) {
    if (image->is_swapchain_image) {
        return;
    }
    if (image->image) {
        vk_DestroyImage(context.device, image->image, NULL);
    }
    if (image->view) {
        vk_DestroyImageView(context.device, image->view, NULL);
    }
    if (image->sampler) {
        vk_DestroySampler(context.device, image->sampler, NULL);
    }
}
