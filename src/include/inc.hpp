#ifndef GALACTIC_INC_HPP
#define GALACTIC_INC_HPP

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif
#include "../debugSystem.hpp"

struct GalaxyParams {
	float galaxyRadius = 20.0f;
	float diskThickness = 0.30f;
	float maxEccentricity = 0.6f;
	int armCount = 2;
	float armTwist = 4.0f;
	float maxOrbitalSpeed = 0.7f;
	float coreRadius = 0.4f;
	int particleCount = 20000;
};

struct GuiDrawParams {
	GalaxyParams galaxyParams{};
	double frameRate{0};
};

#endif // GALACTIC_INC_HPP
