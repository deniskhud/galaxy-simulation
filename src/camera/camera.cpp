#include "camera.hpp"

Camera::Camera(const VulkanContext& context, float aspectRatio) : aspectRatio(aspectRatio) {
    cameraUboBuffer = std::make_unique<Buffer>(context, sizeof(CameraUbo),
                     vk::BufferUsageFlagBits::eUniformBuffer,
                     vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    uploadUbo();
}

void Camera::uploadUbo() {
    auto uboData = getUbo();
    cameraUboBuffer->upload(&uboData, sizeof(uboData));
}

CameraUbo Camera::getUbo() const {
    CameraUbo ubo{};

    glm::vec3 position;

    position.x =
        target.x +
        distance *
        cos(glm::radians(pitch)) *
        cos(glm::radians(yaw));

    position.y =
        target.y +
        distance *
        sin(glm::radians(pitch));

    position.z =
        target.z +
        distance *
        cos(glm::radians(pitch)) *
        sin(glm::radians(yaw));

    ubo.view =
        glm::lookAt(
            position,
            target,
            glm::vec3(0, 1, 0)
        );

    ubo.proj =
        glm::perspective(
            glm::radians(fovYDegrees),
            aspectRatio,
            nearPlane,
            farPlane
        );

    ubo.proj[1][1] *= -1;

    return ubo;
}

void Camera::rotate(
    float dyaw,
    float dpitch
) {
    yaw += dyaw;

    pitch += dpitch;

    pitch =
        glm::clamp(
            pitch,
            -89.0f,
            89.0f
        );
}

void Camera::zoom(float delta) {
    distance -= delta;

    distance =
        glm::clamp(
            distance,
            3.0f,
            100.0f
        );
}