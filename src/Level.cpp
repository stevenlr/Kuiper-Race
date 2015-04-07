#include <cstdlib>
#include <algorithm>
#include <iostream>

#include "Logger.h"
#include "InputHandler.h"

#include "maths/MathsConsts.h"

#include "graphics/Registry.h"
#include "graphics/Mesh.h"
#include "graphics/Rectangle.h"

#include "graphics/opengl/ShaderProgram.h"
#include "graphics/opengl/VertexAttrib.h"
#include "graphics/opengl/Sampler.h"

#include "Level.h"
#include "utility.h"

Level::Level() :
	time(1000 * ((float) rand()) / RAND_MAX),
	hasStarted(false),
	didding(false)
{
	restart();
	segments.resize(SEGMENT_NBR);
}

void Level::init()
{
	shipAudio = Audio::play("ship", 0.0, 1.0f, 0, 0, 0, true);
}

Level::~Level()
{
	for (Segment* seg : segments) {
		delete seg;
	}

	segments.clear();
}

float Level::getShipSpeed()
{
	return ship.getSpeed().length();
}

void Level::generateRandomDirection(Vector3 & direction) const
{
	for(int i = 0; i < 3; ++i)
		direction[i] = Utility::generateMinus1_1Value();

	direction.normalize();
	direction *= SEGMENT_LENGTH;
}

void Level::generateTurn(Vector3 & direction) const
{
	float angle = Utility::generateMinus1_1Value() * PI / 4;
	Vector3 newDirection;

	newDirection[0] = direction[0] * std::cos(angle) - direction[1] * sin(angle);
	newDirection[1] = direction[0] * std::sin(angle) + direction[1] * cos(angle);

	angle = Utility::generateMinus1_1Value() * PI / 4;
	direction[0] = newDirection[0];
	direction[1] = newDirection[1] * std::cos(angle) - newDirection[2] * sin(angle);
	direction[2] = newDirection[1] * std::sin(angle) + newDirection[2] * cos(angle);

	direction.normalize();
	direction *= SEGMENT_LENGTH;
}

void Level::generate()
{
	for (Segment* seg : segments) {
		delete seg;
	}

	Vector3 currentPosition, currentDirection({0, SEGMENT_LENGTH, 0});
	for (int i = 0; i < SEGMENT_NBR; ++i) {
		segments[i] = new Segment;
		segments[i]->generate(currentPosition, currentPosition+currentDirection);
		currentPosition += currentDirection;
		generateTurn(currentDirection);
	}
}

void Level::restart() {
	ship.restart();
	currentSegmentIndex = 0;
	currentTime = 0;
	currentSegmentTime = 0;
	win = false;
	dead = false;
	hasStarted = false;
}

void Level::update(float dt)
{
	InputHandler &input = InputHandler::getInstance();
	if (input.keyWasPressed(InputHandler::Restart)) {
		restart();
	}
	if (input.keyWasPressed(InputHandler::NewLevel)) {
		generate();
		restart();
	}

	if (win || dead) {
		return;
	}

	if (hasStarted) {
		time += dt;
		currentTime += dt;
		currentSegmentTime = std::min(currentSegmentTime + dt, MAX_TIME_PER_SEGMENT);
	}

	if (currentSegmentTime >= MAX_TIME_PER_SEGMENT) {
		Audio::play("timeout", 0.5);
		die();
		return;
	}

	ship.update(dt);
	Audio::setGain(shipAudio, getShipSpeed() / 150);
	Audio::setPitch(shipAudio, 0.85+ ship.getRotationSpeed() / 15);

	// Update asteroid sounds, unused
	/*for (int i = currentSegmentIndex - 1; i < currentSegmentIndex + 2; ++i) {
		if (i < 0 || i >= segments.size()) {
			continue;
		}

		segments[i]->update(dt, ship);
	}*/

	if (getShipSpeed() > 0.1) {
		hasStarted = true;
	}

	if (shipCollidesWithAsteroids()) {
		Audio::play("explosion", 0.2);
		die();
		return;
	}

	if (shipCollidesWithCheckpoint()) {
		reachCheckpoint();
	}
}

void Level::draw(TransformPipeline& tp)
{
	ship.applyLookAt(tp);

	static Sampler samplerMipmap(Sampler::MinLinearMipmapLinear, Sampler::MagLinear, Sampler::Repeat);
	Mesh& sphere = *Registry::models["asteroid"];
	ShaderProgram& shader = *Registry::shaders["asteroid"];

	Mesh &environmentCube = *Registry::models["environment_cube"];
	ShaderProgram &cubemapShader = *Registry::shaders["cubemap"];

	Mesh &planet = *Registry::models["planet"];
	ShaderProgram &planetShader = *Registry::shaders["planet"];
	ShaderProgram &atmosphereShader = *Registry::shaders["planet-atmosphere"];

	samplerMipmap.bind(1);
	samplerMipmap.bind(2);
	samplerMipmap.bind(3);
	samplerMipmap.bind(4);

	// ----- Asteroids -----

	tp.identity();
	shader.bind();
	shader["u_PvmMatrix"].setMatrix4(tp.getPVMMatrix());
	shader["u_NormalMatrix"].setMatrix3(tp.getNormalMatrix());
	shader["u_ViewModelMatrix"].setMatrix4(tp.getViewModelMatrix());
	shader["u_ViewMatrix"].setMatrix4(tp.getViewMatrix());
	shader["u_Time"].set1f(time);

	Registry::textures["asteroid-diffuse"]->bind(1);
	Registry::textures["asteroid-normal"]->bind(2);
	Registry::textures["asteroid-emit"]->bind(3);
	Registry::textures["asteroid-specular"]->bind(4);

	for (int i = currentSegmentIndex - 1; i < currentSegmentIndex + 2; ++i) {
		if (i < 0 || i >= segments.size()) {
			continue;
		}

		Segment *seg = segments[i];
		int n = seg->getAsteroids().size();
		Buffer& buffer = seg->getDataBuffer();

		sphere.addAttrib(4, VertexAttrib(&buffer, 3, VertexAttrib::Float, false, sizeof(Asteroid),
										 reinterpret_cast<const void*>(offsetof(Asteroid, position)), 1));
		sphere.addAttrib(5, VertexAttrib(&buffer, 1, VertexAttrib::Float, false, sizeof(Asteroid),
										 reinterpret_cast<const void*>(offsetof(Asteroid, scale)), 1));
		sphere.addAttrib(6, VertexAttrib(&buffer, 3, VertexAttrib::Float, false, sizeof(Asteroid),
										 reinterpret_cast<const void*>(offsetof(Asteroid, rotation)), 1));

		sphere.drawInstanced(n);
	}

	// ----- Spaceship -----

	ship.draw(tp);

	// ----- Cubemap -----

	tp.saveModel();
	tp.identity();

	cubemapShader.bind();
	cubemapShader["u_PvmMatrix"].setMatrix4(tp.getPVMMatrix());

	samplerMipmap.bind(1);
	Registry::cubemap->bind(1);

	glDepthFunc(GL_LEQUAL);
	environmentCube.draw();
	glDepthFunc(GL_LESS);

	tp.restoreModel();

	// ----- Planet -----

	tp.saveModel();
	tp.identity();
	tp.translation(1000, -75000, -3000);
	tp.scale(14000);
	planetShader.bind();
	samplerMipmap.bind(1);
	Registry::textures["planet"]->bind(1);
	planetShader["u_PvmMatrix"].setMatrix4(tp.getPVMMatrix());
	planetShader["u_NormalMatrix"].setMatrix3(tp.getNormalMatrix());
	planetShader["u_ViewMatrix"].setMatrix4(tp.getViewMatrix());
	planetShader["u_ViewModelMatrix"].setMatrix4(tp.getViewModelMatrix());
	planet.draw();
	tp.restoreModel();

	// ----- Checkpoints -----

	tp.saveModel();
	tp.identity();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	ShaderProgram &checkpointShader = *Registry::shaders["checkpoint"];
	checkpointShader.bind();
	checkpointShader["u_PvmMatrix"].setMatrix4(tp.getPVMMatrix());

	checkpointShader["u_Position"].set3f(segments[currentSegmentIndex]->getCheckpoint());
	checkpointShader["u_Color"].set3f(0, 0.35, 0);
	Registry::models["checkpoint"]->draw();
	glDepthFunc(GL_ALWAYS);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	Registry::models["checkpoint"]->draw();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDepthFunc(GL_LESS);

	if (currentSegmentIndex + 1 < segments.size()) {
		checkpointShader["u_Position"].set3f(segments[currentSegmentIndex + 1]->getCheckpoint());
		checkpointShader["u_Color"].set3f(0.4, 0.2, 0);
		Registry::models["checkpoint"]->draw();
		glDepthFunc(GL_ALWAYS);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		Registry::models["checkpoint"]->draw();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDepthFunc(GL_LESS);
	}

	checkpointShader.unbind();
	glDisable(GL_BLEND);

	tp.restoreModel();
}

void Level::drawHUD(int windowWidth, int windowHeight)
{
	static TransformPipeline tp;
	static Sampler sampler(Sampler::MinLinear, Sampler::MagLinear, Sampler::Repeat);

	float windowRatio = (float) windowWidth / windowHeight;
	tp.orthographicProjection(-windowRatio, windowRatio, -1, 1, -10, 10);

	// ----- Arrow -----

	Vector3 direction = segments[currentSegmentIndex]->getCheckpoint() - ship.getPosition();
	direction.normalize();

	Vector4 direction4;
	direction4[0] = direction[0];
	direction4[1] = direction[1];
	direction4[2] = direction[2];
	direction4[3] = 1;

	Matrix4 spaceshipBaseMatrix = ship.getBaseMatrix();
	spaceshipBaseMatrix.transpose();

	direction4 = spaceshipBaseMatrix * direction4;

	direction[0] = direction4[0];
	direction[1] = direction4[1];
	direction[2] = direction4[2];

	tp.identity();
	tp.translation(0, 0.7, 0);
	tp.scale(0.35);

	float rotAngle = acosf(direction.dot(Vector3({0, 0, 1})));

	if (fabs(rotAngle) > 0.001) {
		Vector3 rotAxis = direction.cross(Vector3({0, 0, 1}));
		rotAxis.normalize();

		tp.rotation(rotAxis, -rotAngle);
	}

	Registry::textures["arrow-diffuse"]->bind(1);
	sampler.bind(1);

	Registry::shaders["arrow"]->bind();
	(*Registry::shaders["arrow"])["u_PvmMatrix"].setMatrix4(tp.getPVMMatrix());
	Registry::models["arrow"]->draw();
	Registry::shaders["arrow"]->unbind();

	// ----- Time bar -----

	float ratio = 1 - currentSegmentTime / MAX_TIME_PER_SEGMENT;
	float width = 1.f / 3;
	float height = 1.f / 30;
	float borderx = 3.f / windowWidth;
	float bordery = 3.f / windowHeight;
	float x = 0.5 - width / 2;
	float y = 0.95;
	static Rectangle rectOut(x, y, width, height, .2, .2, .2);
	static Rectangle rectIn(-1, -1, -1, -1, .9, .0, .0);
	rectIn.setBounds(x + borderx, y + bordery, ratio * (width - 2 * borderx), height - 2 * bordery);

	glDisable(GL_DEPTH_TEST);
	Registry::shaders["overlay"]->bind();
	rectOut.draw();
	if (ratio > 0.5f
		|| (ratio > 0.2f && (int) (time * 5) % 4 != 0)
		|| (ratio <= 0.2f && (int) (time * 5) % 2 != 0)) {
		rectIn.draw();
		didding = false;
	} else if (!didding) {
		Audio::play("ding", 0.2);
		didding = true;
	}
	Registry::shaders["overlay"]->unbind();
	glEnable(GL_DEPTH_TEST);
}

bool Level::shipCollidesWithAsteroids()
{
	for (int i = currentSegmentIndex - 1; i < currentSegmentIndex + 2; ++i) {
		if (i < 0 || i >= segments.size()) {
			continue;
		}

		Segment *seg = segments[i];

		for (const Asteroid& ast : seg->getAsteroids()) {
			const Vector3& asteroidPosition = ast.position;
			float astDistX = ship.getPosition()[0] - asteroidPosition[0];
			float astDistY = ship.getPosition()[1] - asteroidPosition[1];
			float astDistZ = ship.getPosition()[2] - asteroidPosition[2];
			float astMinDist = ship.getRadius() + ast.scale;

			if (astDistX * astDistX + astDistY * astDistY + astDistZ * astDistZ <= astMinDist * astMinDist) {
				return true;
			}
		}
	}

	return false;
}

bool Level::shipCollidesWithCheckpoint()
{
	Segment* seg = segments[currentSegmentIndex];
	float cpDistX = ship.getPosition()[0] - seg->getCheckpoint()[0];
	float cpDistY = ship.getPosition()[1] - seg->getCheckpoint()[1];
	float cpDistZ = ship.getPosition()[2] - seg->getCheckpoint()[2];
	float cpMinDist = ship.getRadius() + CHECKPOINT_RADIUS;
	if (cpDistX * cpDistX + cpDistY * cpDistY + cpDistZ * cpDistZ <= cpMinDist * cpMinDist) {
		return true;
	}
	return false;
}

void Level::die()
{
	dead = true;
}

void Level::reachCheckpoint()
{
	if (currentSegmentIndex == segments.size() - 1) {
		win = true;
		Audio::play("checkpoint", 0.4);
	} else {
		currentSegmentIndex += 1;
		currentSegmentTime = 0;
		Audio::play("checkpoint", 0.4);
	}
}

bool Level::isDead() const
{
	return dead;
}

bool Level::isWin() const
{
	return win;
}

bool Level::hasCrashed() const
{
	return currentSegmentTime < MAX_TIME_PER_SEGMENT && dead;
}
