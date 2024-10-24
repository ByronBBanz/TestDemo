// include C++ headers
#define _USE_MATH_DEFINES
#include <cmath>
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

// vertex positions
std::vector<GLfloat> gVertices;
#define MAX_SLICES 32					// maximum number of circle slices
#define MIN_SLICES 8					// minimum number of circle slices
unsigned int gSlices = MIN_SLICES;		// number of circle slices
float gScaleFactor = static_cast<float>(gWindowHeight) / gWindowWidth; // controls whether circle or elipse

// generate vertices for a circle based on a radius and number of slices
void generate_circle(const float radius, const unsigned int slices, const float scale_factor, std::vector<GLfloat> &vertices)
{
	float slice_angle = M_PI * 2.0f / slices;	// angle of each slice
	float angle = 0;			// angle used to generate x and y coordinates
	float x, y, z = 0.0f;		// (x, y, z) coordinates

	// generate vertex coordinates for a circle
	for (int i = 0; i < slices + 1; i++)
	{
		x = radius * cos(angle) * scale_factor;
		y = radius * sin(angle);

		vertices.push_back(x);
		vertices.push_back(y);
		vertices.push_back(z);

		// update to next angle
		angle += slice_angle;
	}
}

// function initialise scene and render settings
static void init(GLFWwindow* window)
{
	// set the color the color buffer should be cleared to
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	// compile and link a vertex and fragment shader pair
	gShader.compileAndLink("simple.vert", "simple.frag");

	// generate vertices of a triangle fan
	// initialise centre, i.e. (0.0f, 0.0f, 0.0f)
	gVertices.clear();
	gVertices.push_back(0.0f);	// x
	gVertices.push_back(0.0f);	// y
	gVertices.push_back(0.0f);	// z
	
	// generate circle around the centre
	generate_circle(0.5f, gSlices, gScaleFactor, gVertices);

	// create VBO and buffer the data
	glGenBuffers(1, &gVBO);
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * gVertices.size(), &gVertices[0], GL_DYNAMIC_DRAW);

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
	glDrawArrays(GL_TRIANGLE_FAN, 0, gSlices + 2);	// display the vertices based on the primitive type

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
	else if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		// set polygon render mode to wireframe mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		return;
	}
	else if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		// set polygon render mode to solid mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		return;
	}
	else if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		if (gSlices < MAX_SLICES)
		{
			gSlices++;	// increment number of slices

			// initialise circle centre, i.e. (0.0f, 0.0f, 0.0f)
			gVertices.clear();
			gVertices.push_back(0.0f);	// x
			gVertices.push_back(0.0f);	// y
			gVertices.push_back(0.0f);	// z

			// generate circle around the centre
			generate_circle(0.5f, gSlices, gScaleFactor, gVertices);

			// bind and copy data to buffer
			glBindBuffer(GL_ARRAY_BUFFER, gVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * gVertices.size(), &gVertices[0], GL_DYNAMIC_DRAW);
		}
		return;
	}
	else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		if (gSlices > MIN_SLICES)
		{
			gSlices--;	// decrement number of slices

			// initialise circle centre, i.e. (0.0f, 0.0f, 0.0f)
			gVertices.clear();
			gVertices.push_back(0.0f);	// x
			gVertices.push_back(0.0f);	// y
			gVertices.push_back(0.0f);	// z

			// generate circle around the centre
			generate_circle(0.5f, gSlices, gScaleFactor, gVertices);

			// bind and copy data to buffer
			glBindBuffer(GL_ARRAY_BUFFER, gVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * gVertices.size(), &gVertices[0], GL_DYNAMIC_DRAW);
			return;
		}
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
	window = glfwCreateWindow(gWindowWidth, gWindowHeight, "Draw Circle", nullptr, nullptr);

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