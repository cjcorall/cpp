#include "Model.h"

Model::Model(std::string path) {
	loadModel(path);
}

void Model::draw(Shader& shader) {
	for (unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i].draw(shader);
	}
}

void Model::bindArrayBuffer() {
	for (unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i].bindArrayBuffer();
	}
}

void Model::unbindArrayBuffer() {
	for (unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i].bindArrayBuffer();
	}
}

void Model::loadModel(std::string path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	this->has_diffuse = false;
	this->has_specular = false;

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "Error::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));
	
	processNode(scene->mRootNode, scene);
	this->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	this->setColor(glm::vec3(0.5f, 0.5f, 0.5f));
	this->setScale(glm::vec3(1.0f));
}

void Model::processNode(aiNode* node, const aiScene* scene) {
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;

		glm::vec3 position;
		position.x = mesh->mVertices[i].x;
		position.y = mesh->mVertices[i].y;
		position.z = mesh->mVertices[i].z;
		vertex.position = position;

		glm::vec3 normal;
		normal.x = mesh->mNormals[i].x;
		normal.y = mesh->mNormals[i].y;
		normal.z = mesh->mNormals[i].z;
		vertex.normal = normal;

		if (mesh->mTextureCoords[0]) {

			glm::vec2 tex_coords;
			tex_coords.x = mesh->mTextureCoords[0][i].x;
			tex_coords.y = mesh->mTextureCoords[0][i].y;
			vertex.tex_coords = tex_coords;
		} else {
			vertex.tex_coords = glm::vec2(0.0f, 0.0f);
		}

		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}
	
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuse_maps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		int curr_size = 0;
		textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());
		this->has_diffuse = (bool)(textures.size() > curr_size);
		std::vector<Texture> specular_maps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		curr_size = (int)textures.size();
		textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
		this->has_specular = (bool)(textures.size() > curr_size);
	}
	
	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++) {
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
				textures.push_back(textures_loaded[j]);
				skip = true;
				break; 
			}
		}
		if (!skip) {
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}
	return textures;
}

bool Model::hasDiffuse() {
	return this->has_diffuse;
}

bool Model::hasSpecular() {
	return this->has_specular;
}

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma) {
	std::string filename = directory + '/' + std::string(path);

	unsigned int texture_id;
	glGenTextures(1, &texture_id);

	int width, height, num_components;
	stbi_set_flip_vertically_on_load(1);
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &num_components, 0);
	if (data) {
		GLenum format;
		if (num_components == 1) {
			format = GL_RED;
		} else if (num_components == 3) {
			format = GL_RGB;
		} else if (num_components == 4) {
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);

	} else {
		std::cout << "Texture load failed." << std::endl;
		stbi_image_free(data);
	}

	return texture_id;
}

void Model::setPosition(glm::vec3 pos) {
	this->_position = pos;
}

glm::vec3 Model::getPosition() {
	return this->_position;
}

void Model::move(glm::vec3 offset) {
	glm::vec3 old_pos = this->getPosition();
	glm::vec3 new_pos = old_pos + offset;
	this->setPosition(new_pos);
}

void Model::setColor(glm::vec3 color) {
	this->_color = color;
}

glm::vec3 Model::getColor() {
	return this->_color;
}

void Model::setScale(glm::vec3 scale) {
	this->_scale = scale;
}

glm::vec3 Model::getScale() {
	return this->_scale;
}