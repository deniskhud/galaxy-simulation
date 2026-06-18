#include "renderer.hpp"

Renderer::Renderer(const VulkanContext& context, SwapChain& swapChain, const Pipeline& pipeline,
                    const DescriptorPool& descriptors, const ParticleSystem& particles)
    : context(context)
    , swapChain(swapChain)
    , pipeline(pipeline)
    , descriptors(descriptors)
    , particles(particles)
    , commandPool(createCommandPool())
    , commandBuffer(createCommandBuffer())
    , imageAvailableSemaphore(context.getDevice(), vk::SemaphoreCreateInfo{})
    , renderFinishedSemaphore(context.getDevice(), vk::SemaphoreCreateInfo{})
    , inFlightFence(context.getDevice(), vk::FenceCreateInfo{.flags = vk::FenceCreateFlagBits::eSignaled})
{
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

void Renderer::recordCommandBuffer(uint32_t imageIndex) {
    commandBuffer.begin(vk::CommandBufferBeginInfo{
        .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
    });

    vk::Image image = swapChain.getImage(imageIndex);
    vk::ImageView view = swapChain.getImageView(imageIndex);
    vk::Extent2D extent = swapChain.getExtent();

    vk::ImageSubresourceRange colorRange{
        .aspectMask = vk::ImageAspectFlagBits::eColor,
        .baseMipLevel = 0, .levelCount = 1,
        .baseArrayLayer = 0, .layerCount = 1,
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
    commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe,
                                   vk::PipelineStageFlagBits::eColorAttachmentOutput,
                                   {}, {}, {}, toColorAttachment);

    vk::ClearValue clearColor{};
    clearColor.setColor(vk::ClearColorValue{std::array{0.02f, 0.02f, 0.05f, 1.0f}});

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
        .x = 0.0f, .y = 0.0f,
        .width = static_cast<float>(extent.width),
        .height = static_cast<float>(extent.height),
        .minDepth = 0.0f, .maxDepth = 1.0f,
    };
    commandBuffer.setViewport(0, viewport);
    commandBuffer.setScissor(0, vk::Rect2D{.offset = {0, 0}, .extent = extent});

    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                      pipeline.getPipelineLayout(), 0, *descriptors.get(), {});

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
    commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eColorAttachmentOutput,
                                   vk::PipelineStageFlagBits::eBottomOfPipe,
                                   {}, {}, {}, toPresent);

    commandBuffer.end();
}

void Renderer::drawFrame() {
    [[maybe_unused]] auto waitResult =
        context.getDevice().waitForFences(*inFlightFence, VK_TRUE, UINT64_MAX);
    context.getDevice().resetFences(*inFlightFence);

    auto [result, imageIndex] =
        swapChain.getSwapchain().acquireNextImage(UINT64_MAX, *imageAvailableSemaphore, nullptr);

    if (result == vk::Result::eErrorOutOfDateKHR)
    {
        swapChain.recreateSwapChain();
        return;
    }
    commandBuffer.reset();
    recordCommandBuffer(imageIndex);

    vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    vk::SubmitInfo submitInfo{
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &*imageAvailableSemaphore,
        .pWaitDstStageMask = &waitStage,
        .commandBufferCount = 1,
        .pCommandBuffers = &*commandBuffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &*renderFinishedSemaphore,
    };
    context.getGraphicsQueue().submit(submitInfo, *inFlightFence);

    vk::PresentInfoKHR presentInfo{
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &*renderFinishedSemaphore,
        .swapchainCount = 1,
        .pSwapchains = &*swapChain.getSwapchain(),
        .pImageIndices = &imageIndex,
    };
    [[maybe_unused]] auto presentResult = context.getPresentQueue().presentKHR(presentInfo);
}