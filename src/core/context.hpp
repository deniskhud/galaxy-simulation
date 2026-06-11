#ifndef CONTEXT_HPP
#define CONTEXT_HPP
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif
#include <algorithm>
#include <cstdint>
#include <iostream>

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif


class VulkanContext final {
private:
    vk::raii::Context context;
    vk::raii::Instance instance;
    vk::raii::PhysicalDevice physicalDevice;
    vk::raii::Device device;
    vk::raii::Queue queue;
    std::uint32_t queueIndex;

    vk::DebugUtilsMessengerEXT debugMessenger;

    const std::vector<char const*> validationLayers {
        "VK_LAYER_KHRONOS_validation"
    };

    [[nodiscard]] std::vector<char const*> setupValidationLayer();

    bool isDeviceSuitable(const vk::raii::PhysicalDevice &physicalDevice);
    const std::vector<vk::QueueFlagBits> requiredDeviceQueues {
        vk::QueueFlagBits::eGraphics,
    };
    const std::vector<const char*> requiredDeviceExtensions {
        vk::KHRSwapchainExtensionName,
    };

    /**
     * Returns true if all required queue families are available.
     */
    bool isDeviceSupportQueues(const std::vector<vk::QueueFamilyProperties>& deviceQueues);
    bool isDeviceSupportExtensions(const std::vector<vk::ExtensionProperties>& deviceExtensions);
public:
    VulkanContext();
    ~VulkanContext();

    void createInstance(const std::vector<const char*>& requiredInstanceExtensions);
    void pickPhysicalDevice();
    void createLogicalDevice();
    void setupDebugMessenger();
    static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type,
                                                          const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
};

#endif //CONTEXT_HPP
