#pragma once
#include <cstddef>

#include <cstdint>
#define VULKAN_HPP_NO_CONSTRUCTORS
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <optional>
#include <set>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace raii = vk::raii;

const int MAX_FRAMES_IN_FLIGHT = 2;

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;
  bool isComplete() {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

struct SwapChainSupportDetails {
  vk::SurfaceCapabilitiesKHR capabilities;
  std::vector<vk::SurfaceFormatKHR> formats;
  std::vector<vk::PresentModeKHR> presentModes;
};

// VkResult inline CreateDebugUtilsMessengerEXT(
//     vk::Instance instance,
//     const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
//     const VkAllocationCallbacks *pAllocator,
//     VkDebugUtilsMessengerEXT *pDebugMessenger) {

//   auto func = (PFN_vkCreateDebugUtilsMessengerEXT)instance.getProcAddr(
//       "vkCreateDebugUtilsMessengerEXT");
//   if (func != nullptr) {
//     return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
//   } else {
//     return VK_ERROR_EXTENSION_NOT_PRESENT;
//   }
// }

// void inline DestroyDebugUtilsMessengerEXT(
//     vk::Instance instance, VkDebugUtilsMessengerEXT debugMessenger,
//     const VkAllocationCallbacks *pAllocator) {
//   auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)instance.getProcAddr(
//       "vkDestroyDebugUtilsMessengerEXT");
//   if (func != nullptr) {
//     func(instance, debugMessenger, pAllocator);
//   }
// }

class Window {
public:
  void run();

private:
  void initWindow();

  void initVulkan();

  void createSyncObjects();

  void recordCommandBuffer(const vk::CommandBuffer &commandBuffer,
                           uint32_t imageIndex);

  void createCommandBuffers();

  void createCommandPool();

  void createFramebuffers();

  void createRenderPass();

  void createGraphicsPipeline();

  raii::ShaderModule createShaderModule(const std::vector<char> &code);

  void createImageViews();

  void createSwapChain();

  vk::SurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<vk::SurfaceFormatKHR> &availableFormats);

  vk::PresentModeKHR chooseSwapPresentMode(
      const std::vector<vk::PresentModeKHR> &availablePresentModes);

  vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities);
  void createSurface();

  void createLogicalDevice();

  void pickPhysicalDevice();

  bool isDeviceSuitable(const raii::PhysicalDevice &device);

  bool checkDeviceExtensionSupport(const vk::PhysicalDevice &device);

  //寻找当前设备支持的队列列表 图形队列列表和presentFamily
  QueueFamilyIndices findQueueFamilies(const raii::PhysicalDevice &device);

  SwapChainSupportDetails
  querySwapChainSupport(const raii::PhysicalDevice &device);

  void mainLoop() {
    while (!glfwWindowShouldClose(m_window)) {
      glfwPollEvents();
      drawFrame();
    }
    m_device.waitIdle();
  }

  void drawFrame();

  void cleanup();

  std::vector<const char *> getRequiredExtensions();
  void createInstance();

  void recreateSwapChain() {
    m_device.waitIdle();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createGraphicsPipeline();
    createFramebuffers();
  }

  void populateDebugMessengerCreateInfo(
      vk::DebugUtilsMessengerCreateInfoEXT &createInfo);

  void setupDebugMessenger();
  bool checkValidationLayerSupport();

  static std::vector<char> readFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
      throw std::runtime_error("failed to open file!");
    }
    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer;
    buffer.reserve(fileSize);
    file.seekg(0);
    buffer.insert(buffer.begin(), std::istreambuf_iterator<char>(file),
                  std::istreambuf_iterator<char>());
    file.close();
    return buffer;
  }

  // static VKAPI_ATTR VkBool32 VKAPI_CALL
  // debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
  //               VkDebugUtilsMessageTypeFlagsEXT messageType,
  //               const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
  //               void *pUserData) {
  //   std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
  //   return VK_FALSE;
  // }

  raii::Context m_context;
  raii::Instance m_instance {nullptr};

  vk::DebugUtilsMessengerEXT m_debugMessenger;

  raii::PhysicalDevice m_physicalDevice{nullptr};
  raii::Device m_device{nullptr};
  raii::Queue m_graphicsQueue{nullptr};
  raii::SurfaceKHR m_surface{nullptr};
  raii::Queue m_presentQueue{nullptr};
  raii::SwapchainKHR m_swapChain{nullptr};
  std::vector<vk::Image> m_swapChainImages;
  vk::Format m_swapChainImageFormat;
  vk::Extent2D m_swapChainExtent;
  std::vector<raii::ImageView> m_swapChainImageViews;
  raii::PipelineLayout m_pipelineLayout{nullptr};
  raii::RenderPass m_renderPass{nullptr};
  raii::Pipeline m_graphicsPipeline{nullptr};
  std::vector<raii::Framebuffer> m_swapChainFramebuffers;
  raii::CommandPool m_commandPool{nullptr};
  std::vector<raii::CommandBuffer> m_commandBuffers;
  std::vector<raii::Semaphore> m_imageAvailableSemaphores;
  std::vector<raii::Semaphore> m_renderFinishedSemaphores;
  std::vector<raii::Fence> m_inFlightFences;

  uint32_t m_currentFrame = 0;
  GLFWwindow *m_window;
  const uint32_t m_WIDTH = 800;
  const uint32_t m_HEIGHT = 600;
  const std::vector<const char *> m_validationLayers = {
      "VK_LAYER_KHRONOS_validation"};

  const std::vector<const char *> m_deviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};
#ifdef NDEBUG
  const bool m_enableValidationLayers = false;
#else
  const bool m_enableValidationLayers = false;
#endif
};
