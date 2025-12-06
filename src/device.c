#include "device.h"
#include "queue.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

bool is_device_suitible(VkPhysicalDevice *device, VkSurfaceKHR *surface) {
  QueueFamilyIndices indices = find_queue_fams(device, surface);
  return is_queue_family_complete(&indices);
}

VkPhysicalDevice pick_physical_device(VkInstance *instance, VkSurfaceKHR *surface) {
  uint32_t device_count = 0;
  vkEnumeratePhysicalDevices(*instance, &device_count, NULL);

  if (device_count == 0) exit(EXIT_FAILURE);

  VkPhysicalDevice devices[device_count];
  vkEnumeratePhysicalDevices(*instance, &device_count, devices);

  for (size_t i = 0; i < device_count; i++)
    if (is_device_suitible(&devices[i], surface)) return devices[i];

  printf("cannot find suitible physical device");
  exit(EXIT_FAILURE);
}

VkDevice create_logical_device(VkPhysicalDevice *physical_device, QueueFamilyIndices indices) {
  VkDeviceQueueCreateInfo queue_create_info = {};
  queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queue_create_info.queueFamilyIndex = indices.graphics_family;
  queue_create_info.queueCount = 1;

  float queue_priority = 1.0f;
  queue_create_info.pQueuePriorities = &queue_priority;

  VkPhysicalDeviceFeatures device_features = {};

  VkDeviceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.pQueueCreateInfos = &queue_create_info;
  create_info.queueCreateInfoCount = 1;
  create_info.pEnabledFeatures = &device_features;

  #if __APPLE__
  const char* exensions[] = {"VK_KHR_portability_subset"};
  create_info.enabledExtensionCount = 1;
  create_info.ppEnabledExtensionNames = exensions;
  #else
  create_info.enabledExtensionCount = 0;
  #endif

  create_info.enabledLayerCount = 0;

  VkDevice device;
  VkResult result = vkCreateDevice(*physical_device, &create_info, NULL, &device);
  if (result != VK_SUCCESS) {
    printf("unable to create logical device");
    exit(EXIT_FAILURE);
  }
  return device;
}

VkQueue get_graphics_queue(VkDevice *device, QueueFamilyIndices indices) {
  VkQueue graphics_queue;
  vkGetDeviceQueue(*device, indices.graphics_family, 0, &graphics_queue);
  return graphics_queue;
}
