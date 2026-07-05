#include "engine.hpp"

Engine::Engine() = default;

Engine::~Engine() {
	if (vulkanContext) {
		LOG_INFO("Engine::~Engine", "Waiting for device idle before shutdown");
		vulkanContext->getDevice().waitIdle();
	}
}

void Engine::initialize() {
	LOG_INFO("Engine::initialize", "Initializing engine");
	window = std::make_unique<Window>();
	guiParams.frameRate = window->calculateFrameRate();

	vulkanContext = std::make_unique<VulkanContext>(*window);

	swapChain = std::make_unique<SwapChain>(*vulkanContext, *window);
	pipeline = std::make_unique<Pipeline>(*vulkanContext, *swapChain);

	particles = std::make_unique<ParticleSystem>(*vulkanContext, static_cast<std::uint32_t>(guiParams.galaxyParams.particleCount));
	auto [w, h] = window->getFrameBufferSize();
	camera = std::make_unique<Camera>(static_cast<float>(w) / static_cast<float>(h));
	cameraUboBuffer = std::make_unique<Buffer>(
	    *vulkanContext,
	    sizeof(CameraUbo),
	    vk::BufferUsageFlagBits::eUniformBuffer,
	    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
	);
	uploadCameraUbo();

	descriptorPool = std::make_unique<DescriptorPool>(
	    *vulkanContext,
	    *pipeline,
	    cameraUboBuffer->getBufferView(),
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
	LOG_INFO("Engine::initialize", "Engine initialized");
}

void Engine::run() {
	LOG_INFO("Engine::run", "Entering main loop");
	InputState input;
	bool running = true;

	Uint64 previous = SDL_GetPerformanceCounter();
	while (running) {
		Uint64 current = SDL_GetPerformanceCounter();
		float deltaTime =
		    static_cast<float>(current - previous) / static_cast<float>(SDL_GetPerformanceFrequency());
		previous = current;

		window->updateInput(input);
		running = processEvents(input);
		update(deltaTime, input);
		drawGui();
		renderer->drawFrame();
		handleResize();
	}
	LOG_INFO("Engine::run", "Leaving main loop");
}

bool Engine::processEvents(InputState& input) {
	SDL_Event event;
	bool running = true;

	while (SDL_PollEvent(&event)) {
		imGuiSystem->processEvent(event);

		if (!window->processEvent(event, input, !ImGui::GetIO().WantCaptureMouse)) {
			LOG_INFO("Engine::processEvents", "Quit event received");
			running = false;
		}
	}

	return running;
}

void Engine::update(float deltaTime, const InputState& input) {
	camera->update(deltaTime, input);
	uploadCameraUbo();
	guiParams.frameRate = window->calculateFrameRate();
}

void Engine::uploadCameraUbo() {
	auto uboData = camera->getUbo();
	cameraUboBuffer->upload(&uboData, sizeof(uboData));
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

	LOG_WARNING("Engine::handleResize", "Handling framebuffer resize");
	vulkanContext->getDevice().waitIdle();
	swapChain->recreateSwapChain();

	auto [w, h] = window->getFrameBufferSize();
	camera->setAspectRatio(static_cast<float>(w) / static_cast<float>(h));
	uploadCameraUbo();
	window->framebufferResized = false;
}