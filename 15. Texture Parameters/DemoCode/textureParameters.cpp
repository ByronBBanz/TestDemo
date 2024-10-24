#include "utilities.h"

#define STB_IMAGE_IMPLEMENTATION   
#include "stb_image.h"

// global variables
// settings
unsigned int gWindowWidth = 800;
unsigned int gWindowHeight = 600;

// scene content
ShaderProgram gShader;	// shader program object
GLuint gVBO = 0;		// vertex buffer object identifier
GLuint gVAO = 0;		// vertex array object identifier
GLuint gTextureID;		// texture id

glm::mat4 gModelMatrix;			// object matrix
glm::mat4 gViewMatrix;			// view matrix
glm::mat4 gProjectionMatrix;	// projection matrix

Light gLight;			// light properties
Material gMaterial;		// material properties

// controls
bool gWireframe = false;	// wireframe control
bool gReplace = false;		// replace color with texture

// texture wrap selection
enum class TexWrap { REPEAT, MIRROR, CLAMP, BORDER };
TexWrap gWrapS = TexWrap::REPEAT;
TexWrap gWrapT = TexWrap::REPEAT;

// texture filter selection
enum class TexFilter { NEAREST, LINEAR, NEAREST_MIPMAP_NEAREST, LINEAR_MIPMAP_NEAREST, NEAREST_MIPMAP_LINEAR, LINEAR_MIPMAP_LINEAR};
TexFilter gMagFilter = TexFilter::NEAREST;
TexFilter gMinFilter = TexFilter::NEAREST;

// function initialise scene and render settings
static void init(GLFWwindow* window)
{
	// set the color the color buffer should be cleared to
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	glEnable(GL_DEPTH_TEST);	// enable depth buffer test

	// compile and link a vertex and fragment shader pair
	gShader.compileAndLink("lightingAndTexture.vert", "pointLightTexture.frag");

	// initialise view matrix
	gViewMatrix = glm::lookAt(glm::vec3(-5.5f, 2.0f, 8.0f), 
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	// initialise projection matrix
	gProjectionMatrix = glm::perspective(glm::radians(45.0f), 
		static_cast<float>(gWindowWidth) / gWindowHeight, 0.1f, 100.0f);

	// initialise light properties
	gLight.pos = glm::vec3(0.0f, 3.0f, 0.0f);
	gLight.La = glm::vec3(1.0f);
	gLight.Ld = glm::vec3(1.0f);
	gLight.Ls = glm::vec3(1.0f);
	gLight.att = glm::vec3(1.0f, 0.0f, 0.0f);

	// initialise material properties
	gMaterial.Ka = glm::vec3(0.4f, 0.4f, 0.4f);
	gMaterial.Kd = glm::vec3(0.2f, 0.5f, 0.8f);
	gMaterial.Ks = glm::vec3(0.2f, 0.5f, 0.8f);
	gMaterial.shininess = 40.0f;

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

	// set clamp to border colour
	glm::vec4 borderColor(1.0f, 0.0f, 0.0f, 1.0f);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &borderColor[0]);

	// free image data
	stbi_image_free(imageData);

	// initialise model matrices
	gModelMatrix = glm::mat4(1.0f);

	// vertex positions, normals and texture coordinates
	std::vector<GLfloat> vertices =
	{
		-5.0f, 0.0f, 5.0f,	// vertex 0: position
		0.0f, 1.0f, 0.0f,	// vertex 0: normal
		0.0f, 0.0f,			// vertex 0: texture coordinate
		5.0f, 0.0f, 5.0f,	// vertex 1: position
		0.0f, 1.0f, 0.0f,	// vertex 1: normal
		10.0f, 0.0f,		// vertex 1: texture coordinate
		-5.0f, 0.0f, -5.0f,	// vertex 2: position
		0.0f, 1.0f, 0.0f,	// vertex 2: normal
		0.0f, 10.0f,		// vertex 2: texture coordinate
		5.0f, 0.0f, -5.0f,	// vertex 3: position
		0.0f, 1.0f, 0.0f,	// vertex 3: normal
		10.0f, 10.0f,		// vertex 3: texture coordinate
	};

	// create VBO
	glGenBuffers(1, &gVBO);					// generate unused VBO identifier
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// create VAO, specify VBO data and format of the data
	glGenVertexArrays(1, &gVAO);			// generate unused VAO identifier
	glBindVertexArray(gVAO);				// create VAO
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);	// bind the VBO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormTex),
		reinterpret_cast<void*>(offsetof(VertexNormTex, position)));	// specify format of position data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormTex),
		reinterpret_cast<void*>(offsetof(VertexNormTex, normal)));		// specify format of colour data
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexNormTex),
		reinterpret_cast<void*>(offsetof(VertexNormTex, texCoord)));	// specify format of texture coordinate data

	glEnableVertexAttribArray(0);	// enable vertex attributes
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
}

// function used to update the scene
static void update_scene(GLFWwindow* window)
{
	// set magnification filter
	if (gMagFilter == TexFilter::NEAREST)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// set minification filter
	if (gMinFilter == TexFilter::NEAREST)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	else if (gMinFilter == TexFilter::LINEAR)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	else if (gMinFilter == TexFilter::NEAREST_MIPMAP_NEAREST)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	else if (gMinFilter == TexFilter::LINEAR_MIPMAP_NEAREST)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	else if (gMinFilter == TexFilter::NEAREST_MIPMAP_LINEAR)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	else if (gMinFilter == TexFilter::LINEAR_MIPMAP_LINEAR)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// set wrap in S
	if (gWrapS == TexWrap::REPEAT)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	else if (gWrapS == TexWrap::MIRROR)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	else if (gWrapS == TexWrap::CLAMP)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	else if (gWrapS == TexWrap::BORDER)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);

	// set wrap in T
	if (gWrapT == TexWrap::REPEAT)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	else if (gWrapT == TexWrap::MIRROR)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	else if (gWrapT == TexWrap::CLAMP)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	else if (gWrapT == TexWrap::BORDER)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
}

// function to render the scene
static void render_scene()
{
	// clear colour buffer and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gShader.use();						// use the shaders associated with the shader program

	// set light properties
	gShader.setUniform("uLight.pos", gLight.pos);
	gShader.setUniform("uLight.La", gLight.La);
	gShader.setUniform("uLight.Ld", gLight.Ld);
	gShader.setUniform("uLight.Ls", gLight.Ls);
	gShader.setUniform("uLight.att", gLight.att);

	// set material properties
	gShader.setUniform("uMaterial.Ka", gMaterial.Ka);
	gShader.setUniform("uMaterial.Kd", gMaterial.Kd);
	gShader.setUniform("uMaterial.Ks", gMaterial.Ks);
	gShader.setUniform("uMaterial.shininess", gMaterial.shininess);

	// whether to replace with texture
	gShader.setUniform("uReplace", gReplace);

	// set texture
	gShader.setUniform("uTextureSampler", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureID);

	// set viewing position
	gShader.setUniform("uViewpoint", glm::vec3(-5.5f, 2.0f, 8.0f));

	// calculate matrices
	glm::mat4 MVP = gProjectionMatrix * gViewMatrix * gModelMatrix;
	glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(gModelMatrix)));

	// set matrices
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

	TwDefine(" Main label='User Interface' refresh=0.02 text=light size='250 250' ");

	// scene controls
	TwAddVarRW(twBar, "Wireframe", TW_TYPE_BOOLCPP, &gWireframe, " group='Controls' ");

	// light controls
	TwAddVarRW(twBar, "Pos: x", TW_TYPE_FLOAT, &gLight.pos.x, " group='Light' min=-5.0 max=5.0 step=0.1 ");
	TwAddVarRW(twBar, "Pos: y", TW_TYPE_FLOAT, &gLight.pos.y, " group='Light' min=-5.0 max=5.0 step=0.1 ");
	TwAddVarRW(twBar, "Pos: z", TW_TYPE_FLOAT, &gLight.pos.z, " group='Light' min=-5.0 max=5.0 step=0.1 ");

	// texture controls
	TwAddVarRW(twBar, "Replace", TW_TYPE_BOOLCPP, &gReplace, " group='Texture' ");

	// define the enum text
	TwEnumVal filterValue[] = {
		{static_cast<int>(TexFilter::NEAREST), "GL_NEAREST"},
		{static_cast<int>(TexFilter::LINEAR), "GL_LINEAR"},
		{static_cast<int>(TexFilter::NEAREST_MIPMAP_NEAREST), "GL_NEAREST_MIPMAP_NEAREST"},
		{static_cast<int>(TexFilter::LINEAR_MIPMAP_NEAREST), "GL_LINEAR_MIPMAP_NEAREST"},
		{static_cast<int>(TexFilter::NEAREST_MIPMAP_LINEAR), "GL_NEAREST_MIPMAP_LINEAR"},
		{static_cast<int>(TexFilter::LINEAR_MIPMAP_LINEAR), "GL_LINEAR_MIPMAP_LINEAR"},
	};

	// define enum options (ignore warning regarding C++11 enum class)
	TwType magFilterOptions = TwDefineEnum("magFilterType", filterValue, 2);
	TwType minFilterOptions = TwDefineEnum("minFilterType", filterValue, 6);

	// model selection
	TwAddVarRW(twBar, "Mag Filter", magFilterOptions, &gMagFilter, " group='Texture' ");
	TwAddVarRW(twBar, "Min Filter", minFilterOptions, &gMinFilter, " group='Texture' ");

	// define the enum text
	TwEnumVal wrapValue[] = {
		{static_cast<int>(TexWrap::REPEAT), "GL_REPEAT"},
		{static_cast<int>(TexWrap::MIRROR), "GL_MIRRORED_REPEAT"},
		{static_cast<int>(TexWrap::CLAMP), "GL_CLAMP_TO_EDGE"},
		{static_cast<int>(TexWrap::BORDER), "GL_CLAMP_TO_BORDER"},
	};

	// define enum options (ignore warning regarding C++11 enum class)
	TwType wrapOptions = TwDefineEnum("wrapType", wrapValue, 4);

	// model selection
	TwAddVarRW(twBar, "Wrap S", wrapOptions, &gWrapS, " group='Texture' ");
	TwAddVarRW(twBar, "Wrap T", wrapOptions, &gWrapT, " group='Texture' ");

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
	window = glfwCreateWindow(gWindowWidth, gWindowHeight, "Texture Parameters", nullptr, nullptr);

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