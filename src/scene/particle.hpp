#ifndef GALACTIC_PARTICLE_HPP
#define GALACTIC_PARTICLE_HPP
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include "../core/context.hpp"
#include "../core/buffer.hpp"
struct alignas(16) Particle {
    glm::vec4 position;
    glm::vec4 color;
};

class ParticleSystem {
public:
    ParticleSystem(const VulkanContext& context, std::uint32_t count);

    vk::Buffer getSsbo() const { return ssbo.get(); }
    vk::DeviceSize getSsboSize() const { return ssbo.getSize(); }
    std::uint32_t getCount() const { return count; }

private:
    static std::vector<Particle> generateGalaxy(std::uint32_t count);

    std::uint32_t count;
    Buffer ssbo;

    static constexpr float kPi = 3.14159265358979323846f;


};

#endif //GALACTIC_PARTICLE_HPP
