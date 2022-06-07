#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class DirectionalLight {

	public:
		DirectionalLight();
		DirectionalLight(glm::vec3 dir, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec);

		void setDirection(glm::vec3 dir);
		void setAmbient(glm::vec3 dir);
		void setDiffuse(glm::vec3 dir);
		void setSpecular(glm::vec3 dir);

		glm::vec3 getDirection();
		glm::vec3 getAmbient();
		glm::vec3 getDiffuse();
		glm::vec3 getSpecular();


	private:

		glm::vec3 _direction;
		glm::vec3 _ambient;
		glm::vec3 _diffuse;
		glm::vec3 _specular;
};


#endif