#include <stdio.h>
#include <string.h>
#include <cmath>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <vector>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <Window.hpp>
#include <Shader.hpp>
#include <Mesh.hpp>
#include <Camera.hpp>


std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Window mainWindow;
Camera camera;

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

static const char* vShader = "shaders/vs.glsl";
static const char* fShader = "shaders/fs.glsl";

void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 12, 12);
	
	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 12, 12);

	meshList.push_back(obj1);
	meshList.push_back(obj2);
}

void CreateShaders() {
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialize();

	CreateObjects();
	CreateShaders();
	camera = Camera(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f), -90.f, 0.f, 5.f, 0.2f);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0;
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat) mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	// Loop until window closed
	while (!mainWindow.getShouldClose())
	{

		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		// Get + Handle user input events
		glfwPollEvents();

		camera.keyControl(mainWindow.getKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		/*if (direction)
		{
			triOffset += triIncrement;
		}
		else {
			triOffset -= triIncrement;
		}

		if (abs(triOffset) >= triMaxOffset)
		{
			direction = !direction;
		}

		curAngle += 0.01f;
		if (curAngle >= 360) {
			curAngle -= 360;
		}

		if (sizeDirection) {
			curSize += 0.0001f;
		}

		else {
			curSize -= 0.0001f;
		}

		if (curSize >= maxSize || curSize <= minSize) {
			sizeDirection = !sizeDirection;
		}*/

		// Clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();

		glm::mat4 model(1.0f);

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
		model = glm::rotate(model, glm::radians(curAngle), glm::vec3(0.f, 1.f, 0.f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		
		meshList[0]->RenderMesh();

		model = glm::mat4(1.f);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -2.5f));
		//model = glm::rotate(model, glm::radians(curAngle), glm::vec3(0.f, 1.f, 0.f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		meshList[1]->RenderMesh();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}