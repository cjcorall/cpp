#include "DirectionalLight.h"

DirectionalLight::DirectionalLight() {
	this->_direction = glm::vec3(0.1f, -1.0f, -1.5f);
	this->_ambient = glm::vec3(0.075f);
	this->_diffuse = glm::vec3(0.05f);
	this->_specular = glm::vec3(1.0f);
}

DirectionalLight::DirectionalLight(glm::vec3 dir, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec) {
	this->_direction = dir;
	this->_ambient = amb;
	this->_diffuse = diff;
	this->_specular = spec;
}

void DirectionalLight::setDirection(glm::vec3 dir) {
	this->_direction = dir;
}
void DirectionalLight::setAmbient(glm::vec3 amb) {
	this->_ambient = amb;
}
void DirectionalLight::setDiffuse(glm::vec3 diff) {
	this->_diffuse = diff;
}
void DirectionalLight::setSpecular(glm::vec3 spec) {
	this->_specular = spec;
}

glm::vec3 DirectionalLight::getDirection() {
	return this->_direction;
}
glm::vec3 DirectionalLight::getAmbient() {
	return this->_ambient;
}
glm::vec3 DirectionalLight::getDiffuse() {
	return this->_diffuse;
}
glm::vec3 DirectionalLight::getSpecular() {
	return this->_specular;
}