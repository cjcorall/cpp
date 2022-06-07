#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class PointLight {

	public:

		PointLight();
		PointLight(glm::vec3 pos, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float kc, float kl, float kq);

		void setPosition(glm::vec3 dir);
		void setAmbient(glm::vec3 dir);
		void setDiffuse(glm::vec3 dir);
		void setSpecular(glm::vec3 dir);

		void setKC(float kc);
		void setKL(float kl);
		void setKQ(float kq);

		glm::vec3 getPosition();
		glm::vec3 getAmbient();
		glm::vec3 getDiffuse();
		glm::vec3 getSpecular();

		float getKC();
		float getKL();
		float getKQ();

	private:

		glm::vec3 _position;
		glm::vec3 _ambient;
		glm::vec3 _diffuse;
		glm::vec3 _specular;

		float _kc;
		float _kl;
		float _kq;

};

#endif