#ifndef WINDOW_HPP
#define WINDOW_HPP
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <iostream>
#include <stdexcept>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
class Window final {
private:
    int WIDTH = 1920, HEIGHT = 1080;
    SDL_Window* window = nullptr;

    bool SDLCALL framebufferResizeCallback(void* userdata, SDL_Event* event);
public:
    /**
     * Creates Vulkan surface for the SDL window.
     *
     * Uses SDL_Vulkan_CreateSurface to connect Vulkan with the current window.
     * The surface is wrapped in RAII for automatic cleanup.
     *
     * @param instance Vulkan instance
     * @return Vulkan surface handle
     */
    vk::raii::SurfaceKHR createSurface(const vk::raii::Instance& instance) const;
    std::vector<const char*> getRequiredInstanceExtensions() const;

    /*
     * @return {width, height}
     */
    [[nodiscard]] std::pair<int, int> getFrameBufferSize() const {
        int w = 0, h = 0;
        SDL_GetWindowSizeInPixels(window, &w, &h);
        return {w, h};
    }

    Window();
    ~Window();
};


#endif //WINDOW_HPP
