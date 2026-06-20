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

    ParticleSystem particles(context, 20000);

    auto [w, h] = window.getFrameBufferSize();
    Camera camera(context, static_cast<float>(w) / static_cast<float>(h));

    Buffer computeBuffer(
    context,
    sizeof(int),
    vk::BufferUsageFlagBits::eStorageBuffer |
    vk::BufferUsageFlagBits::eTransferDst,
    vk::MemoryPropertyFlagBits::eDeviceLocal
);

    DescriptorPool descriptors(context, pipeline,
    camera.getCameraBuffer().get(), camera.getCameraBuffer().getSize(),
    particles.getSsbo(), particles.getSsboSize(),
    particles.getSsbo(), particles.getSsboSize());

    Renderer renderer(context, swapChain, pipeline, descriptors, particles);

    SDL_Event e;
    float lastX = 0.0f;
    float lastY = 0.0f;
    bool rotating = false;

    bool running = true;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) running = false;
            if (e.type == SDL_EVENT_WINDOW_RESIZED) {
                window.framebufferResized = true;
            }
            if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
        e.button.button == SDL_BUTTON_LEFT)
            {
                rotating = true;

                lastX = e.button.x;
                lastY = e.button.y;
            }
            if (e.type == SDL_EVENT_MOUSE_BUTTON_UP &&
        e.button.button == SDL_BUTTON_LEFT)
            {
                rotating = false;
            }
            if (
        e.type == SDL_EVENT_MOUSE_MOTION &&
        rotating
    ) {
                float dx =
                    e.motion.x -
                    lastX;

                float dy =
                    e.motion.y -
                    lastY;

                camera.rotate(dx, dy);

                lastX =
                    e.motion.x;

                lastY =
                    e.motion.y;
    }

            if (e.type == SDL_EVENT_MOUSE_WHEEL) {
                camera.zoom(
                    e.wheel.y
                );
            }
        }
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