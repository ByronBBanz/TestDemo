// include C++ headers
#include <cstdio>
#include <iostream>
#include <vector>
//using namespace std;	// to avoid having to use std::

// include OpenGL related headers
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

#include "ShaderProgram.h"

// global variables
// settings
unsigned int gWindowWidth = 800;
unsigned int gWindowHeight = 600;

// scene content
ShaderProgram gShader;
std::vector<GLuint> gVBO;
GLuint gVAO = 0;

// function initialise scene and render settings
static void init(GLFWwindow* window)
{
	// set the color the color buffer should be cleared to
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	// compile and link a vertex and fragment shader pair
	gShader.compileAndLink("color.vert", "color.frag");

	std::vector<GLfloat> vertexPositions = {
		-0.8f, -0.3f, 0.0f,	// vertex 0
		-0.2f, -0.3f, 0.0f,	// vertex 1
		-0.5f, 0.3f, 0.0f,	// vertex 2

		0.2f, -0.3f, 0.0f,	// vertex 3
		0.8f, -0.3f, 0.0f,	// vertex 4
		0.5f,  0.3f, 0.0f,	// vertex 5
	};

	std::vector<GLfloat> vertexColors = {
		1.0f, 0.0f, 0.0f,	// vertex 0
		0.0f, 1.0f, 0.0f,	// vertex 1
		0.0f, 0.0f, 1.0f,	// vertex 2

		1.0f, 0.0f, 1.0f,	// vertex 3
		1.0f, 1.0f, 0.0f,	// vertex 4
		0.0f, 1.0f, 1.0f,	// vertex 5
	};

	// create VBO and buffer the data
	gVBO.resize(2, 0);		// allocate space
	glGenBuffers(2, &gVBO[0]);
	// for positions
	glBindBuffer(GL_ARRAY_BUFFER, gVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexPositions.size(), &vertexPositions[0], GL_STATIC_DRAW);
	// for colours
	glBindBuffer(GL_ARRAY_BUFFER, gVBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexColors.size(), &vertexColors[0], GL_STATIC_DRAW);

	// create VAO, specify VBO data and format of the data
	glGenVertexArrays(1, &gVAO);
	glBindVertexArray(gVAO);
	glBindBuffer(GL_ARRAY_BUFFER, gVBO[0]);					// position data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);	// specify format of position data
	glBindBuffer(GL_ARRAY_BUFFER, gVBO[1]);					// colour data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);	// specify format of colour data

	glEnableVertexAttribArray(0);	// enable vertex attributes
	glEnableVertexAttribArray(1);
}

// function to render the scene
static void render_scene()
{
	// clear color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	gShader.use();						// use the shaders associated with the shader program

	glBindVertexArray(gVAO);			// make VAO active
	glDrawArrays(GL_TRIANGLES, 0, 6);	// display the vertices based on the primitive type

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
	window = glfwCreateWindow(gWindowWidth, gWindowHeight, "Vertex Colours", nullptr, nullptr);

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
		render_scene();		// render the scene

		glfwSwapBuffers(window);	// swap buffers
		glfwPollEvents();			// poll for events
	}

	// clean up
	glDeleteBuffers(2, &gVBO[0]);
	glDeleteVertexArrays(1, &gVAO);

	// close the window and terminate GLFW
	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}