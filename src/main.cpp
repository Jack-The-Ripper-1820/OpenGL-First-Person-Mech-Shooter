#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <vector>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <assimp/Importer.hpp>

#include <Window.hpp>
#include <Shader.hpp>
#include <Mesh.hpp>
#include <Camera.hpp>
#include <Texture.hpp>
#include <Light.hpp>
#include <Utils.hpp>
#include <Material.hpp>
#include <Constants.hpp>
#include <Model.hpp>

std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Shader directionalShadowShader;
Window mainWindow;
Camera camera;

Material glossyMaterial, matteMaterial;

Model mech, bugatti, xwingPlayer, xwing;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;

DirectionalLight mainLight;
PointLight pointLights[N_POINT_LIGHTS];
SpotLight spotLights[N_SPOT_LIGHTS];

GLfloat deltaTime = 0.f;
GLfloat lastTime = 0.f;

bool direction = true;
float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncrement = 0.0005f;
float curAngle = 0.f;

bool sizeDirection = true;

float curSize = 0.4f;
float maxSize = 0.8f;
float minSize = 0.1f;

static const char* vShader = "shaders/vertex.glsl";
static const char* fShader = "shaders/fragment.glsl";

unsigned int pointLightCount = 0;
unsigned int spotLightCount = 0;

GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
uniformSpecularIntensity = 0, uniformShininess = 0,
uniformDirectionalLightTransform = 0;

void CreateObjects() {
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
	//  x      y      z         u      v       nx,    ny,   nz
		-1.0f, -1.0f, -0.6f,    0.0f, 0.0f,    0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 1.0f,     0.5f, 0.f,	  0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -0.6f,     1.0f, 0.0f,	  0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,      0.5f, 1.0f,	  0.0f, 0.0f, 0.0f,
	};

	unsigned int floorIndices[] = {
		0, 2, 1, 
		1, 2, 3
	};

	GLfloat floorVerices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};


	Utils::calcAverageNormal(indices, 12, vertices, 32, 8, 5);

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	
	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVerices, floorIndices, 32, 6);

	meshList.push_back(obj1);
	meshList.push_back(obj2);
	meshList.push_back(obj3);
}

void CreateShaders() {
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	directionalShadowShader = Shader();
	directionalShadowShader.CreateFromFiles("shaders/directional_shadow_map_vertex.glsl", "shaders/directional_shadow_map_fragment.glsl");
}

void RenderScene() {
	glm::mat4 model(1.0f);

	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	brickTexture.UseTexture();
	glossyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[0]->RenderMesh();

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 4.0f, -2.5f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	dirtTexture.UseTexture();
	matteMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[1]->RenderMesh();

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	dirtTexture.UseTexture();
	glossyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-7.0f, 0.0f, 10.0f));
	model = glm::scale(model, glm::vec3(0.006f, 0.006f, 0.006f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	matteMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	xwing.RenderModel();
}

void DirectionalShadowMapPass(DirectionalLight* light) {
	directionalShadowShader.UseShader();

	glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());

	light->GetShadowMap()->Write();
	glClear(GL_DEPTH_BUFFER_BIT);

	uniformModel = directionalShadowShader.GetModelLocation();
	glm::mat4 lightTransform = light->CalcLightTransform();
	directionalShadowShader.SetDirectionalLightTransform(&lightTransform);

	RenderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPass(glm::mat4 viewMatrix, glm::mat4 projectionMatrix) {
	shaderList[0].UseShader();

	uniformModel = shaderList[0].GetModelLocation();
	uniformProjection = shaderList[0].GetProjectionLocation();
	uniformView = shaderList[0].GetViewLocation();
	uniformModel = shaderList[0].GetModelLocation();
	uniformEyePosition = shaderList[0].GetEyePositionLocation();
	uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
	uniformShininess = shaderList[0].GetShininessLocation();

	glViewport(0, 0, 1366, 768);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

	shaderList[0].SetDirectionalLight(&mainLight);
	shaderList[0].SetPointLights(pointLights, pointLightCount);
	shaderList[0].SetSpotLights(spotLights, spotLightCount);
	glm::mat4 mainLightTransform = mainLight.CalcLightTransform();
	shaderList[0].SetDirectionalLightTransform(&mainLightTransform);

	mainLight.GetShadowMap()->Read(GL_TEXTURE1);
	shaderList[0].SetTexture(0);
	shaderList[0].SetDirectionalShadowMap(1);

	glm::vec3 lowerLight = camera.getCameraPosition();
	lowerLight.y -= 0.3f;
	//spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

	RenderScene();
}


int main() {
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialize();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.5f);

	brickTexture = Texture("textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("textures/plain.png");
	plainTexture.LoadTextureA();

	glossyMaterial = Material(4.0f, 256);
	matteMaterial = Material(0.3f, 4);

	xwing = Model();
	xwing.LoadModel("models/x-wing.obj");

	mainLight = DirectionalLight(
		1.0f, 1.0f, 1.0f,
		0.1f, 0.3f,
		0.0f, -15.0f, -10.0f,
		2048, 2048);

	std::cout << "directiona" << std::endl;

	pointLights[0] = PointLight(0.0f, 0.0f, 1.0f,
		0.0f, 0.1f,
		0.0f, 0.0f, 0.0f,
		0.3f, 0.2f, 0.1f);

	std::cout << "pointlight" << std::endl;

	pointLightCount++;
	pointLights[1] = PointLight(0.0f, 1.0f, 0.0f,
		0.0f, 0.1f,
		-4.0f, 2.0f, 0.0f,
		0.3f, 0.1f, 0.1f);
	pointLightCount++;

	std::cout << "pointlight" << std::endl;

	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;
	spotLights[1] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, -1.5f, 0.0f,
		-100.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	// Loop until window closed
	while (!mainWindow.getShouldClose()) {
		GLfloat now = glfwGetTime(); 
		deltaTime = now - lastTime; 
		lastTime = now;

		// Get + Handle User Input
		glfwPollEvents();

		camera.keyControl(mainWindow.getKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		DirectionalShadowMapPass(&mainLight);
		RenderPass(camera.calculateViewMatrix(), projection);

		mainWindow.swapBuffers();
	}

	return 0;
}