#include "window.hpp"


Window::Window() {
    SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "wayland");
    // В SDL3 функция возвращает true при успехе
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        throw std::runtime_error("Failed to init SDL3: " + std::string(SDL_GetError()));
    }


    std::cout << "Platform: " << SDL_GetCurrentVideoDriver() << "\n";

    // Создаем окно с поддержкой Vulkan
    window = SDL_CreateWindow(
        "Vulkan SDL3 Window",
        WIDTH, HEIGHT,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN
    );

    SDL_AddEventWatch(framebufferResizeCallback, this);

    if (!window) {
        throw std::runtime_error("Failed to create SDL3 window: " + std::string(SDL_GetError()));
    }
    std::cout << "Window created\n";
}

Window::~Window() {
    if (window) {
        SDL_DestroyWindow(window);
    }

    // Завершаем работу SDL3
    SDL_Quit();
}

bool SDLCALL Window::framebufferResizeCallback(void* userdata, SDL_Event* event)
{
    auto app = reinterpret_cast<Window*>(userdata);
    app->framebufferResized = true;
    return app->framebufferResized;

}

vk::raii::SurfaceKHR Window::createSurface(const vk::raii::Instance& instance){
    VkSurfaceKHR surface;
    if (!SDL_Vulkan_CreateSurface(window, *instance, nullptr, &surface)) {
        throw std::runtime_error("failed to create window surface!");
    }
    return vk::raii::SurfaceKHR(instance, surface);
}

std::vector<const char*> Window::getRequiredInstanceExtensions() {
    std::uint32_t sdlExtensionsCount = 0;
    auto sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlExtensionsCount);

    std::vector extensions(sdlExtensions, sdlExtensions + sdlExtensionsCount);
    /*if (enableValidationLayers) {
        extensions.push_back(vk::EXTDebugUtilsExtensionName);
    }*/
    return extensions;
}