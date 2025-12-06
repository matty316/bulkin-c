#ifndef queue_h
#define queue_h

#include <stdint.h>
#include <stdbool.h>
#include <vulkan/vulkan.h>

typedef struct QueueFamilyIndices {
  uint32_t graphics_family;
  bool has_graphics_family;
  uint32_t present_family;
  bool has_present_family;
} QueueFamilyIndices;

bool is_queue_family_complete(QueueFamilyIndices *indices);
QueueFamilyIndices find_queue_fams(VkPhysicalDevice *device, VkSurfaceKHR *surface);

#endif
