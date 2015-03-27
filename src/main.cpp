#include <iostream>
#include <cmath>
#include <stdexcept>
#include <cstdlib>
#include <ctime>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Logger.h"
#include "InputHandler.h"

#include "graphics/opengl/ShaderProgram.h"
#include "graphics/opengl/Uniform.h"
#include "graphics/opengl/Buffer.h"
#include "graphics/opengl/VertexArray.h"
#include "graphics/opengl/ElementIndexArray.h"
#include "graphics/opengl/Sampler.h"
#include "graphics/opengl/Texture.h"
#include "graphics/opengl/Framebuffer.h"

#include "graphics/Camera.h"
#include "graphics/CobjLoader.h"
#include "graphics/PngLoader.h"
#include "graphics/Mesh.h"
#include "graphics/Registry.h"

#include "maths/Vector.h"
#include "maths/Matrix.h"
#include "maths/MatrixFactory.h"
#include "maths/TransformPipeline.h"

#include "Level.h"
#include "Spaceship.h"

using namespace std;

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

static Level level;
static Spaceship spaceship;

static void initialize()
{
	srand(time(nullptr));

	level.generate_test();

	ShaderProgram *shader = new ShaderProgram("shaders/test.vert", "shaders/test.frag");
	shader->bindAttribLocation("in_Position", 0);
	shader->bindAttribLocation("in_InstancePosition", 4);
	shader->bindFragDataLocation("out_Color", 0);
	shader->link();
	Registry::shaders["test"] = shader;

	Texture *asteroidDiffuse = loadPngTexture("textures/asteroid-diffuse.png", true);
	Registry::textures["asteroid-diffuse"] = asteroidDiffuse;
	Texture *asteroidEmit = loadPngTexture("textures/asteroid-emit.png", true);
	Registry::textures["asteroid-emit"] = asteroidEmit;
	Texture *asteroidNormal = loadPngTexture("textures/asteroid-normal.png", true);
	Registry::textures["asteroid-normal"] = asteroidNormal;
	Texture *asteroidSpecular = loadPngTexture("textures/asteroid-specular.png", true);
	Registry::textures["asteroid-specular"] = asteroidSpecular;

	Mesh *asteroid = loadCobjModel("models/asteroid.cobj");
	Registry::models["asteroid"] = asteroid;
}

static void update(float dt)
{
	level.update(dt);
	//spaceship.update();
}

static void draw(TransformPipeline& tp)
{
	static Camera camera({-3, 0.5, 0.5});

	camera.update();
	tp.lookAt(camera);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	level.draw(tp);
	//spaceship.draw();
}

static void run(int argc, char *argv[])
{
	Logger::init(&cerr);

	GLFWwindow *window;
	srand(time(nullptr));

	if (!glfwInit())
		throw runtime_error("Couldn't initialize GLFW.");

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "KUIPER RACE", nullptr, nullptr);

	if (!window) {
		glfwTerminate();
		throw runtime_error("Couldn't create GLFWwindow.");
	}

	InputHandler &input = InputHandler::getInstance();

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, InputHandler::keyCallback);
	glfwSetCursorPosCallback(window, InputHandler::mousePositionCallback);
	glfwSetMouseButtonCallback(window, InputHandler::mouseButtonCallback);

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		glfwDestroyWindow(window);
		glfwTerminate();
		throw runtime_error("Couldn't initialize GLEW.");
	}

	glfwSwapInterval(1);

	glClearColor(0, 0, 0, 1);

	TransformPipeline tp;
	tp.perspectiveProjection(70, WINDOW_WIDTH, WINDOW_HEIGHT, 0.1f, 10000);

	initialize();

	bool running = true;
	while (running) {
		input.poll();

		if (glfwWindowShouldClose(window) || input.keyWasPressed(InputHandler::Quit))
			running = false;

		update(1 / 60.);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		draw(tp);
		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}

int main(int argc, char *argv[])
{
	try {
		run(argc, argv);
	} catch (const exception &e) {
		LOGERROR << "Exception thrown : " << e.what() << endl;

#ifndef NDEBUG
		cin.get();
#endif
	} catch (...) {
		LOGERROR << "Unknown exception thrown.";

#ifndef NDEBUG
		cin.get();
#endif
	}
}
