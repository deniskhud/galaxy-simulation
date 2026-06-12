#ifndef SWAPCHAIN_HPP
#define SWAPCHAIN_HPP
#include "context.hpp"
#include "../window/window.hpp"
class Swapchain {
public:



private:
    const VulkanContext& context;
    const Window& window;

    vk::raii::SwapchainKHR swapchain;
    std::vector<vk::Image> images;
    std::vector<vk::ImageView> imageViews;
    vk::SurfaceFormatKHR surfaceFormat;
    vk::Extent2D extent;

};


#endif //SWAPCHAIN_HPP
