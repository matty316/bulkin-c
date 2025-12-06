#include "device.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct QueueFamilyIndices {
  uint32_t graphics_family;
  bool has_graphics_family;
} QueueFamilyIndices;

bool is_queue_family_complete(QueueFamilyIndices *indices) {
  return indices->has_graphics_family;
}

QueueFamilyIndices find_queue_fams(VkPhysicalDevice *device) {
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
  }
  return indices;
}

bool is_device_suitible(VkPhysicalDevice *device) {
  QueueFamilyIndices indices = find_queue_fams(device);
  return is_queue_family_complete(&indices);
}

VkPhysicalDevice pick_physical_device(VkInstance *instance) {
  uint32_t device_count = 0;
  vkEnumeratePhysicalDevices(*instance, &device_count, NULL);

  if (device_count == 0) exit(EXIT_FAILURE);

  VkPhysicalDevice devices[device_count];
  vkEnumeratePhysicalDevices(*instance, &device_count, devices);

  for (size_t i = 0; i < device_count; i++)
    if (is_device_suitible(&devices[i])) return devices[i];

  printf("cannot find suitible physical device");
  exit(EXIT_FAILURE);
}

VkDevice create_logical_device(VkPhysicalDevice *physical_device) {
  QueueFamilyIndices indices = find_queue_fams(physical_device);

  VkDeviceQueueCreateInfo queue_create_info = {};
  queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queue_create_info.queueFamilyIndex = indices.graphics_family;
  queue_create_info.queueCount = 1;

  float queue_priority = 1.0f;
  queue_create_info.pQueuePriorities = &queue_priority;

  VkPhysicalDeviceFeatures device_features;

  VkDeviceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.pQueueCreateInfos = &queue_create_info;
  create_info.queueCreateInfoCount = 1;
  create_info.pEnabledFeatures = &device_features;
  create_info.enabledExtensionCount = 0;
  create_info.enabledLayerCount = 0;

  VkDevice device;
  VkResult result = vkCreateDevice(*physical_device, &create_info, NULL, &device);
  if (result != VK_SUCCESS) {
    printf("unable to create logical device");
    exit(EXIT_FAILURE);
  }
  return device;
}
