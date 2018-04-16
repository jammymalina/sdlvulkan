#include "./backend.h"

#include <vulkan/vulkan.h>
#include "../vulkan/functions/functions.h"
#include "../vulkan/tools/tools.h"
#include "../vulkan/context.h"

static bool start_frame(renderer *ren) {
    CHECK_VK(vk_AcquireNextImageKHR(context.device, context.swapchain, UINT64_MAX, 
        context.acquire_semaphores[ren->current_frame], VK_NULL_HANDLE, &ren->current_swap_index));
    
}
