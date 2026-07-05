#ifndef GALACTIC_CAMERA_HPP
#define GALACTIC_CAMERA_HPP
#include "../include/inc.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct alignas(16) CameraUbo {
	glm::mat4 view;
	glm::mat4 proj;
};

class Camera final {
public:
	explicit Camera(float aspectRatio);

	void update(float deltaTime, const InputState& input);
	void setAspectRatio(float ar) { aspectRatio = ar; }

	void rotate(float dyaw, float dpitch);
	void zoom(float delta);
	CameraUbo getUbo() const;

private:
	glm::vec3 position{0.0f, 10.0f, 20.0f};

	glm::vec3 front{0.0f, 0.0f, -1.0f};
	glm::vec3 right{1.0f, 0.0f, 0.0f};
	glm::vec3 up{0.0f, 1.0f, 0.0f};

	float yaw = -90.0f;
	float pitch = 0.0f;

	float moveSpeed = 20.0f;
	float mouseSensitivity = 0.1f;

	float fovYDegrees = 90.0f;
	float aspectRatio{16.0 / 9.0};
	float nearPlane = 0.1f;
	float farPlane = 1000.0f;

	void updateVectors();
};
#endif // GALACTIC_CAMERA_HPP