#include "Shader.h"


Shader::Shader(const char* vertex_path, const char* fragment_path, const char* geometry_path) {
	this->loadShaders(vertex_path, fragment_path, geometry_path);
	this->linkShaders();
}

void Shader::loadShaders(const char* vertex_path, const char* fragment_path, const char* geometry_path) {
	std::string line;
	std::stringstream ss;
	std::ifstream vfile(vertex_path);
	while (std::getline(vfile, line)) {
		ss << line << "\n";
	}
	std::string vsource = ss.str();
	this->vertex_source = vsource.c_str();
	this->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(this->vertex_shader, 1, &(this->vertex_source), NULL);
	glCompileShader(this->vertex_shader);

	ss.str("");
	std::ifstream ffile(fragment_path);
	while (std::getline(ffile, line)) {
		ss << line << "\n";
	}
	std::string fsource = ss.str();
	this->fragment_source = fsource.c_str();
	this->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(this->fragment_shader, 1, &(this->fragment_source), NULL);
	glCompileShader(this->fragment_shader);

	if (geometry_path != nullptr) {

		ss.str("");
		std::ifstream gfile(geometry_path);
		while (std::getline(gfile, line)) {
			ss << line << "\n";
		}
		std::string gsource = ss.str();

		this->geometry_source = gsource.c_str();
		this->geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(this->geometry_shader, 1, &(this->geometry_source), NULL);
		glCompileShader(this->geometry_shader);
	}

	int success;
	char infoLog[512];
	glGetShaderiv(this->vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(this->vertex_shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// fragment shader
	// check for shader compile errors
	glGetShaderiv(this->fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(this->fragment_shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	if (geometry_path != nullptr) {
		// geometry shader
		// check for shader compile errors
		glGetShaderiv(this->geometry_shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(this->geometry_shader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
	}
}

void Shader::linkShaders() {
	this->shader_program = glCreateProgram();
	glAttachShader(this->shader_program, this->vertex_shader);
	if (this->geometry_shader) {
		glAttachShader(this->shader_program, this->geometry_shader);
	}
	glAttachShader(this->shader_program, this->fragment_shader);

	glLinkProgram(this->shader_program);
	// check for linking errors
	int success;
	char infoLog[512];
	glGetProgramiv(this->shader_program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(this->shader_program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(this->vertex_shader);
	glDeleteShader(this->fragment_shader);
	if (this->geometry_shader) {
		glDeleteShader(this->geometry_shader);
	}
}

unsigned int Shader::get() {
	return this->shader_program;
}

void Shader::use() {
	glUseProgram(this->shader_program);
}

void Shader::setInt(const char* id, int i) {
	glUniform1i(glGetUniformLocation(this->shader_program, id), i);
}

void Shader::setFloat(const char* id, float f) {
	glUniform1f(glGetUniformLocation(this->shader_program, id), f);
}

void Shader::setVector(const char* id, glm::vec3 v) {
	glUniform3fv(glGetUniformLocation(this->shader_program, id), 1, value_ptr(v));
}

void Shader::setMatrix(const char* id, glm::mat4 m) {
	glUniformMatrix4fv(glGetUniformLocation(this->shader_program, id), 1, GL_FALSE, glm::value_ptr(m));
}