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
	GalaxyParams galaxyParams;
	Window window{};
	VulkanContext context(window);

	SwapChain swapChain(context, window);

	Pipeline pipeline(context, swapChain);

	ParticleSystem particles(context, static_cast<uint32_t>(galaxyParams.particleCount));

	auto [w, h] = window.getFrameBufferSize();
	Camera camera(context, static_cast<float>(w) / static_cast<float>(h));

	/*Buffer computeBuffer(
	    context,
	    sizeof(int),
	    vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst,
	    vk::MemoryPropertyFlagBits::eDeviceLocal
	);*/

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
	renderer.setGalaxyParams(galaxyParams);

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
		ImGui::Begin("Galaxy");
		ImGui::Text("fps: %.2f", window.calculateFrameRate());

		bool needReinit = false;

		auto initSliderInt = [&](const char* label, int& v, int mn, int mx) {
			ImGui::SliderInt(label, &v, mn, mx);
			if (ImGui::IsItemDeactivatedAfterEdit())
				needReinit = true;
		};
		auto initSliderFloat = [&](const char* label, float& v, float mn, float mx) {
			ImGui::SliderFloat(label, &v, mn, mx);
			if (ImGui::IsItemDeactivatedAfterEdit())
				needReinit = true;
		};

		initSliderInt("Particles", galaxyParams.particleCount, 1000, 100000);
		initSliderFloat("Radius", galaxyParams.galaxyRadius, 1.0f, 100.0f);
		initSliderFloat("Thickness", galaxyParams.diskThickness, 0.01f, 2.0f);
		initSliderFloat("Eccentricity", galaxyParams.maxEccentricity, 0.0f, 0.99f);
		initSliderInt("Arms", galaxyParams.armCount, 2, 10);
		initSliderFloat("Arm Twist", galaxyParams.armTwist, 0.0f, 10.0f);

		ImGui::SliderFloat("Orbital Speed", &galaxyParams.maxOrbitalSpeed, 0.0f, 5.0f);
		ImGui::SliderFloat("Core Radius", &galaxyParams.coreRadius, 0.01f, 5.0f);
		ImGui::End();

		if (needReinit)
			renderer.reinitParticles(galaxyParams);

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