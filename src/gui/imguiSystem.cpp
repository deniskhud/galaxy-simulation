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
}

ImguiSystem::~ImguiSystem() {
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
