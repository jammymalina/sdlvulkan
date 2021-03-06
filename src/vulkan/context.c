#include "./context.h"

#include <SDL2/SDL_vulkan.h>
#include <stdint.h>
#include "./functions/functions.h"
#include "./functions/function_loader.h"
#include "./memory/memory.h"
#include "./memory/staging.h"
#include "./tools/tools.h"
#include "../utils/heap.h"
#include "../logger/logger.h"
#include "../window/config.h"
#include "../renderer/config.h"
#include "../renderer/shaders/shader_manager.h"
#include "../vertex_management/vertex_manager.h"

#ifdef DEBUG
    #include "./debug.h"
#endif

vk_context context;

void init_vk_context(vk_context *ctx) {
    ctx->instance = VK_NULL_HANDLE;
    ctx->surface = VK_NULL_HANDLE;
    ctx->device = VK_NULL_HANDLE;
    ctx->graphics_queue = VK_NULL_HANDLE;
    ctx->present_queue = VK_NULL_HANDLE;
    ctx->command_pool = VK_NULL_HANDLE;
    ctx->swapchain = VK_NULL_HANDLE;
    ctx->extent.width = ctx->extent.height = 0;
    for (size_t i = 0; i < NUM_FRAME_DATA; i++) {
        ctx->acquire_semaphores[i] = VK_NULL_HANDLE;
        ctx->render_complete_semaphores[i] = VK_NULL_HANDLE;
        ctx->command_buffers[i] = VK_NULL_HANDLE;
        ctx->command_buffer_fences[i] = VK_NULL_HANDLE;
        ctx->swapchain_images[i] = VK_NULL_HANDLE;
        ctx->swapchain_views[i] = VK_NULL_HANDLE;
        ctx->framebuffers[i] = VK_NULL_HANDLE;
        ctx->query_pools[i] = VK_NULL_HANDLE;
    }
    ctx->render_pass = VK_NULL_HANDLE;
    ctx->gpus = NULL;
    ctx->gpus_size = 0;
    ctx->supersampling = false;
    ctx->sample_count = VK_SAMPLE_COUNT_1_BIT;
    ctx->pipeline_cache = VK_NULL_HANDLE;
    #ifdef DEBUG
        ctx->debug_callback = VK_NULL_HANDLE;
    #endif
}

static bool create_instance(vk_context *ctx, SDL_Window *window) {
    const char **extensions = NULL;
    unsigned extension_count = 0;

     if(!SDL_Vulkan_GetInstanceExtensions(window, &extension_count, NULL)) {
        log_error("SDL_Vulkan_GetInstanceExtensions(): %s", SDL_GetError());
        return false;
    }

    uint32_t debug_extension_count = 0;
    #ifdef DEBUG
        debug_extension_count = 1;
    #endif

    extensions = mem_alloc(sizeof(const char*) * (extension_count + debug_extension_count));
    CHECK_ALLOC(extensions, "Allocation fail");

    if (!SDL_Vulkan_GetInstanceExtensions(window, &extension_count, extensions)) {
        mem_free(extensions);
        log_error("SDL_Vulkan_GetInstanceExtensions(): %s", SDL_GetError());
        return false;
    }

    // push debug extension + enable validation layers
    #ifdef DEBUG
        extension_count += debug_extension_count;

        const char *validation_layer_names[] = { "VK_LAYER_LUNARG_standard_validation" };
        uint32_t validation_layers_size = 1;

        if (!check_validation_layers(validation_layer_names, validation_layers_size)) {
            log_error("Unavailable validation layers");
            validation_layers_size = 0;
        }

        extensions[extension_count - 1] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
    #else
        const char **validation_layer_names = NULL;
        uint32_t validation_layers_size = 0;
    #endif

    VkApplicationInfo vk_application_info = {
        .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext              = NULL,
        .pApplicationName   = "SDL test app",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName        = "Custom engine",
        .engineVersion      = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion         = VK_API_VERSION_1_1
    };

    VkInstanceCreateInfo vk_instance_create_info = {
        .sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext                   = NULL,
        .flags                   = 0,
        .pApplicationInfo        = &vk_application_info,
        .enabledLayerCount       = validation_layers_size,
        .ppEnabledLayerNames     = validation_layer_names,
        .enabledExtensionCount   = extension_count,
        .ppEnabledExtensionNames = extensions
    };

    VkResult result = vkCreateInstance(&vk_instance_create_info, NULL, &ctx->instance);
    if (result != VK_SUCCESS || ctx->instance == VK_NULL_HANDLE) {
        ctx->instance = VK_NULL_HANDLE;
        mem_free(extensions);
        log_error("Could not create Vulkan instance: %s", vulkan_result_to_string(result));
        return false;
    }

    mem_free(extensions);

    return true;
}

#ifdef DEBUG
    bool setup_debug_callback(vk_context *ctx) {
        VkDebugReportCallbackCreateInfoEXT debug_callback_info = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
            .pNext = NULL,
            .flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT,
            .pfnCallback = vulkan_debug_callback,
            .pUserData = NULL
        };
        CHECK_VK(vkCreateDebugReportCallbackEXT(ctx->instance, &debug_callback_info, NULL, &ctx->debug_callback));
        return true;
    }
#endif

static bool create_surface(vk_context *ctx, SDL_Window *window) {
    if (!SDL_Vulkan_CreateSurface(window, ctx->instance, &ctx->surface)) {
        log_error("SDL_Vulkan_CreateSurface(): %s", SDL_GetError());
        return false;
    }
    return true;
}

static bool init_vulkan_function_loader() {
    PFN_vkGetInstanceProcAddr vk_get_proc = SDL_Vulkan_GetVkGetInstanceProcAddr();
    if(!vk_get_proc) {
        log_error("SDL_Vulkan_GetVkGetInstanceProcAddr error: %s", SDL_GetError());
        return false;
    }
    return load_external_function(vk_get_proc);
}

static bool enumerate_physical_devices(vk_context *ctx) {
    uint32_t num_physical_devices = 0;
    VkPhysicalDevice physical_devices[MAX_PHYSICAL_DEVICES];

    CHECK_VK(vkEnumeratePhysicalDevices(ctx->instance, &num_physical_devices, NULL));
    CHECK_VK_VAL(num_physical_devices > 0, "No physical_devices");
    CHECK_VK_VAL(num_physical_devices <= MAX_PHYSICAL_DEVICES, "Not enough space for physical devices");

    CHECK_VK(vkEnumeratePhysicalDevices(ctx->instance, &num_physical_devices, physical_devices));
    CHECK_VK_VAL(num_physical_devices > 0, "No physical devices");

    ctx->gpus = mem_alloc(num_physical_devices * sizeof(gpu_info));
    CHECK_ALLOC(ctx->gpus, "Allocation fail");

    for (size_t i = 0; i < num_physical_devices; i++) {
        init_gpu_info(&ctx->gpus[i]);
        bool ret = init_gpu_info_props(&ctx->gpus[i], physical_devices[i], ctx->surface);
        if (!ret) {
            log_error("Could not init gpu %zu properties", i);
            return false;
        }
    }

    ctx->gpus_size = num_physical_devices;
    log_info("Number of gpus found: %d", ctx->gpus_size);

    return true;
}

static bool choose_suitable_graphics_gpu(vk_context *ctx) {
    int maxScore = -1;
    for (uint32_t i = 0; i < ctx->gpus_size; i++) {
        if (is_gpu_suitable_for_graphics(&ctx->gpus[i], ctx->surface,
            &ctx->graphics_family_index, &ctx->present_family_index))
        {
            int currentScore = rate_gpu(&ctx->gpus[i]);
            log_info("Found suitable gpu, score: %d", currentScore);
            if (currentScore > maxScore) {
                ctx->selected_gpu = i;
                maxScore = currentScore;
            }
        }
    }

    return maxScore != -1;
}

static bool create_device(vk_context *ctx) {
    uint32_t indices[] = { ctx->graphics_family_index, ctx->present_family_index };

    VkDeviceQueueCreateInfo devq_info[2];
    uint32_t queue_count = indices[0] == indices[1] ? 1 : 2;
    const float priority = 1.0f;
    for (uint32_t i = 0; i < queue_count; i++) {
        VkDeviceQueueCreateInfo qinfo = {
            .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext            = NULL,
            .queueFamilyIndex = indices[i],
            .queueCount       = 1,
            .pQueuePriorities = &priority
        };
        devq_info[i] = qinfo;
    }

    gpu_info *gpu = &ctx->gpus[ctx->selected_gpu];

    VkPhysicalDeviceFeatures device_features = {};
    device_features.textureCompressionBC = VK_TRUE;
    device_features.imageCubeArray       = VK_TRUE;
    device_features.depthClamp           = VK_TRUE;
    device_features.depthBiasClamp       = VK_TRUE;
    device_features.depthBounds          = gpu->features.depthBounds;
    device_features.fillModeNonSolid     = VK_TRUE;

    VkDeviceCreateInfo info = {
        .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext                   = NULL,
        .flags                   = 0,
        .queueCreateInfoCount    = queue_count,
        .pQueueCreateInfos       = devq_info,
        .enabledLayerCount       = 0,
        .ppEnabledLayerNames     = NULL,
        .enabledExtensionCount   = GRAPHICS_DEVICE_EXTENSIONS_SIZE,
        .ppEnabledExtensionNames = GRAPHICS_DEVICE_EXTENSIONS,
        .pEnabledFeatures        = &device_features
    };

    CHECK_VK(vkCreateDevice(gpu->device, &info, NULL, &ctx->device));

    return true;
}

static bool init_queues(vk_context *ctx) {
    vkGetDeviceQueue(ctx->device, ctx->graphics_family_index, 0, &ctx->graphics_queue);
    if (ctx->graphics_family_index == ctx->present_family_index) {
        ctx->present_queue = ctx->graphics_queue;
    } else {
        vkGetDeviceQueue(ctx->device, ctx->present_family_index, 0, &ctx->present_queue);
    }

    return true;
}

static bool create_semaphores(vk_context *ctx) {
    VkSemaphoreCreateInfo sempahore_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0
    };
    for (size_t i = 0; i < NUM_FRAME_DATA; i++) {
        CHECK_VK(vkCreateSemaphore(ctx->device, &sempahore_info, NULL, &ctx->acquire_semaphores[i]));
        CHECK_VK(vkCreateSemaphore(ctx->device, &sempahore_info, NULL, &ctx->render_complete_semaphores[i]));
    }
    return true;
}

static bool create_query_pools(vk_context *ctx) {
    VkQueryPoolCreateInfo query_pool_info = {
        .sType              = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO,
        .pNext              = NULL,
        .flags              = 0,
        .queryType          = VK_QUERY_TYPE_TIMESTAMP,
        .queryCount         = NUM_TIMESTAMP_QUERIES,
        .pipelineStatistics = 0
    };

    for (size_t i = 0; i < NUM_FRAME_DATA; i++) {
        CHECK_VK(vkCreateQueryPool(ctx->device, &query_pool_info, NULL, &ctx->query_pools[i]));
    }

    return true;
}

static bool create_command_pool(vk_context *ctx) {
    VkCommandPoolCreateInfo pool_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = ctx->graphics_family_index
    };

    CHECK_VK(vkCreateCommandPool(ctx->device, &pool_info, NULL, &ctx->command_pool));

    return true;
}

static bool create_command_buffers(vk_context *ctx) {
    VkCommandBufferAllocateInfo allocate_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .commandPool = ctx->command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = NUM_FRAME_DATA
    };

    CHECK_VK(vkAllocateCommandBuffers(ctx->device, &allocate_info, ctx->command_buffers));

    VkFenceCreateInfo fence_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0
    };

    for (size_t i = 0; i < NUM_FRAME_DATA; i++) {
        CHECK_VK(vkCreateFence(ctx->device, &fence_info, NULL, &ctx->command_buffer_fences[i]));
    }

    return true;
}

static bool create_swapchain(vk_context *ctx) {
    gpu_info *gpu = &ctx->gpus[ctx->selected_gpu];

    VkSurfaceFormatKHR surface_format;
    VkPresentModeKHR present_mode;
    VkExtent2D extent, window_size = {
        .width = window_config.width,
        .height = window_config.height
    };
    bool found_sf = choose_surface_format(gpu, &surface_format);
    bool found_pm = choose_present_mode(gpu, &present_mode);
    bool found_ex = choose_extent(gpu, &extent, &window_size);

    if (!(found_sf && found_pm && found_ex)) {
        return false;
    }

    VkSharingMode sharing_mode = ctx->graphics_family_index == ctx->present_family_index ?
        VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
    uint32_t indices[] = { ctx->graphics_family_index, ctx->present_family_index };
    uint32_t queue_count = ctx->graphics_family_index == ctx->present_family_index ? 0 : 2;

    VkSwapchainCreateInfoKHR swapchain_info = {
        .sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext                 = NULL,
        .flags                 = 0,
        .surface               = ctx->surface,
        .minImageCount         = NUM_FRAME_DATA,
        .imageFormat           = surface_format.format,
        .imageColorSpace       = surface_format.colorSpace,
        .imageExtent           = extent,
        .imageArrayLayers      = 1,
        .imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
        .imageSharingMode      = sharing_mode,
        .queueFamilyIndexCount = queue_count,
        .pQueueFamilyIndices   = ctx->graphics_family_index == ctx->present_family_index ? NULL : indices,
        .preTransform          = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode           = present_mode,
        .clipped               = VK_TRUE,
        .oldSwapchain          = ctx->swapchain
    };

    CHECK_VK(vkCreateSwapchainKHR(ctx->device, &swapchain_info, NULL, &ctx->swapchain));

    ctx->surface_format = surface_format;
    ctx->present_mode = present_mode;
    ctx->extent = extent;

    return true;
}

static bool get_depth_format(vk_context *ctx) {
    gpu_info *gpu = &ctx->gpus[ctx->selected_gpu];

    VkFormat formats[] = {
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT
    };
    // Make sure to check it supports optimal tiling and is a depth/stencil format.
    bool success = choose_supported_format(gpu, &ctx->depth_format, formats, 2, VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    if (!success) {
        log_error("No suitable depth format");
        return false;
    }

    return true;
}

static bool create_render_targets(vk_context *ctx) {
    uint32_t num_images = 0;
    CHECK_VK(vkGetSwapchainImagesKHR(ctx->device, ctx->swapchain, &num_images, NULL));
    CHECK_VK_VAL(num_images > 0, "No swapchain images");

    CHECK_VK(vkGetSwapchainImagesKHR(ctx->device, ctx->swapchain, &num_images, ctx->swapchain_images));
    CHECK_VK_VAL(num_images > 0, "No swapchain images");

    for (size_t i = 0; i < NUM_FRAME_DATA; i++) {
        VkImageViewCreateInfo image_view_info = {
            .sType      = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext      = NULL,
            .flags      = 0,
            .image      = ctx->swapchain_images[i],
            .viewType   = VK_IMAGE_VIEW_TYPE_2D,
            .format     = ctx->surface_format.format,
            .components = {
                .r = VK_COMPONENT_SWIZZLE_R,
                .g = VK_COMPONENT_SWIZZLE_G,
                .b = VK_COMPONENT_SWIZZLE_B,
                .a = VK_COMPONENT_SWIZZLE_A
            },
            .subresourceRange = {
                .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel   = 0,
                .levelCount     = 1,
                .baseArrayLayer = 0,
                .layerCount     = 1
            }
        };

        CHECK_VK(vkCreateImageView(ctx->device, &image_view_info, NULL, &ctx->swapchain_views[i]));
    }

    gpu_info *gpu = &ctx->gpus[ctx->selected_gpu];
    VkImageFormatProperties fmt_props = {};
    vkGetPhysicalDeviceImageFormatProperties(gpu->device, ctx->surface_format.format,
        VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, 0, &fmt_props);

    if (render_config.desired_sample_count >= 16 && (fmt_props.sampleCounts & VK_SAMPLE_COUNT_16_BIT)) {
        ctx->sample_count = VK_SAMPLE_COUNT_16_BIT;
    } else if (render_config.desired_sample_count >= 8 && (fmt_props.sampleCounts & VK_SAMPLE_COUNT_8_BIT)) {
        ctx->sample_count = VK_SAMPLE_COUNT_8_BIT;
    } else if (render_config.desired_sample_count >= 4 && (fmt_props.sampleCounts & VK_SAMPLE_COUNT_4_BIT)) {
        ctx->sample_count = VK_SAMPLE_COUNT_4_BIT;
    } else if (render_config.desired_sample_count >= 2 && (fmt_props.sampleCounts & VK_SAMPLE_COUNT_2_BIT)) {
        ctx->sample_count = VK_SAMPLE_COUNT_2_BIT;
    }

    init_image(&ctx->depth_image);
    ctx->depth_image.props.format = FMT_DEPTH;
    ctx->depth_image.props.width = render_config.width;
    ctx->depth_image.props.height = render_config.height;
    ctx->depth_image.props.num_levels = 1;
    ctx->depth_image.props.samples = (texture_samples) ctx->sample_count;
    ctx->depth_image.props.repeat = TR_CLAMP;

    return alloc_image(&ctx->depth_image);
}

static bool create_render_pass(vk_context *ctx) {
    VkAttachmentDescription color_attachment = {
        .flags          = 0,
        .format         = ctx->surface_format.format,
        .samples        = VK_SAMPLE_COUNT_1_BIT,
        .loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };

    VkAttachmentDescription depth_attachment = {
        .flags          = 0,
        .format         = ctx->depth_format,
        .samples        = VK_SAMPLE_COUNT_1_BIT,
        .loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    VkAttachmentReference color_ref = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkAttachmentReference depth_ref = {
        .attachment = 1,
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    VkSubpassDescription subpass = {
        .flags                   = 0,
        .pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount    = 0,
        .pInputAttachments       = NULL,
        .colorAttachmentCount    = 1,
        .pColorAttachments       = &color_ref,
        .pResolveAttachments     = NULL,
        .pDepthStencilAttachment = &depth_ref,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments    = NULL
    };

    VkAttachmentDescription attachments[] = { color_attachment, depth_attachment };

    VkSubpassDependency subpass_dependencies[] = {
        {
            .srcSubpass      = VK_SUBPASS_EXTERNAL,
            .dstSubpass      = 0,
            .srcStageMask    = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            .dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask   = VK_ACCESS_MEMORY_READ_BIT,
            .dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
        },
        {
            .srcSubpass      = 0,
            .dstSubpass      = VK_SUBPASS_EXTERNAL,
            .srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstStageMask    = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            .srcAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dstAccessMask   = VK_ACCESS_MEMORY_READ_BIT,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
        }
    };

    VkRenderPassCreateInfo render_pass_info = {
        .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext           = NULL,
        .flags           = 0,
        .attachmentCount = 2,
        .pAttachments    = attachments,
        .subpassCount    = 1,
        .pSubpasses      = &subpass,
        .dependencyCount = 2,
        .pDependencies   = subpass_dependencies
    };

    CHECK_VK(vkCreateRenderPass(ctx->device, &render_pass_info, NULL, &ctx->render_pass));

    return true;
}

static bool create_framebuffers(vk_context *ctx) {
    for (size_t i = 0; i < NUM_FRAME_DATA; i++) {
        VkImageView attachments[] = { ctx->swapchain_views[i], ctx->depth_image.view };
        VkFramebufferCreateInfo framebuffer_info = {
            .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext           = NULL,
            .flags           = 0,
            .renderPass      = ctx->render_pass,
            .attachmentCount = 2,
            .pAttachments    = attachments,
            .width           = render_config.width,
            .height          = render_config.height,
            .layers          = 1
        };
        CHECK_VK(vkCreateFramebuffer(ctx->device, &framebuffer_info, NULL, &ctx->framebuffers[i]));
    }

    return true;
}

static bool create_pipeline_cache(vk_context *ctx) {
    VkPipelineCacheCreateInfo pipeline_cache_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .initialDataSize = 0,
        .pInitialData = NULL
    };

    CHECK_VK(vkCreatePipelineCache(ctx->device, &pipeline_cache_info, NULL, &ctx->pipeline_cache));

    return true;
}

bool init_vulkan(vk_context *ctx, SDL_Window *window) {
    init_vk_context(ctx);
    return init_vulkan_function_loader() &&
        load_global_functions() &&
        create_instance(ctx, window) &&
        load_instance_vulkan_functions(ctx->instance) &&

        #ifdef DEBUG
            setup_debug_callback(ctx) &&
        #endif

        create_surface(ctx, window) &&
        enumerate_physical_devices(ctx) &&
        choose_suitable_graphics_gpu(ctx) &&
        create_device(ctx) &&
        load_device_level_functions(ctx->device) &&
        init_queues(ctx) &&
        create_semaphores(ctx) &&
        create_query_pools(ctx) &&
        create_command_pool(ctx) &&
        create_command_buffers(ctx) &&
        vk_init_allocator() &&
        vk_init_stage_manager() &&
        create_swapchain(ctx) &&
        get_depth_format(ctx) &&
        create_render_targets(ctx) &&
        create_render_pass(ctx) &&
        create_pipeline_cache(ctx) &&
        create_framebuffers(ctx) &&
        init_ren_pm() &&
        init_vertex_cache();
}

void shutdown_vulkan(vk_context *ctx) {
    destroy_vertex_cache();
    destroy_ren_pm();
    vk_destroy_stage_manager();
    vk_destroy_allocator();
    if (vkDestroyFramebuffer) {
        for (size_t i = 0; i < NUM_FRAME_DATA; i++) {
            if (ctx->framebuffers[i]) {
                vkDestroyFramebuffer(ctx->device, ctx->framebuffers[i], NULL);
            }
        }
    }
    if (vkDestroyPipelineCache && ctx->pipeline_cache) {
        vkDestroyPipelineCache(ctx->device, ctx->pipeline_cache, NULL);
    }
    if (vkDestroyRenderPass && ctx->render_pass) {
        vkDestroyRenderPass(ctx->device, ctx->render_pass, NULL);
    }
    destroy_image(&ctx->depth_image);
    if (vkDestroyImageView) {
        for (size_t i = 0; i < NUM_FRAME_DATA; i++) {
            if (ctx->swapchain_views[i]) {
                vkDestroyImageView(ctx->device, ctx->swapchain_views[i], NULL);
            }
        }
    }
    if (vkDestroySwapchainKHR && ctx->swapchain) {
        vkDestroySwapchainKHR(ctx->device, ctx->swapchain, NULL);
    }
    if (vkDestroyFence) {
        for (size_t i = 0; i < NUM_FRAME_DATA; i++) {
            if (ctx->command_buffer_fences[i]) {
                vkDestroyFence(ctx->device, ctx->command_buffer_fences[i], NULL);
            }
        }
    }
    if (vkFreeCommandBuffers && ctx->command_buffers[0]) {
        vkFreeCommandBuffers(ctx->device, ctx->command_pool, NUM_FRAME_DATA, ctx->command_buffers);
    }
    if (vkDestroyCommandPool && ctx->command_pool) {
        vkDestroyCommandPool(ctx->device, ctx->command_pool, NULL);
    }
    if (vkDestroyQueryPool) {
        for (size_t i = 0; i < NUM_FRAME_DATA; i++) {
            if (ctx->query_pools[i]) {
                vkDestroyQueryPool(ctx->device, ctx->query_pools[i], NULL);
            }
        }
    }
    if (vkDestroySemaphore) {
        for (size_t i = 0; i < NUM_FRAME_DATA; i++) {
            if (ctx->acquire_semaphores[i]) {
                vkDestroySemaphore(ctx->device, ctx->acquire_semaphores[i], NULL);
            }
            if (ctx->render_complete_semaphores[i]) {
                vkDestroySemaphore(ctx->device, ctx->render_complete_semaphores[i], NULL);
            }
        }
    }
    if (ctx->gpus_size > 0) {
        for (size_t i = 0; i < ctx->gpus_size; i++) {
            free_gpu_info(&ctx->gpus[i]);
        }
        mem_free(ctx->gpus);
    }
    if (ctx->device && vkDestroyDevice) {
        vkDestroyDevice(ctx->device, NULL);
    }
    if (ctx->surface && vkDestroySurfaceKHR) {
        vkDestroySurfaceKHR(ctx->instance, ctx->surface, NULL);
    }

    #ifdef DEBUG
        if (ctx->debug_callback && vkDestroyDebugReportCallbackEXT) {
            vkDestroyDebugReportCallbackEXT(ctx->instance, ctx->debug_callback, NULL);
        }
    #endif

    if (ctx->instance && vkDestroyInstance) {
        vkDestroyInstance(ctx->instance, NULL);
    }
    init_vk_context(ctx);
    SDL_Vulkan_UnloadLibrary();
}
