#include "./pipeline_state.h"

#include "../render_state.h"
#include "../../vulkan/context.h"
#include "../../vulkan/gpu_info.h"

VkPipelineRasterizationStateCreateInfo get_rasterization_state_from_pipeline_bits(uint64_t state_bits) {
    VkCullModeFlagBits cull_mode = VK_CULL_MODE_NONE;
    switch (state_bits & RST_CULL_BITS) {
        case RST_CULL_TWOSIDED:
            cull_mode = VK_CULL_MODE_NONE;
            break;
        case RST_CULL_BACKSIDED:
            cull_mode = state_bits & RST_MIRROR_VIEW ? VK_CULL_MODE_FRONT_BIT : VK_CULL_MODE_BACK_BIT;
            break;
        case RST_CULL_FRONTSIDED:
            cull_mode = state_bits & RST_MIRROR_VIEW ? VK_CULL_MODE_BACK_BIT : VK_CULL_MODE_FRONT_BIT;
            break;
        default:
            cull_mode = VK_CULL_MODE_NONE;
            break;
    }

    log_info("Cuck: %lu %lu", RST_CULL_FRONTSIDED, state_bits & RST_CULL_BITS);

    VkPipelineRasterizationStateCreateInfo rasterization_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = cull_mode,
        .frontFace = state_bits & RST_CLOCKWISE ? VK_FRONT_FACE_CLOCKWISE : VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .depthBiasEnable = (state_bits & RST_POLYGON_OFFSET) != 0,
        .depthBiasConstantFactor = 0,
        .depthBiasClamp = 0,
        .depthBiasSlopeFactor = 0,
        .lineWidth = 1.0f
    };
    return rasterization_info;
}

VkPipelineColorBlendAttachmentState get_color_blend_attachment_from_pipeline_bits(uint64_t state_bits) {
    VkBlendFactor src_factor = VK_BLEND_FACTOR_ONE;
    switch (state_bits * RST_BLENDOP_BITS) {
        case RST_SRCBLEND_ZERO:
            src_factor = VK_BLEND_FACTOR_ZERO;
            break;
        case RST_SRCBLEND_ONE:
            src_factor = VK_BLEND_FACTOR_ONE;
            break;
        case RST_SRCBLEND_DST_COLOR:
            src_factor = VK_BLEND_FACTOR_DST_COLOR;
            break;
        case RST_SRCBLEND_ONE_MINUS_DST_COLOR:
            src_factor = VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
            break;
        case RST_SRCBLEND_SRC_ALPHA:
            src_factor = VK_BLEND_FACTOR_SRC_ALPHA;
            break;
        case RST_SRCBLEND_ONE_MINUS_SRC_ALPHA:
            src_factor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            break;
        case RST_SRCBLEND_DST_ALPHA:
            src_factor = VK_BLEND_FACTOR_DST_ALPHA;
            break;
        case RST_SRCBLEND_ONE_MINUS_DST_ALPHA:
            src_factor = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
            break;
        default:
            src_factor = VK_BLEND_FACTOR_ONE;
            break;
    }

    VkBlendFactor dst_factor = VK_BLEND_FACTOR_ZERO;
    switch (state_bits & RST_DSTBLEND_BITS) {
        case RST_DSTBLEND_ZERO:
            dst_factor  = VK_BLEND_FACTOR_ZERO;
            break;
        case RST_DSTBLEND_ONE:
            dst_factor  = VK_BLEND_FACTOR_ONE;
            break;
        case RST_DSTBLEND_SRC_COLOR:
            dst_factor  = VK_BLEND_FACTOR_SRC_COLOR;
            break;
        case RST_DSTBLEND_ONE_MINUS_SRC_COLOR:
            dst_factor  = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
            break;
        case RST_DSTBLEND_SRC_ALPHA:
            dst_factor  = VK_BLEND_FACTOR_SRC_ALPHA;
            break;
        case RST_DSTBLEND_ONE_MINUS_SRC_ALPHA:
            dst_factor  = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            break;
        case RST_DSTBLEND_DST_ALPHA:
            dst_factor  = VK_BLEND_FACTOR_DST_ALPHA;
            break;
        case RST_DSTBLEND_ONE_MINUS_DST_ALPHA:
            dst_factor  = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
            break;
        default:
            dst_factor = VK_BLEND_FACTOR_ZERO;
            break;
    }

    VkBlendOp blend_op = VK_BLEND_OP_ADD;
    switch (state_bits & RST_BLENDOP_BITS) {
        case RST_BLENDOP_MIN:
            blend_op = VK_BLEND_OP_MIN;
            break;
        case RST_BLENDOP_MAX:
            blend_op = VK_BLEND_OP_MAX;
            break;
        case RST_BLENDOP_ADD:
            blend_op = VK_BLEND_OP_ADD;
            break;
        case RST_BLENDOP_SUB:
            blend_op = VK_BLEND_OP_SUBTRACT;
            break;
        default:
            blend_op = VK_BLEND_OP_ADD;
            break;
    }

    VkColorComponentFlags color_mask = 0;
    color_mask = 0;
    color_mask |= state_bits & RST_REDMASK   ? 0 : VK_COLOR_COMPONENT_R_BIT;
    color_mask |= state_bits & RST_GREENMASK ? 0 : VK_COLOR_COMPONENT_G_BIT;
    color_mask |= state_bits & RST_BLUEMASK  ? 0 : VK_COLOR_COMPONENT_B_BIT;
    color_mask |= state_bits & RST_ALPHAMASK ? 0 : VK_COLOR_COMPONENT_A_BIT;


    VkPipelineColorBlendAttachmentState attachment_state = {
        .blendEnable =  src_factor != VK_BLEND_FACTOR_ONE || dst_factor != VK_BLEND_FACTOR_ZERO,
        .srcColorBlendFactor = src_factor,
        .dstColorBlendFactor = dst_factor,
        .colorBlendOp = blend_op,
        .srcAlphaBlendFactor = src_factor,
        .alphaBlendOp = blend_op,
        .colorWriteMask = color_mask
    };
    return attachment_state;
}

static VkStencilOpState get_stecil_op_state(uint64_t stencil_bits) {
    VkStencilOp fail_op = VK_STENCIL_OP_KEEP;
    switch (stencil_bits & RST_STENCIL_OP_FAIL_BITS) {
        case RST_STENCIL_OP_FAIL_KEEP:
            fail_op = VK_STENCIL_OP_KEEP;
            break;
        case RST_STENCIL_OP_FAIL_ZERO:
            fail_op = VK_STENCIL_OP_ZERO;
            break;
        case RST_STENCIL_OP_FAIL_REPLACE:
            fail_op = VK_STENCIL_OP_REPLACE;
            break;
        case RST_STENCIL_OP_FAIL_INCR:
            fail_op = VK_STENCIL_OP_INCREMENT_AND_CLAMP;
            break;
        case RST_STENCIL_OP_FAIL_DECR:
            fail_op = VK_STENCIL_OP_DECREMENT_AND_CLAMP;
            break;
        case RST_STENCIL_OP_FAIL_INVERT:
            fail_op = VK_STENCIL_OP_INVERT;
            break;
        case RST_STENCIL_OP_FAIL_INCR_WRAP:
            fail_op = VK_STENCIL_OP_INCREMENT_AND_WRAP;
            break;
        case RST_STENCIL_OP_FAIL_DECR_WRAP:
            fail_op = VK_STENCIL_OP_DECREMENT_AND_WRAP;
            break;
        default:
            fail_op = VK_STENCIL_OP_KEEP;
            break;
    }

    VkStencilOp depth_fail_op = VK_STENCIL_OP_KEEP;
    switch (stencil_bits & RST_STENCIL_OP_ZFAIL_BITS) {
        case RST_STENCIL_OP_ZFAIL_KEEP:
            depth_fail_op = VK_STENCIL_OP_KEEP;
            break;
        case RST_STENCIL_OP_ZFAIL_ZERO:
            depth_fail_op = VK_STENCIL_OP_ZERO;
            break;
        case RST_STENCIL_OP_ZFAIL_REPLACE:
            depth_fail_op = VK_STENCIL_OP_REPLACE;
            break;
        case RST_STENCIL_OP_ZFAIL_INCR:
            depth_fail_op = VK_STENCIL_OP_INCREMENT_AND_CLAMP;
            break;
        case RST_STENCIL_OP_ZFAIL_DECR:
            depth_fail_op = VK_STENCIL_OP_DECREMENT_AND_CLAMP;
            break;
        case RST_STENCIL_OP_ZFAIL_INVERT:
            depth_fail_op = VK_STENCIL_OP_INVERT;
            break;
        case RST_STENCIL_OP_ZFAIL_INCR_WRAP:
            depth_fail_op = VK_STENCIL_OP_INCREMENT_AND_WRAP;
            break;
        case RST_STENCIL_OP_ZFAIL_DECR_WRAP:
            depth_fail_op = VK_STENCIL_OP_DECREMENT_AND_WRAP;
            break;
        default:
            depth_fail_op = VK_STENCIL_OP_KEEP;
            break;
    }

    VkStencilOp pass_op = VK_STENCIL_OP_KEEP;
    switch (stencil_bits & RST_STENCIL_OP_PASS_BITS) {
        case RST_STENCIL_OP_PASS_KEEP:
            pass_op = VK_STENCIL_OP_KEEP;
            break;
        case RST_STENCIL_OP_PASS_ZERO:
            pass_op = VK_STENCIL_OP_ZERO;
            break;
        case RST_STENCIL_OP_PASS_REPLACE:
            pass_op = VK_STENCIL_OP_REPLACE;
            break;
        case RST_STENCIL_OP_PASS_INCR:
            pass_op = VK_STENCIL_OP_INCREMENT_AND_CLAMP;
            break;
        case RST_STENCIL_OP_PASS_DECR:
            pass_op = VK_STENCIL_OP_DECREMENT_AND_CLAMP;
            break;
        case RST_STENCIL_OP_PASS_INVERT:
            pass_op = VK_STENCIL_OP_INVERT;
            break;
        case RST_STENCIL_OP_PASS_INCR_WRAP:
            pass_op = VK_STENCIL_OP_INCREMENT_AND_WRAP;
            break;
        case RST_STENCIL_OP_PASS_DECR_WRAP:
            pass_op = VK_STENCIL_OP_DECREMENT_AND_WRAP;
            break;
        default:
            pass_op = VK_STENCIL_OP_KEEP;
            break;
    }

    VkStencilOpState state = {
        .failOp = fail_op,
        .passOp = pass_op,
        .depthFailOp = depth_fail_op,
        .compareOp = VK_COMPARE_OP_NEVER,
        .compareMask = 0,
        .writeMask = 0,
        .reference = 0
    };

    return state;
}


VkPipelineDepthStencilStateCreateInfo get_depth_stencil_info_from_pipeline_bits(uint64_t state_bits) {
    gpu_info *gpu = &context.gpus[context.selected_gpu];

    VkCompareOp depth_compare_op = VK_COMPARE_OP_ALWAYS;
    switch (state_bits & RST_DEPTHFUNC_BITS) {
        case RST_DEPTHFUNC_EQUAL:
            depth_compare_op = VK_COMPARE_OP_EQUAL;
            break;
        case RST_DEPTHFUNC_ALWAYS:
            depth_compare_op = VK_COMPARE_OP_ALWAYS;
            break;
        case RST_DEPTHFUNC_LESS:
            depth_compare_op = VK_COMPARE_OP_LESS_OR_EQUAL;
            break;
        case RST_DEPTHFUNC_GREATER:
            depth_compare_op = VK_COMPARE_OP_GREATER_OR_EQUAL;
            break;
        default:
            depth_compare_op = VK_COMPARE_OP_ALWAYS;
            break;
    }

    VkCompareOp stencil_compare_op = VK_COMPARE_OP_ALWAYS;
    switch (state_bits & RST_STENCIL_FUNC_BITS) {
        case RST_STENCIL_FUNC_NEVER:
            stencil_compare_op = VK_COMPARE_OP_NEVER;
            break;
        case RST_STENCIL_FUNC_LESS:
            stencil_compare_op = VK_COMPARE_OP_LESS;
            break;
        case RST_STENCIL_FUNC_EQUAL:
            stencil_compare_op = VK_COMPARE_OP_EQUAL;
            break;
        case RST_STENCIL_FUNC_LEQUAL:
            stencil_compare_op = VK_COMPARE_OP_LESS_OR_EQUAL;
            break;
        case RST_STENCIL_FUNC_GREATER:
            stencil_compare_op = VK_COMPARE_OP_GREATER;
            break;
        case RST_STENCIL_FUNC_NOTEQUAL:
            stencil_compare_op = VK_COMPARE_OP_NOT_EQUAL;
            break;
        case RST_STENCIL_FUNC_GEQUAL:
            stencil_compare_op = VK_COMPARE_OP_GREATER_OR_EQUAL;
            break;
        case RST_STENCIL_FUNC_ALWAYS:
            stencil_compare_op = VK_COMPARE_OP_ALWAYS;
            break;
        default:
            stencil_compare_op = VK_COMPARE_OP_ALWAYS;
            break;
    }

    uint32_t ref =  (((uint32_t) (state_bits & RST_STENCIL_FUNC_REF_BITS)) >> RST_STENCIL_FUNC_REF_SHIFT);
    uint32_t mask = (((uint32_t) (state_bits & RST_STENCIL_FUNC_MASK_BITS)) >> RST_STENCIL_FUNC_MASK_SHIFT);

    VkStencilOpState front_stencil = get_stecil_op_state(state_bits);
    front_stencil.writeMask = 0xFFFFFFFF;
    front_stencil.compareOp = stencil_compare_op;
    front_stencil.compareMask = mask;
    front_stencil.reference = ref;
    VkStencilOpState back_stencil = front_stencil;

    if (state_bits & RST_SEPARATE_STENCIL) {
        front_stencil = get_stecil_op_state(state_bits & RST_STENCIL_FRONT_OPS);
        front_stencil.writeMask = 0xFFFFFFFF;
        front_stencil.compareOp = stencil_compare_op;
        front_stencil.compareMask = mask;
        front_stencil.reference = ref;

        back_stencil = get_stecil_op_state((state_bits & RST_STENCIL_BACK_OPS) >> 12);
        back_stencil.writeMask = 0xFFFFFFFF;
        back_stencil.compareOp = stencil_compare_op;
        back_stencil.compareMask = mask;
        back_stencil.reference = ref;
    }

    VkPipelineDepthStencilStateCreateInfo depth_stencil_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = (state_bits & RST_DEPTHMASK) == 0,
        .depthCompareOp = depth_compare_op,
        .depthBoundsTestEnable = gpu->features.depthBounds && (state_bits & RST_DEPTHMASK) == 0,
        .stencilTestEnable =  (state_bits & (RST_STENCIL_FUNC_BITS | RST_STENCIL_OP_BITS)) != 0,
        .front = front_stencil,
        .back = back_stencil,
        .minDepthBounds = gpu->features.depthBounds ? 0.0f : 0,
        .maxDepthBounds = gpu->features.depthBounds ? 1.0f : 0
    };

    return depth_stencil_info;
}

VkPipelineMultisampleStateCreateInfo get_multisample_info() {
    VkPipelineMultisampleStateCreateInfo multisample_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .rasterizationSamples = context.sample_count,
        .sampleShadingEnable = context.supersampling,
        .minSampleShading = context.supersampling ? 1.0f : 0,
        .pSampleMask = NULL,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE
    };

    return multisample_info;
}

size_t get_dynamic_states_from_pipeline_bits(VkDynamicState dest[MAX_PIPELINE_DYNAMIC_STATES_SIZE],
    uint64_t state_bits)
{
    gpu_info *gpu = &context.gpus[context.selected_gpu];

    size_t i = 0;
    dest[i++] = VK_DYNAMIC_STATE_SCISSOR;
    dest[i++] = VK_DYNAMIC_STATE_VIEWPORT;
    if (state_bits & RST_POLYGON_OFFSET) {
        dest[i++] = VK_DYNAMIC_STATE_DEPTH_BIAS;
    }
    if (gpu->features.depthBounds && (state_bits & RST_DEPTH_TEST_MASK)) {
        dest[i++] = VK_DYNAMIC_STATE_DEPTH_BOUNDS;
    }

    return i;
}
