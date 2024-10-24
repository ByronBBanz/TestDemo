#include "utilities.h"

#define STB_IMAGE_IMPLEMENTATION   
#include "stb_image.h"

// struct for vertex attributes
struct VertexTex
{
	GLfloat position[3];
	GLfloat texCoord[2];
};

// global variables
// settings
unsigned int gWindowWidth = 800;
unsigned int gWindowHeight = 800;

// scene content
ShaderProgram gShader;	// shader program object
GLuint gVBO = 0;		// vertex buffer object identifier
GLuint gVAO = 0;		// vertex array object identifier
GLuint gTextureID;		// texture id

// vertex positions, normals and texture coordinates
std::vector<GLfloat> gVertices =
{
	-0.2f, -0.5f, 0.0f,	// vertex 0: position
	0.0f, 0.0f,			// vertex 0: texture coordinate
	0.8f, -0.5f, 0.0f,	// vertex 1: position
	1.0f, 0.0f,			// vertex 1: texture coordinate
	-0.2f, 0.5f, 0.0f,	// vertex 2: position
	0.0f, 1.0f,			// vertex 2: texture coordinate
	0.8f, 0.5f, 0.0f,	// vertex 3: position
	1.0f, 1.0f,			// vertex 3: texture coordinate
};

// controls
bool gWireframe = false;	// wireframe control

// function initialise scene and render settings
static void init(GLFWwindow* window)
{
	// set the color the color buffer should be cleared to
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	// compile and link a vertex and fragment shader pair
	gShader.compileAndLink("textureCoords.vert", "textureCoords.frag");

	// load image data
	stbi_set_flip_vertically_on_load(true); // flip image about y-axis
	int imageWidth, imageHeight, imageChannels;
	unsigned char* imageData = stbi_load("./images/check.bmp", &imageWidth, &imageHeight, &imageChannels, 0);

	if(!imageData)
		std::cerr << "Unable to load image." << std::endl;	// output error description

	// generate texture
	glGenTextures(1, &gTextureID);
	glBindTexture(GL_TEXTURE_2D, gTextureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
	glGenerateMipmap(GL_TEXTURE_2D);

	// set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// free image data
	stbi_image_free(imageData);

	// create VBO
	glGenBuffers(1, &gVBO);					// generate unused VBO identifier
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * gVertices.size(), &gVertices[0], GL_DYNAMIC_DRAW);

	// create VAO, specify VBO data and format of the data
	glGenVertexArrays(1, &gVAO);			// generate unused VAO identifier
	glBindVertexArray(gVAO);				// create VAO
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);	// bind the VBO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTex),
		reinterpret_cast<void*>(offsetof(VertexTex, position)));	// specify format of position data
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTex),
		reinterpret_cast<void*>(offsetof(VertexTex, texCoord)));	// specify format of texture coordinate data

	glEnableVertexAttribArray(0);	// enable vertex attributes
	glEnableVertexAttribArray(1);
}

// function used to update the scene
static void update_scene(GLFWwindow* window)
{
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * gVertices.size(), &gVertices[0], GL_DYNAMIC_DRAW);
}

// function to render the scene
static void render_scene()
{
	// clear colour buffer and depth buffer
	glClear(GL_COLOR_BUFFER_BIT);

	gShader.use();						// use the shaders associated with the shader program

	// set texture
	gShader.setUniform("uTextureSampler", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureID);

	glBindVertexArray(gVAO);				// make VAO active
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);	// render the vertices

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
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		// reset texture coordinates
		gVertices[3] = gVertices[4] = gVertices[9] = gVertices[13] = 0.0f;
		gVertices[8] = gVertices[14] = gVertices[18] = gVertices[19] = 1.0f;
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

	TwDefine(" Main label='User Interface' refresh=0.02 text=light size='250 250' ");

	// scene controls
	TwAddVarRW(twBar, "Wireframe", TW_TYPE_BOOLCPP, &gWireframe, " group='Controls' ");

	// texture controls
	TwAddVarRW(twBar, "Top-left: S", TW_TYPE_FLOAT, &gVertices[13], " group='Texture Coordinates' min=-2.0 max=2.0 step=0.1");
	TwAddVarRW(twBar, "Top-left: T", TW_TYPE_FLOAT, &gVertices[14], " group='Texture Coordinates' min=-2.0 max=2.0 step=0.1");
	TwAddVarRW(twBar, "Top-right: S", TW_TYPE_FLOAT, &gVertices[18], " group='Texture Coordinates' min=-2.0 max=2.0 step=0.1");
	TwAddVarRW(twBar, "Top-right: T", TW_TYPE_FLOAT, &gVertices[19], " group='Texture Coordinates' min=-2.0 max=2.0 step=0.1");
	TwAddVarRW(twBar, "Bottom-left: S", TW_TYPE_FLOAT, &gVertices[3], " group='Texture Coordinates' min=-2.0 max=2.0 step=0.1");
	TwAddVarRW(twBar, "Bottom-left: T", TW_TYPE_FLOAT, &gVertices[4], " group='Texture Coordinates' min=-2.0 max=2.0 step=0.1");
	TwAddVarRW(twBar, "Bottom-right: S", TW_TYPE_FLOAT, &gVertices[8], " group='Texture Coordinates' min=-2.0 max=2.0 step=0.1");
	TwAddVarRW(twBar, "Bottom-right: T", TW_TYPE_FLOAT, &gVertices[9], " group='Texture Coordinates' min=-2.0 max=2.0 step=0.1");

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
	window = glfwCreateWindow(gWindowWidth, gWindowHeight, "Texture Coordinates", nullptr, nullptr);

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
	glDeleteVertexArrays(1, &gVAO);
	glDeleteTextures(1, &gTextureID);

	// uninitialise tweak bar
	TwDeleteBar(tweakBar);
	TwTerminate();

	// close the window and terminate GLFW
	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}