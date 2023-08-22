#pragma once

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <Constants.hpp>
#include <DirectionalLight.hpp>
#include <PointLight.hpp>
#include <SpotLight.hpp>

class Shader {
public:
	Shader();

	void CreateFromString(const char* vertexCode, const char* fragmentCode);
	void CreateFromFiles(const char* vertexLocation, const char* fragmentLocation);

	std::string ReadFile(const char* fileLocation);

	GLuint GetProjectionLocation();
	GLuint GetModelLocation();
	GLuint GetViewLocation();
	GLuint GetEyePositionLocation();
	GLuint GetSpecularIntensityLocation();
	GLuint GetShininessLocation();
	GLuint GetPointLightCount();

	void SetDirectionalLight(DirectionalLight* directionalLight);
	void SetPointLights(PointLight* pointLight, unsigned int lightCount);
	void SetSpotLights(SpotLight* spotLight, unsigned int lightCount);
	void SetDirectionalShadowMap(GLuint textureUnit);
	void SetTexture(GLuint textureUnit);
	void SetDirectionalLightTransform(glm::mat4* lightTransform);

	void UseShader();
	void ClearShader();

	~Shader();

private:
	int pointLightCount, spotLightCount;

	GLuint shaderID, uniformProjection, uniformModel, uniformView, 
		uniformEyePosition, uniformSpecularIntensity, uniformShininess,
		uniformPointLightCount,
		uniformSpotLightCount,
		uniformDirectionalShadowMap,
		uniformDirectionalLightTransform,
		uniformTexture;

	struct {
		GLuint uniformColor;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;
		GLuint uniformDirection;
	} uniformDirectionalLight;

	struct {
		GLuint uniformColor;
		GLuint uniformAmbientIntensity;;
		GLuint uniformDiffuseIntensity;
		GLuint uniformPosition;
		GLuint uniformConstant;
		GLuint uniformLinear;
		GLuint uniformExponent;
	} uniformPointLight[N_POINT_LIGHTS];

	struct {
		GLuint uniformColor;
		GLuint uniformAmbientIntensity;;
		GLuint uniformDiffuseIntensity;
		GLuint uniformPosition;
		GLuint uniformConstant;
		GLuint uniformLinear;
		GLuint uniformExponent;

		GLuint uniformDirection;
		GLuint uniformEdgeAngle;
	} uniformSpotLight[N_SPOT_LIGHTS];

	void CompileShader(const char* vertexCode, const char* fragmentCode);
	void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType);
};