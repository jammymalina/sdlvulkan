#ifndef VULKAN_FUNCTIONS
#define VULKAN_FUNCTIONS

#include <vulkan/vulkan.h>

#define EXPORTED_VULKAN_FUNCTION(name) extern PFN_vk##name vk_##name;
#define GLOBAL_LEVEL_VULKAN_FUNCTION(name) extern PFN_vk##name vk_##name; 
#define INSTANCE_LEVEL_VULKAN_FUNCTION(name) extern PFN_vk##name vk_##name; 
#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name) extern PFN_vk##name vk_##name; 
#define DEVICE_LEVEL_VULKAN_FUNCTION(name) extern PFN_vk##name vk_##name; 
#define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) extern PFN_vk##name vk_##name;

#include "list.inl" 

#endif // VULKAN_FUNCTIONS

