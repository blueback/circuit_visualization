#include "standard_defs/standard_defs.hpp"
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <cstdlib>
#include <iostream>
#include <optional>
#include <stdexcept>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

const std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

const bool DYNAMIC_STATES_FOR_VIEWPORT_SCISSORS = false;
#define SPLIT_RENDER_PRESENT_MODE
// #define HANDLE_WINDOW_MINIMIZATION

#ifdef SPLIT_RENDER_PRESENT_MODE
const bool isSplitRenderPresentMode = true;
#else  // SPLIT_RENDER_PRESENT_MODE
const bool isSplitRenderPresentMode = false;
#endif // SPLIT_RENDER_PRESENT_MODE

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

struct DeviceQueueCommandUnit {
  int queueIndex;
  VkQueue queue;
  VkCommandPool queueCommandPool;
  VkCommandBuffer queueCommandBuffers[MAX_FRAMES_IN_FLIGHT];
};

struct DeviceQueueCommandUnitSet {
  enum DeviceQueueSetType {
    PRESENTABLE_DEVICE_ALL_THREE_SEPERATE_QUEUE_SET_TYPE,
    PRESENTABLE_DEVICE_GRAPHICS_SEPERATE_QUEUE_SET_TYPE,
    PRESENTABLE_DEVICE_PRESENT_SEPERATE_QUEUE_SET_TYPE,
    PRESENTABLE_DEVICE_TRANSFER_SEPERATE_QUEUE_SET_TYPE,
    PRESENTABLE_DEVICE_ALL_THREE_SAME_QUEUE_SET_TYPE,
    UNPRESENTABLE_DEVICE_SEPERATE_QUEUE_SET_TYPE,
    UNPRESENTABLE_DEVICE_SAME_QUEUE_SET_TYPE
  };
  DeviceQueueSetType type;
  union {
    struct {
      DeviceQueueCommandUnit graphicsQueueCommandUnit;
      DeviceQueueCommandUnit presentQueueCommandUnit;
      DeviceQueueCommandUnit transferQueueCommandUnit;
    } presentable_device_all_three_seperate_queue_command_unit_set;
    struct {
      DeviceQueueCommandUnit graphicsQueueCommandUnit;
      DeviceQueueCommandUnit presentAndTransferQueueCommandUnit;
    } presentable_device_graphics_seperate_queue_command_unit_set;
    struct {
      DeviceQueueCommandUnit presentQueueCommandUnit;
      DeviceQueueCommandUnit graphicsAndTransferQueueCommandUnit;
    } presentable_device_present_seperate_queue_command_unit_set;
    struct {
      DeviceQueueCommandUnit transferQueueCommandUnit;
      DeviceQueueCommandUnit graphicsAndPresentQueueCommandUnit;
    } presentable_device_transfer_seperate_queue_command_unit_set;
    struct {
      DeviceQueueCommandUnit unifiedQueueCommandUnit;
    } presentable_device_all_three_same_queue_command_unit_set;
    struct {
      DeviceQueueCommandUnit graphicsQueueCommandUnit;
      DeviceQueueCommandUnit transferQueueCommandUnit;
    } unpresentable_device_seperate_queue_command_unit_set;
    struct {
      DeviceQueueCommandUnit graphicsAndTransferQueueCommandUnit;
    } unpresentable_device_same_queue_command_unit_set;
  } queue_command_unit_set;

#define DEVICE_QUEUE_COMMAND_UNIT_ITERATOR(func_name)                             \
  void func_name(                                                                 \
      std::function<IteratorStatus(                                               \
          DEVICE_QUEUE_COMMAND_UNIT_IS_CONST DeviceQueueCommandUnit &,            \
          const bool isGraphicsQueue, const bool isPresentQueue,                  \
          const bool isTransferQueue)>                                            \
          f) DEVICE_QUEUE_COMMAND_UNIT_IS_CONST {                                 \
                                                                                  \
    switch (type) {                                                               \
    case PRESENTABLE_DEVICE_ALL_THREE_SEPERATE_QUEUE_SET_TYPE: {                  \
      {                                                                           \
        DEVICE_QUEUE_COMMAND_UNIT_IS_CONST DeviceQueueCommandUnit                 \
            &deviceQueueCommandUnit =                                             \
                queue_command_unit_set                                            \
                    .presentable_device_all_three_seperate_queue_command_unit_set \
                    .graphicsQueueCommandUnit;                                    \
                                                                                  \
        if (f(deviceQueueCommandUnit, true, false, false) == IterationBreak)      \
          return;                                                                 \
      }                                                                           \
                                                                                  \
      {                                                                           \
        DEVICE_QUEUE_COMMAND_UNIT_IS_CONST DeviceQueueCommandUnit                 \
            &deviceQueueCommandUnit =                                             \
                queue_command_unit_set                                            \
                    .presentable_device_all_three_seperate_queue_command_unit_set \
                    .transferQueueCommandUnit;                                    \
                                                                                  \
        if (f(deviceQueueCommandUnit, false, false, true) == IterationBreak)      \
          return;                                                                 \
      }                                                                           \
                                                                                  \
      {                                                                           \
        DEVICE_QUEUE_COMMAND_UNIT_IS_CONST DeviceQueueCommandUnit                 \
            &deviceQueueCommandUnit =                                             \
                queue_command_unit_set                                            \
                    .presentable_device_all_three_seperate_queue_command_unit_set \
                    .presentQueueCommandUnit;                                     \
                                                                                  \
        if (f(deviceQueueCommandUnit, false, true, false) == IterationBreak)      \
          return;                                                                 \
      }                                                                           \
      break;                                                                      \
    }                                                                             \
    case PRESENTABLE_DEVICE_GRAPHICS_SEPERATE_QUEUE_SET_TYPE: {                   \
      {                                                                           \
        DEVICE_QUEUE_COMMAND_UNIT_IS_CONST DeviceQueueCommandUnit                 \
            &deviceQueueCommandUnit =                                             \
                queue_command_unit_set                                            \
                    .presentable_device_graphics_seperate_queue_command_unit_set  \
                    .graphicsQueueCommandUnit;                                    \
                                                                                  \
        if (f(deviceQueueCommandUnit, true, false, false) == IterationBreak)      \
          return;                                                                 \
      }                                                                           \
                                                                                  \
      {                                                                           \
        DEVICE_QUEUE_COMMAND_UNIT_IS_CONST DeviceQueueCommandUnit                 \
            &deviceQueueCommandUnit =                                             \
                queue_command_unit_set                                            \
                    .presentable_device_graphics_seperate_queue_command_unit_set  \
                    .presentAndTransferQueueCommandUnit;                          \
                                                                                  \
        if (f(deviceQueueCommandUnit, false, true, true) == IterationBreak)       \
          return;                                                                 \
      }                                                                           \
      break;                                                                      \
    }                                                                             \
    case PRESENTABLE_DEVICE_PRESENT_SEPERATE_QUEUE_SET_TYPE: {                    \
      {                                                                           \
        DEVICE_QUEUE_COMMAND_UNIT_IS_CONST DeviceQueueCommandUnit                 \
            &deviceQueueCommandUnit =                                             \
                queue_command_unit_set                                            \
                    .presentable_device_present_seperate_queue_command_unit_set   \
                    .presentQueueCommandUnit;                                     \
                                                                                  \
        if (f(deviceQueueCommandUnit, false, true, false) == IterationBreak)      \
          return;                                                                 \
      }                                                                           \
                                                                                  \
      {                                                                           \
        DEVICE_QUEUE_COMMAND_UNIT_IS_CONST DeviceQueueCommandUnit                 \
            &deviceQueueCommandUnit =                                             \
                queue_command_unit_set                                            \
                    .presentable_device_present_seperate_queue_command_unit_set   \
                    .graphicsAndTransferQueueCommandUnit;                         \
                                                                                  \
        if (f(deviceQueueCommandUnit, true, false, true) == IterationBreak)       \
          return;                                                                 \
      }                                                                           \
      break;                                                                      \
    }                                                                             \
    case PRESENTABLE_DEVICE_TRANSFER_SEPERATE_QUEUE_SET_TYPE: {                   \
      {                                                                           \
        DEVICE_QUEUE_COMMAND_UNIT_IS_CONST DeviceQueueCommandUnit                 \
            &deviceQueueCommandUnit =                                             \
                queue_command_unit_set                                            \
                    .presentable_device_transfer_seperate_queue_command_unit_set  \
                    .transferQueueCommandUnit;                                    \
                                                                                  \
        if (f(deviceQueueCommandUnit, false, false, true) == IterationBreak)      \
          return;                                                                 \
      }                                                                           \
                                                                                  \
      {                                                                           \
        DEVICE_QUEUE_COMMAND_UNIT_IS_CONST DeviceQueueCommandUnit                 \
            &deviceQueueCommandUnit =                                             \
                queue_command_unit_set                                            \
                    .presentable_device_transfer_seperate_queue_command_unit_set  \
                    .graphicsAndPresentQueueCommandUnit;                          \
                                                                                  \
        if (f(deviceQueueCommandUnit, true, true, false) == IterationBreak)       \
          return;                                                                 \
      }                                                                           \
      break;                                                                      \
    }                                                                             \
    case PRESENTABLE_DEVICE_ALL_THREE_SAME_QUEUE_SET_TYPE: {                      \
      {                                                                           \
        DEVICE_QUEUE_COMMAND_UNIT_IS_CONST DeviceQueueCommandUnit                 \
            &deviceQueueCommandUnit =                                             \
                queue_command_unit_set                                            \
                    .presentable_device_all_three_same_queue_command_unit_set     \
                    .unifiedQueueCommandUnit;                                     \
                                                                                  \
        if (f(deviceQueueCommandUnit, true, true, true) == IterationBreak)        \
          return;                                                                 \
      }                                                                           \
      break;                                                                      \
    }                                                                             \
    case UNPRESENTABLE_DEVICE_SEPERATE_QUEUE_SET_TYPE: {                          \
      {                                                                           \
        DEVICE_QUEUE_COMMAND_UNIT_IS_CONST DeviceQueueCommandUnit                 \
            &deviceQueueCommandUnit =                                             \
                queue_command_unit_set                                            \
                    .unpresentable_device_seperate_queue_command_unit_set         \
                    .graphicsQueueCommandUnit;                                    \
                                                                                  \
        if (f(deviceQueueCommandUnit, true, false, false) == IterationBreak)      \
          return;                                                                 \
      }                                                                           \
                                                                                  \
      {                                                                           \
        DEVICE_QUEUE_COMMAND_UNIT_IS_CONST DeviceQueueCommandUnit                 \
            &deviceQueueCommandUnit =                                             \
                queue_command_unit_set                                            \
                    .unpresentable_device_seperate_queue_command_unit_set         \
                    .transferQueueCommandUnit;                                    \
                                                                                  \
        if (f(deviceQueueCommandUnit, false, false, true) == IterationBreak)      \
          return;                                                                 \
      }                                                                           \
      break;                                                                      \
    }                                                                             \
    case UNPRESENTABLE_DEVICE_SAME_QUEUE_SET_TYPE: {                              \
      {                                                                           \
        DEVICE_QUEUE_COMMAND_UNIT_IS_CONST DeviceQueueCommandUnit                 \
            &deviceQueueCommandUnit =                                             \
                queue_command_unit_set                                            \
                    .unpresentable_device_same_queue_command_unit_set             \
                    .graphicsAndTransferQueueCommandUnit;                         \
                                                                                  \
        if (f(deviceQueueCommandUnit, true, false, true) == IterationBreak)       \
          return;                                                                 \
      }                                                                           \
      break;                                                                      \
    }                                                                             \
    }                                                                             \
  }

#define DEVICE_QUEUE_COMMAND_UNIT_IS_CONST const
  DEVICE_QUEUE_COMMAND_UNIT_ITERATOR(forEachConstDeviceQueueCommandUnit);
#undef DEVICE_QUEUE_COMMAND_UNIT_IS_CONST
#define DEVICE_QUEUE_COMMAND_UNIT_IS_CONST
  DEVICE_QUEUE_COMMAND_UNIT_ITERATOR(forEachNonConstDeviceQueueCommandUnit);

  static void forEachQueueFamilyOfPhysicalDevice(
      VkPhysicalDevice physicalDevice,
      std::function<IteratorStatus(int, VkQueueFamilyProperties)> f) {
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                                             nullptr);

    std::vector<VkQueueFamilyProperties> queueFamiliesProps(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                                             queueFamiliesProps.data());

    int queueIndex = 0;
    for (const auto &queueFamilyProps : queueFamiliesProps) {
      if (f(queueIndex++, queueFamilyProps) == IterationBreak) {
        return;
      }
    }
  }

  static bool isPresentationSupportedByQueueOnSurface(
      VkPhysicalDevice physicalDevice, int queueIndex, VkSurfaceKHR surface) {
    VkBool32 queueSupportPresentation = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueIndex, surface,
                                         &queueSupportPresentation);

    if (queueSupportPresentation) {
      return true;
    }
    return false;
  }

  static bool
  isGraphicsSupportedByQueue(VkQueueFamilyProperties queueFamilyProps) {
    return queueFamilyProps.queueFlags & VK_QUEUE_GRAPHICS_BIT;
  }

  static bool
  isTransferSupportedByQueue(VkQueueFamilyProperties queueFamilyProps) {
    return queueFamilyProps.queueFlags & VK_QUEUE_TRANSFER_BIT;
  }

  static bool
  isComputeSupportedByQueue(VkQueueFamilyProperties queueFamilyProps) {
    return queueFamilyProps.queueFlags & VK_QUEUE_COMPUTE_BIT;
  }

  static bool isPhysicalDevicePresentable(VkPhysicalDevice physicalDevice,
                                          VkSurfaceKHR surface) {

    bool presentable = false;
    forEachQueueFamilyOfPhysicalDevice(
        physicalDevice, [&](int queueIndex, VkQueueFamilyProperties) {
          if (isPresentationSupportedByQueueOnSurface(physicalDevice,
                                                      queueIndex, surface)) {
            presentable = true;
            return IterationBreak;
          }
          return IterationContinue;
        });
    return presentable;
  }

  static bool checkAndFillPresentableDeviceAllThreeSeperateQueueIndexSet(
      VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface,
      DeviceQueueCommandUnitSet &deviceQueueSet) {

    std::optional<int> graphicsQueueIndex;
    std::optional<int> presentQueueIndex;
    std::optional<int> transferQueueIndex;

    forEachQueueFamilyOfPhysicalDevice(
        physicalDevice,
        [&](int queueIndex, VkQueueFamilyProperties queueFamilyProps) {
          if (isGraphicsSupportedByQueue(queueFamilyProps)) {
            graphicsQueueIndex = queueIndex;
          } else if (isPresentationSupportedByQueueOnSurface(
                         physicalDevice, queueIndex, windowSurface)) {
            presentQueueIndex = queueIndex;
          } else if (isTransferSupportedByQueue(queueFamilyProps)) {
            transferQueueIndex = queueIndex;
          }
          return IterationContinue;
        });

    if (graphicsQueueIndex.has_value() && transferQueueIndex.has_value() &&
        presentQueueIndex.has_value()) {
      deviceQueueSet.type = DeviceQueueCommandUnitSet::
          PRESENTABLE_DEVICE_ALL_THREE_SEPERATE_QUEUE_SET_TYPE;
      deviceQueueSet.queue_command_unit_set
          .presentable_device_all_three_seperate_queue_command_unit_set
          .graphicsQueueCommandUnit.queueIndex = graphicsQueueIndex.value();
      deviceQueueSet.queue_command_unit_set
          .presentable_device_all_three_seperate_queue_command_unit_set
          .presentQueueCommandUnit.queueIndex = presentQueueIndex.value();
      deviceQueueSet.queue_command_unit_set
          .presentable_device_all_three_seperate_queue_command_unit_set
          .transferQueueCommandUnit.queueIndex = transferQueueIndex.value();
      return true;
    }

    return false;
  }

  static bool checkAndFillPresentableDeviceGraphicsSeperateQueueIndexSet(
      VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface,
      DeviceQueueCommandUnitSet &deviceQueueSet) {

    std::optional<int> graphicsSeperateQueueIndex;
    std::optional<int> presentAndTransferQueueIndex;

    forEachQueueFamilyOfPhysicalDevice(
        physicalDevice,
        [&](int queueIndex, VkQueueFamilyProperties queueFamilyProps) {
          if (isGraphicsSupportedByQueue(queueFamilyProps)) {
            graphicsSeperateQueueIndex = queueIndex;
          } else if (isPresentationSupportedByQueueOnSurface(
                         physicalDevice, queueIndex, windowSurface) &&
                     isTransferSupportedByQueue(queueFamilyProps)) {
            presentAndTransferQueueIndex = queueIndex;
          }
          return IterationContinue;
        });

    if (presentAndTransferQueueIndex.has_value() &&
        graphicsSeperateQueueIndex.has_value()) {
      deviceQueueSet.type = DeviceQueueCommandUnitSet::
          PRESENTABLE_DEVICE_GRAPHICS_SEPERATE_QUEUE_SET_TYPE;
      deviceQueueSet.queue_command_unit_set
          .presentable_device_graphics_seperate_queue_command_unit_set
          .graphicsQueueCommandUnit.queueIndex =
          graphicsSeperateQueueIndex.value();
      deviceQueueSet.queue_command_unit_set
          .presentable_device_graphics_seperate_queue_command_unit_set
          .presentAndTransferQueueCommandUnit.queueIndex =
          presentAndTransferQueueIndex.value();
      return true;
    }
    return false;
  }

  static bool checkAndFillPresentableDeviceTransferSeperateQueueIndexSet(
      VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface,
      DeviceQueueCommandUnitSet &deviceQueueSet) {

    std::optional<int> transferSeperateQueueIndex;
    std::optional<int> graphicsAndPresentQueueIndex;

    forEachQueueFamilyOfPhysicalDevice(
        physicalDevice,
        [&](int queueIndex, VkQueueFamilyProperties queueFamilyProps) {
          if (isTransferSupportedByQueue(queueFamilyProps)) {
            transferSeperateQueueIndex = queueIndex;
          } else if (isGraphicsSupportedByQueue(queueFamilyProps) &&
                     isPresentationSupportedByQueueOnSurface(
                         physicalDevice, queueIndex, windowSurface)) {
            graphicsAndPresentQueueIndex = queueIndex;
          }
          return IterationContinue;
        });

    if (graphicsAndPresentQueueIndex.has_value() &&
        transferSeperateQueueIndex.has_value()) {
      deviceQueueSet.type = DeviceQueueCommandUnitSet::
          PRESENTABLE_DEVICE_TRANSFER_SEPERATE_QUEUE_SET_TYPE;
      deviceQueueSet.queue_command_unit_set
          .presentable_device_transfer_seperate_queue_command_unit_set
          .transferQueueCommandUnit.queueIndex =
          transferSeperateQueueIndex.value();
      deviceQueueSet.queue_command_unit_set
          .presentable_device_transfer_seperate_queue_command_unit_set
          .graphicsAndPresentQueueCommandUnit.queueIndex =
          graphicsAndPresentQueueIndex.value();
      return true;
    }
    return false;
  }

  static bool checkAndFillPresentableDevicePresentSeperateQueueIndexSet(
      VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface,
      DeviceQueueCommandUnitSet &deviceQueueSet) {

    std::optional<int> presentSeperateQueueIndex;
    std::optional<int> graphicsAndTransferQueueIndex;

    forEachQueueFamilyOfPhysicalDevice(
        physicalDevice,
        [&](int queueIndex, VkQueueFamilyProperties queueFamilyProps) {
          if (isPresentationSupportedByQueueOnSurface(
                  physicalDevice, queueIndex, windowSurface)) {
            presentSeperateQueueIndex = queueIndex;
          } else if (isGraphicsSupportedByQueue(queueFamilyProps) &&
                     isTransferSupportedByQueue(queueFamilyProps)) {
            graphicsAndTransferQueueIndex = queueIndex;
          }
          return IterationContinue;
        });

    if (graphicsAndTransferQueueIndex.has_value() &&
        presentSeperateQueueIndex.has_value()) {
      deviceQueueSet.type = DeviceQueueCommandUnitSet::
          PRESENTABLE_DEVICE_PRESENT_SEPERATE_QUEUE_SET_TYPE;
      deviceQueueSet.queue_command_unit_set
          .presentable_device_present_seperate_queue_command_unit_set
          .presentQueueCommandUnit.queueIndex =
          presentSeperateQueueIndex.value();
      deviceQueueSet.queue_command_unit_set
          .presentable_device_present_seperate_queue_command_unit_set
          .graphicsAndTransferQueueCommandUnit.queueIndex =
          graphicsAndTransferQueueIndex.value();
      return true;
    }
    return false;
  }

  static bool checkAndFillPresentableDeviceAllThreeSameQueueIndexSet(
      VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface,
      DeviceQueueCommandUnitSet &deviceQueueSet) {

    std::optional<int> unifiedQueueIndex;

    forEachQueueFamilyOfPhysicalDevice(
        physicalDevice,
        [&](int queueIndex, VkQueueFamilyProperties queueFamilyProps) {
          if (isGraphicsSupportedByQueue(queueFamilyProps) &&
              isPresentationSupportedByQueueOnSurface(
                  physicalDevice, queueIndex, windowSurface) &&
              isTransferSupportedByQueue(queueFamilyProps)) {
            unifiedQueueIndex = queueIndex;
            return IterationBreak;
          }
          return IterationContinue;
        });

    if (unifiedQueueIndex.has_value()) {
      deviceQueueSet.type = DeviceQueueCommandUnitSet::
          PRESENTABLE_DEVICE_ALL_THREE_SAME_QUEUE_SET_TYPE;
      deviceQueueSet.queue_command_unit_set
          .presentable_device_all_three_same_queue_command_unit_set
          .unifiedQueueCommandUnit.queueIndex = unifiedQueueIndex.value();
      return true;
    }

    return false;
  }

  static bool checkAndFillUnpresentableDeviceSeperateQueueIndexSet(
      VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface,
      DeviceQueueCommandUnitSet &deviceQueueSet) {

    std::optional<int> graphicsQueueIndex;
    std::optional<int> transferQueueIndex;

    forEachQueueFamilyOfPhysicalDevice(
        physicalDevice,
        [&](int queueIndex, VkQueueFamilyProperties queueFamilyProps) {
          if (isGraphicsSupportedByQueue(queueFamilyProps)) {
            graphicsQueueIndex = queueIndex;
          } else if (isTransferSupportedByQueue(queueFamilyProps)) {
            transferQueueIndex = queueIndex;
          }
          return IterationContinue;
        });

    if (graphicsQueueIndex.has_value() && transferQueueIndex.has_value()) {
      deviceQueueSet.type = DeviceQueueCommandUnitSet::
          UNPRESENTABLE_DEVICE_SEPERATE_QUEUE_SET_TYPE;
      deviceQueueSet.queue_command_unit_set
          .unpresentable_device_seperate_queue_command_unit_set
          .graphicsQueueCommandUnit.queueIndex = graphicsQueueIndex.value();
      deviceQueueSet.queue_command_unit_set
          .unpresentable_device_seperate_queue_command_unit_set
          .transferQueueCommandUnit.queueIndex = transferQueueIndex.value();
      return true;
    }

    return false;
  }

  static bool checkAndFillUnpresentableDeviceSameQueueIndexSet(
      VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface,
      DeviceQueueCommandUnitSet &deviceQueueSet) {

    std::optional<int> unifiedQueueIndex;

    forEachQueueFamilyOfPhysicalDevice(
        physicalDevice,
        [&](int queueIndex, VkQueueFamilyProperties queueFamilyProps) {
          if (isGraphicsSupportedByQueue(queueFamilyProps) &&
              isTransferSupportedByQueue(queueFamilyProps)) {
            unifiedQueueIndex = queueIndex;
            return IterationBreak;
          }
          return IterationContinue;
        });

    if (unifiedQueueIndex.has_value()) {
      deviceQueueSet.type =
          DeviceQueueCommandUnitSet::UNPRESENTABLE_DEVICE_SAME_QUEUE_SET_TYPE;
      deviceQueueSet.queue_command_unit_set
          .unpresentable_device_same_queue_command_unit_set
          .graphicsAndTransferQueueCommandUnit.queueIndex =
          unifiedQueueIndex.value();
      return true;
    }

    return false;
  }

  static void
  decideAndFillQueueSetIndices(VkPhysicalDevice physicalDevice,
                               VkSurfaceKHR windowSurface,
                               DeviceQueueCommandUnitSet &deviceQueueSet) {

    if (isPhysicalDevicePresentable(physicalDevice, windowSurface)) {
      if (checkAndFillPresentableDeviceAllThreeSameQueueIndexSet(
              physicalDevice, windowSurface, deviceQueueSet)) {
        return;
      }
    } else {
      if (checkAndFillUnpresentableDeviceSameQueueIndexSet(
              physicalDevice, windowSurface, deviceQueueSet)) {
        return;
      }

      if (checkAndFillUnpresentableDeviceSeperateQueueIndexSet(
              physicalDevice, windowSurface, deviceQueueSet)) {
        return;
      }
    }

    if (!isSplitRenderPresentMode) {
      if (isPhysicalDevicePresentable(physicalDevice, windowSurface)) {
        if (checkAndFillPresentableDeviceTransferSeperateQueueIndexSet(
                physicalDevice, windowSurface, deviceQueueSet)) {
          return;
        }

        if (checkAndFillPresentableDeviceGraphicsSeperateQueueIndexSet(
                physicalDevice, windowSurface, deviceQueueSet) ||
            checkAndFillPresentableDevicePresentSeperateQueueIndexSet(
                physicalDevice, windowSurface, deviceQueueSet)) {
          return;
        }

        if (checkAndFillPresentableDeviceAllThreeSeperateQueueIndexSet(
                physicalDevice, windowSurface, deviceQueueSet)) {
          return;
        }
      } else {
        if (checkAndFillUnpresentableDeviceSameQueueIndexSet(
                physicalDevice, windowSurface, deviceQueueSet)) {
          return;
        }

        if (checkAndFillUnpresentableDeviceSeperateQueueIndexSet(
                physicalDevice, windowSurface, deviceQueueSet)) {
          return;
        }
      }
    } else {
      if (isPhysicalDevicePresentable(physicalDevice, windowSurface)) {
        if (checkAndFillPresentableDeviceGraphicsSeperateQueueIndexSet(
                physicalDevice, windowSurface, deviceQueueSet)) {
          return;
        }

        if (checkAndFillPresentableDevicePresentSeperateQueueIndexSet(
                physicalDevice, windowSurface, deviceQueueSet) ||
            checkAndFillPresentableDeviceTransferSeperateQueueIndexSet(
                physicalDevice, windowSurface, deviceQueueSet)) {
          return;
        }

        if (checkAndFillPresentableDeviceAllThreeSeperateQueueIndexSet(
                physicalDevice, windowSurface, deviceQueueSet)) {
          return;
        }
      } else {
        if (checkAndFillUnpresentableDeviceSameQueueIndexSet(
                physicalDevice, windowSurface, deviceQueueSet)) {
          return;
        }

        if (checkAndFillUnpresentableDeviceSeperateQueueIndexSet(
                physicalDevice, windowSurface, deviceQueueSet)) {
          return;
        }
      }
    }
  }

  static bool
  isGraphicsQueueSupportedByPhysicalDevice(VkPhysicalDevice physicalDevice) {
    bool hasGraphicsQueue = false;
    forEachQueueFamilyOfPhysicalDevice(
        physicalDevice,
        [&](int queueIndex, VkQueueFamilyProperties queueFamilyProps) {
          if (isGraphicsSupportedByQueue(queueFamilyProps)) {
            hasGraphicsQueue = true;
            return IterationBreak;
          }

          return IterationContinue;
        });
    return hasGraphicsQueue;
  }

  static bool
  isComputeQueueSupportedByPhysicalDevice(VkPhysicalDevice physicalDevice) {
    bool hasComputeQueue = false;
    forEachQueueFamilyOfPhysicalDevice(
        physicalDevice,
        [&](int queueIndex, VkQueueFamilyProperties queueFamilyProps) {
          if (isComputeSupportedByQueue(queueFamilyProps)) {
            hasComputeQueue = true;
            return IterationBreak;
          }

          return IterationContinue;
        });
    return hasComputeQueue;
  }

  static bool
  isTransferQueueSupportedByPhysicalDevice(VkPhysicalDevice physicalDevice) {
    bool hasTransferQueue = false;
    forEachQueueFamilyOfPhysicalDevice(
        physicalDevice,
        [&](int queueIndex, VkQueueFamilyProperties queueFamilyProps) {
          if (isTransferSupportedByQueue(queueFamilyProps)) {
            hasTransferQueue = true;
            return IterationBreak;
          }

          return IterationContinue;
        });
    return hasTransferQueue;
  }

  static std::vector<uint32_t> getUniqueQueueFamiliesIndices(
      const DeviceQueueCommandUnitSet &deviceQueueSet) {
    std::vector<uint32_t> uniqueQueueFamiliesArray;

    deviceQueueSet.forEachConstDeviceQueueCommandUnit(
        [&](const DeviceQueueCommandUnit &deviceQueueCommandUnit,
            const bool isGraphicsQueue, const bool isPresentQueue,
            const bool isTransferQueue) {
          uniqueQueueFamiliesArray.push_back(deviceQueueCommandUnit.queueIndex);
          return IterationContinue;
        });

    return uniqueQueueFamiliesArray;
  }

  static void
  fetchAndFillDeviceQueueSet(VkDevice logicalDevice,
                             DeviceQueueCommandUnitSet &deviceQueueSet) {
    deviceQueueSet.forEachNonConstDeviceQueueCommandUnit(
        [&](DeviceQueueCommandUnit &deviceQueueCommandUnit,
            const bool isGraphicsQueue, const bool isPresentQueue,
            const bool isTransferQueue) {
          vkGetDeviceQueue(logicalDevice, deviceQueueCommandUnit.queueIndex, 0,
                           &(deviceQueueCommandUnit.queue));
          return IterationContinue;
        });
  }

  uint32_t getDeviceGraphicsQueueIndex(void) const {
    std::optional<uint32_t> graphicsQueueIndex;
    forEachConstDeviceQueueCommandUnit(
        [&](const DeviceQueueCommandUnit &deviceQueueCommandUnit,
            const bool isGraphicsQueue, const bool isPresentQueue,
            const bool isTransferQueue) {
          if (isGraphicsQueue) {
            graphicsQueueIndex = deviceQueueCommandUnit.queueIndex;
          }
          return IterationContinue;
        });
    assert(graphicsQueueIndex.has_value());
    return graphicsQueueIndex.value();
  }

  uint32_t getDevicePresentQueueIndex(void) const {
    std::optional<uint32_t> presentQueueIndex;
    forEachConstDeviceQueueCommandUnit(
        [&](const DeviceQueueCommandUnit &deviceQueueCommandUnit,
            const bool isGraphicsQueue, const bool isPresentQueue,
            const bool isTransferQueue) {
          if (isPresentQueue) {
            presentQueueIndex = deviceQueueCommandUnit.queueIndex;
          }
          return IterationContinue;
        });
    assert(presentQueueIndex.has_value());
    return presentQueueIndex.value();
  }

  uint32_t getDeviceTransferQueueIndex(void) const {
    std::optional<uint32_t> transferQueueIndex;
    forEachConstDeviceQueueCommandUnit(
        [&](const DeviceQueueCommandUnit &deviceQueueCommandUnit,
            const bool isGraphicsQueue, const bool isPresentQueue,
            const bool isTransferQueue) {
          if (isTransferQueue) {
            transferQueueIndex = deviceQueueCommandUnit.queueIndex;
          }
          return IterationContinue;
        });
    assert(transferQueueIndex.has_value());
    return transferQueueIndex.value();
  }

  VkQueue getDeviceGraphicsQueue(void) const {
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    forEachConstDeviceQueueCommandUnit(
        [&](const DeviceQueueCommandUnit &deviceQueueCommandUnit,
            const bool isGraphicsQueue, const bool isPresentQueue,
            const bool isTransferQueue) {
          if (isGraphicsQueue) {
            graphicsQueue = deviceQueueCommandUnit.queue;
          }
          return IterationContinue;
        });
    return graphicsQueue;
  }

  VkQueue getDevicePresentQueue(void) const {
    VkQueue presentQueue = VK_NULL_HANDLE;
    forEachConstDeviceQueueCommandUnit(
        [&](const DeviceQueueCommandUnit &deviceQueueCommandUnit,
            const bool isGraphicsQueue, const bool isPresentQueue,
            const bool isTransferQueue) {
          if (isPresentQueue) {
            presentQueue = deviceQueueCommandUnit.queue;
          }
          return IterationContinue;
        });
    return presentQueue;
  }

  VkQueue getDeviceTransferQueue(void) const {
    VkQueue transferQueue = VK_NULL_HANDLE;
    forEachConstDeviceQueueCommandUnit(
        [&](const DeviceQueueCommandUnit &deviceQueueCommandUnit,
            const bool isGraphicsQueue, const bool isPresentQueue,
            const bool isTransferQueue) {
          if (isTransferQueue) {
            transferQueue = deviceQueueCommandUnit.queue;
          }
          return IterationContinue;
        });
    return transferQueue;
  }

  VkCommandBuffer getDeviceGraphicsQueueCommandBuffer(int index) const {
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    forEachConstDeviceQueueCommandUnit(
        [&](const DeviceQueueCommandUnit &deviceQueueCommandUnit,
            const bool isGraphicsQueue, const bool isPresentQueue,
            const bool isTransferQueue) {
          if (isGraphicsQueue) {
            commandBuffer = deviceQueueCommandUnit.queueCommandBuffers[index];
          }
          return IterationContinue;
        });
    return commandBuffer;
  }

  VkCommandBuffer getDeviceTransferQueueCommandBuffer(int index) const {
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    forEachConstDeviceQueueCommandUnit(
        [&](const DeviceQueueCommandUnit &deviceQueueCommandUnit,
            const bool isGraphicsQueue, const bool isPresentQueue,
            const bool isTransferQueue) {
          if (isTransferQueue) {
            commandBuffer = deviceQueueCommandUnit.queueCommandBuffers[index];
          }
          return IterationContinue;
        });
    return commandBuffer;
  }

  VkCommandPool getDeviceGraphicsQueueCommandPool(void) const {
    VkCommandPool commandPool = VK_NULL_HANDLE;
    forEachConstDeviceQueueCommandUnit(
        [&](const DeviceQueueCommandUnit &deviceQueueCommandUnit,
            const bool isGraphicsQueue, const bool isPresentQueue,
            const bool isTransferQueue) {
          if (isGraphicsQueue) {
            commandPool = deviceQueueCommandUnit.queueCommandPool;
          }
          return IterationContinue;
        });
    return commandPool;
  }

  VkCommandPool getDeviceTransferQueueCommandPool(void) const {
    VkCommandPool commandPool = VK_NULL_HANDLE;
    forEachConstDeviceQueueCommandUnit(
        [&](const DeviceQueueCommandUnit &deviceQueueCommandUnit,
            const bool isGraphicsQueue, const bool isPresentQueue,
            const bool isTransferQueue) {
          if (isTransferQueue) {
            commandPool = deviceQueueCommandUnit.queueCommandPool;
          }
          return IterationContinue;
        });
    return commandPool;
  }
};

struct Vertex {
  glm::vec2 pos;
  glm::vec3 color;

  static VkVertexInputBindingDescription getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
  };

  static std::array<VkVertexInputAttributeDescription, 2>
  getAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    return attributeDescriptions;
  }
};

const std::vector<Vertex> vertices = {{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                      {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                                      {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

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

  DeviceQueueCommandUnitSet presentableDeviceQueueCommandUnitSet;

  VkSwapchainKHR presentableSwapChain;
  std::vector<VkImage> presentableSwapChainImages;
  VkFormat presentableSwapChainImageFormat;
  VkExtent2D presentableSwapChainExtent;
  std::vector<VkImageView> presentableSwapChainImageViews;

  VkPipelineLayout presentableGraphicsPipelineLayout;
  VkRenderPass presentableRenderPass;
  VkPipeline presentableGraphicsPipeline;

  std::vector<VkFramebuffer> presentableSwapChainFrameBuffers;

  VkBuffer presentableVertexBuffer;
  VkDeviceMemory presentableVertexBufferMemory;

  std::vector<VkSemaphore> presentableImageAvailableSemaphores;
  std::vector<VkSemaphore> presentableRenderingFinishedSemaphores;
  std::vector<VkFence> presentableInFlightFences;

  std::vector<VkBuffer> presentableStagingBuffers;
  std::vector<VkDeviceMemory> presentableStagingBuffersMemories;
  std::vector<void *> presentableStagingBuffersData;
  size_t stagingBufferSize;

  uint32_t currentFrame = 0;

  std::vector<VkPhysicalDevice> unpresentablePhysicalDevices;
  std::vector<VkDevice> unpresentableLogicalDevices;

  std::vector<DeviceQueueCommandUnitSet> unpresentableDeviceQueueCommandUnitSet;

  std::vector<VkImage> unpresentableDeviceImages;
  std::vector<VkDeviceMemory> unpresentableDeviceImageMemories;
  std::vector<VkImageView> unpresentableDeviceImageViews;

  std::vector<VkPipelineLayout> unpresentableGraphicsPipelineLayouts;
  std::vector<VkRenderPass> unpresentableRenderPasses;
  std::vector<VkPipeline> unpresentableGraphicsPipelines;

  std::vector<VkFramebuffer> unpresentableDeviceFrameBuffers;

  std::vector<VkBuffer> unpresentableVertexBuffers;
  std::vector<VkDeviceMemory> unpresentableVertexBuffersMemories;

  std::vector<std::vector<VkSemaphore>>
      unpresentableRenderingFinishedSemaphores;
  std::vector<std::vector<VkFence>> unpresentableInterDeviceFences;

  std::vector<std::vector<VkBuffer>> unpresentableStagingBuffers;
  std::vector<std::vector<VkDeviceMemory>> unpresentableStagingBuffersMemories;
  std::vector<std::vector<void *>> unpresentableStagingBuffersData;

  bool frameBufferResized = false;

private:
  void initWindow() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, frameBufferResizeCallback);
  }

  static void frameBufferResizeCallback(GLFWwindow *window, int width,
                                        int height) {
    auto app = reinterpret_cast<HelloTriangleApplication *>(
        glfwGetWindowUserPointer(window));
    app->frameBufferResized = true;
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

  static std::string getPhysicalDeviceName(VkPhysicalDevice physicalDevice) {
    VkPhysicalDeviceProperties deviceProperties;

    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    return deviceProperties.deviceName;
  }

  static void forEachAvailablePhysicalDevice(
      VkInstance instance, std::function<IteratorStatus(VkPhysicalDevice)> f) {
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

  static void forEachExtensionOfPhysicalDevice(
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

  static bool isPhysicalDeviceDedicatedGPU(VkPhysicalDevice physicalDevice) {
    VkPhysicalDeviceProperties deviceProperties;

    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      return true;
    }
    return false;
  }

  static bool isPhysicalDeviceIntegratedGPU(VkPhysicalDevice physicalDevice) {
    VkPhysicalDeviceProperties deviceProperties;

    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
      return true;
    }
    return false;
  }

  static bool isPhysicalDeviceCPU(VkPhysicalDevice physicalDevice) {
    VkPhysicalDeviceProperties deviceProperties;

    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU) {
      return true;
    }
    return false;
  }

  static bool
  isGeometryShaderSupportedByPhysicalDevice(VkPhysicalDevice physicalDevice) {
    VkPhysicalDeviceFeatures deviceFeatures;

    vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

    if (!deviceFeatures.geometryShader) {
      return false;
    }

    return true;
  }

  static bool
  isExtensionSupportedByPhysicalDevice(VkPhysicalDevice physicalDevice,
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

  static bool
  checkPhysicalDeviceExtensionSupport(VkPhysicalDevice physicalDevice) {
    for (const auto &deviceExtensionName : deviceExtensions) {
      if (!isExtensionSupportedByPhysicalDevice(physicalDevice,
                                                deviceExtensionName)) {
        return false;
      }
    }
    return true;
  }

  static bool isDeviceSuitable(VkPhysicalDevice device) {
    /*
    printPhysicalDeviceExtensions(device);
    */

    if (!DeviceQueueCommandUnitSet::isGraphicsQueueSupportedByPhysicalDevice(
            device)) {
      return false;
    }

    if (!DeviceQueueCommandUnitSet::isTransferQueueSupportedByPhysicalDevice(
            device)) {
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

  static int rateDeviceSuitability(VkPhysicalDevice device) {
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

  static void printPhysicalDeviceExtensions(VkPhysicalDevice device) {
    std::cout << "Extensions for Device \"" << getPhysicalDeviceName(device)
              << "\" ..." << std::endl;

    forEachExtensionOfPhysicalDevice(
        device, [&](VkExtensionProperties extensionProperties) {
          std::cout << "   Extension: " << extensionProperties.extensionName
                    << std::endl;
          return IterationContinue;
        });
  }

  void pickPhysicalDevice(VkInstance instance, VkSurfaceKHR windowSurface) {
    presentablePhysicalDevice = VK_NULL_HANDLE;
    unpresentablePhysicalDevices.clear();
    forEachAvailablePhysicalDevice(instance, [&](VkPhysicalDevice device) {
      std::cout << "Probing Device :- " << getPhysicalDeviceName(device)
                << "..." << std::endl;
      std::cout << "=================" << std::endl;
      int score = rateDeviceSuitability(device);
      std::cout << "    score :- " << score << std::endl;

      if (!isDeviceSuitable(device)) {
        std::cout << "    skiping device as its not suitable" << std::endl;
        return IterationContinue;
      }

      if (isGeometryShaderSupportedByPhysicalDevice(device)) {
        if (!presentablePhysicalDevice &&
            DeviceQueueCommandUnitSet::isPhysicalDevicePresentable(
                device, windowSurface) &&
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

    if (unpresentablePhysicalDevices.size() == 0) {
      throw std::runtime_error(
          "failed to find unpresentable GPUs with Vulkan support!");
    }
  }

  struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentMode;
  };

  static SwapChainSupportDetails
  querySwapChainDetails(VkPhysicalDevice physicalDevice,
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

  static bool
  isSwapChainAdequateForPresentation(VkPhysicalDevice physicalDevice,
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

  static VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats) {

    for (const auto &availableFormat : availableFormats) {
      if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
          availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        return availableFormat;
      }
    }

    return availableFormats[0];
  }

  static VkPresentModeKHR chooseSwapPresentMode(
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

  static VkExtent2D
  chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities,
                   GLFWwindow *window) {
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

  static void
  createLogicalDevice(VkPhysicalDevice physicalDevice,
                      VkSurfaceKHR windowSurface, VkDevice &logicalDevice,
                      DeviceQueueCommandUnitSet &deviceQueueCommandUnitSet) {

    DeviceQueueCommandUnitSet::decideAndFillQueueSetIndices(
        physicalDevice, windowSurface, deviceQueueCommandUnitSet);

    std::vector<uint32_t> uniqueQueueFamiliesArray =
        DeviceQueueCommandUnitSet::getUniqueQueueFamiliesIndices(
            deviceQueueCommandUnitSet);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    float queuePriority = 1.0f;
    for (uint32_t queueFamilyIndex : uniqueQueueFamiliesArray) {
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

    DeviceQueueCommandUnitSet::fetchAndFillDeviceQueueSet(
        logicalDevice, deviceQueueCommandUnitSet);

    std::cout << "Created Device Queues for Device "
              << getPhysicalDeviceName(physicalDevice) << std::endl;
  }

  void createSurface(VkSurfaceKHR &windowSurface) {
    if (glfwCreateWindowSurface(instance, window, nullptr, &windowSurface) !=
        VK_SUCCESS) {
      std::cerr << "Failed to create vulkan surface!" << std::endl;
    } else {
      std::cout << "Created surface..." << std::endl;
    }
  }

  static uint32_t findMemoryType(uint32_t typeFilter,
                                 VkMemoryPropertyFlags properties,
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

  static void allocateMemoryForBuffer(VkPhysicalDevice physicalDevice,
                                      VkDevice logicalDevice, VkBuffer buffer,
                                      VkDeviceSize bufferSize,
                                      VkMemoryPropertyFlags memoryProperties,
                                      VkDeviceMemory &bufferMemory) {

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(logicalDevice, buffer, &memRequirements);

    assert(bufferSize == memRequirements.size);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(
        memRequirements.memoryTypeBits, memoryProperties, physicalDevice);

    if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &bufferMemory) !=
        VK_SUCCESS) {
      throw std::runtime_error("Failed to allocate buffer memory!");
    }

    if (vkBindBufferMemory(logicalDevice, buffer, bufferMemory, 0) !=
        VK_SUCCESS) {
      throw std::runtime_error("Failed to bind memory to buffer!");
    }
  }

  static void
  createBuffer(VkPhysicalDevice physicalDevice, VkDevice logicalDevice,
               const DeviceQueueCommandUnitSet &deviceQueueCommandUnitSet,
               VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsage,
               VkMemoryPropertyFlags memoryProperties,
               VkSurfaceKHR windowSurface, const bool needGraphics,
               const bool needPresent, const bool needTransfer,
               VkBuffer &buffer, VkDeviceMemory &bufferMemory) {

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = bufferUsage;

    std::vector<uint32_t> uniqueQueueFamiliesArray =
        DeviceQueueCommandUnitSet::getUniqueQueueFamiliesIndices(
            deviceQueueCommandUnitSet);

    if (uniqueQueueFamiliesArray.size() > 1) {
      bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
      bufferInfo.queueFamilyIndexCount =
          static_cast<uint32_t>(uniqueQueueFamiliesArray.size());
      bufferInfo.pQueueFamilyIndices = uniqueQueueFamiliesArray.data();
    } else {
      bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
      bufferInfo.queueFamilyIndexCount = 0;     // Optional
      bufferInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    if (vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &buffer) !=
        VK_SUCCESS) {
      throw std::runtime_error("Failed to create buffer!");
    }

    allocateMemoryForBuffer(physicalDevice, logicalDevice, buffer, bufferSize,
                            memoryProperties, bufferMemory);
  }

  static void allocateMemoryForImage(VkPhysicalDevice physicalDevice,
                                     VkDevice logicalDevice, VkImage image,
                                     VkMemoryPropertyFlags memoryProperties,
                                     VkDeviceMemory &imageMemory) {

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(logicalDevice, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(
        memRequirements.memoryTypeBits, memoryProperties, physicalDevice);

    if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &imageMemory) !=
        VK_SUCCESS) {
      throw std::runtime_error("Failed to allocate image memory!");
    }

    if (vkBindImageMemory(logicalDevice, image, imageMemory, 0) != VK_SUCCESS) {
      throw std::runtime_error("Failed to bind memory to image!");
    }
  }

  static void
  createImage(VkPhysicalDevice physicalDevice, VkDevice logicalDevice,
              const DeviceQueueCommandUnitSet &deviceQueueCommandUnitSet,
              uint32_t width, uint32_t height, VkFormat format,
              VkImageUsageFlags imageUsage,
              VkMemoryPropertyFlags memoryProperties,
              VkSurfaceKHR windowSurface, const bool needGraphics,
              const bool needPresent, const bool needTransfer, VkImage &image,
              VkDeviceMemory &imageMemory) {

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
    createInfo.usage = imageUsage;
    createInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    std::vector<uint32_t> uniqueQueueFamiliesArray =
        DeviceQueueCommandUnitSet::getUniqueQueueFamiliesIndices(
            deviceQueueCommandUnitSet);

    if (uniqueQueueFamiliesArray.size() > 1) {
      createInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
      createInfo.queueFamilyIndexCount =
          static_cast<uint32_t>(uniqueQueueFamiliesArray.size());
      createInfo.pQueueFamilyIndices = uniqueQueueFamiliesArray.data();
    } else {
      createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
      createInfo.queueFamilyIndexCount = 0;     // Optional
      createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    if (vkCreateImage(logicalDevice, &createInfo, nullptr, &image) !=
        VK_SUCCESS) {
      throw std::runtime_error("Failed to create image!");
    }

    allocateMemoryForImage(physicalDevice, logicalDevice, image,
                           memoryProperties, imageMemory);
  }

  static void
  forEachImageOfSwapChain(VkDevice logicalDevice, VkSwapchainKHR swapChain,
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

  static void createSwapChainForPresentation(
      VkPhysicalDevice physicalDevice, VkDevice logicalDevice,
      const DeviceQueueCommandUnitSet &deviceQueueCommandUnitSet,
      GLFWwindow *window, VkSurfaceKHR windowSurface,
      const bool isImageTransferDestination, VkSwapchainKHR &swapChain,
      std::vector<VkImage> &swapChainImages, VkFormat &swapChainImageFormat,
      VkExtent2D &swapChainExtent) {

    SwapChainSupportDetails swapChainSupport =
        querySwapChainDetails(physicalDevice, windowSurface);

    VkSurfaceFormatKHR surfaceFormat =
        chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode =
        chooseSwapPresentMode(swapChainSupport.presentMode);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, window);

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

    if (isImageTransferDestination) {
      createInfo.imageUsage =
          VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    } else {
      createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }

    std::vector<uint32_t> uniqueQueueFamiliesArray =
        DeviceQueueCommandUnitSet::getUniqueQueueFamiliesIndices(
            deviceQueueCommandUnitSet);

    if (uniqueQueueFamiliesArray.size() > 1) {
      createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
      createInfo.queueFamilyIndexCount =
          static_cast<uint32_t>(uniqueQueueFamiliesArray.size());
      createInfo.pQueueFamilyIndices = uniqueQueueFamiliesArray.data();
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

  static void createUnpresentableDeviceImage(
      VkPhysicalDevice physicalDevice, VkDevice logicalDevice,
      const DeviceQueueCommandUnitSet &deviceQueueCommandUnitSet,
      uint32_t width, uint32_t height, VkFormat format, VkImage &image,
      VkDeviceMemory &imageMemory) {

    createImage(physicalDevice, logicalDevice, deviceQueueCommandUnitSet, width,
                height, format,
                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                    VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_NULL_HANDLE,
                /*needGraphics=*/true, /*needPresent=*/false,
                /*needTransfer=*/true, image, imageMemory);

    std::cout
        << "Created Image and allocated memory for unpresentable device \""
        << getPhysicalDeviceName(physicalDevice) << "\"" << std::endl;
  }

  static VkImageViewCreateInfo fillImageViewCreateInfo(VkImage image,
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

  static void
  createImageViewsForPresentation(std::vector<VkImage> &images, VkFormat format,
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

  static void createImageViewForUnpresentableDevice(
      VkImage image, VkFormat format, VkPhysicalDevice physicalDevice,
      VkDevice logicalDevice, VkImageView &imageView) {
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

  static void
  destroyImageViewsForPresentation(VkDevice logicalDevice,
                                   std::vector<VkImageView> &imageViews) {
    for (auto imageView : imageViews) {
      vkDestroyImageView(logicalDevice, imageView, nullptr);
    }
    imageViews.clear();
  }

  static std::vector<char> readFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
      throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();
    return buffer;
  }

  static VkShaderModule createShaderModule(VkPhysicalDevice physicalDevice,
                                           VkDevice logicalDevice,
                                           const std::string &filename) {
    auto shaderCode = readFile(filename);

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = shaderCode.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(shaderCode.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(logicalDevice, &createInfo, nullptr,
                             &shaderModule) != VK_SUCCESS) {
      throw std::runtime_error("failed to create shader module!");
    } else {
      std::cout << "Created Shader module \"" << filename << "\" for device \""
                << getPhysicalDeviceName(physicalDevice) << "\"" << std::endl;
    }

    return shaderModule;
  }

  static void createRenderPass(VkPhysicalDevice physicalDevice,
                               VkDevice logicalDevice, VkFormat format,
                               VkImageLayout finalLayout,
                               VkRenderPass &renderPass) {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = finalLayout;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    // TODO: can these dependencies be also needed for dependencies between
    // render and copy to host visible buffers in
    // recordCommandBufferForUnpresentableDevice
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(logicalDevice, &renderPassInfo, nullptr,
                           &renderPass) != VK_SUCCESS) {
      throw std::runtime_error("failed to create render pass!");
    } else {
      std::cout << "Created render pass for device \""
                << getPhysicalDeviceName(physicalDevice) << "\"" << std::endl;
    }
  }

  static void createGraphicsPipelineLayout(VkPhysicalDevice physicalDevice,
                                           VkDevice logicalDevice,
                                           VkPipelineLayout &pipelineLayout) {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;            // Optional
    pipelineLayoutInfo.pSetLayouts = nullptr;         // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0;    // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    if (vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr,
                               &pipelineLayout) != VK_SUCCESS) {
      throw std::runtime_error("failed to create pipeline layout!");
    } else {
      std::cout << "Created graphics pipeline layout for \""
                << getPhysicalDeviceName(physicalDevice) << "\"" << std::endl;
    }
  }

  static void createGraphicsPipeline(VkPhysicalDevice physicalDevice,
                                     VkDevice logicalDevice, VkExtent2D extent,
                                     const bool isDynamicViewPortAndScissor,
                                     VkRenderPass renderPass,
                                     VkPipelineLayout pipelineLayout,
                                     VkPipeline &graphicsPipeline) {
    VkShaderModule vertShaderModule =
        createShaderModule(physicalDevice, logicalDevice, "shaders/vert.spv");
    VkShaderModule fragShaderModule =
        createShaderModule(physicalDevice, logicalDevice, "shaders/frag.spv");

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                      fragShaderStageInfo};

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescription = Vertex::getAttributeDescriptions();

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount =
        static_cast<uint32_t>(attributeDescription.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescription.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType =
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)extent.width;
    viewport.height = (float)extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = extent;

    VkPipelineViewportStateCreateInfo viewportState{};
    VkPipelineDynamicStateCreateInfo dynamicState{};
    std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
                                                 VK_DYNAMIC_STATE_SCISSOR};

    if (isDynamicViewPortAndScissor) {
      dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
      dynamicState.dynamicStateCount =
          static_cast<uint32_t>(dynamicStates.size());
      dynamicState.pDynamicStates = dynamicStates.data();

      viewportState.sType =
          VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
      viewportState.viewportCount = 1;
      viewportState.scissorCount = 1;
    } else {
      dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
      dynamicState.dynamicStateCount = 0;
      dynamicState.pDynamicStates = nullptr;

      viewportState.sType =
          VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
      viewportState.viewportCount = 1;
      viewportState.pViewports = &viewport;
      viewportState.scissorCount = 1;
      viewportState.pScissors = &scissor;
    }

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f;          // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f;    // Optional

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType =
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;          // Optional
    multisampling.pSampleMask = nullptr;            // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE;      // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType =
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr; // Optional
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;

    pipelineInfo.layout = pipelineLayout;

    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;

    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1;              // Optional

    if (vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1,
                                  &pipelineInfo, nullptr,
                                  &graphicsPipeline) != VK_SUCCESS) {
      throw std::runtime_error("failed to create graphics pipeline!");
    } else {
      std::cout << "Created graphics pipeline for \""
                << getPhysicalDeviceName(physicalDevice) << "\"" << std::endl;
    }

    vkDestroyShaderModule(logicalDevice, vertShaderModule, nullptr);
    vkDestroyShaderModule(logicalDevice, fragShaderModule, nullptr);
  }

  static VkFramebufferCreateInfo
  createFrameBufferInfo(std::vector<VkImageView> &attachments,
                        VkRenderPass renderPass, VkExtent2D extent) {
    VkFramebufferCreateInfo frameBufferInfo{};
    frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frameBufferInfo.renderPass = renderPass;
    frameBufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    frameBufferInfo.pAttachments = attachments.data();
    frameBufferInfo.width = extent.width;
    frameBufferInfo.height = extent.height;
    frameBufferInfo.layers = 1;

    return frameBufferInfo;
  }

  static void createFrameBuffer(VkDevice logicalDevice, VkImageView imageView,
                                VkRenderPass renderPass, VkExtent2D extent,
                                VkFramebuffer &frameBuffer) {
    std::vector<VkImageView> attachments;
    attachments.push_back(imageView);

    VkFramebufferCreateInfo frameBufferInfo =
        createFrameBufferInfo(attachments, renderPass, extent);

    if (vkCreateFramebuffer(logicalDevice, &frameBufferInfo, nullptr,
                            &frameBuffer) != VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer!");
    }
  }

  static void createFrameBuffersForPresentation(
      VkPhysicalDevice physicalDevice, VkDevice logicalDevice,
      std::vector<VkImageView> &imageViews, VkRenderPass renderPass,
      VkExtent2D extent, std::vector<VkFramebuffer> &frameBuffers) {
    frameBuffers.resize(imageViews.size());
    for (size_t i = 0; i < imageViews.size(); i++) {
      createFrameBuffer(logicalDevice, imageViews[i], renderPass, extent,
                        frameBuffers[i]);
      std::cout << "Created presentable framebuffer \"" << i
                << "\" for device \"" << getPhysicalDeviceName(physicalDevice)
                << "\"" << std::endl;
    }
  }

  static void createFrameBufferForUnpresentableDevice(
      VkPhysicalDevice physicalDevice, VkDevice logicalDevice,
      VkImageView imageView, VkRenderPass renderPass, VkExtent2D extent,
      VkFramebuffer &frameBuffer) {
    createFrameBuffer(logicalDevice, imageView, renderPass, extent,
                      frameBuffer);
    std::cout << "Created unpresentable framebuffer \"0\" for device \""
              << getPhysicalDeviceName(physicalDevice) << "\"" << std::endl;
  }

  static void
  destroyFrameBuffersForPresentation(VkDevice logicalDevice,
                                     std::vector<VkFramebuffer> &frameBuffers) {
    for (auto frameBuffer : frameBuffers) {
      vkDestroyFramebuffer(logicalDevice, frameBuffer, nullptr);
    }
    frameBuffers.clear();
  }

  static void createCommandPool(VkPhysicalDevice physicalDevice,
                                VkDevice logicalDevice,
                                uint32_t queueFamilyIndex,
                                VkCommandPool &commandPool) {
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndex;

    if (vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &commandPool) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create command pool!");
    } else {
      std::cout << "Created command pool for Device \""
                << getPhysicalDeviceName(physicalDevice) << "\"" << std::endl;
    }
  }

  static void
  createCommandBuffers(VkPhysicalDevice physicalDevice, VkDevice logicalDevice,
                       VkCommandPool commandPool,
                       VkCommandBuffer commandBuffers[MAX_FRAMES_IN_FLIGHT]) {

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    if (vkAllocateCommandBuffers(logicalDevice, &allocInfo, commandBuffers) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to allocate command buffers!");
    } else {
      std::cout << "Created command buffers for Device \""
                << getPhysicalDeviceName(physicalDevice) << "\"" << std::endl;
    }
  }

  static void createDeviceCommandPoolsAndBuffers(
      VkPhysicalDevice physicalDevice, VkDevice logicalDevice,
      DeviceQueueCommandUnitSet &deviceQueueCommandUnitSet) {

    deviceQueueCommandUnitSet.forEachNonConstDeviceQueueCommandUnit(
        [&](DeviceQueueCommandUnit &deviceQueueCommandUnit,
            const bool isGraphicsQueue, const bool isPresentQueue,
            const bool isTransferQueue) {
          createCommandPool(physicalDevice, logicalDevice,
                            deviceQueueCommandUnit.queueIndex,
                            deviceQueueCommandUnit.queueCommandPool);

          createCommandBuffers(physicalDevice, logicalDevice,
                               deviceQueueCommandUnit.queueCommandPool,
                               deviceQueueCommandUnit.queueCommandBuffers);
          return IterationContinue;
        });
  }

  static void destroyDeviceCommandPoolsAndBuffers(
      VkDevice logicalDevice,
      const DeviceQueueCommandUnitSet &deviceQueueCommandUnitSet) {

    deviceQueueCommandUnitSet.forEachConstDeviceQueueCommandUnit(
        [&](const DeviceQueueCommandUnit &deviceQueueCommandUnit,
            const bool isGraphicsQueue, const bool isPresentQueue,
            const bool isTransferQueue) {
          vkDestroyCommandPool(
              logicalDevice, deviceQueueCommandUnit.queueCommandPool, nullptr);
          return IterationContinue;
        });
  }

  static void
  createVertexBuffer(VkPhysicalDevice physicalDevice, VkDevice logicalDevice,
                     const DeviceQueueCommandUnitSet &deviceQueueCommandUnitSet,
                     VkBuffer &vertexBuffer,
                     VkDeviceMemory &vertexBufferMemory) {

    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    createBuffer(physicalDevice, logicalDevice, deviceQueueCommandUnitSet,
                 bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 VK_NULL_HANDLE, /*needGraphics=*/true, /*needPresent=*/false,
                 /*needTransfer=*/true, vertexBuffer, vertexBufferMemory);

    std::cout << "Created vertex buffer and Allocated memory for device \""
              << getPhysicalDeviceName(physicalDevice) << "\"" << std::endl;

    void *data;
    vkMapMemory(logicalDevice, vertexBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(logicalDevice, vertexBufferMemory);
  }

  static void recordCommandBufferForPresentation(
      VkPhysicalDevice physicalDevice, VkCommandBuffer commandBuffer,
      VkBuffer vertexBuffer, uint32_t imageIndex, VkRenderPass renderPass,
      std::vector<VkFramebuffer> &frameBuffers, VkExtent2D extent,
      VkPipeline graphicsPipeline, const bool isDynamicViewPortAndScissor) {

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;                  // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {

      throw std::runtime_error("failed to begin recording command buffer!");
    } else {
      std::cout << "Begin recording command buffer for presentation with "
                   "imageIndex \""
                << imageIndex << "\" for Device \""
                << getPhysicalDeviceName(physicalDevice) << "\"" << std::endl;
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = frameBuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = extent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      graphicsPipeline);

    VkBuffer vertexBuffers[] = {vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = extent;

    if (isDynamicViewPortAndScissor) {
      vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
      vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer!");
    } else {
      std::cout
          << "End recording command buffer for presentation with imageIndex \""
          << imageIndex << "\" for Device \""
          << getPhysicalDeviceName(physicalDevice) << "\"" << std::endl;
    }
  }

  static void recordCommandBufferForCopyingFromBuffer(
      VkPhysicalDevice physicalDevice, VkDevice logicalDevice,
      VkCommandBuffer commandBuffer, VkImage image, VkExtent2D extent,
      VkBuffer stagingBuffer) {

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;                  // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {

      throw std::runtime_error("failed to begin recording command buffer!");
    } else {
      std::cout << "Begin recording command buffer for copying for Device \""
                << getPhysicalDeviceName(physicalDevice) << "\"" << std::endl;
    }

    VkImageMemoryBarrier barrierBeforeCopy{};
    barrierBeforeCopy.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrierBeforeCopy.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrierBeforeCopy.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrierBeforeCopy.srcQueueFamilyIndex =
        VK_QUEUE_FAMILY_IGNORED; // Ignore for single queue
    barrierBeforeCopy.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierBeforeCopy.image = image;
    barrierBeforeCopy.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrierBeforeCopy.subresourceRange.baseMipLevel = 0;
    barrierBeforeCopy.subresourceRange.levelCount = 1;
    barrierBeforeCopy.subresourceRange.baseArrayLayer = 0;
    barrierBeforeCopy.subresourceRange.layerCount = 1;

    barrierBeforeCopy.srcAccessMask = 0; // No access needed before transition
    barrierBeforeCopy.dstAccessMask =
        VK_ACCESS_TRANSFER_WRITE_BIT; // for transfer operation

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
                         nullptr, 1, &barrierBeforeCopy);

    VkImageSubresourceLayers subresource{};
    subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresource.mipLevel = 0;
    subresource.baseArrayLayer = 0;
    subresource.layerCount = 1;

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource = subresource;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {extent.width, extent.height, 1};

    vkCmdCopyBufferToImage(commandBuffer, stagingBuffer, image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    VkImageMemoryBarrier barrierAfterCopy{};
    barrierAfterCopy.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrierAfterCopy.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrierAfterCopy.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    barrierAfterCopy.srcQueueFamilyIndex =
        VK_QUEUE_FAMILY_IGNORED; // Ignore for single queue
    barrierAfterCopy.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierAfterCopy.image = image;
    barrierAfterCopy.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrierAfterCopy.subresourceRange.baseMipLevel = 0;
    barrierAfterCopy.subresourceRange.levelCount = 1;
    barrierAfterCopy.subresourceRange.baseArrayLayer = 0;
    barrierAfterCopy.subresourceRange.layerCount = 1;

    barrierAfterCopy.srcAccessMask = 0; // No access needed before transition
    barrierAfterCopy.dstAccessMask =
        VK_ACCESS_TRANSFER_WRITE_BIT; // for transfer operation

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
                         nullptr, 1, &barrierAfterCopy);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer!");
    } else {
      std::cout << "End recording command buffer for copying for Device \""
                << getPhysicalDeviceName(physicalDevice) << "\"" << std::endl;
    }
  }

  static void recordCommandBufferForUnpresentableDevice(
      VkPhysicalDevice physicalDevice,
      const DeviceQueueCommandUnitSet &deviceQueueCommandUnitSet,
      const uint32_t currentFrame, VkBuffer vertexBuffer, VkImage image,
      VkRenderPass renderPass, VkFramebuffer frameBuffer, VkExtent2D extent,
      VkPipeline graphicsPipeline, VkBuffer stagingBuffer,
      size_t stagingBufferSize, const bool isDynamicViewPortAndScissor) {

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;                  // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(
            deviceQueueCommandUnitSet.getDeviceGraphicsQueueCommandBuffer(
                currentFrame),
            &beginInfo) != VK_SUCCESS) {

      throw std::runtime_error("failed to begin recording command buffer!");
    } else {
      std::cout << "Begin recording command buffer for unpresentable Device \""
                << getPhysicalDeviceName(physicalDevice) << "\"" << std::endl;
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = frameBuffer;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = extent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(
        deviceQueueCommandUnitSet.getDeviceGraphicsQueueCommandBuffer(
            currentFrame),
        &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(
        deviceQueueCommandUnitSet.getDeviceGraphicsQueueCommandBuffer(
            currentFrame),
        VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    VkBuffer vertexBuffers[] = {vertexBuffer};
    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(
        deviceQueueCommandUnitSet.getDeviceGraphicsQueueCommandBuffer(
            currentFrame),
        0, 1, vertexBuffers, offsets);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = extent;

    if (isDynamicViewPortAndScissor) {
      vkCmdSetViewport(
          deviceQueueCommandUnitSet.getDeviceGraphicsQueueCommandBuffer(
              currentFrame),
          0, 1, &viewport);
      vkCmdSetScissor(
          deviceQueueCommandUnitSet.getDeviceGraphicsQueueCommandBuffer(
              currentFrame),
          0, 1, &scissor);
    }

    vkCmdDraw(deviceQueueCommandUnitSet.getDeviceGraphicsQueueCommandBuffer(
                  currentFrame),
              static_cast<uint32_t>(vertices.size()), 1, 0, 0);

    vkCmdEndRenderPass(
        deviceQueueCommandUnitSet.getDeviceGraphicsQueueCommandBuffer(
            currentFrame));

    VkImageMemoryBarrier barrierBeforeCopy{};
    barrierBeforeCopy.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrierBeforeCopy.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrierBeforeCopy.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    if (deviceQueueCommandUnitSet.getDeviceGraphicsQueue() !=
        deviceQueueCommandUnitSet.getDeviceTransferQueue()) {
      barrierBeforeCopy.srcQueueFamilyIndex =
          deviceQueueCommandUnitSet.getDeviceGraphicsQueueIndex();
      barrierBeforeCopy.dstQueueFamilyIndex =
          deviceQueueCommandUnitSet.getDeviceTransferQueueIndex();
    } else {
      barrierBeforeCopy.srcQueueFamilyIndex =
          VK_QUEUE_FAMILY_IGNORED; // Ignore for single queue
      barrierBeforeCopy.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    }
    barrierBeforeCopy.image = image;
    barrierBeforeCopy.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrierBeforeCopy.subresourceRange.baseMipLevel = 0;
    barrierBeforeCopy.subresourceRange.levelCount = 1;
    barrierBeforeCopy.subresourceRange.baseArrayLayer = 0;
    barrierBeforeCopy.subresourceRange.layerCount = 1;

    barrierBeforeCopy.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    barrierBeforeCopy.dstAccessMask =
        VK_ACCESS_TRANSFER_READ_BIT; // for transfer operation

    vkCmdPipelineBarrier(
        deviceQueueCommandUnitSet.getDeviceGraphicsQueueCommandBuffer(
            currentFrame),
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1,
        &barrierBeforeCopy);

    if (deviceQueueCommandUnitSet.getDeviceGraphicsQueue() !=
        deviceQueueCommandUnitSet.getDeviceTransferQueue()) {
      if (vkEndCommandBuffer(
              deviceQueueCommandUnitSet.getDeviceGraphicsQueueCommandBuffer(
                  currentFrame)) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
      } else {
        std::cout << "End recording command buffer for unpresentable Device \""
                  << getPhysicalDeviceName(physicalDevice) << "\"" << std::endl;
      }

      if (vkBeginCommandBuffer(
              deviceQueueCommandUnitSet.getDeviceTransferQueueCommandBuffer(
                  currentFrame),
              &beginInfo) != VK_SUCCESS) {

        throw std::runtime_error("failed to begin recording command buffer!");
      } else {
        std::cout
            << "Begin recording command buffer for unpresentable Device \""
            << getPhysicalDeviceName(physicalDevice) << "\"" << std::endl;
      }
    }

    VkImageSubresourceLayers subresource{};
    subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresource.mipLevel = 0;
    subresource.baseArrayLayer = 0;
    subresource.layerCount = 1;

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource = subresource;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {extent.width, extent.height, 1};

    vkCmdCopyImageToBuffer(
        deviceQueueCommandUnitSet.getDeviceTransferQueueCommandBuffer(
            currentFrame),
        image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, stagingBuffer, 1, &region);

    VkBufferMemoryBarrier bufferBarrierAfterCopy{};
    bufferBarrierAfterCopy.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    bufferBarrierAfterCopy.srcQueueFamilyIndex =
        deviceQueueCommandUnitSet.getDeviceTransferQueueIndex();
    bufferBarrierAfterCopy.dstQueueFamilyIndex =
        deviceQueueCommandUnitSet.getDeviceTransferQueueIndex();
    bufferBarrierAfterCopy.buffer = stagingBuffer;
    bufferBarrierAfterCopy.offset = 0;
    bufferBarrierAfterCopy.size = stagingBufferSize;
    bufferBarrierAfterCopy.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    bufferBarrierAfterCopy.dstAccessMask = VK_ACCESS_HOST_READ_BIT;

    vkCmdPipelineBarrier(
        deviceQueueCommandUnitSet.getDeviceTransferQueueCommandBuffer(
            currentFrame),
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_HOST_BIT, 0, 0,
        nullptr, 1, &bufferBarrierAfterCopy, 0, nullptr);

    VkImageMemoryBarrier imageBarrierAfterCopy{};
    imageBarrierAfterCopy.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageBarrierAfterCopy.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    imageBarrierAfterCopy.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    if (deviceQueueCommandUnitSet.getDeviceGraphicsQueue() !=
        deviceQueueCommandUnitSet.getDeviceTransferQueue()) {
      imageBarrierAfterCopy.srcQueueFamilyIndex =
          deviceQueueCommandUnitSet.getDeviceTransferQueueIndex();
      imageBarrierAfterCopy.dstQueueFamilyIndex =
          deviceQueueCommandUnitSet.getDeviceGraphicsQueueIndex();
    } else {
      imageBarrierAfterCopy.srcQueueFamilyIndex =
          VK_QUEUE_FAMILY_IGNORED; // Ignore for single queue
      imageBarrierAfterCopy.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    }
    imageBarrierAfterCopy.image = image;
    imageBarrierAfterCopy.subresourceRange.aspectMask =
        VK_IMAGE_ASPECT_COLOR_BIT;
    imageBarrierAfterCopy.subresourceRange.baseMipLevel = 0;
    imageBarrierAfterCopy.subresourceRange.levelCount = 1;
    imageBarrierAfterCopy.subresourceRange.baseArrayLayer = 0;
    imageBarrierAfterCopy.subresourceRange.layerCount = 1;

    imageBarrierAfterCopy.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    imageBarrierAfterCopy.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    vkCmdPipelineBarrier(
        deviceQueueCommandUnitSet.getDeviceTransferQueueCommandBuffer(
            currentFrame),
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0,
        nullptr, 1, &imageBarrierAfterCopy);

    if (vkEndCommandBuffer(
            deviceQueueCommandUnitSet.getDeviceTransferQueueCommandBuffer(
                currentFrame)) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer!");
    } else {
      std::cout << "End recording command buffer for unpresentable Device \""
                << getPhysicalDeviceName(physicalDevice) << "\"" << std::endl;
    }
  }

  static void recordRenderCommandBufferForUnpresentableDevice(
      VkPhysicalDevice physicalDevice,
      const DeviceQueueCommandUnitSet &deviceQueueCommandUnitSet,
      const uint32_t currentFrame, VkBuffer vertexBuffer, VkImage image,
      VkRenderPass renderPass, VkFramebuffer frameBuffer, VkExtent2D extent,
      VkPipeline graphicsPipeline, VkBuffer stagingBuffer,
      size_t stagingBufferSize, const bool isDynamicViewPortAndScissor) {

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;                  // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(
            deviceQueueCommandUnitSet.getDeviceGraphicsQueueCommandBuffer(
                currentFrame),
            &beginInfo) != VK_SUCCESS) {

      throw std::runtime_error("failed to begin recording command buffer!");
    } else {
      std::cout << "Begin recording command buffer for unpresentable Device \""
                << getPhysicalDeviceName(physicalDevice) << "\"" << std::endl;
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = frameBuffer;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = extent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(
        deviceQueueCommandUnitSet.getDeviceGraphicsQueueCommandBuffer(
            currentFrame),
        &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(
        deviceQueueCommandUnitSet.getDeviceGraphicsQueueCommandBuffer(
            currentFrame),
        VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    VkBuffer vertexBuffers[] = {vertexBuffer};
    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(
        deviceQueueCommandUnitSet.getDeviceGraphicsQueueCommandBuffer(
            currentFrame),
        0, 1, vertexBuffers, offsets);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = extent;

    if (isDynamicViewPortAndScissor) {
      vkCmdSetViewport(
          deviceQueueCommandUnitSet.getDeviceGraphicsQueueCommandBuffer(
              currentFrame),
          0, 1, &viewport);
      vkCmdSetScissor(
          deviceQueueCommandUnitSet.getDeviceGraphicsQueueCommandBuffer(
              currentFrame),
          0, 1, &scissor);
    }

    vkCmdDraw(deviceQueueCommandUnitSet.getDeviceGraphicsQueueCommandBuffer(
                  currentFrame),
              static_cast<uint32_t>(vertices.size()), 1, 0, 0);

    vkCmdEndRenderPass(
        deviceQueueCommandUnitSet.getDeviceGraphicsQueueCommandBuffer(
            currentFrame));

    VkImageMemoryBarrier barrierBeforeCopy{};
    barrierBeforeCopy.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrierBeforeCopy.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrierBeforeCopy.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    if (deviceQueueCommandUnitSet.getDeviceGraphicsQueue() !=
        deviceQueueCommandUnitSet.getDeviceTransferQueue()) {
      barrierBeforeCopy.srcQueueFamilyIndex =
          deviceQueueCommandUnitSet.getDeviceGraphicsQueueIndex();
      barrierBeforeCopy.dstQueueFamilyIndex =
          deviceQueueCommandUnitSet.getDeviceTransferQueueIndex();
    } else {
      barrierBeforeCopy.srcQueueFamilyIndex =
          VK_QUEUE_FAMILY_IGNORED; // Ignore for single queue
      barrierBeforeCopy.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    }
    barrierBeforeCopy.image = image;
    barrierBeforeCopy.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrierBeforeCopy.subresourceRange.baseMipLevel = 0;
    barrierBeforeCopy.subresourceRange.levelCount = 1;
    barrierBeforeCopy.subresourceRange.baseArrayLayer = 0;
    barrierBeforeCopy.subresourceRange.layerCount = 1;

    barrierBeforeCopy.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    barrierBeforeCopy.dstAccessMask =
        VK_ACCESS_TRANSFER_READ_BIT; // for transfer operation

    vkCmdPipelineBarrier(
        deviceQueueCommandUnitSet.getDeviceGraphicsQueueCommandBuffer(
            currentFrame),
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1,
        &barrierBeforeCopy);

    if (vkEndCommandBuffer(
            deviceQueueCommandUnitSet.getDeviceTransferQueueCommandBuffer(
                currentFrame)) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer!");
    } else {
      std::cout << "End recording command buffer for unpresentable Device \""
                << getPhysicalDeviceName(physicalDevice) << "\"" << std::endl;
    }
  }

  static void recordTransferCommandBufferForUnpresentableDevice(
      VkPhysicalDevice physicalDevice,
      const DeviceQueueCommandUnitSet &deviceQueueCommandUnitSet,
      VkCommandBuffer tmp_CommandBuffers[MAX_FRAMES_IN_FLIGHT],
      const uint32_t currentFrame, VkBuffer vertexBuffer, VkImage image,
      VkRenderPass renderPass, VkFramebuffer frameBuffer, VkExtent2D extent,
      VkPipeline graphicsPipeline, VkBuffer stagingBuffer,
      size_t stagingBufferSize, const bool isDynamicViewPortAndScissor) {

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;                  // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(tmp_CommandBuffers[currentFrame], &beginInfo) !=
        VK_SUCCESS) {

      throw std::runtime_error("failed to begin recording command buffer!");
    } else {
      std::cout << "Begin recording command buffer for unpresentable Device \""
                << getPhysicalDeviceName(physicalDevice) << "\"" << std::endl;
    }

    VkImageSubresourceLayers subresource{};
    subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresource.mipLevel = 0;
    subresource.baseArrayLayer = 0;
    subresource.layerCount = 1;

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource = subresource;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {extent.width, extent.height, 1};

    vkCmdCopyImageToBuffer(tmp_CommandBuffers[currentFrame], image,
                           VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, stagingBuffer,
                           1, &region);

    VkBufferMemoryBarrier bufferBarrierAfterCopy{};
    bufferBarrierAfterCopy.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    bufferBarrierAfterCopy.srcQueueFamilyIndex =
        deviceQueueCommandUnitSet.getDeviceTransferQueueIndex();
    bufferBarrierAfterCopy.dstQueueFamilyIndex =
        deviceQueueCommandUnitSet.getDeviceTransferQueueIndex();
    bufferBarrierAfterCopy.buffer = stagingBuffer;
    bufferBarrierAfterCopy.offset = 0;
    bufferBarrierAfterCopy.size = stagingBufferSize;
    bufferBarrierAfterCopy.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    bufferBarrierAfterCopy.dstAccessMask = VK_ACCESS_HOST_READ_BIT;

    vkCmdPipelineBarrier(tmp_CommandBuffers[currentFrame],
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_HOST_BIT, 0, 0, nullptr, 1,
                         &bufferBarrierAfterCopy, 0, nullptr);

    VkImageMemoryBarrier imageBarrierAfterCopy{};
    imageBarrierAfterCopy.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageBarrierAfterCopy.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    imageBarrierAfterCopy.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    if (deviceQueueCommandUnitSet.getDeviceGraphicsQueue() !=
        deviceQueueCommandUnitSet.getDeviceTransferQueue()) {
      imageBarrierAfterCopy.srcQueueFamilyIndex =
          deviceQueueCommandUnitSet.getDeviceTransferQueueIndex();
      imageBarrierAfterCopy.dstQueueFamilyIndex =
          deviceQueueCommandUnitSet.getDeviceGraphicsQueueIndex();
    } else {
      imageBarrierAfterCopy.srcQueueFamilyIndex =
          VK_QUEUE_FAMILY_IGNORED; // Ignore for single queue
      imageBarrierAfterCopy.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    }
    imageBarrierAfterCopy.image = image;
    imageBarrierAfterCopy.subresourceRange.aspectMask =
        VK_IMAGE_ASPECT_COLOR_BIT;
    imageBarrierAfterCopy.subresourceRange.baseMipLevel = 0;
    imageBarrierAfterCopy.subresourceRange.levelCount = 1;
    imageBarrierAfterCopy.subresourceRange.baseArrayLayer = 0;
    imageBarrierAfterCopy.subresourceRange.layerCount = 1;

    imageBarrierAfterCopy.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    imageBarrierAfterCopy.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    vkCmdPipelineBarrier(tmp_CommandBuffers[currentFrame],
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0,
                         nullptr, 0, nullptr, 1, &imageBarrierAfterCopy);

    if (vkEndCommandBuffer(tmp_CommandBuffers[currentFrame]) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer!");
    } else {
      std::cout << "End recording command buffer for unpresentable Device \""
                << getPhysicalDeviceName(physicalDevice) << "\"" << std::endl;
    }
  }

  static void createSemaphore(VkPhysicalDevice physicalDevice,
                              VkDevice logicalDevice, VkSemaphore &semaphore) {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &semaphore) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create semaphore!");
    } else {
      std::cout << "Created semaphore" << std::endl;
    }
  }

  static void createFence(VkPhysicalDevice physicalDevice,
                          VkDevice logicalDevice, VkFenceCreateFlags fenceFlags,
                          VkFence &fence) {
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = fenceFlags;

    if (vkCreateFence(logicalDevice, &fenceInfo, nullptr, &fence) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create fence!");
    } else {
      std::cout << "Created fence" << std::endl;
    }
  }

  static void createSyncObjectsForPresentation(
      VkPhysicalDevice physicalDevice, VkDevice logicalDevice,
      std::vector<VkSemaphore> &imageAvailableSemaphores,
      std::vector<VkSemaphore> &renderingFinishedSemaphores,
      std::vector<VkFence> &inFlightFences) {

    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderingFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    std::cout << "Creating semaphores/fences for device \""
              << getPhysicalDeviceName(physicalDevice) << "\" ..." << std::endl;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      std::cout << "For frame [" << i << "]" << std::endl;
      createSemaphore(physicalDevice, logicalDevice,
                      imageAvailableSemaphores[i]);
      createSemaphore(physicalDevice, logicalDevice,
                      renderingFinishedSemaphores[i]);

      createFence(physicalDevice, logicalDevice, VK_FENCE_CREATE_SIGNALED_BIT,
                  inFlightFences[i]);
    }
  }

  void destroySyncObjectsForPresentation(
      VkDevice logicalDevice,
      std::vector<VkSemaphore> &imageAvailableSemaphores,
      std::vector<VkSemaphore> &renderingFinishedSemaphores,
      std::vector<VkFence> &inFlightFences) {

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      vkDestroySemaphore(logicalDevice, imageAvailableSemaphores[i], nullptr);
      vkDestroySemaphore(logicalDevice, renderingFinishedSemaphores[i],
                         nullptr);
      vkDestroyFence(logicalDevice, inFlightFences[i], nullptr);
    }
  }

  void createSyncObjectsForUnpresentableDevice(
      VkPhysicalDevice physicalDevice, VkDevice logicalDevice,
      std::vector<VkSemaphore> &renderingFinishedSemaphores,
      std::vector<VkFence> &interDeviceFences) {

    renderingFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    interDeviceFences.resize(MAX_FRAMES_IN_FLIGHT);

    std::cout << "Creating semaphores/fences for device \""
              << getPhysicalDeviceName(physicalDevice) << "\" ..." << std::endl;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      std::cout << "For frame [" << i << "]" << std::endl;
      createSemaphore(physicalDevice, logicalDevice,
                      renderingFinishedSemaphores[i]);

      createFence(physicalDevice, logicalDevice, 0, interDeviceFences[i]);
    }
  }

  void destroySyncObjectsForUnpresentableDevice(
      VkDevice logicalDevice,
      std::vector<VkSemaphore> &renderingFinishedSemaphores,
      std::vector<VkFence> &interDeviceFences) {

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      vkDestroySemaphore(logicalDevice, renderingFinishedSemaphores[i],
                         nullptr);
      vkDestroyFence(logicalDevice, interDeviceFences[i], nullptr);
    }
  }

  static void createStagingBuffer(
      VkPhysicalDevice physicalDevice, VkDevice logicalDevice,
      const DeviceQueueCommandUnitSet &deviceQueueCommandUnitSet,
      VkExtent2D extent, VkBuffer &stagingBuffer,
      VkDeviceMemory &stagingBufferMemory, void *&stagingBufferData,
      VkBufferUsageFlags bufferUsage,
      const bool isPresentableStagingBufferSizeSet, size_t &stagingBufferSize) {

    VkDeviceSize bufferSize = extent.width * extent.height * 4; // 4 for (RGBA)

    createBuffer(physicalDevice, logicalDevice, deviceQueueCommandUnitSet,
                 bufferSize, bufferUsage,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 VK_NULL_HANDLE, /*needGraphics=*/false,
                 /*needPresent=*/false, /*needTransfer=*/true, stagingBuffer,
                 stagingBufferMemory);

    vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0,
                &stagingBufferData);

    if (isPresentableStagingBufferSizeSet) {
      assert(stagingBufferSize == bufferSize);
    } else {
      stagingBufferSize = bufferSize;
    }
  }

  static void createStagingBuffers(
      VkPhysicalDevice physicalDevice, VkDevice logicalDevice,
      const DeviceQueueCommandUnitSet &deviceQueueCommandUnitSet,
      VkExtent2D extent, std::vector<VkBuffer> &stagingBuffers,
      std::vector<VkDeviceMemory> &stagingBuffersMemories,
      std::vector<void *> &stagingBuffersData, VkBufferUsageFlags bufferFlags,
      bool isPresentableStagingBufferSizeSet, size_t &stagingBufferSize) {

    stagingBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    stagingBuffersMemories.resize(MAX_FRAMES_IN_FLIGHT);
    stagingBuffersData.resize(MAX_FRAMES_IN_FLIGHT);

    std::cout << "Creating staging buffers for device \""
              << getPhysicalDeviceName(physicalDevice) << "\" ..." << std::endl;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      std::cout << "For frame [" << i << "]" << std::endl;
      createStagingBuffer(
          physicalDevice, logicalDevice, deviceQueueCommandUnitSet, extent,
          stagingBuffers[i], stagingBuffersMemories[i], stagingBuffersData[i],
          bufferFlags, isPresentableStagingBufferSizeSet, stagingBufferSize);
      if (!isPresentableStagingBufferSizeSet) {
        isPresentableStagingBufferSizeSet = true;
      }
    }
  }

  static void createStagingBuffersForPresentableDevice(
      VkPhysicalDevice physicalDevice, VkDevice logicalDevice,
      const DeviceQueueCommandUnitSet &deviceQueueCommandUnitSet,
      VkExtent2D extent, std::vector<VkBuffer> &stagingBuffers,
      std::vector<VkDeviceMemory> &stagingBuffersMemories,
      std::vector<void *> &stagingBuffersData, size_t &stagingBufferSize) {

    createStagingBuffers(
        physicalDevice, logicalDevice, deviceQueueCommandUnitSet, extent,
        stagingBuffers, stagingBuffersMemories, stagingBuffersData,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, false, stagingBufferSize);
  }

  static void createStagingBuffersForUnpresentableDevice(
      VkPhysicalDevice physicalDevice, VkDevice logicalDevice,
      const DeviceQueueCommandUnitSet &deviceQueueCommandUnitSet,
      VkExtent2D extent, std::vector<VkBuffer> &stagingBuffers,
      std::vector<VkDeviceMemory> &stagingBuffersMemories,
      std::vector<void *> &stagingBuffersData,
      const bool isPresentableStagingBufferSizeSet, size_t &stagingBufferSize) {

    createStagingBuffers(physicalDevice, logicalDevice,
                         deviceQueueCommandUnitSet, extent, stagingBuffers,
                         stagingBuffersMemories, stagingBuffersData,
                         VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                         isPresentableStagingBufferSizeSet, stagingBufferSize);
  }

  static void destroyStagingBufferAndMemory(
      VkDevice logicalDevice, std::vector<VkBuffer> &stagingBuffers,
      std::vector<VkDeviceMemory> &stagingBuffersMemories,
      std::vector<void *> &stagingBuffersData) {

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      vkDestroyBuffer(logicalDevice, stagingBuffers[i], nullptr);
      vkFreeMemory(logicalDevice, stagingBuffersMemories[i], nullptr);
    }
    stagingBuffers.clear();
    stagingBuffersMemories.clear();
    stagingBuffersData.clear();
  }

  void initVulkan() {
    createInstance();

    setupDebugMessenger();

    createSurface(presentableWindowSurface);

    pickPhysicalDevice(instance, presentableWindowSurface);

    createLogicalDevice(presentablePhysicalDevice, presentableWindowSurface,
                        presentableLogicalDevice,
                        presentableDeviceQueueCommandUnitSet);

    createSwapChainForPresentation(
        presentablePhysicalDevice, presentableLogicalDevice,
        presentableDeviceQueueCommandUnitSet, window, presentableWindowSurface,
        isSplitRenderPresentMode, presentableSwapChain,
        presentableSwapChainImages, presentableSwapChainImageFormat,
        presentableSwapChainExtent);

    createImageViewsForPresentation(
        presentableSwapChainImages, presentableSwapChainImageFormat,
        presentablePhysicalDevice, presentableLogicalDevice,
        presentableSwapChainImageViews);

    createRenderPass(presentablePhysicalDevice, presentableLogicalDevice,
                     presentableSwapChainImageFormat,
                     VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, presentableRenderPass);

    createGraphicsPipelineLayout(presentablePhysicalDevice,
                                 presentableLogicalDevice,
                                 presentableGraphicsPipelineLayout);

    createGraphicsPipeline(
        presentablePhysicalDevice, presentableLogicalDevice,
        presentableSwapChainExtent, DYNAMIC_STATES_FOR_VIEWPORT_SCISSORS,
        presentableRenderPass, presentableGraphicsPipelineLayout,
        presentableGraphicsPipeline);

    createFrameBuffersForPresentation(
        presentablePhysicalDevice, presentableLogicalDevice,
        presentableSwapChainImageViews, presentableRenderPass,
        presentableSwapChainExtent, presentableSwapChainFrameBuffers);

    createDeviceCommandPoolsAndBuffers(presentablePhysicalDevice,
                                       presentableLogicalDevice,
                                       presentableDeviceQueueCommandUnitSet);

    createVertexBuffer(presentablePhysicalDevice, presentableLogicalDevice,
                       presentableDeviceQueueCommandUnitSet,
                       presentableVertexBuffer, presentableVertexBufferMemory);

    createSyncObjectsForPresentation(
        presentablePhysicalDevice, presentableLogicalDevice,
        presentableImageAvailableSemaphores,
        presentableRenderingFinishedSemaphores, presentableInFlightFences);

    createStagingBuffersForPresentableDevice(
        presentablePhysicalDevice, presentableLogicalDevice,
        presentableDeviceQueueCommandUnitSet, presentableSwapChainExtent,
        presentableStagingBuffers, presentableStagingBuffersMemories,
        presentableStagingBuffersData, stagingBufferSize);

    unpresentableLogicalDevices.resize(unpresentablePhysicalDevices.size());
    unpresentableDeviceQueueCommandUnitSet.resize(
        unpresentablePhysicalDevices.size());
    unpresentableDeviceImages.resize(unpresentablePhysicalDevices.size());
    unpresentableDeviceImageMemories.resize(
        unpresentablePhysicalDevices.size());
    unpresentableDeviceImageViews.resize(unpresentablePhysicalDevices.size());

    unpresentableGraphicsPipelineLayouts.resize(
        unpresentablePhysicalDevices.size());
    unpresentableRenderPasses.resize(unpresentablePhysicalDevices.size());
    unpresentableGraphicsPipelines.resize(unpresentablePhysicalDevices.size());
    unpresentableDeviceFrameBuffers.resize(unpresentablePhysicalDevices.size());

    unpresentableVertexBuffers.resize(unpresentablePhysicalDevices.size());
    unpresentableVertexBuffersMemories.resize(
        unpresentablePhysicalDevices.size());

    unpresentableRenderingFinishedSemaphores.resize(
        unpresentablePhysicalDevices.size());
    unpresentableInterDeviceFences.resize(unpresentablePhysicalDevices.size());

    unpresentableStagingBuffers.resize(unpresentablePhysicalDevices.size());
    unpresentableStagingBuffersMemories.resize(
        unpresentablePhysicalDevices.size());
    unpresentableStagingBuffersData.resize(unpresentablePhysicalDevices.size());

    for (uint32_t i = 0; i < unpresentablePhysicalDevices.size(); i++) {
      createLogicalDevice(unpresentablePhysicalDevices[i],
                          presentableWindowSurface,
                          unpresentableLogicalDevices[i],
                          unpresentableDeviceQueueCommandUnitSet[i]);

      createUnpresentableDeviceImage(
          unpresentablePhysicalDevices[i], unpresentableLogicalDevices[i],
          unpresentableDeviceQueueCommandUnitSet[i],
          presentableSwapChainExtent.width, presentableSwapChainExtent.height,
          presentableSwapChainImageFormat, unpresentableDeviceImages[i],
          unpresentableDeviceImageMemories[i]);

      createImageViewForUnpresentableDevice(
          unpresentableDeviceImages[i], presentableSwapChainImageFormat,
          unpresentablePhysicalDevices[i], unpresentableLogicalDevices[i],
          unpresentableDeviceImageViews[i]);

      createRenderPass(
          unpresentablePhysicalDevices[i], unpresentableLogicalDevices[i],
          presentableSwapChainImageFormat, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
          unpresentableRenderPasses[i]);

      createGraphicsPipelineLayout(unpresentablePhysicalDevices[i],
                                   unpresentableLogicalDevices[i],
                                   unpresentableGraphicsPipelineLayouts[i]);

      createGraphicsPipeline(
          unpresentablePhysicalDevices[i], unpresentableLogicalDevices[i],
          presentableSwapChainExtent, DYNAMIC_STATES_FOR_VIEWPORT_SCISSORS,
          unpresentableRenderPasses[i], unpresentableGraphicsPipelineLayouts[i],
          unpresentableGraphicsPipelines[i]);

      createFrameBufferForUnpresentableDevice(
          unpresentablePhysicalDevices[i], unpresentableLogicalDevices[i],
          unpresentableDeviceImageViews[i], unpresentableRenderPasses[i],
          presentableSwapChainExtent, unpresentableDeviceFrameBuffers[i]);

      createDeviceCommandPoolsAndBuffers(
          unpresentablePhysicalDevices[i], unpresentableLogicalDevices[i],
          unpresentableDeviceQueueCommandUnitSet[i]);

      createVertexBuffer(
          unpresentablePhysicalDevices[i], unpresentableLogicalDevices[i],
          unpresentableDeviceQueueCommandUnitSet[i],
          unpresentableVertexBuffers[i], unpresentableVertexBuffersMemories[i]);

      createSyncObjectsForUnpresentableDevice(
          unpresentablePhysicalDevices[i], unpresentableLogicalDevices[i],
          unpresentableRenderingFinishedSemaphores[i],
          unpresentableInterDeviceFences[i]);

      createStagingBuffersForUnpresentableDevice(
          unpresentablePhysicalDevices[i], unpresentableLogicalDevices[i],
          unpresentableDeviceQueueCommandUnitSet[i], presentableSwapChainExtent,
          unpresentableStagingBuffers[i],
          unpresentableStagingBuffersMemories[i],
          unpresentableStagingBuffersData[i], true, stagingBufferSize);
    }
  }

  static void recreateSwapChainForPresentableDevice(
      VkPhysicalDevice physicalDevice, VkDevice logicalDevice,
      const DeviceQueueCommandUnitSet &deviceQueueCommandUnitSet,
      GLFWwindow *window, VkSurfaceKHR windowSurface, VkSwapchainKHR &swapChain,
      std::vector<VkImage> &swapChainImages,
      std::vector<VkImageView> &swapChainImageViews,
      VkFormat &swapChainImageFormat, VkExtent2D &swapChainExtent,
      VkRenderPass renderPass,
      std::vector<VkFramebuffer> &swapChainFrameBuffers) {

#ifdef HANDLE_WINDOW_MINIMIZATION
    int window_width = 0, window_height = 0;
    glfwGetFramebufferSize(window, &window_width, &window_height);
    while (window_width == 0 || window_height == 0) {
      glfwWaitEvents();
      glfwGetFramebufferSize(window, &window_width, &window_height);
    }
#endif // HANDLE_WINDOW_MINIMIZATION

    vkDeviceWaitIdle(logicalDevice);

    cleanupSwapChainOfPresentableDevice(logicalDevice, swapChain,
                                        swapChainImages, swapChainFrameBuffers,
                                        swapChainImageViews);

    createSwapChainForPresentation(
        physicalDevice, logicalDevice, deviceQueueCommandUnitSet, window,
        windowSurface, isSplitRenderPresentMode, swapChain, swapChainImages,
        swapChainImageFormat, swapChainExtent);
    createImageViewsForPresentation(swapChainImages, swapChainImageFormat,
                                    physicalDevice, logicalDevice,
                                    swapChainImageViews);

    createFrameBuffersForPresentation(physicalDevice, logicalDevice,
                                      swapChainImageViews, renderPass,
                                      swapChainExtent, swapChainFrameBuffers);
  }

  static void recreateStagingBuffers(
      VkPhysicalDevice physicalDevice_p, VkDevice logicalDevice_p,
      const DeviceQueueCommandUnitSet &deviceQueueCommandUnitSet_p,
      std::vector<VkBuffer> &stagingBuffers_p,
      std::vector<VkDeviceMemory> &stagingBuffersMemories_p,
      std::vector<void *> &stagingBuffersData_p, size_t &stagingBufferSize,
      VkExtent2D extent, VkPhysicalDevice physicalDevice_unp,
      VkDevice logicalDevice_unp,
      const DeviceQueueCommandUnitSet &deviceQueueCommandUnitSet_unp,
      std::vector<VkBuffer> &stagingBuffers_unp,
      std::vector<VkDeviceMemory> &stagingBuffersMemories_unp,
      std::vector<void *> &stagingBuffersData_unp) {

    destroyStagingBufferAndMemory(logicalDevice_p, stagingBuffers_p,
                                  stagingBuffersMemories_p,
                                  stagingBuffersData_p);

    destroyStagingBufferAndMemory(logicalDevice_unp, stagingBuffers_unp,
                                  stagingBuffersMemories_unp,
                                  stagingBuffersData_unp);

    createStagingBuffersForPresentableDevice(
        physicalDevice_p, logicalDevice_p, deviceQueueCommandUnitSet_p, extent,
        stagingBuffers_p, stagingBuffersMemories_p, stagingBuffersData_p,
        stagingBufferSize);

    createStagingBuffersForUnpresentableDevice(
        physicalDevice_unp, logicalDevice_unp, deviceQueueCommandUnitSet_unp,
        extent, stagingBuffers_unp, stagingBuffersMemories_unp,
        stagingBuffersData_unp, true, stagingBufferSize);
  }

  static void recreateFrameBufferforUnpresentableDevice(
      VkPhysicalDevice physicalDevice, VkDevice logicalDevice,
      const DeviceQueueCommandUnitSet &deviceQueueCommandUnitSet,
      VkImage &image, VkDeviceMemory &imageMemory, VkImageView &imageView,
      VkFormat &imageFormat, VkExtent2D extent, VkRenderPass renderPass,
      VkFramebuffer &frameBuffer) {

    vkDeviceWaitIdle(logicalDevice);

    cleanupImageFrameBufferForUnpresentableDevice(
        logicalDevice, image, imageMemory, imageView, frameBuffer);

    createUnpresentableDeviceImage(
        physicalDevice, logicalDevice, deviceQueueCommandUnitSet, extent.width,
        extent.height, imageFormat, image, imageMemory);

    createImageViewForUnpresentableDevice(image, imageFormat, physicalDevice,
                                          logicalDevice, imageView);

    createFrameBufferForUnpresentableDevice(physicalDevice, logicalDevice,
                                            imageView, renderPass, extent,
                                            frameBuffer);
  }

  void drawFrame(VkPhysicalDevice physicalDevice, VkDevice logicalDevice,
                 const DeviceQueueCommandUnitSet &deviceQueueCommandUnitSet,
                 GLFWwindow *window, VkSurfaceKHR windowSurface,
                 VkSwapchainKHR &swapChain,
                 std::vector<VkImage> &swapChainImages,
                 std::vector<VkImageView> &swapChainImageViews,
                 VkFormat &swapChainImageFormat, VkExtent2D &extent,
                 VkRenderPass renderPass,
                 std::vector<VkFramebuffer> &swapChainFrameBuffers,
                 VkPipeline graphicsPipeline, VkBuffer vertexBuffer,
                 std::vector<VkSemaphore> imageAvailableSemaphores,
                 std::vector<VkSemaphore> renderingFinishedSemaphores,
                 std::vector<VkFence> inFlightFences,
                 const bool isDynamicViewPortAndScissor) {

    vkWaitForFences(logicalDevice, 1, &inFlightFences[currentFrame], VK_TRUE,
                    UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(
        logicalDevice, swapChain, UINT64_MAX,
        imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
      recreateSwapChainForPresentableDevice(
          physicalDevice, logicalDevice, deviceQueueCommandUnitSet, window,
          windowSurface, swapChain, swapChainImages, swapChainImageViews,
          swapChainImageFormat, extent, renderPass, swapChainFrameBuffers);

      std::cout << "EXTENT(aq) -> width:" << extent.width
                << ", height:" << extent.height << std::endl;

      return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
      throw std::runtime_error("failed to aquire swap chain image!");
    }

    // Only reset the fence if we are submitting work
    vkResetFences(logicalDevice, 1, &inFlightFences[currentFrame]);

    vkResetCommandBuffer(
        deviceQueueCommandUnitSet.getDeviceGraphicsQueueCommandBuffer(
            currentFrame),
        0);

    recordCommandBufferForPresentation(
        physicalDevice,
        deviceQueueCommandUnitSet.getDeviceGraphicsQueueCommandBuffer(
            currentFrame),
        vertexBuffer, imageIndex, renderPass, swapChainFrameBuffers, extent,
        graphicsPipeline, isDynamicViewPortAndScissor);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkCommandBuffer commandBuffers[] = {
        deviceQueueCommandUnitSet.getDeviceGraphicsQueueCommandBuffer(
            currentFrame)};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = commandBuffers;
    VkSemaphore signalSemaphores[] = {
        renderingFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(deviceQueueCommandUnitSet.getDeviceGraphicsQueue(), 1,
                      &submitInfo,
                      inFlightFences[currentFrame]) != VK_SUCCESS) {
      throw std::runtime_error("failed to submit draw command buffer!");
    } else {
      std::cout << "Submitted draw command buffer for device \""
                << getPhysicalDeviceName(physicalDevice) << "\"" << std::endl;
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    presentInfo.pResults = nullptr; // Optional

    result = vkQueuePresentKHR(
        deviceQueueCommandUnitSet.getDevicePresentQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
        frameBufferResized) {

      frameBufferResized = false;

      recreateSwapChainForPresentableDevice(
          physicalDevice, logicalDevice, deviceQueueCommandUnitSet, window,
          windowSurface, swapChain, swapChainImages, swapChainImageViews,
          swapChainImageFormat, extent, renderPass, swapChainFrameBuffers);
      std::cout << "EXTENT(pr) -> width:" << extent.width
                << ", height:" << extent.height << std::endl;
    } else if (result != VK_SUCCESS) {
      throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
  }

  void drawFrame2(
      VkCommandBuffer tmp_CommandBuffers_unp[MAX_FRAMES_IN_FLIGHT],
      VkPhysicalDevice physicalDevice_unp, VkDevice logicalDevice_unp,
      const DeviceQueueCommandUnitSet &deviceQueueCommandUnitSet_unp,
      VkImage &image_unp, VkDeviceMemory &imageMemory_unp,
      VkImageView &imageView_unp, VkFormat &imageFormat_unp,
      VkRenderPass renderPass_unp, VkPipeline graphicsPipeline_unp,
      VkFramebuffer &frameBuffer_unp, VkBuffer vertexBuffer_unp,
      std::vector<VkSemaphore> &renderingFinishedSemaphores_unp,
      std::vector<VkFence> &interDeviceFences_unp,
      std::vector<VkBuffer> &stagingBuffers_unp,
      std::vector<VkDeviceMemory> &stagingBuffersMemories_unp,
      std::vector<void *> &stagingBufferData_unp,
      VkPhysicalDevice physicalDevice_p, VkDevice logicalDevice_p,
      const DeviceQueueCommandUnitSet &deviceQueueCommandUnitSet_p,
      GLFWwindow *window, VkSurfaceKHR windowSurface_p,
      VkSwapchainKHR &swapChain_p, std::vector<VkImage> &swapChainImages_p,
      std::vector<VkImageView> &swapChainImageViews_p,
      VkFormat &swapChainImageFormat_p, VkExtent2D &extent,
      VkRenderPass renderPass_p,
      std::vector<VkFramebuffer> &swapChainFrameBuffers_p,
      std::vector<VkSemaphore> &imageAvailableSemaphores_p,
      std::vector<VkSemaphore> &copyingFinishedSemaphores_p,
      std::vector<VkFence> &inFlightFences_p,
      std::vector<VkBuffer> &stagingBuffers_p,
      std::vector<VkDeviceMemory> &stagingBuffersMemories_p,
      std::vector<void *> &stagingBufferData_p, size_t &stagingBufferSize,
      const bool isDynamicViewPortAndScissor) {

    vkWaitForFences(logicalDevice_p, 1, &inFlightFences_p[currentFrame],
                    VK_TRUE, UINT64_MAX);

    if (deviceQueueCommandUnitSet_unp.getDeviceGraphicsQueueIndex() !=
        deviceQueueCommandUnitSet_unp.getDeviceTransferQueueIndex()) {

      vkResetCommandBuffer(
          deviceQueueCommandUnitSet_unp.getDeviceGraphicsQueueCommandBuffer(
              currentFrame),
          0);

      recordCommandBufferForUnpresentableDevice(
          physicalDevice_unp, deviceQueueCommandUnitSet_unp, currentFrame,
          vertexBuffer_unp, image_unp, renderPass_unp, frameBuffer_unp, extent,
          graphicsPipeline_unp, stagingBuffers_unp[currentFrame],
          stagingBufferSize, isDynamicViewPortAndScissor);

      VkSubmitInfo submitInfo_unp{};
      VkCommandBuffer commandBuffers_unp[] = {
          deviceQueueCommandUnitSet_unp.getDeviceGraphicsQueueCommandBuffer(
              currentFrame)};
      submitInfo_unp.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submitInfo_unp.waitSemaphoreCount = 0;
      submitInfo_unp.pWaitSemaphores = nullptr;   // Optional
      submitInfo_unp.pWaitDstStageMask = nullptr; // Optional
      submitInfo_unp.commandBufferCount = 1;
      submitInfo_unp.pCommandBuffers = commandBuffers_unp;
      submitInfo_unp.signalSemaphoreCount = 0;
      submitInfo_unp.pSignalSemaphores = nullptr; // Optional

      if (vkQueueSubmit(deviceQueueCommandUnitSet_unp.getDeviceGraphicsQueue(),
                        1, &submitInfo_unp,
                        interDeviceFences_unp[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
      } else {
        std::cout << "Submitted draw and copy command buffer for device \""
                  << getPhysicalDeviceName(physicalDevice_unp) << "\""
                  << std::endl;
      }
    } else {
      vkResetCommandBuffer(
          deviceQueueCommandUnitSet_unp.getDeviceGraphicsQueueCommandBuffer(
              currentFrame),
          0);

      recordRenderCommandBufferForUnpresentableDevice(
          physicalDevice_unp, deviceQueueCommandUnitSet_unp, currentFrame,
          vertexBuffer_unp, image_unp, renderPass_unp, frameBuffer_unp, extent,
          graphicsPipeline_unp, stagingBuffers_unp[currentFrame],
          stagingBufferSize, isDynamicViewPortAndScissor);

      VkSubmitInfo renderSubmitInfo_unp{};
      VkCommandBuffer renderCommandBuffers_unp[] = {
          deviceQueueCommandUnitSet_unp.getDeviceGraphicsQueueCommandBuffer(
              currentFrame)};
      VkSemaphore transferSignalSemaphores_unp[] = {
          renderingFinishedSemaphores_unp[currentFrame]};
      renderSubmitInfo_unp.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      renderSubmitInfo_unp.waitSemaphoreCount = 0;
      renderSubmitInfo_unp.pWaitSemaphores = nullptr;   // Optional
      renderSubmitInfo_unp.pWaitDstStageMask = nullptr; // Optional
      renderSubmitInfo_unp.commandBufferCount = 1;
      renderSubmitInfo_unp.pCommandBuffers = renderCommandBuffers_unp;
      renderSubmitInfo_unp.signalSemaphoreCount = 1;
      renderSubmitInfo_unp.pSignalSemaphores = transferSignalSemaphores_unp;

      if (vkQueueSubmit(deviceQueueCommandUnitSet_unp.getDeviceGraphicsQueue(),
                        1, &renderSubmitInfo_unp,
                        VK_NULL_HANDLE) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
      } else {
        std::cout << "Submitted draw command buffer for device \""
                  << getPhysicalDeviceName(physicalDevice_unp) << "\""
                  << std::endl;
      }

      vkResetCommandBuffer(tmp_CommandBuffers_unp[currentFrame], 0);

      recordTransferCommandBufferForUnpresentableDevice(
          physicalDevice_unp, deviceQueueCommandUnitSet_unp,
          tmp_CommandBuffers_unp, currentFrame, vertexBuffer_unp, image_unp,
          renderPass_unp, frameBuffer_unp, extent, graphicsPipeline_unp,
          stagingBuffers_unp[currentFrame], stagingBufferSize,
          isDynamicViewPortAndScissor);

      VkSubmitInfo transferSubmitInfo_unp{};
      VkCommandBuffer transferCommandBuffers_unp[] = {
          tmp_CommandBuffers_unp[currentFrame]};
      VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_TRANSFER_BIT};
      transferSubmitInfo_unp.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      transferSubmitInfo_unp.waitSemaphoreCount = 1;
      transferSubmitInfo_unp.pWaitSemaphores = transferSignalSemaphores_unp;
      transferSubmitInfo_unp.pWaitDstStageMask = waitStages;
      transferSubmitInfo_unp.commandBufferCount = 1;
      transferSubmitInfo_unp.pCommandBuffers = transferCommandBuffers_unp;
      transferSubmitInfo_unp.signalSemaphoreCount = 0;
      transferSubmitInfo_unp.pSignalSemaphores = nullptr;

      if (vkQueueSubmit(deviceQueueCommandUnitSet_unp.getDeviceTransferQueue(),
                        1, &transferSubmitInfo_unp,
                        interDeviceFences_unp[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
      } else {
        std::cout << "Submitted copy command buffer for device \""
                  << getPhysicalDeviceName(physicalDevice_unp) << "\""
                  << std::endl;
      }
    }

    //////////////////////////////////////////////////////////////////////
    ///                     END copy to host visible buffer
    //////////////////////////////////////////////////////////////////////

    vkWaitForFences(logicalDevice_unp, 1, &interDeviceFences_unp[currentFrame],
                    VK_TRUE, UINT64_MAX);
    vkResetFences(logicalDevice_unp, 1, &interDeviceFences_unp[currentFrame]);

    //////////////////////////////////////////////////////////////////////
    ///                   START copy to presenter visible buffer
    //////////////////////////////////////////////////////////////////////

    memcpy(stagingBufferData_p[currentFrame],
           stagingBufferData_unp[currentFrame], stagingBufferSize);

    //////////////////////////////////////////////////////////////////////
    ///                    END copy to presenter visible buffer
    //////////////////////////////////////////////////////////////////////

    uint32_t imageIndex_p;
    VkResult result =
        vkAcquireNextImageKHR(logicalDevice_p, swapChain_p, UINT64_MAX,
                              imageAvailableSemaphores_p[currentFrame],
                              VK_NULL_HANDLE, &imageIndex_p);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
      recreateSwapChainForPresentableDevice(
          physicalDevice_p, logicalDevice_p, deviceQueueCommandUnitSet_p,
          window, windowSurface_p, swapChain_p, swapChainImages_p,
          swapChainImageViews_p, swapChainImageFormat_p, extent, renderPass_p,
          swapChainFrameBuffers_p);

      recreateFrameBufferforUnpresentableDevice(
          physicalDevice_unp, logicalDevice_unp, deviceQueueCommandUnitSet_unp,
          image_unp, imageMemory_unp, imageView_unp, imageFormat_unp, extent,
          renderPass_unp, frameBuffer_unp);

      recreateStagingBuffers(
          physicalDevice_p, logicalDevice_p, deviceQueueCommandUnitSet_p,
          stagingBuffers_p, stagingBuffersMemories_p, stagingBufferData_p,
          stagingBufferSize, extent, physicalDevice_unp, logicalDevice_unp,
          deviceQueueCommandUnitSet_unp, stagingBuffers_unp,
          stagingBuffersMemories_unp, stagingBufferData_unp);

      std::cout << "EXTENT(aq) -> width:" << extent.width
                << ", height:" << extent.height << std::endl;

      return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
      throw std::runtime_error("failed to aquire swap chain image!");
    }

    // Only reset the fence if we are submitting work
    vkResetFences(logicalDevice_p, 1, &inFlightFences_p[currentFrame]);

    vkResetCommandBuffer(
        deviceQueueCommandUnitSet_p.getDeviceTransferQueueCommandBuffer(
            currentFrame),
        0);

    recordCommandBufferForCopyingFromBuffer(
        physicalDevice_p, logicalDevice_p,
        deviceQueueCommandUnitSet_p.getDeviceTransferQueueCommandBuffer(
            currentFrame),
        swapChainImages_p[imageIndex_p], extent,
        stagingBuffers_p[currentFrame]);

    VkSubmitInfo copySubmitInfo_p{};
    copySubmitInfo_p.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkSemaphore copyWaitSemaphores_p[] = {
        imageAvailableSemaphores_p[currentFrame]};
    VkPipelineStageFlags copyWaitStages_p[] = {VK_PIPELINE_STAGE_TRANSFER_BIT};
    VkCommandBuffer commandBuffers_p[] = {
        deviceQueueCommandUnitSet_p.getDeviceTransferQueueCommandBuffer(
            currentFrame)};
    copySubmitInfo_p.waitSemaphoreCount = 1;
    copySubmitInfo_p.pWaitSemaphores = copyWaitSemaphores_p;
    copySubmitInfo_p.pWaitDstStageMask = copyWaitStages_p;
    copySubmitInfo_p.commandBufferCount = 1;
    copySubmitInfo_p.pCommandBuffers = commandBuffers_p;
    VkSemaphore copySignalSemaphores_p[] = {
        copyingFinishedSemaphores_p[currentFrame]};
    copySubmitInfo_p.signalSemaphoreCount = 1;
    copySubmitInfo_p.pSignalSemaphores = copySignalSemaphores_p;

    if (vkQueueSubmit(deviceQueueCommandUnitSet_p.getDeviceGraphicsQueue(), 1,
                      &copySubmitInfo_p,
                      inFlightFences_p[currentFrame]) != VK_SUCCESS) {
      throw std::runtime_error("failed to submit draw command buffer!");
    } else {
      std::cout << "Submitted copy command buffer for device \""
                << getPhysicalDeviceName(physicalDevice_p) << "\"" << std::endl;
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = copySignalSemaphores_p;

    VkSwapchainKHR swapChains[] = {swapChain_p};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex_p;

    presentInfo.pResults = nullptr; // Optional

    result = vkQueuePresentKHR(
        deviceQueueCommandUnitSet_p.getDevicePresentQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
        frameBufferResized) {

      frameBufferResized = false;

      recreateSwapChainForPresentableDevice(
          physicalDevice_p, logicalDevice_p, deviceQueueCommandUnitSet_p,
          window, windowSurface_p, swapChain_p, swapChainImages_p,
          swapChainImageViews_p, swapChainImageFormat_p, extent, renderPass_p,
          swapChainFrameBuffers_p);

      recreateFrameBufferforUnpresentableDevice(
          physicalDevice_unp, logicalDevice_unp, deviceQueueCommandUnitSet_unp,
          image_unp, imageMemory_unp, imageView_unp, imageFormat_unp, extent,
          renderPass_unp, frameBuffer_unp);

      recreateStagingBuffers(
          physicalDevice_p, logicalDevice_p, deviceQueueCommandUnitSet_p,
          stagingBuffers_p, stagingBuffersMemories_p, stagingBufferData_p,
          stagingBufferSize, extent, physicalDevice_unp, logicalDevice_unp,
          deviceQueueCommandUnitSet_unp, stagingBuffers_unp,
          stagingBuffersMemories_unp, stagingBufferData_unp);

      std::cout << "EXTENT(pr) -> width:" << extent.width
                << ", height:" << extent.height << std::endl;
    } else if (result != VK_SUCCESS) {
      throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
  }

  void mainLoop() {
    VkCommandBuffer tmp_CommandBuffers[MAX_FRAMES_IN_FLIGHT];
    createCommandBuffers(unpresentablePhysicalDevices[0],
                         unpresentableLogicalDevices[0],
                         unpresentableDeviceQueueCommandUnitSet[0]
                             .getDeviceTransferQueueCommandPool(),
                         tmp_CommandBuffers);
    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();
      if (!isSplitRenderPresentMode) {
        drawFrame(presentablePhysicalDevice, presentableLogicalDevice,
                  presentableDeviceQueueCommandUnitSet, window,
                  presentableWindowSurface, presentableSwapChain,
                  presentableSwapChainImages, presentableSwapChainImageViews,
                  presentableSwapChainImageFormat, presentableSwapChainExtent,
                  presentableRenderPass, presentableSwapChainFrameBuffers,
                  presentableGraphicsPipeline, presentableVertexBuffer,
                  presentableImageAvailableSemaphores,
                  presentableRenderingFinishedSemaphores,
                  presentableInFlightFences,
                  DYNAMIC_STATES_FOR_VIEWPORT_SCISSORS);
      } else {
        drawFrame2(
            tmp_CommandBuffers, unpresentablePhysicalDevices[0],
            unpresentableLogicalDevices[0],
            unpresentableDeviceQueueCommandUnitSet[0],
            unpresentableDeviceImages[0], unpresentableDeviceImageMemories[0],
            unpresentableDeviceImageViews[0], presentableSwapChainImageFormat,
            unpresentableRenderPasses[0], unpresentableGraphicsPipelines[0],
            unpresentableDeviceFrameBuffers[0], unpresentableVertexBuffers[0],
            unpresentableRenderingFinishedSemaphores[0],
            unpresentableInterDeviceFences[0], unpresentableStagingBuffers[0],
            unpresentableStagingBuffersMemories[0],
            unpresentableStagingBuffersData[0], presentablePhysicalDevice,
            presentableLogicalDevice, presentableDeviceQueueCommandUnitSet,
            window, presentableWindowSurface, presentableSwapChain,
            presentableSwapChainImages, presentableSwapChainImageViews,
            presentableSwapChainImageFormat, presentableSwapChainExtent,
            presentableRenderPass, presentableSwapChainFrameBuffers,
            presentableImageAvailableSemaphores,
            presentableRenderingFinishedSemaphores, // using this for
                                                    // copyFinishSemaphore
            presentableInFlightFences, presentableStagingBuffers,
            presentableStagingBuffersMemories, presentableStagingBuffersData,
            stagingBufferSize, DYNAMIC_STATES_FOR_VIEWPORT_SCISSORS);
      }
    }

    vkQueueWaitIdle(
        presentableDeviceQueueCommandUnitSet.getDeviceGraphicsQueue());
    vkQueueWaitIdle(
        presentableDeviceQueueCommandUnitSet.getDevicePresentQueue());
    vkDeviceWaitIdle(presentableLogicalDevice);

    for (uint32_t i = 0; i < unpresentableDeviceImages.size(); i++) {
      vkQueueWaitIdle(
          unpresentableDeviceQueueCommandUnitSet[i].getDeviceGraphicsQueue());
      vkDeviceWaitIdle(unpresentableLogicalDevices[i]);
    }
  }

  static void cleanupSwapChainOfPresentableDevice(
      VkDevice logicalDevice, VkSwapchainKHR swapChain,
      std::vector<VkImage> &swapChainImages,
      std::vector<VkFramebuffer> &swapChainFrameBuffers,
      std::vector<VkImageView> &swapChainImageViews) {
    destroyFrameBuffersForPresentation(logicalDevice, swapChainFrameBuffers);

    destroyImageViewsForPresentation(logicalDevice, swapChainImageViews);

    vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);

    swapChainImages.clear();
  }

  static void cleanupImageFrameBufferForUnpresentableDevice(
      VkDevice logicalDevice, VkImage image, VkDeviceMemory imageMemory,
      VkImageView imageView, VkFramebuffer frameBuffer) {

    vkDestroyFramebuffer(logicalDevice, frameBuffer, nullptr);

    vkDestroyImageView(logicalDevice, imageView, nullptr);

    vkFreeMemory(logicalDevice, imageMemory, nullptr);

    vkDestroyImage(logicalDevice, image, nullptr);
  }

  void cleanup() {

    for (uint32_t i = 0; i < unpresentableDeviceImages.size(); i++) {
      destroyStagingBufferAndMemory(unpresentableLogicalDevices[i],
                                    unpresentableStagingBuffers[i],
                                    unpresentableStagingBuffersMemories[i],
                                    unpresentableStagingBuffersData[i]);

      destroySyncObjectsForUnpresentableDevice(
          unpresentableLogicalDevices[i],
          unpresentableRenderingFinishedSemaphores[i],
          unpresentableInterDeviceFences[i]);

      destroyDeviceCommandPoolsAndBuffers(
          unpresentableLogicalDevices[i],
          unpresentableDeviceQueueCommandUnitSet[i]);

      cleanupImageFrameBufferForUnpresentableDevice(
          unpresentableLogicalDevices[i], unpresentableDeviceImages[i],
          unpresentableDeviceImageMemories[i], unpresentableDeviceImageViews[i],
          unpresentableDeviceFrameBuffers[i]);

      vkDestroyBuffer(unpresentableLogicalDevices[i],
                      unpresentableVertexBuffers[i], nullptr);
      vkFreeMemory(unpresentableLogicalDevices[i],
                   unpresentableVertexBuffersMemories[i], nullptr);

      vkDestroyPipeline(unpresentableLogicalDevices[i],
                        unpresentableGraphicsPipelines[i], nullptr);

      vkDestroyPipelineLayout(unpresentableLogicalDevices[i],
                              unpresentableGraphicsPipelineLayouts[i], nullptr);

      vkDestroyRenderPass(unpresentableLogicalDevices[i],
                          unpresentableRenderPasses[i], nullptr);
    }

    destroyStagingBufferAndMemory(
        presentableLogicalDevice, presentableStagingBuffers,
        presentableStagingBuffersMemories, presentableStagingBuffersData);

    destroySyncObjectsForPresentation(
        presentableLogicalDevice, presentableImageAvailableSemaphores,
        presentableRenderingFinishedSemaphores, presentableInFlightFences);

    destroyDeviceCommandPoolsAndBuffers(presentableLogicalDevice,
                                        presentableDeviceQueueCommandUnitSet);

    cleanupSwapChainOfPresentableDevice(
        presentableLogicalDevice, presentableSwapChain,
        presentableSwapChainImages, presentableSwapChainFrameBuffers,
        presentableSwapChainImageViews);

    vkDestroyBuffer(presentableLogicalDevice, presentableVertexBuffer, nullptr);

    vkFreeMemory(presentableLogicalDevice, presentableVertexBufferMemory,
                 nullptr);

    vkDestroyPipeline(presentableLogicalDevice, presentableGraphicsPipeline,
                      nullptr);

    vkDestroyPipelineLayout(presentableLogicalDevice,
                            presentableGraphicsPipelineLayout, nullptr);

    vkDestroyRenderPass(presentableLogicalDevice, presentableRenderPass,
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
