#include "src/camera/camera.hpp"
#include "src/core/buffer.hpp"
#include "src/core/context.hpp"
#include "src/core/descriptors.hpp"
#include "src/core/swapchain.hpp"
#include "src/gui/imguiSystem.hpp"
#include "src/pipelines/pipeline.hpp"
#include "src/renderer/renderer.hpp"
#include "src/scene/particle.hpp"
#include "src/window/window.hpp"

int main() {
	Window window{};
	VulkanContext context(window);

	SwapChain swapChain(context, window);

	Pipeline pipeline(context, swapChain);

	int particlesCount = 20000;
	ParticleSystem particles(context, particlesCount);

	auto [w, h] = window.getFrameBufferSize();
	Camera camera(context, static_cast<float>(w) / static_cast<float>(h));

	Buffer computeBuffer(
	    context,
	    sizeof(int),
	    vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst,
	    vk::MemoryPropertyFlagBits::eDeviceLocal
	);

	DescriptorPool descriptors(
	    context,
	    pipeline,
	    camera.getCameraBuffer().get(),
	    camera.getCameraBuffer().getSize(),
	    particles.getSsboBuffer().get(),
	    particles.getSsboBuffer().getSize(),
	    particles.getSsboBuffer().get(),
	    particles.getSsboBuffer().getSize()
	);

	ImguiSystem imguiSystem(
	    context.getInstance(),
	    context.getPhysicalDevice(),
	    context.getDevice(),
	    context.getGraphicsQueueFamilyIndex(),
	    context.getGraphicsQueue(),
	    swapChain.getSwapChainImageCount(),
	    swapChain.getColorFormat(),
	    window.getWindow()
	);

	Renderer renderer(context, swapChain, pipeline, descriptors, particles, imguiSystem);
	SDL_Event e;
	float lastX = 0.0f;
	float lastY = 0.0f;
	bool rotating = false;

	bool running = true;
	while (running) {
		while (SDL_PollEvent(&e)) {
			imguiSystem.processEvent(e);
			if (e.type == SDL_EVENT_QUIT)
				running = false;
			if (e.type == SDL_EVENT_WINDOW_RESIZED) {
				window.framebufferResized = true;
			}

			ImGuiIO& io = ImGui::GetIO();

			if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && e.button.button == SDL_BUTTON_LEFT) {
				rotating = true;

				lastX = e.button.x;
				lastY = e.button.y;
			}
			if (e.type == SDL_EVENT_MOUSE_BUTTON_UP && e.button.button == SDL_BUTTON_LEFT) {
				rotating = false;
			}
			if (e.type == SDL_EVENT_MOUSE_MOTION && rotating && !io.WantCaptureMouse) {
				float dx = e.motion.x - lastX;

				float dy = e.motion.y - lastY;

				camera.rotate(dx, dy);

				lastX = e.motion.x;

				lastY = e.motion.y;
			}

			if (e.type == SDL_EVENT_MOUSE_WHEEL && !io.WantCaptureMouse) {
				camera.zoom(e.wheel.y);
			}
		}

		imguiSystem.beginFrame();
		ImGui::Begin("Debug");
		ImGui::Text("fps: %2.f", window.calculateFrameRate());

		ImGui::SliderInt("Particles", &particlesCount, 100, 10000000);
		ImGui::Button("Button", ImVec2(50, 30));
		ImGui::End();

		camera.uploadUbo();

		renderer.drawFrame();

		if (window.framebufferResized) {
			context.getDevice().waitIdle();

			swapChain.recreateSwapChain();

			window.framebufferResized = false;
		}
	}
	context.getDevice().waitIdle();
	return 0;
}