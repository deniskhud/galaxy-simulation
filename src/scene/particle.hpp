#ifndef GALACTIC_PARTICLE_HPP
#define GALACTIC_PARTICLE_HPP
#include "../core/buffer.hpp"
#include "../core/context.hpp"

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
struct alignas(16) Particle {
	glm::vec4 position;
	glm::vec4 color;
	glm::vec4 orbit;
};

class ParticleSystem {
public:
	ParticleSystem(const VulkanContext& context, std::uint32_t count);

	const Buffer& getSsboBuffer() const { return *ssboBuffer; }
	std::uint32_t getCount() const { return count; }

private:
	static std::vector<Particle> generateGalaxy(std::uint32_t count);

	std::uint32_t count;
	std::unique_ptr<Buffer> ssboBuffer;

	static constexpr float kPi = 3.14159265358979323846f;
};

#endif // GALACTIC_PARTICLE_HPP