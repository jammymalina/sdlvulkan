#include "./image.h"
#include "./context.h"

VkFormat texture_format_to_vk_format(texture_format format) {
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


static VkComponentMapping texture_format_to_component_mapping(texture_format format) {
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