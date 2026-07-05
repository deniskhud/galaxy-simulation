#ifndef WINDOW_HPP
#define WINDOW_HPP
#include "../include/inc.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <iostream>
#include <stdexcept>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

class Window final {
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
	[[nodiscard]] vk::raii::SurfaceKHR createSurface(const vk::raii::Instance& instance) const;
	[[nodiscard]] std::vector<const char*> getRequiredInstanceExtensions() const;
	SDL_Window* getWindow() { return window; }

	std::uint32_t getFrameRate();
	/*
	 * @return {width, height}
	 */
	[[nodiscard]] std::pair<int, int> getFrameBufferSize() const;
	bool framebufferResized = false;
	Window();
	~Window();

	bool processEvent(const SDL_Event& event, InputState& input, bool allowInput);
	void updateInput(InputState& input) const;
	double calculateFrameRate();

private:
	int width{}, height{};
	SDL_Window* window = nullptr;

	bool SDLCALL framebufferResizeCallback(void* userdata, SDL_Event* event);

	bool rotating = false;
	float lastX{};
	float lastY{};

	Uint64 prev = SDL_GetPerformanceCounter();
	double freq = static_cast<double>(SDL_GetPerformanceFrequency());
};
#endif // WINDOW_HPP