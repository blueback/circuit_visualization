#include "standard_defs/standard_defs.hpp"
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>
#include <optional>
#include <stdexcept>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

const std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger) {

  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkCreateDebugUtilsMessengerEXT");

  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks *pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) {
    func(instance, debugMessenger, pAllocator);
  }
}

class HelloTriangleApplication {
public:
  void run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
  }

private:
  enum RenderAndPresentDeviceOptions {
    RENDER_AND_PRESENT_WITH_PRIMARY,
    RENDER_WITH_PRIMARY_PRESENT_WITH_SECONDARY,
    RENDER_AND_PRESENT_WITH_SECONDARY
  };

  GLFWwindow *window;
  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;

  VkSurfaceKHR presentableWindowSurface;

  VkPhysicalDevice presentablePhysicalDevice = VK_NULL_HANDLE;
  VkDevice presentableLogicalDevice;

  std::vector<VkPhysicalDevice> unpresentablePhysicalDevices;
  std::vector<VkDevice> unpresentableLogicalDevices;

  VkQueue graphicsQueueForPresentable;
  std::vector<VkQueue> graphicsQueuesForUnpresentable;

  VkQueue presentationQueueForPresentable;

  VkSwapchainKHR presentableSwapChain;
  std::vector<VkImage> presentableSwapChainImages;
  VkFormat presentableSwapChainImageFormat;
  VkExtent2D presentableSwapChainExtent;
  std::vector<VkImageView> presentableSwapChainImageViews;

  std::vector<VkImage> unpresentableDeviceImages;
  std::vector<VkDeviceMemory> unpresentableDeviceImageMemories;
  std::vector<VkImageView> unpresentableDeviceImageViews;

private:
  void initWindow() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
  }

  void checkExtensions() {
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);

    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                           extensions.data());

    std::cout << "available extensions:\n";
    std::cout << "=====================\n";
    for (const auto &extension : extensions) {
      std::cout << '\t' << extension.extensionName << '\n';
    }

    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions =
        glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::cout << "\nFound GLFW required Extensions:\n";
    std::cout << "=====================\n";
    uint32_t found_count(0);
    for (uint32_t i = 0; i < glfwExtensionCount; i++) {
      for (const auto &extension : extensions) {
        if (strcmp(glfwExtensions[i], extension.extensionName) == 0) {
          std::cout << "\t    " << extension.extensionName << '\n';
          found_count++;
          break;
        }
      }
    }

    if (found_count < glfwExtensionCount) {
      std::cout << "\nNOT Found GLFW required Extensions:\n";
      std::cout << "=====================\n";
      for (uint32_t i = 0; i < glfwExtensionCount; i++) {
        for (const auto &extension : extensions) {
          if (strcmp(glfwExtensions[i], extension.extensionName) == 0) {
            std::cout << "\t    " << extension.extensionName << '\n';
          }
        }
      }
    }
  }

  bool checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char *layerName : validationLayers) {
      bool layerFound = false;

      for (const auto &layerProperties : availableLayers) {
        if (strcmp(layerName, layerProperties.layerName) == 0) {
          layerFound = true;
          break;
        }
      }

      if (!layerFound) {
        return false;
      }
    }

    return true;
  }

  std::vector<const char *> getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions,
                                         glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
      extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    return extensions;
  }

  static VKAPI_ATTR VkBool32 VKAPI_CALL
  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData) {
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
      // message is important enough to show
      std::cerr << "for " << pCallbackData->objectCount << " VK_OBJECTES: -\n";
      for (uint32_t i = 0; i < pCallbackData->objectCount; i++) {
        std::cerr << '\t' << i << '.' << pCallbackData->pObjects[i].objectHandle
                  << "\n";
      }
    }

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
  }

  void populateDebugMessengerCreateInfo(
      VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // Optional
  }

  void createInstance() {
    if (enableValidationLayers) {
      if (!checkValidationLayerSupport()) {
        throw std::runtime_error(
            "validation layers requested, but not available!");
      } else {
        std::cout << "validation layer requested...\n";
      }
    } else {
      std::cout << "validation layer not requested\n";
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    checkExtensions();
    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers) {
      createInfo.enabledLayerCount =
          static_cast<uint32_t>(validationLayers.size());
      createInfo.ppEnabledLayerNames = validationLayers.data();

      populateDebugMessengerCreateInfo(debugCreateInfo);
      createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
    } else {
      createInfo.enabledLayerCount = 0;

      createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
      throw std::runtime_error("failed to create VkInstance!");
    }
    printf("Created VkInstance!\n");
  }

  void setupDebugMessenger() {
    if (!enableValidationLayers)
      return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    populateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr,
                                     &debugMessenger) != VK_SUCCESS) {
      throw std::runtime_error("failed to set up debug messenger!");
    }
  }

  std::string getPhysicalDeviceName(VkPhysicalDevice physicalDevice) {
    VkPhysicalDeviceProperties deviceProperties;

    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    return deviceProperties.deviceName;
  }

  void forEachAvailablePhysicalDevice(
      std::function<IteratorStatus(VkPhysicalDevice)> f) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
      return;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    std::sort(devices.begin(), devices.end(),
              [&](VkPhysicalDevice d1, VkPhysicalDevice d2) {
                int score1 = rateDeviceSuitability(d1);
                int score2 = rateDeviceSuitability(d2);
                return score1 > score2;
              });

    for (const auto &device : devices) {
      if (f(device) == IterationBreak) {
        return;
      }
    }
  }

  void forEachExtensionOfPhysicalDevice(
      VkPhysicalDevice device,
      std::function<IteratorStatus(VkExtensionProperties)> f) {
    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                         nullptr);

    if (extensionCount == 0) {
      return;
    }

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                         extensions.data());

    for (const auto &extension : extensions) {
      if (f(extension) == IterationBreak) {
        return;
      }
    }
  }

  void forEachQueueFamilyOfDevice(
      VkPhysicalDevice physicalDevice,
      std::function<IteratorStatus(int, VkQueueFamilyProperties)> f) {
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                                             nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                                             queueFamilies.data());

    int queueIndex = 0;
    for (const auto &queueFamily : queueFamilies) {
      if (f(queueIndex++, queueFamily) == IterationBreak) {
        return;
      }
    }
  }

  bool isPresentationSupportedByQueueOnSurface(VkPhysicalDevice physicalDevice,
                                               int queueIndex,
                                               VkSurfaceKHR surface) {
    VkBool32 queueSupportPresentation = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueIndex, surface,
                                         &queueSupportPresentation);

    if (queueSupportPresentation) {
      return true;
    }
    return false;
  }

  bool isPhysicalDevicePresentable(VkPhysicalDevice physicalDevice,
                                   VkSurfaceKHR surface) {

    bool presentable = false;
    forEachQueueFamilyOfDevice(
        physicalDevice,
        [&](int queueIndex, VkQueueFamilyProperties queueFamilyProperties) {
          if (isPresentationSupportedByQueueOnSurface(physicalDevice,
                                                      queueIndex, surface)) {
            presentable = true;
            return IterationBreak;
          }
          return IterationContinue;
        });
    return presentable;
  }

  uint32_t getPresentationQueueFamilyIndex(VkPhysicalDevice physicalDevice,
                                           VkSurfaceKHR surface) {

    std::optional<uint32_t> presentationQueueIndex;
    forEachQueueFamilyOfDevice(
        physicalDevice,
        [&](int queueIndex, VkQueueFamilyProperties queueFamilyProperties) {
          if (isPresentationSupportedByQueueOnSurface(physicalDevice,
                                                      queueIndex, surface)) {
            presentationQueueIndex = queueIndex;
            return IterationBreak;
          }
          return IterationContinue;
        });
    assert(presentationQueueIndex.has_value());
    return presentationQueueIndex.value();
  }

  bool isPhysicalDeviceDedicatedGPU(VkPhysicalDevice physicalDevice) {
    VkPhysicalDeviceProperties deviceProperties;

    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      return true;
    }
    return false;
  }

  bool isPhysicalDeviceIntegratedGPU(VkPhysicalDevice physicalDevice) {
    VkPhysicalDeviceProperties deviceProperties;

    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
      return true;
    }
    return false;
  }

  bool isPhysicalDeviceCPU(VkPhysicalDevice physicalDevice) {
    VkPhysicalDeviceProperties deviceProperties;

    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU) {
      return true;
    }
    return false;
  }

  bool
  isGeometryShaderSupportedByPhysicalDevice(VkPhysicalDevice physicalDevice) {
    VkPhysicalDeviceFeatures deviceFeatures;

    vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

    if (!deviceFeatures.geometryShader) {
      return false;
    }

    return true;
  }

  bool
  isGraphicsQueueSupportedByPhysicalDevice(VkPhysicalDevice physicalDevice) {
    bool hasGraphicsQueue = false;
    forEachQueueFamilyOfDevice(
        physicalDevice,
        [&](int queueIndex, VkQueueFamilyProperties queueFamilyProperties) {
          if (queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            hasGraphicsQueue = true;
            return IterationBreak;
          }

          return IterationContinue;
        });
    return hasGraphicsQueue;
  }

  uint32_t getGraphicsQueueFamilyIndex(VkPhysicalDevice physicalDevice) {
    std::optional<uint32_t> graphicsQueueIndex;
    forEachQueueFamilyOfDevice(
        physicalDevice,
        [&](int queueIndex, VkQueueFamilyProperties queueFamilyProperties) {
          if (queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsQueueIndex = queueIndex;
            return IterationBreak;
          }
          return IterationContinue;
        });
    assert(graphicsQueueIndex.has_value());
    return graphicsQueueIndex.value();
  }

  bool
  isComputeQueueSupportedByPhysicalDevice(VkPhysicalDevice physicalDevice) {
    bool hasComputeQueue = false;
    forEachQueueFamilyOfDevice(
        physicalDevice,
        [&](int queueIndex, VkQueueFamilyProperties queueFamilyProperties) {
          if (queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) {
            hasComputeQueue = true;
            return IterationBreak;
          }

          return IterationContinue;
        });
    return hasComputeQueue;
  }

  bool isExtensionSupportedByPhysicalDevice(VkPhysicalDevice physicalDevice,
                                            const char *extensionName) {
    bool isExtensionSupported = false;
    forEachExtensionOfPhysicalDevice(
        physicalDevice, [&](VkExtensionProperties extensionProperties) {
          if (strcmp(extensionProperties.extensionName, extensionName) == 0) {
            isExtensionSupported = true;
            return IterationBreak;
          }
          return IterationContinue;
        });
    return isExtensionSupported;
  }

  bool checkPhysicalDeviceExtensionSupport(VkPhysicalDevice physicalDevice) {
    for (const auto &deviceExtensionName : deviceExtensions) {
      if (!isExtensionSupportedByPhysicalDevice(physicalDevice,
                                                deviceExtensionName)) {
        return false;
      }
    }
    return true;
  }

  bool isDeviceSuitable(VkPhysicalDevice device) {
    /*
    printPhysicalDeviceExtensions(device);
    */

    if (!isGraphicsQueueSupportedByPhysicalDevice(device)) {
      return false;
    }

    if (!checkPhysicalDeviceExtensionSupport(device)) {
      return false;
    }

    if (isPhysicalDeviceDedicatedGPU(device) ||
        isPhysicalDeviceIntegratedGPU(device) || isPhysicalDeviceCPU(device)) {
      if (isGeometryShaderSupportedByPhysicalDevice(device)) {
        return true;
      }
    }
    return false;
  }

  int rateDeviceSuitability(VkPhysicalDevice device) {
    int score = 0;

    // Application can't function without geometry shaders
    if (!isGeometryShaderSupportedByPhysicalDevice(device)) {
      return 0;
    }

    // Discrete GPUs have a significant performance advantage
    if (isPhysicalDeviceDedicatedGPU(device)) {
      score += 100000;
    } else if (isPhysicalDeviceIntegratedGPU(device)) {
      score += 1000;
    } else if (isPhysicalDeviceCPU(device)) {
      score += 10;
    } else {
      return 0;
    }

    // Maximum possible size of textures affects graphics quality
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    score += deviceProperties.limits.maxImageDimension2D;

    return score;
  }

  void printPhysicalDeviceExtensions(VkPhysicalDevice device) {
    std::cout << "Extensions for Device \"" << getPhysicalDeviceName(device)
              << "\" ..." << std::endl;

    forEachExtensionOfPhysicalDevice(
        device, [&](VkExtensionProperties extensionProperties) {
          std::cout << "   Extension: " << extensionProperties.extensionName
                    << std::endl;
          return IterationContinue;
        });
  }

  void pickPhysicalDevice(VkSurfaceKHR windowSurface) {
    forEachAvailablePhysicalDevice([&](VkPhysicalDevice device) {
      std::cout << "Probing Device :- " << getPhysicalDeviceName(device)
                << "..." << std::endl;
      std::cout << "=================" << std::endl;
      int score = rateDeviceSuitability(device);
      std::cout << "    score :- " << score << std::endl;

      if (!isDeviceSuitable(device)) {
        std::cout << "    skiping device as its not suitable" << std::endl;
        return IterationContinue;
      }

      if (isGeometryShaderSupportedByPhysicalDevice(device) &&
          isGraphicsQueueSupportedByPhysicalDevice(device)) {
        if (isPhysicalDevicePresentable(device, windowSurface) &&
            isSwapChainAdequateForPresentation(device, windowSurface)) {
          std::cout << "    device can also render ON-screen" << std::endl;
          presentablePhysicalDevice = device;
        } else {
          std::cout << "    device can only render OFF-screen" << std::endl;
          unpresentablePhysicalDevices.push_back(device);
        }
      }
      return IterationContinue;
    });

    if (presentablePhysicalDevice == VK_NULL_HANDLE) {
      throw std::runtime_error(
          "failed to find presentable GPUs with Vulkan support!");
    }
  }

  /*
  struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
      return graphicsFamily.has_value() && presentFamily.has_value();
    }
  };

  QueueFamilyIndices findQueueFamilyIndices(VkPhysicalDevice device) {
    QueueFamilyIndices indices;
    // Logic to find graphics queue family to populate struct with
    // Assign index to queue families that could be found
    if (isGraphicsQueueSupportedByPhysicalDevice(device)) {
      indices.graphicsFamily = getGraphicsQueueFamilyIndex(device);
    }
    if (isPhysicalDevicePresentable(device, surface)) {
      indices.presentFamily = getPresentationQueueFamilyIndex(device, surface);
    }
    return indices;
  }
  */

  void createGraphicsQueue(VkPhysicalDevice physicalDevice,
                           VkDevice logicalDevice, VkQueue &graphicaQueue) {

    assert(isGraphicsQueueSupportedByPhysicalDevice(physicalDevice));

    vkGetDeviceQueue(logicalDevice, getGraphicsQueueFamilyIndex(physicalDevice),
                     0, &graphicaQueue);

    std::cout << "Created graphics queue for "
              << getPhysicalDeviceName(physicalDevice) << std::endl;
  }

  void createPresentationQueue(VkPhysicalDevice physicalDevice,
                               VkDevice logicalDevice,
                               VkSurfaceKHR windowSurface,
                               VkQueue &presentationQueue) {

    assert(isPhysicalDevicePresentable(physicalDevice, windowSurface));

    vkGetDeviceQueue(
        logicalDevice,
        getPresentationQueueFamilyIndex(physicalDevice, windowSurface), 0,
        &presentationQueue);

    std::cout << "Created presentation queue for "
              << getPhysicalDeviceName(physicalDevice) << std::endl;
  }

  struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentMode;
  };

  SwapChainSupportDetails querySwapChainDetails(VkPhysicalDevice physicalDevice,
                                                VkSurfaceKHR windowSurface) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, windowSurface,
                                              &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, windowSurface,
                                         &formatCount, nullptr);

    if (formatCount != 0) {
      details.formats.resize(formatCount);
      vkGetPhysicalDeviceSurfaceFormatsKHR(
          physicalDevice, windowSurface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, windowSurface,
                                              &presentModeCount, nullptr);

    if (presentModeCount != 0) {
      details.presentMode.resize(presentModeCount);
      vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, windowSurface,
                                                &presentModeCount,
                                                details.presentMode.data());
    }

    return details;
  }

  bool isSwapChainAdequateForPresentation(VkPhysicalDevice physicalDevice,
                                          VkSurfaceKHR windowSurface) {
    SwapChainSupportDetails swapChainSupport =
        querySwapChainDetails(physicalDevice, windowSurface);

    if (swapChainSupport.formats.empty()) {
      return false;
    }

    if (swapChainSupport.presentMode.empty()) {
      return false;
    }

    return true;
  }

  VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats) {

    for (const auto &availableFormat : availableFormats) {
      if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
          availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        return availableFormat;
      }
    }

    return availableFormats[0];
  }

  VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> availablePresentModes) {

    for (const auto &availablePresentMode : availablePresentModes) {
      if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
        return availablePresentMode;
      }
    }

    for (const auto &availablePresentMode : availablePresentModes) {
      if (availablePresentMode == VK_PRESENT_MODE_FIFO_KHR) {
        return availablePresentMode;
      }
    }

    assert(0);
    return VK_PRESENT_MODE_FIFO_KHR;
  }

  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
    if (capabilities.currentExtent.width !=
        std::numeric_limits<uint32_t>::max()) {
      return capabilities.currentExtent;
    } else {
      int width, height;
      glfwGetFramebufferSize(window, &width, &height);

      VkExtent2D actualExtent = {static_cast<uint32_t>(width),
                                 static_cast<uint32_t>(height)};

      actualExtent.width =
          std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                     capabilities.maxImageExtent.width);
      actualExtent.height =
          std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                     capabilities.maxImageExtent.height);

      return actualExtent;
    }
  }

  void createLogicalDevice(VkPhysicalDevice physicalDevice,
                           std::set<uint32_t> &uniqueQueueFamilies,
                           VkDevice &logicalDevice) {

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    float queuePriority = 1.0f;
    for (uint32_t queueFamilyIndex : uniqueQueueFamilies) {
      VkDeviceQueueCreateInfo queueCreateInfo{};
      queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
      queueCreateInfo.queueCount = 1;
      queueCreateInfo.pQueuePriorities = &queuePriority;
      queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount =
        static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount =
        static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers) {
      createInfo.enabledLayerCount =
          static_cast<uint32_t>(validationLayers.size());
      createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
      createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create logical device!");
    } else {
      std::cout << "Created logical device for "
                << getPhysicalDeviceName(physicalDevice) << std::endl;
    }
  }

  void createPresentableLogicalDevice(VkPhysicalDevice physicalDevice,
                                      VkSurfaceKHR windowSurface,
                                      VkDevice &logicalDevice) {

    std::set<uint32_t> uniqueQueueFamilies;

    assert(isPhysicalDevicePresentable(physicalDevice, windowSurface));
    uniqueQueueFamilies.insert(
        getPresentationQueueFamilyIndex(physicalDevice, windowSurface));

    if (isGraphicsQueueSupportedByPhysicalDevice(physicalDevice)) {
      uniqueQueueFamilies.insert(getGraphicsQueueFamilyIndex(physicalDevice));
    }

    createLogicalDevice(physicalDevice, uniqueQueueFamilies, logicalDevice);
  }

  void createUnpresentableLogicalDevice(VkPhysicalDevice physicalDevice,
                                        VkDevice &logicalDevice) {

    std::set<uint32_t> uniqueQueueFamilies;

    if (isGraphicsQueueSupportedByPhysicalDevice(physicalDevice)) {
      uniqueQueueFamilies.insert(getGraphicsQueueFamilyIndex(physicalDevice));
    }

    createLogicalDevice(physicalDevice, uniqueQueueFamilies, logicalDevice);
  }

  void createSurface(VkSurfaceKHR &windowSurface) {
    if (glfwCreateWindowSurface(instance, window, nullptr, &windowSurface) !=
        VK_SUCCESS) {
      std::cerr << "Failed to create vulkan surface!" << std::endl;
    } else {
      std::cout << "Created surface..." << std::endl;
    }
  }

  void forEachImageOfSwapChain(VkDevice logicalDevice, VkSwapchainKHR swapChain,
                               std::function<IteratorStatus(VkImage)> f) {
    uint32_t imageCount;
    vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr);
    std::vector<VkImage> images(imageCount);
    vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount,
                            images.data());

    for (auto const &image : images) {
      if (f(image) == IterationBreak) {
        return;
      }
    }
  }

  void createSwapChainForPresentation(VkPhysicalDevice physicalDevice,
                                      VkDevice logicalDevice,
                                      VkSurfaceKHR windowSurface,
                                      VkSwapchainKHR &swapChain,
                                      std::vector<VkImage> &swapChainImages,
                                      VkFormat &swapChainImageFormat,
                                      VkExtent2D &swapChainExtent) {

    SwapChainSupportDetails swapChainSupport =
        querySwapChainDetails(physicalDevice, windowSurface);

    VkSurfaceFormatKHR surfaceFormat =
        chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode =
        chooseSwapPresentMode(swapChainSupport.presentMode);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount) {
      imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = windowSurface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    // createInfo.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    assert(isGraphicsQueueSupportedByPhysicalDevice(physicalDevice));
    assert(isPhysicalDevicePresentable(physicalDevice, windowSurface));
    uint32_t graphicsQueueFamilyIndex =
        getGraphicsQueueFamilyIndex(physicalDevice);
    uint32_t presentationQueueFamilyIndex =
        getPresentationQueueFamilyIndex(physicalDevice, windowSurface);
    uint32_t queueFamilyIndices[] = {graphicsQueueFamilyIndex,
                                     presentationQueueFamilyIndex};

    if (graphicsQueueFamilyIndex != presentationQueueFamilyIndex) {
      createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
      createInfo.queueFamilyIndexCount = 2;
      createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
      createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
      createInfo.queueFamilyIndexCount = 0;     // Optional
      createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &swapChain) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create swap chain!");
    } else {
      std::cout << "Created Swap Chain for device \""
                << getPhysicalDeviceName(physicalDevice) << "\"" << std::endl;
    }

    forEachImageOfSwapChain(logicalDevice, swapChain, [&](VkImage image) {
      swapChainImages.push_back(image);
      return IterationContinue;
    });

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
  }

  void createUnpresentableDeviceImage(VkPhysicalDevice physicalDevice,
                                      VkDevice logicalDevice, uint32_t width,
                                      uint32_t height, VkFormat format,
                                      VkImage &image) {
    VkImageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType = VK_IMAGE_TYPE_2D;
    createInfo.extent.width = width;
    createInfo.extent.height = height;
    createInfo.extent.depth = 1;
    createInfo.mipLevels = 1;
    createInfo.arrayLayers = 1;
    createInfo.format = format;
    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    createInfo.usage =
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(logicalDevice, &createInfo, nullptr, &image) !=
        VK_SUCCESS) {
      throw std::runtime_error(
          "Failed to create image for unpresentable device");
    } else {
      std::cout << "Creating image for unpresentable device \""
                << getPhysicalDeviceName(physicalDevice) << "\"" << std::endl;
    }
  }

  uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties,
                          VkPhysicalDevice physicalDevice) {

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
      if ((typeFilter & (1 << i)) &&
          (memProperties.memoryTypes[i].propertyFlags & properties) ==
              properties) {
        return i;
      }
    }

    // If no suitable memory type is found
    throw std::runtime_error("Failed to find suitable memory type!");
  }

  void allocateMemoryForUnpresentableDeviceImage(
      VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkImage image,
      VkDeviceMemory &imageMemory) {

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(logicalDevice, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex =
        findMemoryType(memRequirements.memoryTypeBits,
                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, physicalDevice);

    if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &imageMemory) !=
        VK_SUCCESS) {
      throw std::runtime_error(
          "Failed to allocate memory for unpresentable images!");
    } else {
      std::cout << "Creating memory for image for Device \""
                << getPhysicalDeviceName(physicalDevice) << "\"" << std::endl;
    }

    if (vkBindImageMemory(logicalDevice, image, imageMemory, 0) != VK_SUCCESS) {
      throw std::runtime_error("Failed to bind memory to unpresentable image!");
    } else {
      std::cout << "Binding memory to image for Device \""
                << getPhysicalDeviceName(physicalDevice) << "\"" << std::endl;
    }
  }

  VkImageViewCreateInfo fillImageViewCreateInfo(VkImage image,
                                                VkFormat format) {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = image;
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = format;

    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    return createInfo;
  }

  void createImageViewsForPresentation(std::vector<VkImage> &images,
                                       VkFormat format,
                                       VkPhysicalDevice physicalDevice,
                                       VkDevice logicalDevice,
                                       std::vector<VkImageView> &imageViews) {
    imageViews.resize(images.size());
    for (size_t i = 0; i < images.size(); i++) {
      VkImageViewCreateInfo createInfo =
          fillImageViewCreateInfo(images[i], format);

      if (vkCreateImageView(logicalDevice, &createInfo, nullptr,
                            &imageViews[i]) != VK_SUCCESS) {
        throw std::runtime_error(
            "failed to create image views for presentation!");
      } else {
        std::cout << "Created image view \"" << i << "\" for device \""
                  << getPhysicalDeviceName(physicalDevice) << "\"" << std::endl;
      }
    }
  }

  void createImageViewForUnpresentableDevice(VkImage image, VkFormat format,
                                             VkPhysicalDevice physicalDevice,
                                             VkDevice logicalDevice,
                                             VkImageView &imageView) {
    VkImageViewCreateInfo createInfo = fillImageViewCreateInfo(image, format);

    if (vkCreateImageView(logicalDevice, &createInfo, nullptr, &imageView) !=
        VK_SUCCESS) {
      throw std::runtime_error(
          "failed to create image views for presentation!");
    } else {
      std::cout << "Created image view \"0\" for device \""
                << getPhysicalDeviceName(physicalDevice) << "\"" << std::endl;
    }
  }

  void destroyImageViewsForPresentation(VkDevice logicalDevice,
                                        std::vector<VkImageView> &imageViews) {
    for (auto imageView : imageViews) {
      vkDestroyImageView(logicalDevice, imageView, nullptr);
    }
  }

  void initVulkan() {
    createInstance();

    setupDebugMessenger();

    createSurface(presentableWindowSurface);

    pickPhysicalDevice(presentableWindowSurface);

    createPresentableLogicalDevice(presentablePhysicalDevice,
                                   presentableWindowSurface,
                                   presentableLogicalDevice);
    createGraphicsQueue(presentablePhysicalDevice, presentableLogicalDevice,
                        graphicsQueueForPresentable);
    createPresentationQueue(presentablePhysicalDevice, presentableLogicalDevice,
                            presentableWindowSurface,
                            presentationQueueForPresentable);

    createSwapChainForPresentation(
        presentablePhysicalDevice, presentableLogicalDevice,
        presentableWindowSurface, presentableSwapChain,
        presentableSwapChainImages, presentableSwapChainImageFormat,
        presentableSwapChainExtent);

    createImageViewsForPresentation(
        presentableSwapChainImages, presentableSwapChainImageFormat,
        presentablePhysicalDevice, presentableLogicalDevice,
        presentableSwapChainImageViews);

    unpresentableLogicalDevices.resize(unpresentablePhysicalDevices.size());
    graphicsQueuesForUnpresentable.resize(unpresentablePhysicalDevices.size());
    unpresentableDeviceImages.resize(unpresentablePhysicalDevices.size());
    unpresentableDeviceImageMemories.resize(
        unpresentablePhysicalDevices.size());
    unpresentableDeviceImageViews.resize(unpresentablePhysicalDevices.size());

    for (uint32_t i = 0; i < unpresentablePhysicalDevices.size(); i++) {
      createUnpresentableLogicalDevice(unpresentablePhysicalDevices[i],
                                       unpresentableLogicalDevices[i]);

      createGraphicsQueue(unpresentablePhysicalDevices[i],
                          unpresentableLogicalDevices[i],
                          graphicsQueuesForUnpresentable[i]);

      createUnpresentableDeviceImage(
          unpresentablePhysicalDevices[i], unpresentableLogicalDevices[i],
          presentableSwapChainExtent.width, presentableSwapChainExtent.height,
          presentableSwapChainImageFormat, unpresentableDeviceImages[i]);

      allocateMemoryForUnpresentableDeviceImage(
          unpresentablePhysicalDevices[i], unpresentableLogicalDevices[i],
          unpresentableDeviceImages[i], unpresentableDeviceImageMemories[i]);

      createImageViewForUnpresentableDevice(
          unpresentableDeviceImages[i], presentableSwapChainImageFormat,
          unpresentablePhysicalDevices[i], unpresentableLogicalDevices[i],
          unpresentableDeviceImageViews[i]);
    }
  }

  void mainLoop() {
    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();
    }
  }

  void cleanup() {

    for (uint32_t i = 0; i < unpresentableDeviceImages.size(); i++) {
      vkDestroyImageView(unpresentableLogicalDevices[i],
                         unpresentableDeviceImageViews[i], nullptr);

      vkFreeMemory(unpresentableLogicalDevices[i],
                   unpresentableDeviceImageMemories[i], nullptr);

      vkDestroyImage(unpresentableLogicalDevices[i],
                     unpresentableDeviceImages[i], nullptr);
    }

    destroyImageViewsForPresentation(presentableLogicalDevice,
                                     presentableSwapChainImageViews);

    vkDestroySwapchainKHR(presentableLogicalDevice, presentableSwapChain,
                          nullptr);

    vkDestroyDevice(presentableLogicalDevice, nullptr);
    for (auto const &logicalDevice : unpresentableLogicalDevices) {
      vkDestroyDevice(logicalDevice, nullptr);
    }

    if (enableValidationLayers) {
      DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }

    vkDestroySurfaceKHR(instance, presentableWindowSurface, nullptr);

    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(window);

    glfwTerminate();
  }
};

int main() {
  HelloTriangleApplication app;

  try {
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
