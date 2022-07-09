#ifndef SHADER_H
#define SHADER_H

#include <sstream>
#include <fstream>
#include <string>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Shader {
	public:
		Shader(const char* vertex_path, const char* fragment_path, const char* geometry_path = nullptr);
		unsigned int get();
		void use();
		void setInt(const char* id, int i);
		void setFloat(const char* id, float f);
		void setVector(const char* id, glm::vec3 v);
		void setMatrix(const char* id, glm::mat4 m);

	private:
		const char* vertex_source;
		const char* fragment_source;
		const char* geometry_source;
		unsigned int vertex_shader;
		unsigned int fragment_shader;
		unsigned int geometry_shader;
		unsigned int shader_program;

		void loadShaders(const char* vertex_path, const char* fragment_path, const char* geometry_path = nullptr);
		void linkShaders();
};

#endif
