#ifndef GALACTIC_CAMERA_HPP
#define GALACTIC_CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../core/buffer.hpp"
#include <memory>
struct alignas(16) CameraUbo {
    glm::mat4 view;
    glm::mat4 proj;
};

class Camera final {
public:
    Camera(const VulkanContext& context, float aspectRatio);

    void rotate(float dyaw, float dpitch);
    void zoom(float delta);
    CameraUbo getUbo() const;

    const Buffer& getCameraBuffer() const {
        return *cameraUboBuffer;
    }
    void uploadUbo();
private:
    glm::vec3 target{0.0f, 0.0f, 0.0f};

    float distance = 20.0f;

    float yaw = 45.0f;
    float pitch = -45.0f;

    float fovYDegrees = 90.0f;
    float aspectRatio{16.0 / 9.0};
    float nearPlane = 0.1f;
    float farPlane = 100.0f;

    std::unique_ptr<Buffer> cameraUboBuffer;
};
#endif //GALACTIC_CAMERA_HPP