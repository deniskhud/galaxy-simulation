#include "particle.hpp"

#include <algorithm>
#include <cmath>
#include <random>

glm::vec3 blackbodyColor(float kelvin) {
	float temp = kelvin / 100.0f;
	float r, g, b;

	// Red
	if (temp <= 66.0f) {
		r = 255.0f;
	} else {
		r = 329.698727446f * std::pow(temp - 60.0f, -0.1332047592f);
	}

	// Green
	if (temp <= 66.0f) {
		g = 99.4708025861f * std::log(temp) - 161.1195681661f;
	} else {
		g = 288.1221695283f * std::pow(temp - 60.0f, -0.0755148492f);
	}

	// Blue
	if (temp >= 66.0f) {
		b = 255.0f;
	} else if (temp <= 19.0f) {
		b = 0.0f;
	} else {
		b = 138.5177312231f * std::log(temp - 10.0f) - 305.0447927307f;
	}

	return glm::clamp(glm::vec3(r, g, b) / 255.0f, glm::vec3(0.0f), glm::vec3(1.0f));
}

std::vector<Particle> ParticleSystem::generateGalaxy(std::uint32_t count) {
	std::vector<Particle> particles(count);
	std::mt19937 rng(42);
	std::uniform_real_distribution<float> spreadDist(-0.15f, 0.15f);
	std::uniform_real_distribution<float> radiusDist(0.0f, 1.0f);
	std::uniform_real_distribution<float> heightDist(-1.0f, 1.0f);
	std::uniform_real_distribution<float> anomalyDist(0.0f, 2.0f * kPi);
	std::uniform_real_distribution<float> tempDist(3000.0f, 9000.0f);

	constexpr float armCount = 2.0f;
	constexpr float armTwist = 4.0f;
	constexpr float galaxyRadius = 20.0f;
	constexpr float diskThickness = 0.30f;
	constexpr float maxEccentricity = 0.6f; // насколько вытянут эллипс в середине диска

	for (std::uint32_t i = 0; i < count; ++i) {
		float t = radiusDist(rng);
		// float a = t * galaxyRadius;
		float a = -std::log(1.0f - t) * galaxyRadius * 0.5f;
		float rNorm = std::clamp(a / galaxyRadius, 0.0f, 1.0f);
		float armOffset = static_cast<float>(i % static_cast<std::uint32_t>(armCount)) * (2.0f * kPi / armCount);
		float tilt = armOffset + rNorm * armTwist + spreadDist(rng);

		// округлый core (e≈0) -> вытянутые эллипсы в середине диска -> снова почти
		// круг к краю
		float eccentricity = maxEccentricity * std::sin(kPi * std::clamp(rNorm, 0.0f, 1.0f));

		float anomaly = anomalyDist(rng); // случайная фаза — звезда где-то на своём эллипсе

		float b = a * std::sqrt(std::max(1.0f - eccentricity * eccentricity, 0.0f));
		float c = a * eccentricity; // фокус эллипса = центр галактики (1-й закон Кеплера)

		float xLocal = a * std::cos(anomaly) - c;
		float zLocal = b * std::sin(anomaly);
		float x = xLocal * std::cos(tilt) - zLocal * std::sin(tilt);
		float z = xLocal * std::sin(tilt) + zLocal * std::cos(tilt);
		float y = heightDist(rng) * diskThickness * (1.0f - t);

		particles[i].position = glm::vec4(x, y, z, 1.0f);
		particles[i].orbit = glm::vec4(a, eccentricity, tilt, anomaly);

		float temperature = tempDist(rng);
		glm::vec3 rgb = blackbodyColor(temperature);
		float luminosity = std::pow(temperature / 9000.0f, 4.0f); // Stefan-Boltzmann, нормировано
		particles[i].color = glm::vec4(rgb, luminosity);

		/*glm::vec3 coreColor(1.0f, 0.9f, 0.6f);
		glm::vec3 edgeColor(0.4f, 0.6f, 1.0f);
		glm::vec3 color = glm::mix(coreColor, edgeColor, t);
		particles[i].color = glm::vec4(color, 1.0f);*/
	}
	return particles;
}

ParticleSystem::ParticleSystem(const VulkanContext& context, std::uint32_t count) : count(count) {
	auto data = generateGalaxy(count);
	ssboBuffer = std::make_unique<Buffer>(Buffer::createDeviceLocal(
	    context, data.data(), data.size() * sizeof(Particle), vk::BufferUsageFlagBits::eStorageBuffer
	));
}