#ifndef VULKAN_DEBUG_H
#define VULKAN_DEBUG_H

#include <vulkan/vulkan.h>
#include <stdint.h>
#include <stdbool.h>

bool check_validation_layers(const char **names, uint32_t names_size);

VkLayerProperties *get_available_validation_layers(uint32_t *layers_size);
bool is_validation_layer_available(const char *name, VkLayerProperties *available_layers, uint32_t layers_size);
bool are_validation_layers_available(const char **names,
    uint32_t names_size, VkLayerProperties *available_layers, uint32_t layers_size);

VKAPI_ATTR VkBool32 VKAPI_CALL vulkan_debug_callback(VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char *layer_prefix,
    const char *msg, void *userData);



#endif // VULKAN_DEBUG_H
