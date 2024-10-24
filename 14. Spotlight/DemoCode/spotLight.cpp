#include "utilities.h"

// global variables
// settings
unsigned int gWindowWidth = 800;
unsigned int gWindowHeight = 600;

// frame stats
float gFrameRate = 60.0f;
float gFrameTime = 1 / gFrameRate;

// scene content
ShaderProgram gShader;	// shader program object
GLuint gVBO = 0;		// vertex buffer object identifier
GLuint gVAO = 0;		// vertex array object identifier

glm::mat4 gModelMatrix;			// object matrix
glm::mat4 gViewMatrix;			// view matrix
glm::mat4 gProjectionMatrix;	// projection matrix

Light gLight;			// light properties
Material gMaterial;		// material properties

// controls
bool gWireframe = false;	// wireframe control

// function initialise scene and render settings
static void init(GLFWwindow* window)
{
	// set the color the color buffer should be cleared to
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	glEnable(GL_DEPTH_TEST);	// enable depth buffer test

	// compile and link a vertex and fragment shader pair
	gShader.compileAndLink("lighting.vert", "spotLight.frag");

	// initialise view matrix
	gViewMatrix = glm::lookAt(glm::vec3(0.0f, 2.0f, 4.0f), 
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	// initialise projection matrix
	gProjectionMatrix = glm::perspective(glm::radians(45.0f), 
		static_cast<float>(gWindowWidth) / gWindowHeight, 0.1f, 10.0f);

	// initialise light properties
	gLight.pos = glm::vec3(0.0f, 1.0f, 0.0f);
	gLight.dir = glm::vec3(0.0f, -1.0f, 0.0f);
	gLight.La = glm::vec3(0.0f);
	gLight.Ld = glm::vec3(1.0f, 1.0f, 0.0f);
	gLight.Ls = glm::vec3(1.0f, 1.0f, 0.0f);
	gLight.att = glm::vec3(1.0f, 0.0f, 0.0f);
	gLight.innerAngle = 15.0f;
	gLight.outerAngle = 45.0f;

	// initialise material properties
	gMaterial.Ka = glm::vec3(0.4f);
	gMaterial.Kd = glm::vec3(1.0f);
	gMaterial.Ks = glm::vec3(1.0f);
	gMaterial.shininess = 40.0f;

	// initialise model matrices
	gModelMatrix = glm::mat4(1.0f);

	// vertex positions and normals
	std::vector<GLfloat> vertices =
	{
		-2.0f, 0.0f, 2.0f,	// vertex 0: position
		0.0f, 1.0f, 0.0f,	// vertex 0: normal
		2.0f, 0.0f, 2.0f,	// vertex 1: position
		0.0f, 1.0f, 0.0f,	// vertex 1: normal
		-2.0f, 0.0f, -2.0f,	// vertex 2: position
		0.0f, 1.0f, 0.0f,	// vertex 2: normal
		2.0f, 0.0f, -2.0f,	// vertex 3: position
		0.0f, 1.0f, 0.0f,	// vertex 3: normal
	};

	// create VBO
	glGenBuffers(1, &gVBO);					// generate unused VBO identifier
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// create VAO, specify VBO data and format of the data
	glGenVertexArrays(1, &gVAO);			// generate unused VAO identifier
	glBindVertexArray(gVAO);				// create VAO
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);	// bind the VBO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormal),
		reinterpret_cast<void*>(offsetof(VertexNormal, position)));	// specify format of position data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormal),
		reinterpret_cast<void*>(offsetof(VertexNormal, normal)));		// specify format of colour data

	glEnableVertexAttribArray(0);	// enable vertex attributes
	glEnableVertexAttribArray(1);
}

// function used to update the scene
static void update_scene(GLFWwindow* window)
{
}

// function to render the scene
static void render_scene()
{
	// clear colour buffer and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gShader.use();						// use the shaders associated with the shader program

	// set light properties
	gShader.setUniform("uLight.pos", gLight.pos);
	gShader.setUniform("uLight.dir", gLight.dir);
	gShader.setUniform("uLight.La", gLight.La);
	gShader.setUniform("uLight.Ld", gLight.Ld);
	gShader.setUniform("uLight.Ls", gLight.Ls);
	gShader.setUniform("uLight.att", gLight.att);
	gShader.setUniform("uLight.innerAngle", glm::radians(gLight.innerAngle));
	gShader.setUniform("uLight.outerAngle", glm::radians(gLight.outerAngle));

	// set material properties
	gShader.setUniform("uMaterial.Ka", gMaterial.Ka);
	gShader.setUniform("uMaterial.Kd", gMaterial.Kd);
	gShader.setUniform("uMaterial.Ks", gMaterial.Ks);
	gShader.setUniform("uMaterial.shininess", gMaterial.shininess);

	// set viewing position
	gShader.setUniform("uViewpoint", glm::vec3(0.0f, 2.0f, 4.0f));

	// set model and modelviewprojection matrices
	glm::mat4 MVP = gProjectionMatrix * gViewMatrix * gModelMatrix;
	glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(gModelMatrix)));

	// set uniform variables
	gShader.setUniform("uModelViewProjectionMatrix", MVP);
	gShader.setUniform("uModelMatrix", gModelMatrix);
	gShader.setUniform("uNormalMatrix", normalMatrix);

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

	TwDefine(" Main label='User Interface' refresh=0.02 text=light size='250 500' ");

	// create frame stat entries
	TwAddVarRO(twBar, "Frame Rate", TW_TYPE_FLOAT, &gFrameRate, " group='Frame Stats' precision=2 ");
	TwAddVarRO(twBar, "Frame Time", TW_TYPE_FLOAT, &gFrameTime, " group='Frame Stats' ");

	// scene controls
	TwAddVarRW(twBar, "Wireframe", TW_TYPE_BOOLCPP, &gWireframe, " group='Controls' ");

	// light controls
	TwAddVarRW(twBar, "Pos: x", TW_TYPE_FLOAT, &gLight.pos.x, " group='Light' min=-5.0 max=5.0 step=0.1 ");
	TwAddVarRW(twBar, "Pos: y", TW_TYPE_FLOAT, &gLight.pos.y, " group='Light' min=-5.0 max=5.0 step=0.1 ");
	TwAddVarRW(twBar, "Pos: z", TW_TYPE_FLOAT, &gLight.pos.z, " group='Light' min=-5.0 max=5.0 step=0.1 ");
	TwAddVarRW(twBar, "Direction", TW_TYPE_DIR3F, &gLight.dir, " group='Light' ");
	TwAddVarRW(twBar, "La", TW_TYPE_COLOR3F, &gLight.La, " group='Light' ");
	TwAddVarRW(twBar, "Ld", TW_TYPE_COLOR3F, &gLight.Ld, " group='Light' ");
	TwAddVarRW(twBar, "Ls", TW_TYPE_COLOR3F, &gLight.Ls, " group='Light' ");
	TwAddVarRW(twBar, "Constant", TW_TYPE_FLOAT, &gLight.att.x, " group='Light' min=0.0 max=1.0 step=0.01 ");
	TwAddVarRW(twBar, "Linear", TW_TYPE_FLOAT, &gLight.att.y, " group='Light' min=0.0 max=1.0 step=0.01 ");
	TwAddVarRW(twBar, "Quadratic", TW_TYPE_FLOAT, &gLight.att.z, " group='Light' min=0.0 max=1.0 step=0.01 ");
	TwAddVarRW(twBar, "InnerAngle", TW_TYPE_FLOAT, &gLight.innerAngle, " group='Light' min=0.0 max=90.0 step=1 ");
	TwAddVarRW(twBar, "OuterAngle", TW_TYPE_FLOAT, &gLight.outerAngle, " group='Light' min=0.0 max=90.0 step=1 ");

	// material controls
	TwAddVarRW(twBar, "Ka", TW_TYPE_COLOR3F, &gMaterial.Ka, " group='Material' ");
	TwAddVarRW(twBar, "Kd", TW_TYPE_COLOR3F, &gMaterial.Kd, " group='Material' ");
	TwAddVarRW(twBar, "Ks", TW_TYPE_COLOR3F, &gMaterial.Ks, " group='Material' ");
	TwAddVarRW(twBar, "Shininess", TW_TYPE_FLOAT, &gMaterial.shininess, " group='Material' min=1 max=255 step=1 ");

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
	window = glfwCreateWindow(gWindowWidth, gWindowHeight, "Spotlight", nullptr, nullptr);

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

	// timing data
	double lastUpdateTime = glfwGetTime();	// last update time
	double elapsedTime = lastUpdateTime;	// time since last update
	int frameCount = 0;						// number of frames since last update

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

	// uninitialise tweak bar
	TwDeleteBar(tweakBar);
	TwTerminate();

	// close the window and terminate GLFW
	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}