#include "window.hpp"

Window::Window() {
	SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "wayland");
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		LOG_ERROR("Window::Window", "Failed to init SDL3: {}", std::string(SDL_GetError()));
	}
	LOG_INFO("Window::Window", "SDL video driver: {}", SDL_GetCurrentVideoDriver());

	const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());
	if (mode) {
		width = mode->w;
		height = mode->h;
	} else {
		LOG_WARNING("Window::Window", "Failed to query display mode: {}", std::string(SDL_GetError()));
	}

	window = SDL_CreateWindow("Vulkan SDL3 Window", width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);

	// SDL_AddEventWatch(framebufferResizeCallback, this);

	if (!window) {
		LOG_ERROR("Window::Window", "Failed to create SDL3 window: {}", std::string(SDL_GetError()));
	}
	LOG_INFO("Window::Window","Window created: width {}, height {}", width, height);
}

Window::~Window() {
	if (window) {
		LOG_INFO("Window::~Window", "Destroying SDL window");
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

bool Window::processEvent(const SDL_Event& e, InputState& input, bool allowInput) {
	if (e.type == SDL_EVENT_QUIT) {
		return false;
	}

	if (e.type == SDL_EVENT_WINDOW_RESIZED) {
		framebufferResized = true;
		LOG_INFO("Window::processEvent", "Window resize event received");
	}

	if (!allowInput) {
		if (e.type == SDL_EVENT_MOUSE_BUTTON_UP && e.button.button == SDL_BUTTON_LEFT) {
			rotating = false;
		}
		return true;
	}

	if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && e.button.button == SDL_BUTTON_LEFT) {
		rotating = true;
		lastX = e.button.x;
		lastY = e.button.y;
	}

	if (e.type == SDL_EVENT_MOUSE_BUTTON_UP && e.button.button == SDL_BUTTON_LEFT) {
		rotating = false;
	}

	if (e.type == SDL_EVENT_MOUSE_MOTION && rotating) {
		input.mouseDX += e.motion.x - lastX;
		input.mouseDY += e.motion.y - lastY;

		lastX = e.motion.x;
		lastY = e.motion.y;
	}

	if (e.type == SDL_EVENT_MOUSE_WHEEL) {
		input.wheel += e.wheel.y;
	}

	return true;
}

void Window::updateInput(InputState& input) const {
	input.mouseDX = 0;
	input.mouseDY = 0;
	input.wheel = 0;

	const bool* keys = SDL_GetKeyboardState(nullptr);

	input.forward = keys[SDL_SCANCODE_W];
	input.backward = keys[SDL_SCANCODE_S];
	input.left = keys[SDL_SCANCODE_A];
	input.right = keys[SDL_SCANCODE_D];
	input.up = keys[SDL_SCANCODE_SPACE];
	input.down = keys[SDL_SCANCODE_LSHIFT];
}

vk::raii::SurfaceKHR Window::createSurface(const vk::raii::Instance& instance) const {
	VkSurfaceKHR surface;
	if (!SDL_Vulkan_CreateSurface(window, *instance, nullptr, &surface)) {
		LOG_ERROR("Window::createSurface", "Failed to create Vulkan surface: {}", std::string(SDL_GetError()));
	}
	LOG_INFO("Window::createSurface", "Vulkan surface created");
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

double Window::calculateFrameRate() {
	Uint64 now = SDL_GetPerformanceCounter();
	double dt = (double)(now - prev) / freq;
	prev = now;

	double fps = 1.0 / dt;
	return fps;
}