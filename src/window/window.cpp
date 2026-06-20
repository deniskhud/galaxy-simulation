#include "window.hpp"

Window::Window() {
	SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "wayland");
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		throw std::runtime_error("Failed to init SDL3: " + std::string(SDL_GetError()));
	}
	std::cout << "Platform: " << SDL_GetCurrentVideoDriver() << "\n";

	const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());
	if (mode) {
		width = mode->w;
		height = mode->h;
	}

	window = SDL_CreateWindow("Vulkan SDL3 Window", width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);

	// SDL_AddEventWatch(framebufferResizeCallback, this);

	if (!window) {
		throw std::runtime_error("Failed to create SDL3 window: " + std::string(SDL_GetError()));
	}
	debugSystem::log(LogLevel::INFO,
	                 "window",
	                 "window created: width " + std::to_string(width) + ", height " + std::to_string(height));
}

Window::~Window() {
	if (window) {
		SDL_DestroyWindow(window);
	}
	SDL_Quit();
}

bool SDLCALL Window::framebufferResizeCallback(void* userdata, SDL_Event* event) {
	auto app = reinterpret_cast<Window*>(userdata);
	/*app->framebufferResized = true;
	return app->framebufferResized;*/
	return true;
}

vk::raii::SurfaceKHR Window::createSurface(const vk::raii::Instance& instance) const {
	VkSurfaceKHR surface;
	if (!SDL_Vulkan_CreateSurface(window, *instance, nullptr, &surface)) {
		throw std::runtime_error("failed to create window surface!");
	}
	return vk::raii::SurfaceKHR(instance, surface);
}

std::vector<const char*> Window::getRequiredInstanceExtensions() const {
	std::uint32_t sdlExtensionsCount = 0;
	auto sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlExtensionsCount);

	std::vector extensions(sdlExtensions, sdlExtensions + sdlExtensionsCount);
	if (enableValidationLayers) {
		extensions.push_back(vk::EXTDebugUtilsExtensionName);
	}
	return extensions;
}

std::pair<int, int> Window::getFrameBufferSize() const {
	int w = 0, h = 0;
	SDL_GetWindowSizeInPixels(window, &w, &h);
	return {w, h};
}