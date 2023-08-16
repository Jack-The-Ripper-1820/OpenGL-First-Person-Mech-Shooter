#include <Light.hpp>

Light::Light()
{
	color = glm::vec3(1.0f, 1.0f, 1.0f);
	ambientIntensity = 1.f;

	direction = glm::vec3(0.f, -1.f, 0.f);
	diffuseIntensity = 0.f;
}

Light::Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat ambientIntensity,
	GLfloat xDir, GLfloat yDir, GLfloat zDir, GLfloat diffuseIntensity)
{
	color = glm::vec3(glm::clamp(red, 0.0f, 1.0f),
		glm::clamp(green, 0.0f, 1.0f),
		glm::clamp(blue, 0.0f, 1.0f));

	this->ambientIntensity = ambientIntensity;

	direction = glm::vec3(xDir, yDir, zDir);
	this->diffuseIntensity = diffuseIntensity;
}

Light::~Light()
{
}

void Light::UseLight(GLfloat ambientIntensityLocation, GLfloat ambientColorLocation,
	GLfloat diffuseIntensityLocation, GLfloat directionLocation)
{
	glUniform3f(ambientColorLocation, color.x, color.y, color.z);
	glUniform1f(ambientIntensityLocation, ambientIntensity);

	glUniform3f(directionLocation, direction.x, direction.y, direction.z);
	glUniform1f(diffuseIntensityLocation, diffuseIntensity);
}
