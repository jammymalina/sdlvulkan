#ifndef VULKAN_TOOLS_H
#define VULKAN_TOOLS_H

#include <vulkan/vulkan.h>

#define CHECK_VK(x)                                                         \
	do {                                                                    \
		VkResult r = x;                                                     \
		if (r != VK_SUCCESS) {                                              \
			log_error("VK error: %s - %s", #x, vulkan_result_to_string(r)); \
			return false;                                                   \
		}                                                                   \
	} while(0)

#define CHECK_VK_VAL(x, s)                                                  \
	do {                                                                    \
		if (!(x)) {                                                         \
			log_error("VK error: %s - %s", s, #x);                          \
			return false;                                                   \
		}                                                                   \
	} while (0)

const char *vulkan_result_to_string(VkResult result);

#endif // VULKAN_TOOLS_H
