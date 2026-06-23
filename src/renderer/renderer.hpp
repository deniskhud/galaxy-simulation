#ifndef GALACTIC_RENDERER_HPP
#define GALACTIC_RENDERER_HPP

#include "../core/context.hpp"
#include "../core/descriptors.hpp"
#include "../core/swapchain.hpp"
#include "../gui/imguiSystem.hpp"
#include "../pipelines/pipeline.hpp"
#include "../scene/particle.hpp"

#include <chrono>
class Renderer {
public:
	Renderer(
	    const VulkanContext& context,
	    SwapChain& swapChain,
	    const Pipeline& pipeline,
	    const DescriptorPool& descriptors,
	    const ParticleSystem& particles,
	    ImguiSystem& imguiSystem
	);

	void drawFrame();

private:
	std::chrono::steady_clock::time_point lastFrameTime = std::chrono::steady_clock::now();
	float totalTime = 0.0f;

	static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
	std::uint32_t frameIndex = 0;

	vk::raii::CommandPool createCommandPool() const;
	vk::raii::CommandBuffer createCommandBuffer() const;
	void recordCommandBuffer(const vk::raii::CommandBuffer& commandBuffer, uint32_t imageIndex, float deltaTime);

	std::vector<vk::raii::CommandBuffer> createCommandBuffers();
	std::vector<vk::raii::Semaphore> createRenderFinishedSemaphores();
	std::vector<vk::raii::Semaphore> createImageAvailableSemaphores();
	std::vector<vk::raii::Fence> createInFlightFences();

	const VulkanContext& context;
	SwapChain& swapChain;
	const Pipeline& pipeline;
	const DescriptorPool& descriptors;
	const ParticleSystem& particles;
	ImguiSystem& imguiSystem;

	vk::raii::CommandPool commandPool = nullptr;
	std::vector<vk::raii::CommandBuffer> commandBuffers;
	std::vector<vk::raii::Semaphore> imageAvailableSemaphores;
	std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
	std::vector<vk::raii::Fence> inFlightFences;
};

#endif // GALACTIC_RENDERER_HPP
