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

using namespace std;

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

static Level level;

static void initialize()
{
	//srand(0);
	srand(time(nullptr));

	level.generate();

	ShaderProgram *asteroidShader = new ShaderProgram("shaders/asteroid.vert", "shaders/default.frag");
	asteroidShader->bindAttribLocation("in_Position", 0);
	asteroidShader->bindAttribLocation("in_Normal", 1);
	asteroidShader->bindAttribLocation("in_TextureCoords", 2);
	asteroidShader->bindAttribLocation("in_Tangent", 3);
	asteroidShader->bindAttribLocation("in_InstancePosition", 4);
	asteroidShader->bindAttribLocation("in_InstanceScale", 5);
	asteroidShader->bindAttribLocation("in_InstanceRotation", 6);
	asteroidShader->bindFragDataLocation("out_Color", 0);
	asteroidShader->link();
	Registry::shaders["asteroid"] = asteroidShader;

	asteroidShader->bind();
	(*asteroidShader)["u_DiffuseTexture"].set1i(1);
	(*asteroidShader)["u_NormalTexture"].set1i(2);
	(*asteroidShader)["u_EmitTexture"].set1i(3);
	(*asteroidShader)["u_SpecularTexture"].set1i(4);
	asteroidShader->unbind();

	Registry::textures["asteroid-diffuse"] = loadPngTexture("textures/asteroid-diffuse.png", true);
	Registry::textures["asteroid-emit"] = loadPngTexture("textures/asteroid-emit.png", true);
	Registry::textures["asteroid-normal"] = loadPngTexture("textures/asteroid-normal.png", true);
	Registry::textures["asteroid-specular"] = loadPngTexture("textures/asteroid-specular.png", true);
	Registry::models["asteroid"] = loadCobjModel("models/asteroid.cobj");

	// ----- Cubemap -----

	Cubemap *cubemap = loadPngCubemap({
		"textures/stars.png",
		"textures/stars.png",
		"textures/stars.png",
		"textures/stars.png",
		"textures/sun.png",
		"textures/stars.png"
	}, true);
	Registry::cubemap = cubemap;

	Registry::models["environment_cube"] = loadCobjModel("models/environment_cube.cobj");

	ShaderProgram *shaderCubemap = new ShaderProgram("shaders/cubemap.vert", "shaders/cubemap.frag");
	shaderCubemap->bindAttribLocation("in_Position", 0);
	shaderCubemap->bindFragDataLocation("out_Color", 0);
	shaderCubemap->link();

	shaderCubemap->bind();
	(*shaderCubemap)["u_Cubemap"].set1i(1);
	shaderCubemap->unbind();

	Registry::shaders["cubemap"] = shaderCubemap;

	// ----- Planet -----

	Registry::textures["planet"] = loadPngTexture("textures/planet.png", true);
	Registry::models["planet"] = loadCobjModel("models/planet.cobj");

	ShaderProgram *planetShader = new ShaderProgram("shaders/planet.vert", "shaders/planet.frag");
	planetShader->bindAttribLocation("in_Position", 0);
	planetShader->bindAttribLocation("in_Normal", 1);
	planetShader->bindAttribLocation("in_TextureCoords", 2);
	planetShader->bindFragDataLocation("out_Color", 0);
	planetShader->link();
	planetShader->bind();
	(*planetShader)["u_Texture"].set1i(1);
	planetShader->unbind();

	Registry::shaders["planet"] = planetShader;

	planetShader = new ShaderProgram("shaders/planet.vert", "shaders/atmosphere.frag");
	planetShader->bindAttribLocation("in_Position", 0);
	planetShader->bindAttribLocation("in_Normal", 1);
	planetShader->bindAttribLocation("in_TextureCoords", 2);
	planetShader->bindFragDataLocation("out_Color", 0);
	planetShader->link();
	planetShader->bind();
	(*planetShader)["u_Texture"].set1i(1);
	planetShader->unbind();

	Registry::shaders["planet-atmosphere"] = planetShader;

	// ----- Screen quad -----

	float quadCoords[] = {0, 0, 1, 0, 1, 1, 0, 1};
	unsigned int quadIndices[] = {0, 1, 3, 3, 1, 2};

	VertexArray *quadVao = new VertexArray(VertexArray::Triangles, 6);

	Buffer *quadCoordsBuffer = new Buffer(Buffer::Array, Buffer::StaticDraw);
	quadCoordsBuffer->data(8 * sizeof(float), reinterpret_cast<const void*>(quadCoords));

	Buffer *quadIndicesBuffer = new Buffer(Buffer::ElementArray, Buffer::StaticDraw);
	quadIndicesBuffer->data(6 * sizeof(unsigned int), reinterpret_cast<const void*>(quadIndices));

	quadVao->addAttrib(0, VertexAttrib(quadCoordsBuffer, 2, VertexAttrib::Float));
	quadVao->setElementIndexArray(ElementIndexArray(quadIndicesBuffer));

	Registry::screenQuad = quadVao;

	// ----- Spaceship -----

	Registry::textures["ship-diffuse"] = loadPngTexture("textures/ship-diffuse.png", true);
	Registry::textures["ship-emit"] = loadPngTexture("textures/ship-emit.png", true);
	Registry::textures["ship-normal"] = loadPngTexture("textures/ship-normal.png", true);
	Registry::textures["ship-specular"] = loadPngTexture("textures/ship-specular.png", true);
	Registry::models["ship"] = loadCobjModel("models/ship.cobj");

	ShaderProgram *shipShader = new ShaderProgram("shaders/ship.vert", "shaders/default.frag");
	shipShader->bindAttribLocation("in_Position", 0);
	shipShader->bindAttribLocation("in_Normal", 1);
	shipShader->bindAttribLocation("in_TextureCoords", 2);
	shipShader->bindAttribLocation("in_Tangent", 3);
	shipShader->bindFragDataLocation("out_Color", 0);
	shipShader->link();
	Registry::shaders["ship"] = shipShader;

	shipShader->bind();
	(*shipShader)["u_DiffuseTexture"].set1i(1);
	(*shipShader)["u_NormalTexture"].set1i(2);
	(*shipShader)["u_EmitTexture"].set1i(3);
	(*shipShader)["u_SpecularTexture"].set1i(4);
	shipShader->unbind();

	// ----- Overlay -----

	ShaderProgram *overlayShader = new ShaderProgram("shaders/overlay.vert", "shaders/overlay.frag");
	overlayShader->bindAttribLocation("in_Position", 0);
	overlayShader->bindFragDataLocation("out_Color", 0);
	overlayShader->link();
	Registry::shaders["overlay"] = overlayShader;

	// ----- Checkpoint -----

	float pointCoords[] = {0, 0, 0};
	VertexArray *pointVao = new VertexArray(VertexArray::Points, 1);
	Buffer *pointCoordsBuffer = new Buffer(Buffer::Array, Buffer::StaticDraw);
	pointCoordsBuffer->data(3 * sizeof(float), reinterpret_cast<const void*>(pointCoords));
	pointVao->addAttrib(0, VertexAttrib(pointCoordsBuffer, 3, VertexAttrib::Float));
	Registry::point = pointVao;

	ShaderProgram *checkpointShader = new ShaderProgram("shaders/checkpoint.vert", "shaders/checkpoint.frag");
	checkpointShader->bindAttribLocation("in_Position", 0);
	checkpointShader->bindFragDataLocation("out_Color", 0);
	checkpointShader->link();
	Registry::shaders["checkpoint"] = checkpointShader;

	Registry::models["checkpoint"] = loadCobjModel("models/checkpoint.cobj");

	// ----- Arrow -----

	Registry::models["arrow"] = loadCobjModel("models/arrow.cobj");	

	ShaderProgram *arrowShader = new ShaderProgram("shaders/arrow.vert", "shaders/arrow.frag");
	arrowShader->bindAttribLocation("in_Position", 0);
	arrowShader->bindAttribLocation("in_TextureCoords", 2);
	arrowShader->bindFragDataLocation("out_Color", 0);
	arrowShader->link();

	arrowShader->bind();
	(*arrowShader)["u_DiffuseTexture"].set1i(1);
	arrowShader->unbind();

	Registry::shaders["arrow"] = arrowShader;
	Registry::textures["arrow-diffuse"] = loadPngTexture("textures/arrow-diffuse.png", true);

	// ----- Screens -----

	Registry::textures["screen-title"] = loadPngTexture("textures/screens/title.png", false);
	Registry::textures["screen-won"] = loadPngTexture("textures/screens/won.png", false);
	Registry::textures["screen-crashed"] = loadPngTexture("textures/screens/crashed.png", false);
	Registry::textures["screen-timeout"] = loadPngTexture("textures/screens/time-out.png", false);
	Registry::textures["screen-controls"] = loadPngTexture("textures/screens/controls.png", false);
	Registry::textures["screen-level"] = loadPngTexture("textures/screens/level.png", false);

	ShaderProgram *screenShader = new ShaderProgram("shaders/postprocess.vert", "shaders/screen.frag");
	screenShader->bindAttribLocation("in_Position", 0);
	screenShader->bindFragDataLocation("out_Color", 0);
	screenShader->link();
	screenShader->bind();
	(*screenShader)["u_Texture"].set1i(1);
	screenShader->unbind();

	Registry::shaders["screen"] = screenShader;
}

static void update(float dt)
{
	level.update(dt);
}

static void draw(TransformPipeline& tp)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	level.draw(tp);
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
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
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
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	TransformPipeline tp;
	tp.perspectiveProjection(70, WINDOW_WIDTH, WINDOW_HEIGHT, 0.1f, 100000);

	TransformPipeline tpHud;
	tpHud.orthographicProjection(-WINDOW_WIDTH / 2, WINDOW_WIDTH / 2, -WINDOW_HEIGHT / 2, WINDOW_HEIGHT / 2, -10, 10);

	initialize();

	Texture gbufferDiffuse(WINDOW_WIDTH, WINDOW_HEIGHT, Texture::RGBA8, Texture::RGB, Texture::UnsignedByte);
	Texture gbufferDepth(WINDOW_WIDTH, WINDOW_HEIGHT, Texture::DepthComponent32f, Texture::Depth, Texture::Float);

	Framebuffer framebuffer;
	framebuffer.bind(Framebuffer::DrawFramebuffer);
	framebuffer.attachTexture(gbufferDiffuse, Framebuffer::Color0);
	framebuffer.attachTexture(gbufferDepth, Framebuffer::Depth);
	framebuffer.drawBuffers({Framebuffer::Color0});
	framebuffer.unbind(Framebuffer::DrawFramebuffer);

	ShaderProgram postprocessShader("shaders/postprocess.vert", "shaders/postprocess.frag");
	postprocessShader.bindAttribLocation("in_Position", 0);
	postprocessShader.bindFragDataLocation("out_Color", 0);
	postprocessShader.link();
	postprocessShader.bind();
	postprocessShader["u_Texture"].set1i(1);
	postprocessShader["u_Depth"].set1i(2);
	postprocessShader["u_Width"].set1f(WINDOW_WIDTH);
	postprocessShader["u_Height"].set1f(WINDOW_HEIGHT);
	postprocessShader.unbind();

	Sampler samplerScreenquad(Sampler::MinLinear, Sampler::MagLinear, Sampler::ClampToEdge);

	ShaderProgram *screenShader = Registry::shaders["screen"];

	bool running = true;
	bool titleScreen = true;
	float currentTime = glfwGetTime();

	while (running) {
		float dt = glfwGetTime() - currentTime;
		currentTime = glfwGetTime();

		input.poll();

		if (glfwWindowShouldClose(window) || input.keyWasPressed(InputHandler::Quit))
			running = false;

		update(dt);

		float speed = level.getShipSpeed();
		tp.perspectiveProjection(58 + speed * 0.9, WINDOW_WIDTH, WINDOW_HEIGHT, 0.1f, 100000);

		if (speed > 0.1) {
			titleScreen = false;
		}

		framebuffer.bind(Framebuffer::DrawFramebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		draw(tp);
		framebuffer.unbind(Framebuffer::DrawFramebuffer);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_CULL_FACE);

		postprocessShader.bind();
		postprocessShader["u_ViewMatrix"].setMatrix4(tp.getViewMatrix());
		postprocessShader["u_ProjectionMatrix"].setMatrix4(tp.getProjectionMatrix());
		samplerScreenquad.bind(1);
		gbufferDiffuse.bind(1);
		samplerScreenquad.bind(2);
		gbufferDepth.bind(2);
		Registry::screenQuad->bind();
		Registry::screenQuad->drawElements();
		Registry::screenQuad->unbind();
		postprocessShader.unbind();

		glClear(GL_DEPTH_BUFFER_BIT);
		level.drawHUD(WINDOW_WIDTH, WINDOW_HEIGHT);
		glEnable(GL_CULL_FACE);

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		screenShader->bind();
		samplerScreenquad.bind(1);
		Registry::screenQuad->bind();

		if (titleScreen) {
			Registry::textures["screen-title"]->bind(1);
			Registry::screenQuad->drawElements();

			Registry::textures["screen-level"]->bind(1);
			Registry::screenQuad->drawElements();

			Registry::textures["screen-controls"]->bind(1);
			Registry::screenQuad->drawElements();
		} else if (level.isDead()) {
			if (level.hasCrashed()) {
				Registry::textures["screen-crashed"]->bind(1);
				Registry::screenQuad->drawElements();
			} else {
				Registry::textures["screen-timeout"]->bind(1);
				Registry::screenQuad->drawElements();
			}

			Registry::textures["screen-level"]->bind(1);
			Registry::screenQuad->drawElements();
		} else if (level.isWin()) {
			Registry::textures["screen-won"]->bind(1);
			Registry::screenQuad->drawElements();

			Registry::textures["screen-level"]->bind(1);
			Registry::screenQuad->drawElements();
		}

		Registry::screenQuad->unbind();
		screenShader->unbind();
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

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
