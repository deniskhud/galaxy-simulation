#ifndef GALACTIC_RENDERER_HPP
#define GALACTIC_RENDERER_HPP

#include "../core/context.hpp"
#include "../core/descriptors.hpp"
#include "../core/swapchain.hpp"
#include "../gui/imguiSystem.hpp"
#include "../pipelines/pipeline.hpp"
#include "../scene/particle.hpp"

#include <chrono>

struct GalaxyParams {
	float galaxyRadius = 20.0f;
	float diskThickness = 0.30f;
	float maxEccentricity = 0.6f;
	int armCount = 2.0f;
	float armTwist = 4.0f;
	float maxOrbitalSpeed = 0.7f;
	float coreRadius = 0.4f;
	int particleCount = 20000;
};

class Renderer {
public:
	Renderer(
	    const VulkanContext& context,
	    SwapChain& swapChain,
	    const Pipeline& pipeline,
	    DescriptorPool& descriptors,
	    ParticleSystem& particles,
	    ImguiSystem& imguiSystem
	);

	void drawFrame();

	void reinitParticles(GalaxyParams& galaxyParams);
	void setGalaxyParams(const GalaxyParams& p) { galaxyParams = p; }

private:
	std::chrono::steady_clock::time_point lastFrameTime = std::chrono::steady_clock::now();
	float totalTime = 0.0f;

	static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
	std::uint32_t frameIndex = 0;

	GalaxyParams galaxyParams{};

	vk::raii::CommandPool createCommandPool() const;

	void recordCommandBuffer(const vk::raii::CommandBuffer& commandBuffer, uint32_t imageIndex, float deltaTime);

	std::vector<vk::raii::CommandBuffer> createCommandBuffers();
	std::vector<vk::raii::Semaphore> createRenderFinishedSemaphores();
	std::vector<vk::raii::Semaphore> createImageAvailableSemaphores();
	std::vector<vk::raii::Fence> createInFlightFences();

	const VulkanContext& context;
	SwapChain& swapChain;
	const Pipeline& pipeline;
	DescriptorPool& descriptors;
	ParticleSystem& particles;
	ImguiSystem& imguiSystem;

	vk::raii::CommandPool commandPool = nullptr;
	std::vector<vk::raii::CommandBuffer> commandBuffers;
	std::vector<vk::raii::Semaphore> imageAvailableSemaphores;
	std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
	std::vector<vk::raii::Fence> inFlightFences;

	vk::raii::CommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(const vk::raii::CommandBuffer& cmd);

	bool needsReinit{true};
};

#endif // GALACTIC_RENDERER_HPP
