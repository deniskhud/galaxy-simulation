#include "engine.hpp"

Engine::Engine() = default;

Engine::~Engine() {
	if (vulkanContext) {
		vulkanContext->getDevice().waitIdle();
	}
}

void Engine::initialize() {
	window = std::make_unique<Window>();
	guiParams.frameRate = window->calculateFrameRate();

	vulkanContext = std::make_unique<VulkanContext>(*window);

	swapChain = std::make_unique<SwapChain>(*vulkanContext, *window);
	pipeline = std::make_unique<Pipeline>(*vulkanContext, *swapChain);

	particles = std::make_unique<ParticleSystem>(*vulkanContext, static_cast<std::uint32_t>(guiParams.galaxyParams.particleCount));
	auto [w, h] = window->getFrameBufferSize();
	camera = std::make_unique<Camera>(*vulkanContext, static_cast<float>(w) / static_cast<float>(h));

	descriptorPool = std::make_unique<DescriptorPool>(
	    *vulkanContext,
	    *pipeline,
	    camera->getCameraBuffer().getBufferView(),
	    particles->getSsboBuffer().getBufferView()
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

	renderer->setGalaxyParams(guiParams.galaxyParams);
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
	guiParams.frameRate = window->calculateFrameRate();
}

void Engine::drawGui() {
	if (imGuiSystem->drawGui(guiParams)) {
		renderer->reinitParticles(guiParams.galaxyParams);
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
