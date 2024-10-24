// include C++ headers
#include <cstdio>
#include <iostream>
#include <vector>
//using namespace std;	// to avoid having to use std::

// include OpenGL related headers
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>	// include GLM (ideally should only use the GLM headers that are actually used)
#include <glm/gtx/transform.hpp>
//using namespace glm;	// to avoid having to use glm::

#include "ShaderProgram.h"

// vertex attribute format
struct VertexColor
{
	GLfloat position[3];
	GLfloat color[3];
};

// global variables
// settings
unsigned int gWindowWidth = 800;
unsigned int gWindowHeight = 800;
const float gTranslateSensitivity = 0.5f;
const float gRotateSensitivity = 0.5f;

// frame stats
float gFrameRate = 60.0f;
float gFrameTime = 1 / gFrameRate;

// scene content
ShaderProgram gShader;	// shader program object
GLuint gVBO = 0;		// vertex buffer object identifier
GLuint gVAO = 0;		// vertex array object identifier

std::map<std::string, glm::mat4> gModelMatrix;	// object model matrices

// function initialise scene and render settings
static void init(GLFWwindow* window)
{
	// set the color the color buffer should be cleared to
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	// compile and link a vertex and fragment shader pair
	gShader.compileAndLink("modelTransform.vert", "color.frag");

	// initialise model matrices to the identity matrix
	gModelMatrix["Base"] = glm::mat4(1.0f);
	gModelMatrix["Arm1"] = glm::mat4(1.0f);
	gModelMatrix["Arm2"] = glm::mat4(1.0f);
	gModelMatrix["RefBase"] = glm::mat4(1.0f);
	gModelMatrix["RefArm1"] = glm::mat4(1.0f);
	gModelMatrix["RefArm2"] = glm::mat4(1.0f);

	// vertex positions and colours
	std::vector<GLfloat> vertices = {
		0.0f, 0.2f, 0.0f,		// vertex 0: position
		0.0f, 1.0f, 0.0f,		// vertex 0: colour
		-0.05f, -0.2f, 0.0f,	// vertex 1: position
		0.0f, 0.0f, 1.0f,		// vertex 1: colour
		0.05f, -0.2f, 0.0f,		// vertex 2: position
		0.0f, 1.0f, 0.0f,		// vertex 2: colour
	};

	// create VBO and buffer the data
	glGenBuffers(1, &gVBO);					// generate unused VBO identifier
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);	// bind the VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// create VAO, specify VBO data and format of the data
	glGenVertexArrays(1, &gVAO);			// generate unused VAO identifier
	glBindVertexArray(gVAO);				// create VAO
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);	// bind the VBO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexColor),
		reinterpret_cast<void*>(offsetof(VertexColor, position)));	// specify format of position data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexColor),
		reinterpret_cast<void*>(offsetof(VertexColor, color)));		// specify format of colour data

	glEnableVertexAttribArray(0);	// enable vertex attributes
	glEnableVertexAttribArray(1);
}

// function used to update the scene
static void update_scene(GLFWwindow* window)
{
	// declare variables to transform the object
	static float rotateAngle1 = 0.0f;
	static float rotateAngle2 = 0.0f;
	static float rotateAngle3 = 0.0f;
	static glm::vec3 basePos = glm::vec3(-0.1f, -0.6f, 0.0f);

	// update the variables based on keyboard input
	// update rotation angles
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		rotateAngle1 += gRotateSensitivity * gFrameTime;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		rotateAngle1 -= gRotateSensitivity * gFrameTime;

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		rotateAngle2 += gRotateSensitivity * gFrameTime;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		rotateAngle2 -= gRotateSensitivity * gFrameTime;

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		rotateAngle3 += gRotateSensitivity * gFrameTime;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		rotateAngle3 -= gRotateSensitivity * gFrameTime;

	// update move vector
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		basePos.y += gTranslateSensitivity * gFrameTime;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		basePos.y -= gTranslateSensitivity * gFrameTime;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		basePos.x -= gTranslateSensitivity * gFrameTime;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		basePos.x += gTranslateSensitivity * gFrameTime;

	// update Base's model matrix
	gModelMatrix["Base"] = glm::translate(basePos)
		* glm::rotate(rotateAngle1, glm::vec3(0.0f, 0.0f, 1.0f))
		* glm::translate(glm::vec3(0.0f, 0.2f, 0.0f));

	// update Arm 1's model matrix based on Base's model matrix
	gModelMatrix["Arm1"] = gModelMatrix["Base"]
		* glm::translate(glm::vec3(0.0f, 0.2f, 0.0f))
		* glm::rotate(rotateAngle2, glm::vec3(0.0f, 0.0f, 1.0f))
		* glm::translate(glm::vec3(0.0f, 0.2f, 0.0f));

	// update Arm 2's model matrix based on Arm 1's model matrix
	gModelMatrix["Arm2"] = gModelMatrix["Arm1"]
		* glm::translate(glm::vec3(0.0f, 0.2f, 0.0f))
		* glm::rotate(rotateAngle3, glm::vec3(0.0f, 0.0f, 1.0f))
		* glm::translate(glm::vec3(0.0f, 0.2f, 0.0f));

	// reflections
	glm::mat4 reflectX = glm::scale(glm::vec3(-1.0f, 1.0f, 1.0f));
	gModelMatrix["RefBase"] = reflectX * gModelMatrix["Base"];
	gModelMatrix["RefArm1"] = reflectX * gModelMatrix["Arm1"];
	gModelMatrix["RefArm2"] = reflectX * gModelMatrix["Arm2"];
}

// function to render the scene
static void render_scene()
{
	// clear color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	gShader.use();						// use the shaders associated with the shader program

	glBindVertexArray(gVAO);			// make VAO active

	// Base
	gShader.setUniform("uModelMatrix", gModelMatrix["Base"]);	// set uniform variable
	glDrawArrays(GL_TRIANGLES, 0, 6);							// render vertices

	// Arm 1
	gShader.setUniform("uModelMatrix", gModelMatrix["Arm1"]);	// set uniform variable
	glDrawArrays(GL_TRIANGLES, 0, 6);							// render vertices

	// Arm 2
	gShader.setUniform("uModelMatrix", gModelMatrix["Arm2"]);	// set uniform variable
	glDrawArrays(GL_TRIANGLES, 0, 6);							// render vertices

	// reflection
	gShader.setUniform("uModelMatrix", gModelMatrix["RefBase"]);	// set uniform variable
	glDrawArrays(GL_TRIANGLES, 0, 6);								// render vertices

	gShader.setUniform("uModelMatrix", gModelMatrix["RefArm1"]);	// set uniform variable
	glDrawArrays(GL_TRIANGLES, 0, 6);								// render vertices

	gShader.setUniform("uModelMatrix", gModelMatrix["RefArm2"]);	// set uniform variable
	glDrawArrays(GL_TRIANGLES, 0, 6);								// render vertices

	// flush the graphics pipeline
	glFlush();
}

// key press or release callback function
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// close the window when the ESCAPE key is pressed
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		// set flag to close the window
		glfwSetWindowShouldClose(window, GL_TRUE);
		return;
	}
}

// error callback function
static void error_callback(int error, const char* description)
{
	std::cerr << description << std::endl;	// output error description
}

int main(void)
{
	GLFWwindow* window = nullptr;	// GLFW window handle

	glfwSetErrorCallback(error_callback);	// set GLFW error callback function

	// initialise GLFW
	if (!glfwInit())
	{
		// if failed to initialise GLFW
		exit(EXIT_FAILURE);
	}

	// minimum OpenGL version 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create a window and its OpenGL context
	window = glfwCreateWindow(gWindowWidth, gWindowHeight, "2D Arm", nullptr, nullptr);

	// check if window created successfully
	if (window == nullptr)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);	// set window context as the current context
	glfwSwapInterval(1);			// swap buffer interval

	// initialise GLEW
	if (glewInit() != GLEW_OK)
	{
		// if failed to initialise GLEW
		std::cerr << "GLEW initialisation failed" << std::endl;
		exit(EXIT_FAILURE);
	}

	// set GLFW callback functions
	glfwSetKeyCallback(window, key_callback);

	// initialise scene and render settings
	init(window);

	// timing data
	double lastUpdateTime = glfwGetTime();	// last update time
	double elapsedTime = lastUpdateTime;	// time since last update
	int frameCount = 0;						// number of frames since last update

	// the rendering loop
	while (!glfwWindowShouldClose(window))
	{
		update_scene(window);	// update the scene
		render_scene();			// render the scene

		glfwSwapBuffers(window);	// swap buffers
		glfwPollEvents();			// poll for events

		frameCount++;
		elapsedTime = glfwGetTime() - lastUpdateTime;	// time since last update

		// if elapsed time since last update > 1 second
		if (elapsedTime > 1.0)
		{
			gFrameTime = elapsedTime / frameCount;	// average time per frame
			gFrameRate = 1 / gFrameTime;			// frames per second
			lastUpdateTime = glfwGetTime();			// set last update time to current time
			frameCount = 0;							// reset frame counter
		}
	}

	// clean up
	glDeleteBuffers(1, &gVBO);
	glDeleteVertexArrays(1, &gVAO);

	// close the window and terminate GLFW
	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}