#include "imguiSystem.hpp"

ImguiSystem::ImguiSystem(
    const vk::raii::Instance& instance,
    const vk::raii::PhysicalDevice& physicalDevice,
    const vk::raii::Device& device,
    uint32_t queueFamily,
    const vk::raii::Queue& queue,
    uint32_t imageCount,
    vk::Format colorFormat,
    SDL_Window* window
) {
	context.reset(ImGui::CreateContext());

	ImGuiIO& io = ImGui::GetIO();
	vk::DescriptorPoolSize poolSizes[] = {
	    {vk::DescriptorType::eSampler, 64},
	    {vk::DescriptorType::eSampledImage, 64},
	};

	vk::DescriptorPoolCreateInfo poolInfo{
	    .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
	    .maxSets = 64,
	    .poolSizeCount = 2,
	    .pPoolSizes = poolSizes
	};

	descriptorPool = vk::raii::DescriptorPool(device, poolInfo);

	ImGui_ImplSDL3_InitForVulkan(window);

	VkFormat fmt = static_cast<VkFormat>(colorFormat);

	ImGui_ImplVulkan_InitInfo initInfo{};
	initInfo.Instance = *instance;
	initInfo.PhysicalDevice = *physicalDevice;
	initInfo.Device = *device;
	initInfo.QueueFamily = queueFamily;
	initInfo.Queue = *queue;
	initInfo.DescriptorPool = *descriptorPool;
	initInfo.MinImageCount = imageCount;
	initInfo.ImageCount = imageCount;
	initInfo.UseDynamicRendering = true;

	initInfo.PipelineInfoMain.PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
	initInfo.PipelineInfoMain.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
	initInfo.PipelineInfoMain.PipelineRenderingCreateInfo.pColorAttachmentFormats = &fmt;

	ImGui_ImplVulkan_Init(&initInfo);
	LOG_INFO("ImguiSystem::ImguiSystem", "ImGui initialized with {} swapchain images", imageCount);
}

ImguiSystem::~ImguiSystem() {
	LOG_INFO("ImguiSystem::~ImguiSystem", "Shutting down ImGui");
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplSDL3_Shutdown();
}

void ImguiSystem::processEvent(const SDL_Event& event) {
	ImGui_ImplSDL3_ProcessEvent(&event);
}

void ImguiSystem::beginFrame() {
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();
}

void ImguiSystem::render(const vk::raii::CommandBuffer& cmdBuf) {
	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *cmdBuf);
}

bool ImguiSystem::drawGui(GuiDrawParams& params) {
	beginFrame();
	ImGui::Begin("Galaxy");
	ImGui::Text("fps: %.2f", params.frameRate);

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

	initSliderInt("Particles", params.galaxyParams.particleCount, 1000, 10000000);
	initSliderFloat("Radius", params.galaxyParams.galaxyRadius, 1.0f, 100.0f);
	initSliderFloat("Thickness", params.galaxyParams.diskThickness, 0.01f, 2.0f);
	initSliderFloat("Eccentricity", params.galaxyParams.maxEccentricity, 0.0f, 0.99f);
	initSliderInt("Arms", params.galaxyParams.armCount, 2, 10);
	initSliderFloat("Arm Twist", params.galaxyParams.armTwist, 0.0f, 10.0f);

	ImGui::SliderFloat("Orbital Speed", &params.galaxyParams.maxOrbitalSpeed, 0.0f, 5.0f);
	ImGui::SliderFloat("Core Radius", &params.galaxyParams.coreRadius, 0.01f, 5.0f);
	ImGui::End();

	return needReinit;
}
