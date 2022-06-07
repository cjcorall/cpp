#include "PointLight.h"

PointLight::PointLight() {
	this->_position = glm::vec3(0.0f, 1.0f, -1.5f);
	this->_ambient = glm::vec3(0.075f);
	this->_diffuse = glm::vec3(0.05f);
	this->_specular = glm::vec3(1.0f);
	this->_kc = 1.0f;
	this->_kl = 0.09f;
	this->_kq = 0.032f;
}

PointLight::PointLight(glm::vec3 pos, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float kc, float kl, float kq) {
	this->_position = pos;
	this->_ambient = amb;
	this->_diffuse = diff;
	this->_specular = spec;
	this->_kc = kc;
	this->_kl = kl;
	this->_kq = kq;
}

void PointLight::setPosition(glm::vec3 pos) {
	this->_position = pos;
}
void PointLight::setAmbient(glm::vec3 amb) {
	this->_ambient = amb;
}
void PointLight::setDiffuse(glm::vec3 diff) {
	this->_diffuse = diff;
}
void PointLight::setSpecular(glm::vec3 spec) {
	this->_specular = spec;
}

void PointLight::setKC(float kc) {
	this->_kc = kc;
}
void PointLight::setKL(float kl) {
	this->_kl = kl;
}
void PointLight::setKQ(float kq) {
	this->_kq = kq;
}

glm::vec3 PointLight::getPosition() {
	return this->_position;
}
glm::vec3 PointLight::getAmbient() {
	return this->_ambient;
}
glm::vec3 PointLight::getDiffuse() {
	return this->_diffuse;
}
glm::vec3 PointLight::getSpecular() {
	return this->_specular;
}

float PointLight::getKC() {
	return this->_kc;
}
float PointLight::getKL() {
	return this->_kl;
}
float PointLight::getKQ() {
	return this->_kq;
}