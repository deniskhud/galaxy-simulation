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
    int WIDTH, HEIGHT;
    SDL_Window* window = nullptr;

    bool SDLCALL Window::framebufferResizeCallback(void* userdata, SDL_Event* event);
public:
    /*
     *
     */
    vk::raii::SurfaceKHR createSurface(const vk::raii::Instance& instance);

    Window();
    ~Window();
};


#endif //WINDOW_HPP
