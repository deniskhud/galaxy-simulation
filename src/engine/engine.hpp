#ifndef GALACTIC_ENGINE_HPP
#define GALACTIC_ENGINE_HPP
#include "../camera/camera.hpp"
#include "../core/buffer.hpp"
#include "../core/context.hpp"
#include "../core/descriptors.hpp"
#include "../core/swapchain.hpp"
#include "../pipelines/pipeline.hpp"
#include "../renderer/renderer.hpp"
#include "../scene/particle.hpp"
#include "../window/window.hpp"
#include "../gui/imguiSystem.hpp"
#include <memory>

class Engine {
public:
	Engine();
	~Engine();

	void initialize();
	void run();

private:
	bool processEvents(InputState& input);
	void update(float deltaTime, const InputState& input);
	void drawGui();
	void handleResize();

	GuiDrawParams guiParams{};
	std::unique_ptr<Window> window;
	std::unique_ptr<VulkanContext> vulkanContext;
	std::unique_ptr<SwapChain> swapChain;
	std::unique_ptr<Pipeline> pipeline;
	std::unique_ptr<ParticleSystem> particles;
	std::unique_ptr<Camera> camera;
	std::unique_ptr<DescriptorPool> descriptorPool;
	std::unique_ptr<Renderer> renderer;
	std::unique_ptr<ImguiSystem> imGuiSystem;
};

#endif // GALACTIC_ENGINE_HPP
