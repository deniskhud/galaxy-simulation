#ifndef GALACTIC_INC_HPP
#define GALACTIC_INC_HPP

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif
#include "../debugSystem.hpp"

#endif // GALACTIC_INC_HPP
