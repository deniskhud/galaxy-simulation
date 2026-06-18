#include "particle.hpp"
#include <random>
#include <cmath>

std::vector<Particle> ParticleSystem::generateGalaxy(std::uint32_t count) {
    std::vector<Particle> particles(count);

    std::mt19937 rng(42); // фиксированный seed — картинка детерминирована, удобно дебажить
    std::uniform_real_distribution<float> spreadDist(-0.15f, 0.15f);
    std::uniform_real_distribution<float> radiusDist(0.0f, 1.0f);
    std::uniform_real_distribution<float> heightDist(-1.0f, 1.0f);

    constexpr float armCount = 2.0f;
    constexpr float armTwist = 4.0f;       // насколько туго закручены рукава
    constexpr float galaxyRadius = 5.0f;
    constexpr float diskThickness = 0.15f;

    for (std::uint32_t i = 0; i < count; ++i) {
        float t = radiusDist(rng); // нормализованный радиус 0..1
        float radius = t * galaxyRadius;

        float armOffset = static_cast<float>(i % static_cast<std::uint32_t>(armCount))
                         * (2.0f * kPi / armCount);
        float angle = armOffset + t * armTwist + spreadDist(rng);

        float x = std::cos(angle) * radius;
        float z = std::sin(angle) * radius;
        float y = heightDist(rng) * diskThickness * (1.0f - t); // диск тоньше к краям

        particles[i].position = glm::vec4(x, y, z, 1.0f);

        glm::vec3 coreColor(1.0f, 0.9f, 0.6f);
        glm::vec3 edgeColor(0.4f, 0.6f, 1.0f);
        glm::vec3 color = glm::mix(coreColor, edgeColor, t);
        particles[i].color = glm::vec4(color, 1.0f);
    }

    return particles;
}

ParticleSystem::ParticleSystem(const VulkanContext& context, std::uint32_t count)
    : count(count)
    , ssbo([&] {
          auto data = generateGalaxy(count);
          return Buffer::createDeviceLocal(context, data.data(),
                                            data.size() * sizeof(Particle),
                                            vk::BufferUsageFlagBits::eStorageBuffer);
      }())
{
}