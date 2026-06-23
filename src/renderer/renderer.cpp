#include "renderer.hpp"

Renderer::Renderer(
    const VulkanContext& context,
    SwapChain& swapChain,
    const Pipeline& pipeline,
    const DescriptorPool& descriptors,
    const ParticleSystem& particles
)
    : context(context), swapChain(swapChain), pipeline(pipeline), descriptors(descriptors), particles(particles),
      commandPool(createCommandPool()),
      imageAvailableSemaphore(context.getDevice(), vk::SemaphoreCreateInfo{}),
      renderFinishedSemaphore(context.getDevice(), vk::SemaphoreCreateInfo{}),
      inFlightFence(context.getDevice(), vk::FenceCreateInfo{.flags = vk::FenceCreateFlagBits::eSignaled}) {

	renderFinishedSemaphores = createRenderFinishedSemaphores();
	imageAvailableSemaphores = createImageAvailableSemaphores();
	inFlightFences = createInFlightFences();
	createCommandBuffers();
}

vk::raii::CommandPool Renderer::createCommandPool() const {
	vk::CommandPoolCreateInfo poolInfo{
	    .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
	    .queueFamilyIndex = context.getGraphicsQueueFamilyIndex(),
	};
	return vk::raii::CommandPool(context.getDevice(), poolInfo);
}

vk::raii::CommandBuffer Renderer::createCommandBuffer() const {
	vk::CommandBufferAllocateInfo allocInfo{
	    .commandPool = *commandPool,
	    .level = vk::CommandBufferLevel::ePrimary,
	    .commandBufferCount = 1,
	};
	auto buffers = context.getDevice().allocateCommandBuffers(allocInfo);
	return std::move(buffers.front());
}

void Renderer::createCommandBuffers() {
	vk::CommandBufferAllocateInfo allocInfo{
		.commandPool = *commandPool,
		.level = vk::CommandBufferLevel::ePrimary,
		.commandBufferCount = MAX_FRAMES_IN_FLIGHT,
	};
	commandBuffers = vk::raii::CommandBuffers( context.getDevice(), allocInfo );
}


void Renderer::recordCommandBuffer(const vk::raii::CommandBuffer& commandBuffer, uint32_t imageIndex, float deltaTime) {
	commandBuffer.begin(
	    vk::CommandBufferBeginInfo{
	        .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
	    }
	);

	SimParams params{
	    .time = totalTime,
	    .maxOrbitalSpeed = 0.7f,
	    .coreRadius = 0.4f,
	    .particleCount = particles.getCount(),
	};

	commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline.getComputePipeline());
	commandBuffer.bindDescriptorSets(
	    vk::PipelineBindPoint::eCompute, pipeline.getComputePipelineLayout(), 0, *descriptors.getComputeSet(), {}
	);
	commandBuffer.pushConstants<SimParams>(
	    pipeline.getComputePipelineLayout(), vk::ShaderStageFlagBits::eCompute, 0, params
	);

	uint32_t groupCount = (particles.getCount() + 255) / 256;
	commandBuffer.dispatch(groupCount, 1, 1);

	vk::BufferMemoryBarrier ssboBarrier{
	    .srcAccessMask = vk::AccessFlagBits::eShaderWrite,
	    .dstAccessMask = vk::AccessFlagBits::eVertexAttributeRead,
	    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
	    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
	    .buffer = particles.getSsboBuffer().get(),
	    .offset = 0,
	    .size = VK_WHOLE_SIZE,
	};
	commandBuffer.pipelineBarrier(
	    vk::PipelineStageFlagBits::eComputeShader, vk::PipelineStageFlagBits::eVertexInput, {}, {}, ssboBarrier, {}
	);

	vk::Image image = swapChain.getImage(imageIndex);
	vk::ImageView view = swapChain.getImageView(imageIndex);
	vk::Extent2D extent = swapChain.getExtent();

	vk::ImageSubresourceRange colorRange{
	    .aspectMask = vk::ImageAspectFlagBits::eColor,
	    .baseMipLevel = 0,
	    .levelCount = 1,
	    .baseArrayLayer = 0,
	    .layerCount = 1,
	};

	vk::ImageMemoryBarrier toColorAttachment{
	    .srcAccessMask = {},
	    .dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite,
	    .oldLayout = vk::ImageLayout::eUndefined,
	    .newLayout = vk::ImageLayout::eColorAttachmentOptimal,
	    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
	    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
	    .image = image,
	    .subresourceRange = colorRange,
	};
	commandBuffer.pipelineBarrier(
	    vk::PipelineStageFlagBits::eTopOfPipe,
	    vk::PipelineStageFlagBits::eColorAttachmentOutput,
	    {},
	    {},
	    {},
	    toColorAttachment
	);

	vk::ClearValue clearColor{};
	clearColor.setColor(vk::ClearColorValue{std::array{0.00f, 0.00f, 0.00f, 1.0f}});

	vk::RenderingAttachmentInfo colorAttachment{
	    .imageView = view,
	    .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
	    .loadOp = vk::AttachmentLoadOp::eClear,
	    .storeOp = vk::AttachmentStoreOp::eStore,
	    .clearValue = clearColor,
	};

	vk::RenderingInfo renderingInfo{
	    .renderArea = {.offset = {0, 0}, .extent = extent},
	    .layerCount = 1,
	    .colorAttachmentCount = 1,
	    .pColorAttachments = &colorAttachment,
	};

	commandBuffer.beginRendering(renderingInfo);

	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.getPipeline());

	vk::Viewport viewport{
	    .x = 0.0f,
	    .y = 0.0f,
	    .width = static_cast<float>(extent.width),
	    .height = static_cast<float>(extent.height),
	    .minDepth = 0.0f,
	    .maxDepth = 1.0f,
	};
	commandBuffer.setViewport(0, viewport);
	commandBuffer.setScissor(0, vk::Rect2D{.offset = {0, 0}, .extent = extent});

	commandBuffer.bindDescriptorSets(
	    vk::PipelineBindPoint::eGraphics, pipeline.getPipelineLayout(), 0, *descriptors.get(), {}
	);

	commandBuffer.draw(particles.getCount(), 1, 0, 0);

	commandBuffer.endRendering();

	vk::ImageMemoryBarrier toPresent{
	    .srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite,
	    .dstAccessMask = {},
	    .oldLayout = vk::ImageLayout::eColorAttachmentOptimal,
	    .newLayout = vk::ImageLayout::ePresentSrcKHR,
	    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
	    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
	    .image = image,
	    .subresourceRange = colorRange,
	};
	commandBuffer.pipelineBarrier(
	    vk::PipelineStageFlagBits::eColorAttachmentOutput,
	    vk::PipelineStageFlagBits::eBottomOfPipe,
	    {},
	    {},
	    {},
	    toPresent
	);

	commandBuffer.end();
}

void Renderer::drawFrame() {
	auto now = std::chrono::steady_clock::now();
	float deltaTime = std::chrono::duration<float>(now - lastFrameTime).count();
	lastFrameTime = now;
	totalTime += deltaTime;
	//ждем результат и сбрасываем сигнал
	[[maybe_unused]] auto waitResult = context.getDevice().waitForFences(*inFlightFences[frameIndex], VK_TRUE, UINT64_MAX);
	context.getDevice().resetFences(*inFlightFences[frameIndex]);

	auto [result, imageIndex] =
	    swapChain.getSwapchain().acquireNextImage(UINT64_MAX, *imageAvailableSemaphores[frameIndex], nullptr);
	// new func
	if (result == vk::Result::eErrorOutOfDateKHR) {
		swapChain.recreateSwapChain();
		return;
	}
	commandBuffers[frameIndex].reset();
	recordCommandBuffer(commandBuffers[frameIndex], imageIndex, deltaTime);

	vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	vk::SubmitInfo submitInfo{
	    .waitSemaphoreCount = 1,
	    .pWaitSemaphores = &*imageAvailableSemaphores[frameIndex],
	    .pWaitDstStageMask = &waitStage,
	    .commandBufferCount = 1,
	    .pCommandBuffers = &*commandBuffers[frameIndex],
	    .signalSemaphoreCount = 1,
	    .pSignalSemaphores = &*renderFinishedSemaphores[imageIndex],
	};

	context.getGraphicsQueue().submit(submitInfo, *inFlightFences[frameIndex]);

	vk::PresentInfoKHR presentInfo{
	    .waitSemaphoreCount = 1,
	    .pWaitSemaphores = &*renderFinishedSemaphores[imageIndex],
	    .swapchainCount = 1,
	    .pSwapchains = &*swapChain.getSwapchain(),
	    .pImageIndices = &imageIndex,
	};
	[[maybe_unused]] auto presentResult = context.getPresentQueue().presentKHR(presentInfo);
	frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}

std::vector<vk::raii::Semaphore> Renderer::createRenderFinishedSemaphores() {
	std::vector<vk::raii::Semaphore> result{};
	for (size_t i = 0; i < swapChain.getSwapChainImageCount(); ++i) {
		result.emplace_back(vk::raii::Semaphore{context.getDevice(), vk::SemaphoreCreateInfo{}});
	}
	return result;
}

std::vector<vk::raii::Semaphore> Renderer::createImageAvailableSemaphores() {
	std::vector<vk::raii::Semaphore> result{};
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		result.emplace_back(vk::raii::Semaphore{context.getDevice(), vk::SemaphoreCreateInfo{}});
	}
	return result;
}

std::vector<vk::raii::Fence> Renderer::createInFlightFences() {
	std::vector<vk::raii::Fence> result{};
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		result.emplace_back(vk::raii::Fence{context.getDevice(), vk::FenceCreateInfo{.flags = vk::FenceCreateFlagBits::eSignaled}});
	}
	return result;
}