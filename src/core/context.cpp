#include "context.hpp"

void VulkanContext::createInstance(const std::vector<const char*>& requiredInstanceExtensions) {
    constexpr vk::ApplicationInfo appInfo {
        .pApplicationName = "Vulkan Engine",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "vulkan",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion =  vk::ApiVersion14,
    };
    std::vector<char const*> validationLayers {};
    if (enableValidationLayers) {
        validationLayers = setupValidationLayer();
    }
    // Check if the required extensions are supported by the Vulkan implementation.
    auto extensionProperties = context.enumerateInstanceExtensionProperties();
		auto unsupportedPropertyIt =
		    std::ranges::find_if(requiredInstanceExtensions,
		                         [&extensionProperties](auto const &requiredExtension) {
			                         return std::ranges::none_of(extensionProperties,
			                                                     [requiredExtension](auto const &extensionProperty) { return strcmp(extensionProperty.extensionName, requiredExtension) == 0; });
		                         });
		if (unsupportedPropertyIt != requiredInstanceExtensions.end())
		{
			throw std::runtime_error("Required extension not supported: " + std::string(*unsupportedPropertyIt));
		}

    vk::InstanceCreateInfo createInfo{
        .pApplicationInfo        = &appInfo,
        .enabledLayerCount       = static_cast<uint32_t>(validationLayers.size()),
        .ppEnabledLayerNames     = validationLayers.data(),
        .enabledExtensionCount   = static_cast<uint32_t>(requiredInstanceExtensions.size()),
        .ppEnabledExtensionNames = requiredInstanceExtensions.data() };

    instance = vk::raii::Instance(context, createInfo);
}

std::vector<char const*> VulkanContext::setupValidationLayer() {
    std::vector<char const*> requiredLayers;
    if (true) {
        requiredLayers.assign(validationLayers.begin(), validationLayers.end());
    }

    // Check if the required layers are supported by the Vulkan implementation.
    auto layerProperties = context.enumerateInstanceLayerProperties();
    if (std::ranges::any_of(requiredLayers, [&layerProperties](auto const& requiredLayer) {
        return std::ranges::none_of(layerProperties,
                                   [requiredLayer](auto const& layerProperty)
                                   { return strcmp(layerProperty.layerName, requiredLayer) == 0; });
    }))
    {
        throw std::runtime_error("One or more required layers are not supported!");
    }
    return requiredLayers;
}

void VulkanContext::setupDebugMessenger() {
    if (!enableValidationLayers) return;

    vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                                    vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
    vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                                                        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                                                        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
    vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT{
        .messageSeverity = severityFlags,
        .messageType = messageTypeFlags,
        .pfnUserCallback = &debugCallback,
    };
    debugMessenger = instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
}

static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type,
                                                          const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    std::cerr << "validation layer: type" << vk::to_string(type) << " msg: " << pCallbackData->pMessage << std::endl;
    return vk::False;
}


void VulkanContext::pickPhysicalDevice() {
    auto physicalDevices = instance.enumeratePhysicalDevices();
    auto const deviceIt = std::ranges::find_if(physicalDevices, [](auto const& physicalDevice) {
        return isDeviceSuitable(physicalDevice);
    });
    if (deviceIt ==  physicalDevices.end()) {
        throw std::runtime_error("Failed to find suitable physical device!");
    }
    physicalDevice = *deviceIt;
}

bool VulkanContext::isDeviceSuitable(const vk::raii::PhysicalDevice &physicalDevice) {
    bool supportVulkan1_3 = physicalDevice.getProperties().apiVersion >= vk::ApiVersion14;

    auto deviceQueueFamilies = physicalDevice.getQueueFamilyProperties();
    bool supportQueues = isDeviceSupportQueues(deviceQueueFamilies);

    auto availableDeviceExtensions = physicalDevice.enumerateDeviceExtensionProperties();
    bool supportDeviceExtensions = isDeviceSupportExtensions(availableDeviceExtensions);

    auto features = physicalDevice.getFeatures2
            <vk::PhysicalDeviceFeatures2,
            vk::PhysicalDeviceVulkan11Features,
            vk::PhysicalDeviceVulkan13Features>();

    bool supportRequiredFeatures = features.get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering;

    return supportVulkan1_3 && supportQueues && supportDeviceExtensions && supportRequiredFeatures;
}

bool VulkanContext::isDeviceSupportQueues(const std::vector<vk::QueueFamilyProperties> &deviceQueues) {
    return std::ranges::all_of(requiredDeviceQueues, [&deviceQueues](auto const &requiredFlag) {
        return std::ranges::any_of(deviceQueues, [&requiredFlag](auto const &queue) {
            return !!(queue.queueFlags & requiredFlag);
        });
    });
}

bool VulkanContext::isDeviceSupportExtensions(const std::vector<vk::ExtensionProperties>& deviceExtensions) {
    return std::ranges::all_of(requiredDeviceExtensions, [&deviceExtensions](auto const &requiredExtension) {
        return std::ranges::any_of(deviceExtensions, [&requiredExtension](auto const &e) {
            return strcmp(e.extensionName, requiredExtension) == 0;
        });
    });
}

std::uint32_t VulkanContext::findQueueFamilyIndex(vk::QueueFlagBits requiredFlag, const vk::raii::SurfaceKHR& surface) {
    auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
    for (uint32_t i = 0; i < queueFamilyProperties.size(); i++) {
        if (queueFamilyProperties[i].queueFlags & requiredFlag
                && physicalDevice.getSurfaceSupportKHR(i, *surface)) {
            return i;
        }
    }
}

void VulkanContext::createLogicalDevice(const vk::raii::SurfaceKHR& surface) {
    queueIndex = findQueueFamilyIndex(vk::QueueFlagBits::eGraphics, surface);

    vk::StructureChain<vk::PhysicalDeviceFeatures2,
                           vk::PhysicalDeviceVulkan11Features,
                           vk::PhysicalDeviceVulkan13Features,
                           vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
            featureChain = {
        {.features = {.samplerAnisotropy = true}},                                                          // vk::PhysicalDeviceFeatures2
        {.shaderDrawParameters = true},                              // vk::PhysicalDeviceVulkan11Features
        {.synchronization2 = true, .dynamicRendering = true},        // vk::PhysicalDeviceVulkan13Features
        {.extendedDynamicState = true},                         // vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT
    };

    float queuePriority = 0.5f;

    vk::DeviceQueueCreateInfo deviceQueueCreateInfo{
        .queueFamilyIndex = queueIndex,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority
    };

    vk::DeviceCreateInfo deviceCreateInfo{
        .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos       = &deviceQueueCreateInfo,
        .enabledExtensionCount   = static_cast<uint32_t>(requiredDeviceExtensions.size()),
        .ppEnabledExtensionNames = requiredDeviceExtensions.data()
    };

    device = vk::raii::Device(physicalDevice, deviceCreateInfo);
    queue  = vk::raii::Queue(device, queueIndex, 0);
}