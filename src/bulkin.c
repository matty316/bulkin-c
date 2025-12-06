#include "bulkin.h"
#include "device.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#define WIDTH 800
#define HEIGHT 600

typedef struct Game {
  GLFWwindow *window;
  VkInstance instance;
  VkDebugUtilsMessengerEXT debug_messenger;
  VkPhysicalDevice physical_device;
  VkDevice device;
} Game;

#define LAYER_COUNT 1
const char* validation_layers[LAYER_COUNT] = {
  "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
const bool enable_validation_layers = false;
#else
const bool enable_validation_layers = true;
#endif

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
  VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
  VkDebugUtilsMessageTypeFlagsEXT message_type,
  const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data,
  void *p_user_data) {
  printf("validation layer: %s\n", p_callback_data->pMessage);

  return VK_FALSE;
}

VkResult create_debug_utils_messenger_ext(
  VkInstance *instance,
  const VkDebugUtilsMessengerCreateInfoEXT *p_create_info,
  const VkAllocationCallbacks *p_allocator,
  VkDebugUtilsMessengerEXT *p_debug_messenger) {
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(*instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != NULL)
      return func(*instance, p_create_info, p_allocator, p_debug_messenger);
    else
      return VK_ERROR_EXTENSION_NOT_PRESENT;
}

bool check_valiadtion_layer_support() {
  uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, NULL);

  VkLayerProperties available[layer_count];
  vkEnumerateInstanceLayerProperties(&layer_count, available);

  for (size_t i = 0; i < LAYER_COUNT; i++) {
    const char* layer_name = validation_layers[i];
    bool layer_found = false;

    for (size_t j = 0; j < layer_count; j++) {
      VkLayerProperties layer_properties = available[j];
      if (strcmp(layer_name, layer_properties.layerName) == 0) {
        layer_found = true;
        break;
      }
    }

    if (!layer_found) return false;
  }

  return true;
}

void init_window(Game *game) {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  game->window = glfwCreateWindow(WIDTH, HEIGHT, "BULKIN'", NULL, NULL);
}

void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT *create_info) {
  create_info->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  create_info->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  create_info->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  create_info->pfnUserCallback = debug_callback;
  create_info->pUserData = NULL;
}

void create_instance(Game *game) {
  if (enable_validation_layers && !check_valiadtion_layer_support()) {
    printf("validation layers requested but not found\n");
    exit(EXIT_FAILURE);
  }

  VkApplicationInfo app_info = {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "Bulkin";
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = "Bulkin Engine";
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;

  create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

  uint32_t glfw_extension_count = 0;
  const char** glfw_extensions;

  glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

  uint32_t additional_extensions_count = 2;
  const char* additional_extentions[] = {
    VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME
  };

  uint32_t extension_count = glfw_extension_count + additional_extensions_count;
  const char *extensions[extension_count];

  for (size_t i = 0; i < glfw_extension_count; i++) {
    extensions[i] = glfw_extensions[i];
  }

  for (size_t i = 0; i < additional_extensions_count; i++) {
    extensions[glfw_extension_count + i] = additional_extentions[i];
  }

  create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

  create_info.enabledExtensionCount = extension_count;
  create_info.ppEnabledExtensionNames = extensions;

  VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
  if (enable_validation_layers) {
    create_info.enabledLayerCount = (uint32_t)LAYER_COUNT;
    create_info.ppEnabledLayerNames = validation_layers;

    populate_debug_messenger_create_info(&debug_create_info);
    create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_create_info;
  } else {
    create_info.enabledLayerCount = 0;
    create_info.pNext = NULL;
  }

  if (vkCreateInstance(&create_info, NULL, &game->instance) != VK_SUCCESS) {
    printf("unable to create instance\n");
    exit(EXIT_FAILURE);
  }
}

void setup_debug_messenger(Game *game) {
  if (!enable_validation_layers) return;

  VkDebugUtilsMessengerCreateInfoEXT create_info = {};
  populate_debug_messenger_create_info(&create_info);

  if (create_debug_utils_messenger_ext(&game->instance, &create_info, NULL, &game->debug_messenger) != VK_SUCCESS) {
    printf("failed to setup debug messenger\n");
    exit(EXIT_FAILURE);
  }
}

void init_vulkan(Game *game) {
  create_instance(game);
  setup_debug_messenger(game);
  game->physical_device = pick_physical_device(&game->instance);
  game->device = create_logical_device(&game->physical_device);
}

void main_loop(Game *game) {
  while (!glfwWindowShouldClose(game->window)) {
    glfwPollEvents();
  }
}

void destroy_debug_messenger_ext(VkInstance *instance, VkDebugUtilsMessengerEXT *debugMessenger, const VkAllocationCallbacks *p_allocator) {
  PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(*instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != NULL)
    func(*instance, *debugMessenger, p_allocator);
}

void cleanup(Game *game) {
  if (enable_validation_layers)
    destroy_debug_messenger_ext(&game->instance, &game->debug_messenger, NULL);
  vkDestroyDevice(game->device, NULL);
  vkDestroyInstance(game->instance, NULL);
  glfwDestroyWindow(game->window);
  glfwTerminate();
}

void run() {
  Game game;
  init_window(&game);
  init_vulkan(&game);
  main_loop(&game);
  cleanup(&game);
}
