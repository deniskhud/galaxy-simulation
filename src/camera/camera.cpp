#include "camera.hpp"

Camera::Camera(float aspectRatio) : aspectRatio(aspectRatio) {}

/*CameraUbo Camera::getUbo() const {
    CameraUbo ubo{};
    ubo.view = glm::lookAt(position, target, up);
    ubo.proj = glm::perspective(glm::radians(fovYDegrees), aspectRatio, nearPlane, farPlane);
    ubo.proj[1][1] *= -1.0f; // Vulkan: ось Y в clip space перевёрнута относительно OpenGL
    return ubo;
}*/

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