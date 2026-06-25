#ifndef GALACTIC_CAMERA_HPP
#define GALACTIC_CAMERA_HPP
#include "../core/buffer.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include "../window/window.hpp"
struct alignas(16) CameraUbo {
	glm::mat4 view;
	glm::mat4 proj;
};

class InputState final {
public:
	void processEvent(const SDL_Event& event);
	void update();

	bool forward{};
	bool backward{};
	bool left{};
	bool right{};
	bool up{};
	bool down{};

	float mouseDX{};
	float mouseDY{};

	float wheel{};

private:
	bool rotating = false;
	float lastX{};
	float lastY{};
};

class Camera final {
public:
	Camera(const VulkanContext& context, float aspectRatio);

	void updateCamera(float deltaTime, const InputState& input);
	void setAspectRatio(float ar) { aspectRatio = ar; }

	void rotate(float dyaw, float dpitch);
	void zoom(float delta);
	CameraUbo getUbo() const;

	const Buffer& getCameraBuffer() const { return *cameraUboBuffer; }
	void uploadUbo();

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

	std::unique_ptr<Buffer> cameraUboBuffer;

	void updateVectors();
};
#endif // GALACTIC_CAMERA_HPP