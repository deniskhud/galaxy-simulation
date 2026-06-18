#include "src/window/window.hpp"
#include "src/core/context.hpp"
#include "src/core/swapchain.hpp"
#include "src/core/buffer.hpp"
#include "src/core/descriptors.hpp"
#include "src/pipelines/pipeline.hpp"
#include "src/renderer/renderer.hpp"
#include "src/scene/particle.hpp"
#include "src/camera/camera.hpp"

int main() {
    Window window {};
    VulkanContext context(window);

    SwapChain swapChain(context, window);

    Pipeline pipeline(context, swapChain);

    ParticleSystem particles(context, 1000);

    auto [w, h] = window.getFrameBufferSize();
    Camera camera((float)w / (float)h);
    Buffer cameraUbo(context, sizeof(CameraUbo),
                     vk::BufferUsageFlagBits::eUniformBuffer,
                     vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    auto uboData = camera.getUbo();
    cameraUbo.upload(&uboData, sizeof(uboData));

    DescriptorPool descriptors(context, pipeline,
                               cameraUbo.get(), cameraUbo.getSize(),
                               particles.getSsbo(), particles.getSsboSize());

    Renderer renderer(context, swapChain, pipeline, descriptors, particles);

    SDL_Event e;
    bool running = true;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) running = false;
        }
        renderer.drawFrame();
    }
    context.getDevice().waitIdle();
    return 0;
}