#pragma once

#include <gl\glew.h>
#include <glm/glm.hpp>

class Light {
public:
	Light();
	Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat ambientIntensity,  GLfloat diffuseIntensity);
	~Light();

	virtual void UseLight(GLfloat ambientIntensityLocation, GLfloat ambientColorLocation,
		GLfloat diffuseIntensityLocation);
protected:
	glm::vec3 color;
	GLfloat ambientIntensity;
	GLfloat diffuseIntensity;
};