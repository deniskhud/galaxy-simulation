#include "camera.hpp"

Camera::Camera(float aspectRatio) : aspectRatio(aspectRatio) {}

CameraUbo Camera::getUbo() const {
    CameraUbo ubo{};
    ubo.view = glm::lookAt(position, target, up);
    ubo.proj = glm::perspective(glm::radians(fovYDegrees), aspectRatio, nearPlane, farPlane);
    ubo.proj[1][1] *= -1.0f; // Vulkan: ось Y в clip space перевёрнута относительно OpenGL
    return ubo;
}