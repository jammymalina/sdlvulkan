#include "function_loader.h"

#include <stddef.h>
#include <stdbool.h>
#include "./functions.h"
#include "../../logger/logger.h"

#define EXPORTED_VULKAN_FUNCTION(name) PFN_##name name = NULL;
#define GLOBAL_LEVEL_VULKAN_FUNCTION(name) PFN_##name name = NULL;
#define INSTANCE_LEVEL_VULKAN_FUNCTION(name) PFN_##name name = NULL;
#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name) PFN_##name name = NULL;
#define DEVICE_LEVEL_VULKAN_FUNCTION(name) PFN_##name name = NULL;
#define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) PFN_##name name = NULL;

#include "list.inl"

bool load_external_function(PFN_vkGetInstanceProcAddr vk_get_proc) {
    if (!vk_get_proc) {
        return false;
    }
    vkGetInstanceProcAddr = vk_get_proc;
    return true;
}

bool load_global_functions() {
    #define GLOBAL_LEVEL_VULKAN_FUNCTION(name)                                \
        name = (PFN_##name) vkGetInstanceProcAddr(NULL, #name);               \
        if(name == NULL) {                                                    \
            log_error("Could not load global level function: " #name);        \
            return false;                                                     \
        } else {                                                              \
            log_debug("Successfully loaded global Vulkan function: " #name);  \
        }

    #include "list.inl"

    return true;
}

bool load_instance_vulkan_functions(VkInstance instance)
{
    #define INSTANCE_LEVEL_VULKAN_FUNCTION(name)                              \
        name = (PFN_##name) vkGetInstanceProcAddr(instance, #name);           \
        if (name == NULL) {                                                   \
            log_error("Could not load instance level function: " #name);      \
            return false;                                                     \
        } else {                                                              \
            log_debug("Successfully loaded instance level function: " #name); \
        }

    #define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name)               \
        name = (PFN_##name) vkGetInstanceProcAddr(instance, #name);           \
        if (name == NULL) {                                                   \
            log_error("Could not load instance level function: " #name);      \
            return false;                                                     \
        } else {                                                              \
            log_debug("Successfully loaded instance level function: " #name); \
        }

    #include "list.inl"

    return true;
}

bool load_device_level_functions(VkDevice device) {
    #define DEVICE_LEVEL_VULKAN_FUNCTION(name)                                \
        name = (PFN_##name) vkGetDeviceProcAddr(device, #name);               \
        if (name == NULL) {                                                   \
            log_error("Could not load device level function: " #name);        \
            return false;                                                     \
        } else {                                                              \
            log_debug("Successfully loaded device level function: " #name);   \
        }

    #define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension)      \
        name = (PFN_##name) vkGetDeviceProcAddr(device, #name);               \
        if (name == NULL) {                                                   \
            log_error("Could not load device level function: " #name);        \
            return false;                                                     \
        } else {                                                              \
            log_debug("Successfully loaded device level function: " #name);   \
        }                                                                     \

    #include "list.inl"

    return true;
}

