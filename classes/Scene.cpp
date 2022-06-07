#include "Scene.h"

Scene::Scene() {
	this->render_window = nullptr;
	this->num_models = 0;
	this->num_shaders = 0;
	this->num_dlights = 0;
	this->num_cameras = 0;
	this->active_camera = "";
}

Scene::Scene(GLFWwindow* window) {
	this->render_window = window;
	this->num_models = 0;
	this->num_shaders = 0;
	this->num_dlights = 0;
	this->num_cameras = 0;
	this->active_camera = "";
}

Scene::Scene(const Scene& scene) {
	this->render_window = scene.render_window;
	this->num_models = scene.num_models;
	this->num_shaders = scene.num_shaders;
	this->num_dlights = scene.num_dlights;
	this->num_cameras = scene.num_cameras;
	this->models = scene.models;
	this->shaders = scene.shaders;
	this->cameras = scene.cameras;
	this->dlights = scene.dlights;
	this->active_camera = scene.active_camera;
}

void Scene::updateCameras() {
	auto camera_iter = this->cameras.begin();

	while (camera_iter != this->cameras.end()) {
		camera_iter->second->update();
		++camera_iter;
	}
}

void Scene::prepareShaders() {
	auto shader_iter = this->shaders.begin();

	while (shader_iter != this->shaders.end()) {
		shader_iter->second->use();
		shader_iter->second->setVector("cameraPos", this->getActiveCamera()->getPosition());
		shader_iter->second->setMatrix("mat_view", this->getActiveCamera()->getView());
		shader_iter->second->setMatrix("mat_proj", this->getActiveCamera()->getProjection());
		this->prepareLights(shader_iter->first);
		++shader_iter;
	}
}

void Scene::prepareLights(std::string shader_id) {
	
	Shader* shader = this->getShader(shader_id);
	std::stringstream ss;
	std::string num;
	std::string base_id;

	auto dlight_iter = this->dlights.begin();
	int i = 0;

	while (dlight_iter != this->dlights.end()) {
		ss << i;
		num = ss.str();
		base_id = "dLights[" + num + "].";

		shader->setVector((base_id + "direction").c_str(), dlight_iter->second->getDirection());
		shader->setVector((base_id + "ambient").c_str(), dlight_iter->second->getAmbient());
		shader->setVector((base_id + "diffuse").c_str(), dlight_iter->second->getDiffuse());
		shader->setVector((base_id + "specular").c_str(), dlight_iter->second->getSpecular());

		ss.str("");
		i++;
		++dlight_iter;
	}

	auto plight_iter = this->plights.begin();
	i = 0;

	while (plight_iter != this->plights.end()) {
		ss << i;
		num = ss.str();
		base_id = "pLights[" + num + "].";

		shader->setVector((base_id + "position").c_str(), plight_iter->second->getPosition());
		shader->setVector((base_id + "ambient").c_str(), plight_iter->second->getAmbient());
		shader->setVector((base_id + "diffuse").c_str(), plight_iter->second->getDiffuse());
		shader->setVector((base_id + "specular").c_str(), plight_iter->second->getSpecular());
		shader->setFloat((base_id + "kc").c_str(), plight_iter->second->getKC());
		shader->setFloat((base_id + "kl").c_str(), plight_iter->second->getKL());
		shader->setFloat((base_id + "kq").c_str(), plight_iter->second->getKQ());

		ss.str("");
		i++;
		++plight_iter;
	}
}

void Scene::renderModels() {
	auto model_iter = this->models.begin();
	while (model_iter != this->models.end()) {
		Shader* shader = this->getAssignedShader(model_iter->first);
		shader->use();
		shader->setInt("material.diffuse", 0);
		shader->setInt("material.specular", 1);
		shader->setFloat("material.shininess", 256.0f);
		shader->setVector("output_color", model_iter->second->getColor());
		shader->setInt("has_diffuse", (int)model_iter->second->hasDiffuse());
		shader->setInt("has_specular", (int)model_iter->second->hasSpecular());
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, model_iter->second->getPosition());
		model = glm::scale(model, model_iter->second->getScale());
		shader->setMatrix("mat_model", model);
		model_iter->second->draw(*shader);
		++model_iter;
	}
}
void Scene::renderScene() {
	this->updateCameras();
	this->renderModels();
}

void Scene::addModel(std::string id, std::string path) {
	this->models.insert(std::make_pair(id, new Model(path)));
	this->num_models++;
}
void Scene::addShader(std::string id, const char* vpath, const char* fpath) {
	this->shaders.insert(std::make_pair(id, new Shader(vpath, fpath)));
	this->num_shaders++;
}
void Scene::addCamera(std::string id) {
	this->cameras.insert(std::make_pair(id, new Camera(this->render_window)));
	this->num_cameras++;
	if (this->active_camera.compare("") == 0) {
		this->active_camera = id;
	}
}
void Scene::addDirectionalLight(std::string id, glm::vec3 dir, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec) {
	this->dlights.insert(std::make_pair(id, new DirectionalLight(dir, amb, diff, spec)));
	this->num_dlights++;
}

void Scene::addPointLight(std::string id, glm::vec3 pos, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float kc, float kl, float kq) {
	this->plights.insert(std::make_pair(id, new PointLight(pos, amb, diff, spec, kc, kl, kq)));
	this->num_plights++;
}

Model* Scene::getModel(std::string id) {
	return this->models[id];
}
Shader* Scene::getShader(std::string id) {
	return this->shaders[id];
}
Camera* Scene::getCamera(std::string id) {
	return this->cameras[id];
}
DirectionalLight* Scene::getDirectionalLight(std::string id) {
	return this->dlights[id];
}
PointLight* Scene::getPointLight(std::string id) {
	return this->plights[id];
}

void Scene::assignShader(std::string model_id, std::string shader_id) {
	this->assigned_shaders.insert(std::make_pair(model_id, shader_id));
}

Shader* Scene::getAssignedShader(std::string model_id) {
	return this->shaders[this->assigned_shaders[model_id]];
}

void Scene::setActiveCamera(std::string id) {
	this->active_camera = id;
}

Camera* Scene::getActiveCamera() {
	return this->cameras[this->active_camera];
}

void Scene::clearAll() {
	this->clearShaders();
	this->clearModels();
	this->clearCameras();
	this->clearLights();
}

void Scene::clearShaders() {
	auto shader_iter = this->shaders.begin();

	while (shader_iter != this->shaders.end()) {
		glDeleteProgram(shader_iter->second->get());
		++shader_iter;
	}
	this->shaders.erase(this->shaders.begin(), this->shaders.end());
}

void Scene::clearModels() {
	auto model_iter = this->models.begin();

	while (model_iter != this->models.end()) {
		delete model_iter->second;
		++model_iter;
	}
	this->models.erase(this->models.begin(), this->models.end());
}

void Scene::clearCameras() {
	auto camera_iter = this->cameras.begin();

	while (camera_iter != this->cameras.end()) {
		delete camera_iter->second;
		++camera_iter;
	}
	this->cameras.erase(this->cameras.begin(), this->cameras.end());
}

void Scene::clearLights() {
	auto dlight_iter = this->dlights.begin();

	while (dlight_iter != this->dlights.end()) {
		delete dlight_iter->second;
		++dlight_iter;
	}
	this->dlights.erase(this->dlights.begin(), this->dlights.end());

	auto plight_iter = this->plights.begin();

	while (plight_iter != this->plights.end()) {
		delete plight_iter->second;
		++plight_iter;
	}
	this->plights.erase(this->plights.begin(), this->plights.end());
}