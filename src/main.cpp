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
	srand(0);

	level.generate_test();

	ShaderProgram *shader = new ShaderProgram("shaders/test.vert", "shaders/test.frag");
	shader->bindAttribLocation("in_Position", 0);
	shader->bindAttribLocation("in_Normal", 1);
	shader->bindAttribLocation("in_TextureCoords", 2);
	shader->bindAttribLocation("in_Tangent", 3);
	shader->bindAttribLocation("in_InstancePosition", 4);
	shader->bindAttribLocation("in_InstanceScale", 5);
	shader->bindAttribLocation("in_InstanceRotation", 6);
	shader->bindFragDataLocation("out_Color", 0);
	shader->link();
	Registry::shaders["test"] = shader;

	shader->bind();
	(*shader)["u_DiffuseTexture"].set1i(1);
	(*shader)["u_NormalTexture"].set1i(2);
	(*shader)["u_EmitTexture"].set1i(3);
	(*shader)["u_SpecularTexture"].set1i(4);
	shader->unbind();

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

	Mesh *environmentCube = loadCobjModel("models/environment_cube.cobj");
	Registry::models["environment_cube"] = environmentCube;

	ShaderProgram *shaderCubemap = new ShaderProgram("shaders/cubemap.vert", "shaders/cubemap.frag");
	shaderCubemap->bindAttribLocation("in_Position", 0);
	shaderCubemap->bindFragDataLocation("out_Color", 0);
	shaderCubemap->link();

	shaderCubemap->bind();
	(*shaderCubemap)["u_Cubemap"].set1i(1);
	shaderCubemap->unbind();

	Registry::shaders["cubemap"] = shaderCubemap;

	// ----- Planet -----

	Texture *planetTexture = loadPngTexture("textures/planet.png", true);
	Registry::textures["planet"] = planetTexture;

	Mesh *planetMesh = loadCobjModel("models/planet.cobj");
	Registry::models["planet"] = planetMesh;

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
	tp.perspectiveProjection(70, WINDOW_WIDTH, WINDOW_HEIGHT, 0.1f, 100000);

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

	bool running = true;
	while (running) {
		input.poll();

		if (glfwWindowShouldClose(window) || input.keyWasPressed(InputHandler::Quit))
			running = false;

		update(1 / 60.);

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

		glEnable(GL_CULL_FACE);

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
