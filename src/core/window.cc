#include <window.hh>

void Window::run() {
  initWindow();
  initVulkan();
  mainLoop();
  cleanup();
}

void Window::initWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  m_window = glfwCreateWindow(m_WIDTH, m_HEIGHT, "Vulkan", nullptr, nullptr);
}
void Window::initVulkan() {
  createInstance();
  setupDebugMessenger();
  createSurface();
  pickPhysicalDevice();
  createLogicalDevice();
  createSwapChain();
  createImageViews();
  createRenderPass();
  createGraphicsPipeline();
  createFramebuffers();
  createCommandPool();
  createCommandBuffers();
  createSyncObjects();
}

void Window::createInstance() {

  if (m_enableValidationLayers && !checkValidationLayerSupport()) {
    throw std::runtime_error("validation layers requested, but not available!");
  }

  vk::ApplicationInfo appInfo{.pApplicationName = "Hello Triangle",
                              .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                              .pEngineName = "No Engine",
                              .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                              .apiVersion = VK_API_VERSION_1_0};

  vk::InstanceCreateInfo createInfo{.pApplicationInfo = &appInfo};

  auto extensions = getRequiredExtensions();
  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();

  vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

  if (m_enableValidationLayers) {
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(m_validationLayers.size());
    createInfo.ppEnabledLayerNames = m_validationLayers.data();

    populateDebugMessengerCreateInfo(debugCreateInfo);
    createInfo.pNext = reinterpret_cast<vk::DebugUtilsMessengerCreateInfoEXT *>(
        &debugCreateInfo);
  } else {
    createInfo.enabledLayerCount = 0;
    createInfo.pNext = nullptr;
  }

  m_instance = vk::createInstance(createInfo);
}

void Window::populateDebugMessengerCreateInfo(
    vk::DebugUtilsMessengerCreateInfoEXT &createInfo) {

  createInfo.setMessageSeverity(
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning);

  createInfo.setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
  createInfo.setPfnUserCallback(debugCallback);
}

void Window::setupDebugMessenger() {
  if (!m_enableValidationLayers)
    return;

  vk::DebugUtilsMessengerCreateInfoEXT createInfo;

  populateDebugMessengerCreateInfo(createInfo);

  m_debugMessenger = m_instance.createDebugUtilsMessengerEXT(createInfo);
}

void Window::createSurface() {

  if (glfwCreateWindowSurface((VkInstance)m_instance, m_window, nullptr,
                              &m_surface) != VK_SUCCESS) {
    throw std::runtime_error("failed to create window surface!");
  }
}

void Window::pickPhysicalDevice() {
  auto devices = m_instance.enumeratePhysicalDevices();
  // m_instance.enumeratePhysicalDevices();

  if (devices.size() == 0) {
    throw std::runtime_error("failed to find GPUs with Vulkan support!");
  }

  for (const auto &device : devices) {
    if (isDeviceSuitable(device)) {

      m_physicalDevice = device;
      break;
    }
  }
  if (m_physicalDevice == vk::PhysicalDevice(nullptr)) {
    throw std::runtime_error("failed to find a suitable GPU!");
  }
}

bool Window::isDeviceSuitable(vk::PhysicalDevice device) {
  auto deviceProperties = device.getProperties();

  spdlog::info("{0},{1},{2}", deviceProperties.deviceName,
               deviceProperties.vendorID, deviceProperties.deviceID);

  QueueFamilyIndices indices = findQueueFamilies(device);

  bool extensionsSupported = checkDeviceExtensionSupport(device);

  bool swapChainAdequate = false;
  if (extensionsSupported) {
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
    swapChainAdequate = !swapChainSupport.formats.empty() &&
                        !swapChainSupport.presentModes.empty();
  }

  return indices.isComplete() && extensionsSupported && swapChainAdequate &&
         deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu;
}

QueueFamilyIndices Window::findQueueFamilies(const vk::PhysicalDevice &device) {

  QueueFamilyIndices indices;

  auto queueFamilies = device.getQueueFamilyProperties();

  int i = 0;
  for (const auto &queueFamily : queueFamilies) {
    if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
      indices.graphicsFamily = i;
    }
    auto presentSupport = device.getSurfaceSupportKHR(i, m_surface);
    spdlog::info("after get queue family{}", i);
    if (presentSupport) {
      indices.presentFamily = i;
    }
    i++;
  }
  return indices;
}
