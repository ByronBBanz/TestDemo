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
GLuint gVBO = 0;
GLuint gVAO = 0;

// function initialise scene and render settings
static void init(GLFWwindow* window)
{
	// set the color the color buffer should be cleared to
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	// compile and link a vertex and fragment shader pair
	gShader.compileAndLink("pointsize.vert", "simple.frag");

	// vertex positions
	std::vector<GLfloat> vertices = 
	{
		-0.7f, 0.8f, 0.0f,		// 0: point 1
		-0.5f, 0.6f, 0.0f,		// 1: line 1, vertex 0
		-0.6f, -0.6f, 0.0f,		// 2: line 1, vertex 1
		-0.2f, -0.5f, 0.0f,		// 3: point 2
		0.0f, -0.4f, 0.0f,		// 4: point 3
		0.0f, 0.5f, 0.0f,		// 5: line loop, vertex 0
		0.5f, 0.6f, 0.0f,		// 6: line loop, vertex 1
		0.1f, -0.1f, 0.0f,		// 7: line loop, vertex 2
		0.3f, 0.8f, 0.0f,		// 8: line loop, vertex 3
		0.4f, 0.0f, 0.0f,		// 9: line loop, vertex 4
		0.5f, -0.4f, 0.0f,		// 10: point 4
		-0.4f, -0.8f, 0.0f,		// 11: line 2, vertex 0
		0.8f, -0.1f, 0.0f,		// 12: line 2, vertex 1
	};

	// enable point size
	glEnable(GL_PROGRAM_POINT_SIZE);

	// create VBO and buffer the data
	glGenBuffers(1, &gVBO);
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// create VAO, specify VBO data and format of the data
	glGenVertexArrays(1, &gVAO);
	glBindVertexArray(gVAO);
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);	// specify format of the data

	glEnableVertexAttribArray(0);	// enable vertex attributes
}

// function to render the scene
static void render_scene()
{
	// clear color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	gShader.use();						// use the shaders associated with the shader program

	glBindVertexArray(gVAO);			// make VAO active
	glDrawArrays(GL_POINTS, 0, 1);		// render 1 vertex starting at index 0 as a point
	glDrawArrays(GL_LINES, 1, 2);		// render 2 vertices starting at index 1 as a line
	glDrawArrays(GL_POINTS, 3, 2);		// render 2 vertices starting at index 3 as points
	glDrawArrays(GL_LINE_LOOP, 5, 5);	// render 5 vertices starting at index 5 as a line loop
	glDrawArrays(GL_POINTS, 10, 1);		// render 1 vertex starting at index 10 as a point
	glDrawArrays(GL_LINES, 11, 2);		// render 2 vertices starting at index 11 as a line

//	glDrawArrays(GL_TRIANGLES, 0, 3);	// render 3 vertices starting at index 0 as a triangle
//	glDrawArrays(GL_LINE_STRIP, 5, 5);	// render 5 vertices starting at index 5 as a line strip
//	glDrawArrays(GL_TRIANGLES, 4, 3);	// render 3 vertices starting at index 4 as a triangle

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
	window = glfwCreateWindow(gWindowWidth, gWindowHeight, "Primitives", nullptr, nullptr);

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
	glDeleteBuffers(1, &gVBO);
	glDeleteVertexArrays(1, &gVAO);

	// close the window and terminate GLFW
	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}