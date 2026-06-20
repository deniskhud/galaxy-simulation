#ifndef GALACTIC_RENDERER_HPP
#define GALACTIC_RENDERER_HPP

#include "../core/context.hpp"
#include "../core/descriptors.hpp"
#include "../core/swapchain.hpp"
#include "../pipelines/pipeline.hpp"
#include "../scene/particle.hpp"

#include <chrono>
class Renderer {
public:
	Renderer(const VulkanContext& context,
	         SwapChain& swapChain,
	         const Pipeline& pipeline,
	         const DescriptorPool& descriptors,
	         const ParticleSystem& particles);

	void drawFrame();

private:
	std::chrono::steady_clock::time_point lastFrameTime = std::chrono::steady_clock::now();
	float totalTime = 0.0f;

	vk::raii::CommandPool createCommandPool() const;
	vk::raii::CommandBuffer createCommandBuffer() const;
	void recordCommandBuffer(uint32_t imageIndex, float deltaTime);

	const VulkanContext& context;
	SwapChain& swapChain;
	const Pipeline& pipeline;
	const DescriptorPool& descriptors;
	const ParticleSystem& particles;

	vk::raii::CommandPool commandPool = nullptr;
	vk::raii::CommandBuffer commandBuffer = nullptr;

	vk::raii::Semaphore imageAvailableSemaphore = nullptr;
	vk::raii::Semaphore renderFinishedSemaphore = nullptr;
	vk::raii::Fence inFlightFence = nullptr;
};

#endif // GALACTIC_RENDERER_HPP
