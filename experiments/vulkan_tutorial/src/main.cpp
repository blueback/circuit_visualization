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

  VkSurfaceKHR surface;

  VkPhysicalDevice presentablePhysicalDevice = VK_NULL_HANDLE;
  VkDevice presentableLogicalDevice;

  std::vector<VkPhysicalDevice> unPresentablePhysicalDevices;
  std::vector<VkDevice> unPresentableLogicalDevices;

  VkQueue graphicsQueueForPresentable;
  std::vector<VkQueue> graphicsQueuesForUnPresentable;

  VkQueue presentationQueueForPresentable;

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

  void pickPhysicalDevice() {
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
        if (isPhysicalDevicePresentable(device, surface)) {
          std::cout << "    device can also render ON-screen" << std::endl;
          presentablePhysicalDevice = device;
        } else {
          std::cout << "    device can only render OFF-screen" << std::endl;
          unPresentablePhysicalDevices.push_back(device);
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
                               VkQueue &presentationQueue) {

    assert(isPhysicalDevicePresentable(physicalDevice, surface));

    vkGetDeviceQueue(logicalDevice,
                     getPresentationQueueFamilyIndex(physicalDevice, surface),
                     0, &presentationQueue);

    std::cout << "Created presentation queue for "
              << getPhysicalDeviceName(physicalDevice) << std::endl;
  }

  void createLogicalDevice(VkPhysicalDevice physicalDevice,
                           VkDevice &logicalDevice) {

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies;

    if (isPhysicalDevicePresentable(physicalDevice, surface)) {
      uniqueQueueFamilies.insert(
          getPresentationQueueFamilyIndex(physicalDevice, surface));
    }

    if (isGraphicsQueueSupportedByPhysicalDevice(physicalDevice)) {
      uniqueQueueFamilies.insert(getGraphicsQueueFamilyIndex(physicalDevice));
    }

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

  void createSurface() {
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) !=
        VK_SUCCESS) {
      std::cerr << "Failed to create vulkan surface!" << std::endl;
    } else {
      std::cout << "Created surface..." << std::endl;
    }
  }

  void initVulkan() {
    createInstance();

    setupDebugMessenger();

    createSurface();

    pickPhysicalDevice();

    createLogicalDevice(presentablePhysicalDevice, presentableLogicalDevice);
    createGraphicsQueue(presentablePhysicalDevice, presentableLogicalDevice,
                        graphicsQueueForPresentable);
    createPresentationQueue(presentablePhysicalDevice, presentableLogicalDevice,
                            presentationQueueForPresentable);

    for (auto const &physicalDevice : unPresentablePhysicalDevices) {
      unPresentableLogicalDevices.push_back(nullptr);
      graphicsQueuesForUnPresentable.push_back(nullptr);
      createLogicalDevice(
          physicalDevice,
          unPresentableLogicalDevices[unPresentableLogicalDevices.size() - 1]);
      createGraphicsQueue(
          physicalDevice,
          unPresentableLogicalDevices[unPresentableLogicalDevices.size() - 1],
          graphicsQueuesForUnPresentable[graphicsQueuesForUnPresentable.size() -
                                         1]);
    }
  }

  void mainLoop() {
    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();
    }
  }

  void cleanup() {
    vkDestroyDevice(presentableLogicalDevice, nullptr);
    for (auto const &logicalDevice : unPresentableLogicalDevices) {
      vkDestroyDevice(logicalDevice, nullptr);
    }

    if (enableValidationLayers) {
      DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }

    vkDestroySurfaceKHR(instance, surface, nullptr);

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
