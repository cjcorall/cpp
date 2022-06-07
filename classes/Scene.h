#ifndef SCENE_H
#define SCENE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Model.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Shader.h"
#include "Camera.h"

#include <vector>
#include <string>
#include <map>
#include <utility>

class Scene {

	public:

		Scene();
		Scene(GLFWwindow* window);
		Scene(const Scene &scene);

		void prepareShaders();
		void prepareLights(std::string shader_id);
		void renderModels();
		void renderScene();

		void addModel(std::string id, std::string path);
		void addShader(std::string id, const char* vpath, const char* fpath);
		void addCamera(std::string id);
		void addDirectionalLight(std::string id, glm::vec3 dir, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec);
		void addPointLight(std::string id, glm::vec3 pos, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float kc, float kl, float kq);

		Model* getModel(std::string id);
		Shader* getShader(std::string id);
		Camera* getCamera(std::string id);
		DirectionalLight* getDirectionalLight(std::string id);
		PointLight* getPointLight(std::string id);

		void assignShader(std::string model_id, std::string shader_id);
		Shader* getAssignedShader(std::string model_id);

		void setActiveCamera(std::string id);
		Camera* getActiveCamera();
		void updateCameras();

		void clearAll();
		void clearShaders();
		void clearModels();
		void clearCameras();
		void clearLights();

	private:

		std::map<std::string, Model*> models;
		std::map<std::string, Shader*> shaders;
		std::map<std::string, Camera*> cameras;
		std::map<std::string, DirectionalLight*> dlights;
		std::map<std::string, PointLight*> plights;

		std::map<std::string, std::string> assigned_shaders;

		unsigned int num_models;
		unsigned int num_shaders;
		unsigned int num_dlights;
		unsigned int num_plights;
		unsigned int num_cameras;

		GLFWwindow* render_window;
		std::string active_camera;
		

};

#endif