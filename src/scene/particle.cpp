#include "particle.hpp"

#include <algorithm>
#include <cmath>
#include <random>

ParticleSystem::ParticleSystem(const VulkanContext& context, std::uint32_t count) : count(count) {
	resizeBuffer(context, count);
}

void ParticleSystem::resizeBuffer(const VulkanContext& context, const std::uint32_t newSize) {
	count = newSize;
	ssboBuffer = std::make_unique<Buffer>(
	    context,
	    count * sizeof(Particle),
	    vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst,
	    vk::MemoryPropertyFlagBits::eDeviceLocal
	);
}