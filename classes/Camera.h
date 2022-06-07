#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
	
	public:
		Camera(GLFWwindow* w);
		glm::mat4 getView();
		glm::mat4 getProjection();
		glm::vec3 getPosition();
		void update();
		void moveBackward();
		void moveForward();
		void moveLeft();
		void moveRight();
		void moveUp();
		void moveDown();
		void mouseCallback(double xpos, double ypos);
		void scrollCallback(double xoffset, double yoffset);

	private:
		GLFWwindow* window;

		glm::mat4 view;
		glm::mat4 projection;

		glm::vec3 world_up;
		glm::vec3 pos;
		glm::vec3 up;
		glm::vec3 right;
		glm::vec3 front;
		glm::vec3 direction;

		float speed;
		float pitch;
		float yaw;

		float last_frame;
		float current_frame;
		float delta_time;

		float lastx;
		float lasty;

		float fov;

};

#endif