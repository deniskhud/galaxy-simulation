#ifndef GALACTIC_IMGUISYSTEM_HPP
#define GALACTIC_IMGUISYSTEM_HPP
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_vulkan.h"

#include <SDL3/SDL.h>
#include <vulkan/vulkan_raii.hpp>

class ImguiSystem {
public:
	ImguiSystem(
	    const vk::raii::Instance& instance,
	    const vk::raii::PhysicalDevice& physicalDevice,
	    const vk::raii::Device& device,
	    uint32_t queueFamily,
	    const vk::raii::Queue& queue,
	    uint32_t imageCount,
	    vk::Format colorFormat,
	    SDL_Window* window
	);

	~ImguiSystem();

	void processEvent(const SDL_Event& event);
	void beginFrame();
	void render(const vk::raii::CommandBuffer& cmdBuf);

private:
	vk::raii::DescriptorPool descriptorPool = nullptr;

	std::unique_ptr<ImGuiContext, decltype(&ImGui::DestroyContext)> context{nullptr, ImGui::DestroyContext};
};
#endif // GALACTIC_IMGUISYSTEM_HPP