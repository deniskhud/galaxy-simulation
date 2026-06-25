#include "camera.hpp"

void InputState::processEvent(const SDL_Event& e) {

	if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
		e.button.button == SDL_BUTTON_LEFT)
	{
		rotating = true;

		lastX = e.button.x;
		lastY = e.button.y;
	}

	if (e.type == SDL_EVENT_MOUSE_BUTTON_UP &&
		e.button.button == SDL_BUTTON_LEFT)
	{
		rotating = false;
	}

	if (e.type == SDL_EVENT_MOUSE_MOTION &&
		rotating)
	{
		mouseDX += e.motion.x - lastX;
		mouseDY += e.motion.y - lastY;

		lastX = e.motion.x;
		lastY = e.motion.y;
	}

	if (e.type == SDL_EVENT_MOUSE_WHEEL) {
		wheel += e.wheel.y;
	}
}

void InputState::update() {
	mouseDX = 0;
	mouseDY = 0;

	wheel = 0;

	const bool* keys =
		SDL_GetKeyboardState(nullptr);

	forward =
		keys[SDL_SCANCODE_W];

	backward =
		keys[SDL_SCANCODE_S];

	left =
		keys[SDL_SCANCODE_A];

	right =
		keys[SDL_SCANCODE_D];

	up =
		keys[SDL_SCANCODE_SPACE];

	down =
		keys[SDL_SCANCODE_LSHIFT];
}



Camera::Camera(const VulkanContext& context, float aspectRatio) : aspectRatio(aspectRatio) {
	cameraUboBuffer = std::make_unique<Buffer>(
	    context,
	    sizeof(CameraUbo),
	    vk::BufferUsageFlagBits::eUniformBuffer,
	    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
	);

	uploadUbo();
	updateVectors();
}

void Camera::updateCamera(float deltaTime, const InputState &input) {
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

	uploadUbo();
}

void Camera::uploadUbo() {
	auto uboData = getUbo();
	cameraUboBuffer->upload(&uboData, sizeof(uboData));
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