#include "bulkin.h"
#include "device.h"
#include <stdlib.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#define WIDTH 800
#define HEIGHT 600

typedef struct Game {
  GLFWwindow *window;
  VkInstance instance;
  VkPhysicalDevice physical_device;
} Game;

void init_window(Game *game) {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  game->window = glfwCreateWindow(WIDTH, HEIGHT, "BULKIN'", NULL, NULL);
}

void create_instance(Game *game) {
  VkApplicationInfo app_info;
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "Bulkin";
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = "Bulkin Engine";
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo create_info;
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;

  uint32_t glfw_extension_count = 0;
  const char** glfw_extensions;

  glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

  uint32_t extension_count = glfw_extension_count + 1;
  const char* required_extensions[extension_count];

  for (size_t i = 0; i < glfw_extension_count; i++) {
    required_extensions[i] = glfw_extensions[i];
  }

  required_extensions[glfw_extension_count] = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
  create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

  create_info.enabledExtensionCount = extension_count;
  create_info.ppEnabledExtensionNames = required_extensions;

  create_info.enabledLayerCount = 0;

  if (vkCreateInstance(&create_info, nullptr, &game->instance) != VK_SUCCESS) {
    exit(EXIT_FAILURE);
  }
}

void init_vulkan(Game *game) {
  create_instance(game);
  game->physical_device = pick_physical_device(&game->instance);
}

void main_loop(Game *game) {
  while (!glfwWindowShouldClose(game->window)) {
    glfwPollEvents();
  }
}

void cleanup(Game *game) {
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
