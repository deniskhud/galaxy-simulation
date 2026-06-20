#ifndef SWAPCHAIN_HPP
#define SWAPCHAIN_HPP
#include "../window/window.hpp"
#include "context.hpp"

class SwapChain {
public:
	SwapChain(const VulkanContext& context, const Window& window);

	/*** getters ***/
	vk::Format getSwapChainImageFormat() const;
	const vk::raii::SwapchainKHR& getSwapchain() const;
	const vk::Extent2D& getExtent() const;
	const vk::raii::ImageView& getImageView(std::uint32_t index) const;
	const vk::Image& getImage(std::uint32_t index) const;
	void recreateSwapChain();

private:
	const VulkanContext& context;
	const Window& window;

	vk::raii::SwapchainKHR swapChain = nullptr;
	std::vector<vk::Image> swapChainImages;
	std::vector<vk::raii::ImageView> imageViews;
	vk::SurfaceFormatKHR swapChainSurfaceFormat;
	vk::Extent2D swapChainExtent;

	void createSwapChain(const vk::raii::SurfaceKHR& surface);
	void cleanupSwapChain();

	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const& availableFormats);
	vk::PresentModeKHR chooseSwapPresentMode(std::vector<vk::PresentModeKHR> const& availablePresentModes);
	[[nodiscard]] vk::Extent2D chooseSwapExtent(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities);
	uint32_t chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities);

	void createImageViews();
};
#endif // SWAPCHAIN_HPP