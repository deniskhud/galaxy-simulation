#include "camera.hpp"

Camera::Camera(float aspectRatio) : aspectRatio(aspectRatio) {
	updateVectors();
}

void Camera::update(float deltaTime, const InputState& input) {
	float velocity = moveSpeed * deltaTime;

	if (input.forward)
		position += front * velocity;

	if (input.backward)
		position -= front * velocity;

	if (input.left)
		position -= right * velocity;

	if (input.right)
		position += right * velocity;

	if (input.up)
		position += up * velocity;

	if (input.down)
		position -= up * velocity;

	rotate(input.mouseDX, input.mouseDY);
	zoom(input.wheel);
}

CameraUbo Camera::getUbo() const {
	CameraUbo ubo{};
	ubo.view =
		glm::lookAt(
			position,
			position + front,
			up
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

void Camera::rotate(float dyaw, float dpitch) {
	yaw += dyaw * mouseSensitivity;
	pitch -= dpitch * mouseSensitivity;
	pitch = glm::clamp(pitch, -89.0f, 89.0f);

	updateVectors();
}

void Camera::updateVectors() {
	front.x = static_cast<float>(cos(glm::radians(yaw)) * cos(glm::radians(pitch)));
	front.y = static_cast<float>(sin(glm::radians(pitch)));
	front.z = static_cast<float>(sin(glm::radians(yaw)) * cos(glm::radians(pitch)));

	front = glm::normalize(front);
	right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
	up = glm::normalize(glm::cross(right, front));
}

void Camera::zoom(float delta) {
	//distance -= delta;
	///distance = glm::clamp(distance, 3.0f, 200.0f);
}