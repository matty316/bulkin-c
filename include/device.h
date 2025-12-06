#ifndef device_h
#define device_h

#include <vulkan/vulkan.h>

VkPhysicalDevice pick_physical_device(VkInstance *instance);
VkDevice create_logical_device(VkPhysicalDevice *physical_device);

#endif
