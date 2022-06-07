#include "Camera.h"

Camera::Camera(GLFWwindow* w) {
	this->window = w;

	this->world_up = glm::vec3(0.0f, 1.0f, 0.0f);
	this->view = glm::mat4(1.0f);
	this->projection = glm::mat4(1.0f);
	
	this->pos = glm::vec3(0.0f, 1.0f, 3.0f);
	this->front = glm::vec3(0.0f, 0.0f, -1.0f);

	this->right = glm::normalize(glm::cross(this->world_up, this->front));
	this->up = glm::normalize(glm::cross(this->front, this->right));

	this->speed = 2.5f;
	this->pitch = 0.0f;
	this->yaw = -90.0f;

	this->direction.x = 0.0f;
	this->direction.y = 0.0f;
	this->direction.z = 0.0f;

	this->last_frame = 0.0f;
	this->current_frame = 0.0f;
	this->delta_time = 0.0f;

	this->lastx = 400.0f;
	this->lasty = 300.0f;

	this->fov = 45.0f;
}

glm::mat4 Camera::getView() {
	return this->view;
}

glm::mat4 Camera::getProjection() {
	return this->projection;
}

glm::vec3 Camera::getPosition() {
	return this->pos;
}

void Camera::update() {
	this->current_frame = glfwGetTime();
	this->delta_time = this->current_frame - this->last_frame;
	this->last_frame = this->current_frame;
	this->speed = 5.0f * this->delta_time;
	this->projection = glm::perspective(glm::radians(this->fov), 800.0f / 600.0f, 0.1f, 100.f);
	this->view = glm::lookAt(this->pos, this->pos + this->front, this->up);
}

void Camera::moveForward() {
	this->pos += this->speed * this->front;
}

void Camera::moveBackward() {
	this->pos -= this->speed * this->front;
}

void Camera::moveLeft() {
	this->pos -= this->speed * glm::normalize(glm::cross(this->front, this->up));
}

void Camera::moveRight() {
	this->pos += this->speed * glm::normalize(glm::cross(this->front, this->up));
}

void Camera::moveUp() {
	this->pos += this->speed * this->up;
}

void Camera::moveDown() {
	this->pos -= this->speed * this->up;
}

void Camera::mouseCallback(double xpos, double ypos) {
	float xoffset = xpos - lastx;
	float yoffset = ypos - lasty;
	this->lastx = xpos;
	this->lasty = ypos;

	const float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	this->yaw += xoffset;
	this->pitch -= yoffset;

	if (this->pitch > 89.0f) {
		this->pitch = 89.0f;
	} else if (pitch < -89.0f) {
		this->pitch = -89.0f;
	}

	this->direction.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
	this->direction.y = sin(glm::radians(this->pitch));
	this->direction.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
	this->front = glm::normalize(this->direction);
}

void Camera::scrollCallback(double xoffset, double yoffset) {
	this->fov -= (float)yoffset*2;
	if (this->fov < 1.0f) {
		this->fov = 1.0f;
	}
	if (this->fov > 45.0f) {
		this->fov = 45.0f;
	}
}