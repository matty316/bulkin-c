#ifndef device_h
#define device_h

#include "queue.h"

#include <vulkan/vulkan.h>

VkPhysicalDevice pick_physical_device(VkInstance *instance, VkSurfaceKHR *surface);
VkDevice create_logical_device(VkPhysicalDevice *physical_device, QueueFamilyIndices indices);
VkQueue get_graphics_queue(VkDevice *device, QueueFamilyIndices indices);

#endif
