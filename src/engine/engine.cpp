#include "engine.hpp"

Engine::Engine() = default;

Engine::~Engine() {
	if (vulkanContext) {
		vulkanContext->getDevice().waitIdle();
	}
}

void Engine::initialize() {
	window = std::make_unique<Window>();
	vulkanContext = std::make_unique<VulkanContext>(*window);

	swapChain = std::make_unique<SwapChain>(*vulkanContext, *window);
	pipeline = std::make_unique<Pipeline>(*vulkanContext, *swapChain);

	particles = std::make_unique<ParticleSystem>(*vulkanContext, static_cast<std::uint32_t>(galaxyParams.particleCount));
	auto [w, h] = window->getFrameBufferSize();
	camera = std::make_unique<Camera>(*vulkanContext, static_cast<float>(w) / static_cast<float>(h));

	descriptorPool = std::make_unique<DescriptorPool>(
	    *vulkanContext,
	    *pipeline,
	    camera->getCameraBuffer().get(),
	    camera->getCameraBuffer().getSize(),
	    particles->getSsboBuffer().get(),
	    particles->getSsboBuffer().getSize(),
	    particles->getSsboBuffer().get(),
	    particles->getSsboBuffer().getSize()
	);

	imGuiSystem = std::make_unique<ImguiSystem>(
	    vulkanContext->getInstance(),
	    vulkanContext->getPhysicalDevice(),
	    vulkanContext->getDevice(),
	    vulkanContext->getGraphicsQueueFamilyIndex(),
	    vulkanContext->getGraphicsQueue(),
	    swapChain->getSwapChainImageCount(),
	    swapChain->getColorFormat(),
	    window->getWindow()
	);

	renderer = std::make_unique<Renderer>(
	    *vulkanContext,
	    *swapChain,
	    *pipeline,
	    *descriptorPool,
	    *particles,
	    *imGuiSystem
	);

	renderer->setGalaxyParams(galaxyParams);
}

void Engine::run() {
	InputState input;
	bool running = true;

	Uint64 previous = SDL_GetPerformanceCounter();
	while (running) {
		Uint64 current = SDL_GetPerformanceCounter();
		float deltaTime =
		    static_cast<float>(current - previous) / static_cast<float>(SDL_GetPerformanceFrequency());
		previous = current;

		input.update();
		running = processEvents(input);
		update(deltaTime, input);
		drawGui();
		renderer->drawFrame();
		handleResize();
	}
}

bool Engine::processEvents(InputState& input) {
	SDL_Event event;
	bool running = true;

	while (SDL_PollEvent(&event)) {
		imGuiSystem->processEvent(event);

		if (event.type == SDL_EVENT_QUIT) {
			running = false;
		}

		if (event.type == SDL_EVENT_WINDOW_RESIZED) {
			window->framebufferResized = true;
		}

		if (!ImGui::GetIO().WantCaptureMouse) {
			input.processEvent(event);
		}
	}

	return running;
}

void Engine::update(float deltaTime, const InputState& input) {
	camera->updateCamera(deltaTime, input);
}

void Engine::drawGui() {
	imGuiSystem->beginFrame();
	ImGui::Begin("Galaxy");
	ImGui::Text("fps: %.2f", window->calculateFrameRate());

	bool needReinit = false;

	auto initSliderInt = [&](const char* label, int& v, int mn, int mx) {
		ImGui::SliderInt(label, &v, mn, mx);
		if (ImGui::IsItemDeactivatedAfterEdit()) {
			needReinit = true;
		}
	};

	auto initSliderFloat = [&](const char* label, float& v, float mn, float mx) {
		ImGui::SliderFloat(label, &v, mn, mx);
		if (ImGui::IsItemDeactivatedAfterEdit()) {
			needReinit = true;
		}
	};

	initSliderInt("Particles", galaxyParams.particleCount, 1000, 10000000);
	initSliderFloat("Radius", galaxyParams.galaxyRadius, 1.0f, 100.0f);
	initSliderFloat("Thickness", galaxyParams.diskThickness, 0.01f, 2.0f);
	initSliderFloat("Eccentricity", galaxyParams.maxEccentricity, 0.0f, 0.99f);
	initSliderInt("Arms", galaxyParams.armCount, 2, 10);
	initSliderFloat("Arm Twist", galaxyParams.armTwist, 0.0f, 10.0f);

	ImGui::SliderFloat("Orbital Speed", &galaxyParams.maxOrbitalSpeed, 0.0f, 5.0f);
	ImGui::SliderFloat("Core Radius", &galaxyParams.coreRadius, 0.01f, 5.0f);
	ImGui::End();

	if (needReinit) {
		renderer->reinitParticles(galaxyParams);
	}
}

void Engine::handleResize() {
	if (!window->framebufferResized) {
		return;
	}

	vulkanContext->getDevice().waitIdle();
	swapChain->recreateSwapChain();

	auto [w, h] = window->getFrameBufferSize();
	camera->setAspectRatio(static_cast<float>(w) / static_cast<float>(h));
	window->framebufferResized = false;
}
