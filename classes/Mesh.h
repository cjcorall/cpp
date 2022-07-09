#ifndef MESH_H
#define MESH_H

#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

#include "Shader.h"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec2 tex_coords;
};

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh {

public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	void draw(Shader& shader);
	void bindArrayBuffer();
	void unbindArrayBuffer();

private:
	unsigned int VAO, VBO, EBO;
	void setupMesh();

};

#endif