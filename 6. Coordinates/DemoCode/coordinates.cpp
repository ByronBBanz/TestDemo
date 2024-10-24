// include C++ headers
#include <cstdio>
#include <iostream>
#include <vector>
//using namespace std;	// to avoid having to use std::

// include OpenGL related headers
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <AntTweakBar.h>
#include <glm/glm.hpp>
//using namespace glm;	// to avoid having to use glm::

#include "ShaderProgram.h"

// global variables
// settings
unsigned int gWindowWidth = 800;
unsigned int gWindowHeight = 600;

// scene content
ShaderProgram gShader;
GLuint gVBO = 0;
GLuint gVAO = 0;

// controls
glm::ivec2 gCursor = glm::ivec2(0);
glm::vec3 gPosition = glm::vec3(0.0f);
glm::vec3 gColor = glm::vec3(1.0f, 0.0f, 0.0f);
int gPointSize = 10;
bool gAxes = true;
bool gGrid = true;

// function initialise scene and render settings
static void init(GLFWwindow* window)
{
	// set the color the color buffer should be cleared to
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	// compile and link a vertex and fragment shader pair
	gShader.compileAndLink("coordinates.vert", "coordinates.frag");

	// enable point size
	glEnable(GL_PROGRAM_POINT_SIZE);

	// vertex positions
	std::vector<GLfloat> vertices = 
	{
		0.0f, 0.0f, 0.0f,	// point
		-1.0f, 0.0f, 0.0f,	// x-axis
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,	// y-axis
		0.0f, -1.0f, 0.0f
	};

	// generate grid
	for (float i = 0.1f; i < 1.0f; i += 0.1f)
	{
		vertices.insert(vertices.end(), {
			i, -1.0f, 0.0f,
			i, 1.0f, 0.0f,
			-i, -1.0f, 0.0f,
			-i, 1.0f, 0.0f,
			-1.0f, i, 0.0f,
			1.0f, i, 0.0f,
			-1.0f, -i, 0.0f,
			1.0f, -i, 0.0f,
		});
	}

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

	// render axes
	if (gAxes)
	{
		gShader.setUniform("uPosition", glm::vec3(0.0f));
		gShader.setUniform("uColor", glm::vec3(0.8f));

		glDrawArrays(GL_LINES, 1, 4);
	}

	// render grid
	if (gGrid)
	{
		gShader.setUniform("uPosition", glm::vec3(0.0f));
		gShader.setUniform("uColor", glm::vec3(0.4f));

		glDrawArrays(GL_LINES, 5, 9*8);
	}

	// render point
	gShader.setUniform("uPointSize", gPointSize);
	gShader.setUniform("uPosition", gPosition);
	gShader.setUniform("uColor", gColor);

	glBindVertexArray(gVAO);			// make VAO active
	glDrawArrays(GL_POINTS, 0, 1);		// render the point

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

// cursor movement callback function
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	// pass cursor position to tweak bar
	TwEventMousePosGLFW(static_cast<int>(xpos), static_cast<int>(ypos));

	// update mouse cursor coordinates
	gCursor.x = static_cast<int>(xpos);
	gCursor.y = static_cast<int>(ypos);
}

// mouse button callback function
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	// pass mouse button status to tweak bar
	TwEventMouseButtonGLFW(button, action);
}

// window resize callback function
static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// update window size variables
	gWindowWidth = width;
	gWindowHeight = height;

	// resize viewport
	glViewport(0, 0, gWindowWidth, gWindowHeight);

	// give tweak bar the new graphics window size
	TwWindowSize(gWindowWidth, gWindowHeight);
}

// error callback function
static void error_callback(int error, const char* description)
{
	std::cerr << description << std::endl;	// output error description
}

// create and populate tweak bar elements
TwBar* create_UI(const std::string name)
{
	// create a tweak bar
	TwBar* twBar = TwNewBar(name.c_str());

	// give tweak bar the size of graphics window
	TwWindowSize(gWindowWidth, gWindowHeight);
	TwDefine(" TW_HELP visible=false ");	// disable help menu
	TwDefine(" GLOBAL fontsize=3 ");		// set large font size
	TwDefine(" Main label='User Interface' refresh=0.02 text=light size='220 300' ");

	// display cursor coordinates
	TwAddVarRW(twBar, "Coord: x", TW_TYPE_INT32, &gCursor.x, " group='Cursor' ");
	TwAddVarRW(twBar, "Coord: y", TW_TYPE_INT32, &gCursor.y, " group='Cursor' ");

	// point position
	TwAddVarRW(twBar, "Pos: x", TW_TYPE_FLOAT, &gPosition.x, " group='Position' min=-1.0 max=1.0 step=0.01 ");
	TwAddVarRW(twBar, "Pos: y", TW_TYPE_FLOAT, &gPosition.y, " group='Position' min=-1.0 max=1.0 step=0.01 ");

	// rendering controls
	TwAddVarRW(twBar, "Axes", TW_TYPE_BOOLCPP, &gAxes, " group='Display' ");
	TwAddVarRW(twBar, "Grid", TW_TYPE_BOOLCPP, &gGrid, " group='Display' ");
	TwAddVarRW(twBar, "Point Size", TW_TYPE_INT32, &gPointSize, " group='Display' min=1 max=50 step=1 ");
	TwAddVarRW(twBar, "Point Color", TW_TYPE_COLOR3F, &gColor, " group='Display' opened=true ");

	return twBar;
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
	window = glfwCreateWindow(gWindowWidth, gWindowHeight, "2D Coordinates", nullptr, nullptr);

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
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// use sticky mode to avoid missing state changes from polling
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// initialise scene and render settings
	init(window);

	// initialise AntTweakBar
	TwInit(TW_OPENGL_CORE, nullptr);
	TwBar* tweakBar = create_UI("Main");		// create and populate tweak bar elements

	// the rendering loop
	while (!glfwWindowShouldClose(window))
	{
		render_scene();			// render the scene

		TwDraw();				// draw tweak bar

		glfwSwapBuffers(window);	// swap buffers
		glfwPollEvents();			// poll for events
	}

	// clean up
	glDeleteBuffers(1, &gVBO);
	glDeleteVertexArrays(1, &gVAO);
	
	// delete and uninitialise tweak bar
	TwDeleteBar(tweakBar);
	TwTerminate(); 	

	// close the window and terminate GLFW
	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}
