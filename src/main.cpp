#include <cassert>
#include <cstring>
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define EIGEN_DONT_ALIGN_STATICALLY
#include <Eigen/Dense>

#include "TPCamera.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Shape.h"
#include "Texture.h"
#include "WObject.h"
#include "WBalloon.h"

using namespace std;
using namespace Eigen;

GLFWwindow *window; // Main application window
string RESOURCE_DIR = "./"; // Where the resources are loaded from

shared_ptr<TPCamera> camera;
shared_ptr<Program> prog;

Vector3f lightPosition;
shared_ptr<Shape> light;
shared_ptr<Shape> ground;
shared_ptr<Shape> box;
shared_ptr<Texture> texture;
shared_ptr<Texture> texture2;

// Texture transformation matrix for the ground
Matrix3f T1;
// Texture transformation matrix for the boxes
Matrix3f T2;

// The world objects
WorldObject level;
WorldObject sun;
vector<WorldObject> boxes;

// The balloon and movement speed
WBalloon balloon;
int rows;
int columns;
Vector3f color;
float movit;

bool keyToggles[256] = {false}; // only for English keyboards!

// This function is called when a GLFW error occurs
static void error_callback(int error, const char *description)
{
	cerr << description << endl;
}

// This function is called when a key is pressed
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

// This function is called when the mouse moves
static void cursor_position_callback(GLFWwindow* window, double xmouse, double ymouse)
{
	camera->mouseMoved((float)xmouse, (float)ymouse);
}

static void char_callback(GLFWwindow *window, unsigned int key)
{
	keyToggles[key] = !keyToggles[key];
	if (key == 'w')
	{
		// Move forward
		balloon.move(camera->getForward(), movit);
	}
	if (key == 'a')
	{
		// Move left
		balloon.move(camera->getRight(), -movit);
	}
	if (key == 's')
	{
		// Move backward
		balloon.move(camera->getForward(), -movit);
	}
	if (key == 'd')
	{
		// Move right
		balloon.move(camera->getRight(), movit);
	}
}

// If the window is resized, capture the new size and reset the viewport
static void resize_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// Creates the light, ground, and world objects using randoms and the ROW and COL variables
void generateWObjects()
{
	// Ground
	level = WorldObject(ground, Vector3f(0.0f,0.0f,0.0f),
		Vector3f(0.0f,0.0f,0.0f),
		Vector3f(0.0f,0.0f,0.0f),
		Vector3f(20.0f, 1.0f, 20.0f));
	// Negative s for texture use instead
	level.setMaterial(Vector3f(0.0f, 0.0f, 0.0f),
		Vector3f(0.0f, 0.0f, 0.0f),
		Vector3f(0.0f, 0.0f, 0.0f),
		-1.0f, 1.0f);

	// Light
	lightPosition = Vector3f(-10.0f,10.0f,-10.0f);

	sun = WorldObject(light, lightPosition,
		Vector3f(0.0f,0.0f,0.0f),
		Vector3f(0.0f,0.0f,0.0f),
		Vector3f(2.0f, 2.0f, 2.0f));
	sun.setMaterial(Vector3f(1.0f, 1.0f, 0.0f),
		Vector3f(1.0f, 1.0f, 0.0f),
		Vector3f(1.0f, 1.0f, 1.0f),
		300.0f, 0.8f);

	// Boxes
	boxes.push_back(WorldObject(box, Vector3f(-8.5f,0.0f,8.5f),
		Vector3f(0.0f, 0.0f, 0.0f),
		Vector3f(0.0f, 2.5f, 0.0f),
		Vector3f(5.0f, 5.0f, 5.0f)));
	boxes[0].setMaterial(Vector3f(0.0f, 0.0f, 0.0f),
		Vector3f(0.2f, 0.2f, 0.2f),
		Vector3f(0.0f, 0.0f, 0.0f),
		-1.0f, 1.0f);
	boxes.push_back(WorldObject(box, Vector3f(-4.0f,0.0f,8.7f),
		Vector3f(0.0f, 0.0f, 0.0f),
		Vector3f(0.0f, 2.0f, 0.0f),
		Vector3f(4.0f, 4.0f, 4.0f)));
	boxes[1].setMaterial(Vector3f(0.0f, 0.0f, 0.0f),
		Vector3f(0.2f, 0.2f, 0.2f),
		Vector3f(0.0f, 0.0f, 0.0f),
		-1.0f, 1.0f);
	boxes.push_back(WorldObject(box, Vector3f(-10.0f,0.0f,5.0f),
		Vector3f(0.0f, 0.0f, 0.0f),
		Vector3f(0.0f, 1.0f, 0.0f),
		Vector3f(2.0f, 2.0f, 2.0f)));
	boxes[2].setMaterial(Vector3f(0.0f, 0.0f, 0.0f),
		Vector3f(0.2f, 0.2f, 0.2f),
		Vector3f(0.0f, 0.0f, 0.0f),
		-1.0f, 1.0f);
	boxes.push_back(WorldObject(box, Vector3f(3.0f,0.0f,-1.3f),
		Vector3f(0.0f, 0.0f, 0.0f),
		Vector3f(0.0f, 1.5f, 0.0f),
		Vector3f(10.0f, 3.0f, 20.0f)));
	boxes[3].setMaterial(Vector3f(0.0f, 0.0f, 0.0f),
		Vector3f(0.2f, 0.2f, 0.2f),
		Vector3f(0.0f, 0.0f, 0.0f),
		-1.0f, 1.0f);
	boxes.push_back(WorldObject(box, Vector3f(-3.0f,0.0f,-8.3f),
		Vector3f(0.0f, 0.0f, 0.0f),
		Vector3f(0.0f, 1.0f, 0.0f),
		Vector3f(2.0f, 2.0f, 4.0f)));
	boxes[4].setMaterial(Vector3f(0.0f, 0.0f, 0.0f),
		Vector3f(0.2f, 0.2f, 0.2f),
		Vector3f(0.0f, 0.0f, 0.0f),
		-1.0f, 1.0f);
}

// This function is called once to initialize the scene and OpenGL
static void init()
{
	// Initialize time.
	glfwSetTime(0.0);
	
	// Set background color.
	glClearColor(0.529f, 0.808f, 0.98f, 1.0f);
	// Enable z-buffer test.
	glEnable(GL_DEPTH_TEST);
	// Enable setting gl_PointSize from vertex shader
	glEnable(GL_PROGRAM_POINT_SIZE);
	// Enable quad creation from sprite
	glEnable(GL_POINT_SPRITE);

	prog = make_shared<Program>();
	prog->setShaderNames(RESOURCE_DIR + "vert.glsl", RESOURCE_DIR + "frag.glsl");
	prog->setVerbose(true);
	prog->init();
	prog->addAttribute("aPos");
	prog->addAttribute("aNor");
	prog->addAttribute("aTex");
	prog->addUniform("T1");
	prog->addUniform("MV");
	prog->addUniform("VM");
	prog->addUniform("P");
	prog->addUniform("lightPos");
	prog->addUniform("ka");
	prog->addUniform("kd");
	prog->addUniform("ks");
	prog->addUniform("s");
	prog->addUniform("alpha");
	prog->addUniform("texture");

	// Load the ground texture
	texture = make_shared<Texture>();
	texture->setFilename(RESOURCE_DIR + "grass.jpg");
	texture->init();
	texture->setUnit(prog->getUniform("texture"));
	texture->setWrapModes(GL_REPEAT, GL_REPEAT);

	// Load the box texture
	texture2 = make_shared<Texture>();
	texture2->setFilename(RESOURCE_DIR + "box.jpg");
	texture2->init();
	texture2->setUnit(prog->getUniform("texture"));
	texture2->setWrapModes(GL_REPEAT, GL_REPEAT);

	// Scale the texture to the ground size
	T1.setIdentity();
	T1(0,0) = 20.0f;
	T1(1,1) = 20.0f;

	// Scale the texture to the box size
	T2.setIdentity();
	T2(0,0) = 1.0f;
	T2(1,1) = 1.0f;

	// Load the light shape
	light = make_shared<Shape>();
	light->loadMesh(RESOURCE_DIR + "sphere.obj");
	light->init();

	// Load the ground shape
	ground = make_shared<Shape>();
	ground->loadMesh(RESOURCE_DIR + "ground.obj");
	ground->init();

	// Load the box shape
	box = make_shared<Shape>();
	box->loadMesh(RESOURCE_DIR + "cube.obj");
	box->init();

	// Generate all of the world objects
	generateWObjects();
	
	// Create Water Balloon
	balloon = WBalloon(Vector3f(-8.5f, 6.0f, 8.5f), 1.0f, columns, rows, color);
	balloon.setFloor(5.0f);

	// Create the camera
	camera = make_shared<TPCamera>(balloon.getPosition(), 10.0f);

	movit = 0.1f;
	
	GLSL::checkError(GET_FILE_LINE);
}

// This function is called every frame to draw the scene.
static void render()
{
	// Clear framebuffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);

	if(keyToggles[(unsigned)'l']) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if(!keyToggles[(unsigned)'f']) {
		camera->updateTarget(balloon.getPosition());
	}
	
	// Get current frame buffer size.
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	camera->setAspect((float)width/(float)height);
	
	// Matrix stacks
	auto P = make_shared<MatrixStack>();
	auto MV = make_shared<MatrixStack>();
	
	// Apply camera transforms
	P->pushMatrix();
	camera->applyProjectionMatrix(P);
	MV->pushMatrix();
	camera->applyViewMatrix(MV);
	
	// Draw particles
	prog->bind();

	glDepthMask(GL_TRUE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	texture->bind(prog->getUniform("texture"));

	glUniformMatrix3fv(prog->getUniform("T1"), 1, GL_FALSE, T1.data());

	glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());

	glUniformMatrix4fv(prog->getUniform("VM"), 1, GL_FALSE, MV->topMatrix().data());

	glUniform3f(prog->getUniform("lightPos"), lightPosition[0], lightPosition[1], lightPosition[2]);

	// Draw the ground
	level.draw(MV, prog);
	texture->unbind();

	// Draw the boxes
	texture2->bind(prog->getUniform("texture"));
	glUniformMatrix3fv(prog->getUniform("T1"), 1, GL_FALSE, T2.data());
	
	for (int box = 0; box < boxes.size(); box++)
	{
		boxes[box].draw(MV, prog);
	}
	
	// Draw the balloon
	balloon.draw(MV, prog);

	// Draw the light
	sun.draw(MV, prog);

	GLSL::checkError(GET_FILE_LINE);
	
	texture2->unbind();
	prog->unbind();

	MV->popMatrix();
	P->popMatrix();
	
	GLSL::checkError(GET_FILE_LINE);
}

// Give the balloon the current floor position
void floorCheck()
{
	float floor = 0.0f;
	for (int box = 0; box < boxes.size(); box++)
	{
		if (boxes[box].inBounds(balloon.getPosition()))
		{
			floor = boxes[box].getHeight();
		}
	}
	balloon.setFloor(floor);
}

// Check for collisions and move the balloon
void colMove()
{
	Vector3f posit = balloon.getPosition();
	float rad = balloon.getRadius();
	Vector2f push = Vector2f(0.0f, 0.0f);
	for (int box = 0; box < boxes.size(); box++)
	{
		push += boxes[box].block(posit,rad);
	}
	balloon.step(push);
}

int main(int argc, char **argv)
{
	if(argc < 2) {
		cout << "Please specify the resource directory." << endl;
		return 0;
	}
	RESOURCE_DIR = argv[1] + string("/");

	if (argc > 3)
	{
		rows = stoi(argv[2]);
		columns = stoi(argv[3]);
	}
	else
	{
		rows = 15;
		columns = 15;
	}

	if (argc > 6)
	{
		color = Vector3f(stoi(argv[4]) / 255.0f, stoi(argv[5]) / 255.0f, stoi(argv[6]) / 255.0f);
	}
	else
	{
		color = Vector3f(0.9f, 0.0f, 0.0f);
	}

	// Set error callback.
	glfwSetErrorCallback(error_callback);
	// Initialize the library.
	if(!glfwInit()) {
		return -1;
	}
	// Create a windowed mode window and its OpenGL context.
	window = glfwCreateWindow(640, 480, "Water Balloon Simulator", NULL, NULL);
	if(!window) {
		glfwTerminate();
		return -1;
	}
	// Make the window's context current.
	glfwMakeContextCurrent(window);
	// Initialize GLEW.
	glewExperimental = true;
	if(glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		return -1;
	}
	glGetError(); // A bug in glewInit() causes an error that we can safely ignore.
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
	GLSL::checkVersion();
	// Set vsync.
	glfwSwapInterval(1);
	// Set keyboard callback.
	glfwSetKeyCallback(window, key_callback);
	// Set char callback.
	glfwSetCharCallback(window, char_callback);
	// Set cursor position callback.
	glfwSetCursorPosCallback(window, cursor_position_callback);
	// Set the window resize call back.
	glfwSetFramebufferSizeCallback(window, resize_callback);
	// Initialize scene.
	init();
	// Loop until the user closes the window.
	while(!glfwWindowShouldClose(window)) {
		// Step Balloon
		floorCheck();
		colMove();
		camera->updatePosition();
		// Render scene.
		render();
		// Swap front and back buffers.
		glfwSwapBuffers(window);
		// Poll for and process events.
		glfwPollEvents();
	}
	// Quit program.
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
