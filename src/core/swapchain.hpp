#ifndef SWAPCHAIN_HPP
#define SWAPCHAIN_HPP
#include "context.hpp"
#include "../window/window.hpp"

class SwapChain {
public:
    SwapChain(const VulkanContext& context, const Window& window)
    : context(context),window(window) {
        createSwapChain(context.getSurface());
        createImageViews();
    }

    vk::Format getSwapChainImageFormat() const {
        return swapChainSurfaceFormat.format;
    }

    const vk::raii::SwapchainKHR& getSwapchain() const {
        return swapChain;
    }
    const vk::Extent2D& getExtent() const {
        return swapChainExtent;
    }
    const vk::raii::ImageView& getImageView(std::uint32_t index) const {
        return imageViews.at(index);
    }
    const vk::Image& getImage(std::uint32_t index) const {
        return swapChainImages.at(index);
    }

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

    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const &availableFormats);
    vk::PresentModeKHR chooseSwapPresentMode(std::vector<vk::PresentModeKHR> const &availablePresentModes);
    vk::Extent2D chooseSwapExtent(vk::SurfaceCapabilitiesKHR const &surfaceCapabilities);
    uint32_t chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const &surfaceCapabilities);

    void createImageViews();
};

#endif //SWAPCHAIN_HPP
