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
unsigned int gWindowHeight = 600;

// scene content
ShaderProgram gShader;	// shader program object
GLuint gVBO = 0;		// vertex buffer object identifier
GLuint gIBO = 0;		// index buffer object identifier
GLuint gVAO = 0;		// vertex array object identifier

glm::mat4 gModelMatrix;		// object's matrix
glm::mat4 gViewMatrix;		// view matrix
std::map<std::string, glm::mat4> gProjectionMatrix;		// projection matrices

// controls
bool gWireframe = false;	// wireframe control
float gDistance = 3.0f;		// camera distance
float gRotation = 0.0f;		// cube rotation

// function initialise scene and render settings
static void init(GLFWwindow* window)
{
	// set the color the color buffer should be cleared to
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	glEnable(GL_DEPTH_TEST);	// enable depth buffer test

	// compile and link a vertex and fragment shader pair
	gShader.compileAndLink("modelViewProj.vert", "color.frag");

	// initialise view matrix
	gViewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	// initialise projection matrices
	gProjectionMatrix["Left"] = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 10.0f);
	gProjectionMatrix["Right"] = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 10.0f);
	gProjectionMatrix["Main"] = glm::ortho(0.0f, static_cast<float>(gWindowWidth), 0.0f, static_cast<float>(gWindowHeight), 0.1f, 10.0f);

	// vertex positions and colours
	std::vector<GLfloat> vertices =
	{
		// colour cube
		-0.5f, 0.5f, 0.5f,	// vertex 0: position
		1.0f, 0.0f, 1.0f,	// vertex 0: colour
		-0.5f, -0.5f, 0.5f,	// vertex 1: position
		1.0f, 0.0f, 0.0f,	// vertex 1: colour
		0.5f, 0.5f, 0.5f,	// vertex 2: position
		1.0f, 1.0f, 1.0f,	// vertex 2: colour
		0.5f, -0.5f, 0.5f,	// vertex 3: position
		1.0f, 1.0f, 0.0f,	// vertex 3: colour
		-0.5f, 0.5f, -0.5f,	// vertex 4: position
		0.0f, 0.0f, 1.0f,	// vertex 4: colour
		-0.5f, -0.5f, -0.5f,// vertex 5: position
		0.0f, 0.0f, 0.0f,	// vertex 5: colour
		0.5f, 0.5f, -0.5f,	// vertex 6: position
		0.0f, 1.0f, 1.0f,	// vertex 6: colour
		0.5f, -0.5f, -0.5f,	// vertex 7: position
		0.0f, 1.0f, 0.0f,	// vertex 7: colour

		// lines
		0.0f, 400.0f, 0.0f,		// line 1 vertex 0: position
		1.0f, 1.0f, 1.0f,		// line 1 vertex 0: colour
		800.0f, 400.0f, 0.0f,	// line 1 vertex 1: position
		1.0f, 1.0f, 1.0f,		// line 1 vertex 1: colour
		400.0f, 0.0f, 0.0f,		// line 2 vertex 0: position
		1.0f, 1.0f, 1.0f,		// line 2 vertex 0: colour
		400.0f, 400.0f, 0.0f,	// line 2 vertex 1: position
		1.0f, 1.0f, 1.0f,		// line 2 vertex 1: colour
	};

	// colour cube indices
	std::vector<GLuint> indices = {
		0, 1, 2,	// triangle 1
		2, 1, 3,	// triangle 2
		4, 5, 0,	// triangle 3
		0, 5, 1,	// triangle 4
		2, 3, 6,	// triangle 5
		6, 3, 7,	// triangle 6	
		4, 0, 6,	// triangle 7
		6, 0, 2,	// triangle 8
		1, 5, 3,	// triangle 9
		3, 5, 7,	// triangle 10
		5, 4, 7,	// triangle 11
		7, 4, 6,	// triangle 12
	};

	// create VBO
	glGenBuffers(1, &gVBO);					// generate unused VBO identifier
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// generate identifier for IBO and copy data to GPU
	glGenBuffers(1, &gIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// create VAO, specify VBO data and format of the data
	glGenVertexArrays(1, &gVAO);			// generate unused VAO identifier
	glBindVertexArray(gVAO);				// create VAO
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);	// bind the VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
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
	// update view matrix based on camera distance
	gViewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, gDistance), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	// update model matrix
	gModelMatrix = glm::rotate(glm::radians(gRotation), glm::vec3(0.0f, 1.0f, 0.0f));
}

// function to render the scene
static void render_scene()
{
	// clear colour buffer and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gShader.use();						// use the shaders associated with the shader program

	glBindVertexArray(gVAO);			// make VAO active

	glm::mat4 MVP;

	/**************************************
	 * Left viewport
	 **************************************/
	glViewport(0, 0, 400, 400);

	// use the left orthographic projection matrix to set model-view-project matrix
	MVP = gProjectionMatrix["Left"] * gViewMatrix * gModelMatrix;
	// set uniform model transformation matrix
	gShader.setUniform("uModelViewProjectionMatrix", MVP);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);	// display the vertices based on their indices and primitive type

	/**************************************
	 * Right viewport
	 **************************************/
	glViewport(400, 0, 400, 400);

	// use the perspective projection matrix to set model-view-project matrix
	MVP = gProjectionMatrix["Right"] * gViewMatrix * gModelMatrix;
	// set uniform model transformation matrix
	gShader.setUniform("uModelViewProjectionMatrix", MVP);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);	// display the vertices based on their indices and primitive type

	/**************************************
	 * Main viewport
	 **************************************/
	glViewport(0, 0, 800, 600);

	// draw lines in screen space
	// use the main orthographic projection matrix to set model-view-project matrix
	MVP = gProjectionMatrix["Main"] * gViewMatrix;
	// set uniform model transformation matrix
	gShader.setUniform("uModelViewProjectionMatrix", MVP);
	glDrawArrays(GL_LINES, 8, 4);	// display the lines

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

// mouse movement callback function
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	// pass cursor position to tweak bar
	TwEventMousePosGLFW(static_cast<int>(xpos), static_cast<int>(ypos));
}

// mouse button callback function
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	// pass mouse button status to tweak bar
	TwEventMouseButtonGLFW(button, action);
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

	TwDefine(" Main label='User Interface' refresh=0.02 text=light size='250 120' ");

	// toggle wireframe render settings
	TwAddVarRW(twBar, "Wireframe", TW_TYPE_BOOLCPP, &gWireframe, " group='Controls' ");

	// scene settings
	TwAddVarRW(twBar, "Camera Distance", TW_TYPE_FLOAT, &gDistance, " group='Controls' min=1.0 max=10.0 step=0.1 ");
	TwAddVarRW(twBar, "Rotation", TW_TYPE_FLOAT, &gRotation, " group='Controls' min=-360.0 max=360.0 step=1.0 ");

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
	window = glfwCreateWindow(gWindowWidth, gWindowHeight, "Multiple Viewports", nullptr, nullptr);

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

	// initialise scene and render settings
	init(window);

	// initialise AntTweakBar
	TwInit(TW_OPENGL_CORE, nullptr);
	TwBar* tweakBar = create_UI("Main");		// create and populate tweak bar elements

	// the rendering loop
	while (!glfwWindowShouldClose(window))
	{
		update_scene(window);	// update the scene

		// if wireframe set polygon render mode to wireframe
		if (gWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		render_scene();			// render the scene

		// set polygon render mode to fill
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		TwDraw();				// draw tweak bar

		glfwSwapBuffers(window);	// swap buffers
		glfwPollEvents();			// poll for events
	}

	// clean up
	glDeleteBuffers(1, &gVBO);
	glDeleteBuffers(1, &gIBO);
	glDeleteVertexArrays(1, &gVAO);

	// uninitialise tweak bar
	TwDeleteBar(tweakBar);
	TwTerminate();

	// close the window and terminate GLFW
	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}