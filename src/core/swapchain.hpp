#ifndef SWAPCHAIN_HPP
#define SWAPCHAIN_HPP
#include "context.hpp"
#include "../window/window.hpp"

class SwapChain {
public:
    SwapChain(const VulkanContext& context, const Window& window)
    : context(context),window(window)
    {  }

    vk::Format getSwapChainImageFormat() const {
        return swapChainSurfaceFormat.format;
    }

private:
    const VulkanContext& context;
    const Window& window;

    vk::raii::SwapchainKHR swapChain = nullptr;
    std::vector<vk::Image> swapChainImages;
    std::vector<vk::ImageView> imageViews;
    vk::SurfaceFormatKHR swapChainSurfaceFormat;
    vk::Extent2D swapChainExtent;

    void createSwapChain(const vk::raii::SurfaceKHR& surface);
    void recreateSwapChain();
    void cleanupSwapChain();

    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const &availableFormats);
    vk::PresentModeKHR chooseSwapPresentMode(std::vector<vk::PresentModeKHR> const &availablePresentModes);
    vk::Extent2D chooseSwapExtent(vk::SurfaceCapabilitiesKHR const &surfaceCapabilities);
    uint32_t chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const &surfaceCapabilities);

    void createImageViews();
};

#endif //SWAPCHAIN_HPP
