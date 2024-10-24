// include C++ headers
#include <cstdio>
#include <iostream>
#include <vector>
//using namespace std;	// to avoid having to use std::

// include OpenGL related headers
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
//using namespace glm;	// to avoid having to use glm::

#include "ShaderProgram.h"

#define ANIMATION_SPEED 0.05f

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

// scene content
ShaderProgram gShader;	// shader program object
GLuint gVBO = 0;		// vertex buffer object identifier
GLuint gVAO = 0;		// vertex array object identifier

glm::mat4 gModelMatrix;	// object's model matrix

// function initialise scene and render settings
static void init(GLFWwindow* window)
{
	// set the color the color buffer should be cleared to
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	// compile and link a vertex and fragment shader pair
	gShader.compileAndLink("modelTransform.vert", "color.frag");

	// initialise model matrix to the identity matrix
	gModelMatrix = glm::mat4(1.0f);

	//	gModelMatrix *= glm::translate(glm::vec3(0.3f, 0.0f, 0.0f));
	//	gModelMatrix *= glm::rotate(glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	//	gModelMatrix *= glm::scale(glm::vec3(2.0f, 2.0f, 1.0f));
	//	gModelMatrix *= glm::translate(glm::vec3(0.3f, 0.0f, 0.0f));

	/*
		gModelMatrix *= glm::translate(glm::vec3(0.3f, 0.0f, 0.0f))
			* glm::rotate(glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::scale(glm::vec3(2.0f, 2.0f, 1.0f));
	*/

	// vertex positions and colours
	std::vector<GLfloat> vertices = {
		// triangle 1
		-0.2f, 0.2f, 0.0f,	// vertex 0: position
		1.0f, 0.0f, 0.0f,	// vertex 0: colour
		-0.2f, -0.2f, 0.0f,	// vertex 1: position
		1.0f, 0.0f, 0.0f,	// vertex 1: colour
		0.2f, 0.2f, 0.0f,	// vertex 2: position
		1.0f, 0.0f, 0.0f,	// vertex 2: colour

		// triangle 2
		0.2f, 0.2f, 0.0f,	// vertex 0: position
		0.0f, 1.0f, 0.0f,	// vertex 0: colour
		-0.2f, -0.2f, 0.0f,	// vertex 1: position
		0.0f, 1.0f, 0.0f,	// vertex 1: colour
		0.2f, -0.2f, 0.0f,	// vertex 2: position
		0.0f, 1.0f, 0.0f,	// vertex 2: colour
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
static void update_scene()
{
	static float angle = 0.0f;		// will be used for sine and cosine functions
	angle += ANIMATION_SPEED;	// update angle

	gModelMatrix = glm::translate(glm::vec3(sinf(angle) * 0.5f, 0.0f, 0.0f));
//	gModelMatrix = glm::rotate(angle, glm::vec3(0.0f, 0.0f, 1.0f));
//	gModelMatrix = glm::scale(glm::vec3(cosf(angle) + 1.0, cosf(angle) + 1.0f, 1.0f));

//	gModelMatrix = glm::translate(glm::vec3(sinf(angle)*0.5f, 0.0f, 0.0f));
//	gModelMatrix *= glm::rotate(angle, glm::vec3(0.0f, 0.0f, 1.0f));

/*
	gModelMatrix = glm::translate(glm::vec3(sinf(angle)*0.5f, cosf(angle)*0.5f, 0.0f))
		* glm::rotate(angle, glm::vec3(0.0f, 0.0f, 1.0f))
		* glm::scale(vec3(cosf(angle) + 1.0f, cosf(angle) + 1.0f, 1.0f));
*/
}

// function to render the scene
static void render_scene()
{
	// clear color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	gShader.use();						// use the shaders associated with the shader program

	// set uniform modelling transformation matrix
	gShader.setUniform("uModelMatrix", gModelMatrix);

	glBindVertexArray(gVAO);			// make VAO active
	glDrawArrays(GL_TRIANGLES, 0, 6);	// render vertices based on primitive type

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
	window = glfwCreateWindow(gWindowWidth, gWindowHeight, "2D Transformation", nullptr, nullptr);

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

	// the rendering loop
	while (!glfwWindowShouldClose(window))
	{
		//update_scene();		// update the scene
		render_scene();		// render the scene

		glfwSwapBuffers(window);	// swap buffers
		glfwPollEvents();			// poll for events
	}

	// clean up
	glDeleteBuffers(1, &gVBO);
	glDeleteVertexArrays(1, &gVAO);

	// close the window and terminate GLFW
	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}