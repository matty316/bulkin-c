#include "queue.h"

bool is_queue_family_complete(QueueFamilyIndices *indices) {
  return indices->has_graphics_family && indices->has_present_family;
}

QueueFamilyIndices find_queue_fams(VkPhysicalDevice *device, VkSurfaceKHR *surface) {
  QueueFamilyIndices indices;
  indices.has_graphics_family = false;

  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(*device, &queue_family_count, NULL);
  VkQueueFamilyProperties properties[queue_family_count];
  vkGetPhysicalDeviceQueueFamilyProperties(*device, &queue_family_count, properties);

  for (size_t i = 0; i < queue_family_count; i++) {
    if (is_queue_family_complete(&indices)) break;

    if (properties->queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphics_family = i;
      indices.has_graphics_family = true;
    }

    VkBool32 present_support = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(*device, i, *surface, &present_support);
    if (present_support) {
      indices.present_family = i;
      indices.has_present_family = true;
    }
  }
  return indices;
}
