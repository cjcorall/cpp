#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "stb_image.h"

#include <string>
#include <iostream>
#include <map>
#include <fstream>
#include <sstream>
#include <vector>

#include "Mesh.h"
#include "Shader.h"

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

class Model {
	public:
		Model(std::string path);
		void draw(Shader& shader);
		bool hasDiffuse();
		bool hasSpecular();
		void bindArrayBuffer();
		void unbindArrayBuffer();
		void setPosition(glm::vec3 pos);
		glm::vec3 getPosition();
		void move(glm::vec3 offset);
		void setColor(glm::vec3 color);
		glm::vec3 getColor();
		void setScale(glm::vec3 scale);
		glm::vec3 getScale();

	private:
		std::vector<Texture> textures_loaded;
		std::vector<Mesh> meshes;
		std::string directory;
		bool has_diffuse;
		bool has_specular;

		void loadModel(std::string path);
		void processNode(aiNode* node, const aiScene* scene);
		Mesh processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
		glm::vec3 _position;
		glm::vec3 _color;
		glm::vec3 _scale;
};

#endif