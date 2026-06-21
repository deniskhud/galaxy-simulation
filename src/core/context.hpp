#ifndef CONTEXT_HPP
#define CONTEXT_HPP
#include <vulkan/vulkan.hpp>
#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif
#include "../window/window.hpp"

#include <algorithm>
#include <cstdint>
#include <iostream>

class VulkanContext final {
public:
	VulkanContext(const Window& window);
	~VulkanContext();

	/* getters */
	const vk::raii::Device& getDevice() const;
	const vk::raii::PhysicalDevice& getPhysicalDevice() const;
	std::uint32_t getGraphicsQueueFamilyIndex() const;
	const vk::raii::Queue& getGraphicsQueue() const;
	const vk::raii::Queue& getPresentQueue() const;
	const vk::raii::Instance& getInstance() const;
	const vk::raii::SurfaceKHR& getSurface() const;

private:
	vk::raii::Context context;
	vk::raii::Instance instance = nullptr;
	vk::raii::PhysicalDevice physicalDevice = nullptr;
	vk::raii::Device device = nullptr;
	vk::raii::Queue queue = nullptr;

	vk::raii::SurfaceKHR surface = nullptr;

	std::uint32_t queueIndex;

	vk::DebugUtilsMessengerEXT debugMessenger;

	const std::vector<char const*> requiredValidationLayers{"VK_LAYER_KHRONOS_validation"};
	[[nodiscard]] std::vector<char const*> setupValidationLayer();

	bool isDeviceSuitable(const vk::raii::PhysicalDevice& physicalDevice);

	const std::vector<vk::QueueFlagBits> requiredDeviceQueues{
	    vk::QueueFlagBits::eGraphics,
	};
	const std::vector<const char*> requiredDeviceExtensions{
	    vk::KHRSwapchainExtensionName,
	};

	void checkExtensionsSupport(const std::vector<const char*>& requiredInstanceExtensions);

	/**
	 * Returns true if all required queue families are available.
	 */
	bool isDeviceSupportQueues(const std::vector<vk::QueueFamilyProperties>& deviceQueues);
	bool isDeviceSupportExtensions(const std::vector<vk::ExtensionProperties>& deviceExtensions);

	std::uint32_t findQueueFamilyIndex(vk::QueueFlagBits requiredFlag, const vk::raii::SurfaceKHR& surface);

	void createInstance(const std::vector<const char*>& requiredInstanceExtensions);
	void pickPhysicalDevice();
	void createLogicalDevice(const vk::raii::SurfaceKHR& surface);
	void setupDebugMessenger();
	static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
	    vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
	    vk::DebugUtilsMessageTypeFlagsEXT type,
	    const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
	    void* pUserData
	);
};
#endif // CONTEXT_HPP