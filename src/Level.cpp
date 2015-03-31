#include <cstdlib>

#include "Logger.h"
#include "InputHandler.h"

#include "graphics/Registry.h"
#include "graphics/Mesh.h"

#include "graphics/opengl/ShaderProgram.h"
#include "graphics/opengl/VertexAttrib.h"
#include "graphics/opengl/Sampler.h"

#include "Level.h"

Level::Level() :
	currentSegmentIndex(0),
	currentSegmentTime(0),
	time(1000 * ((float) rand()) / RAND_MAX),
	dead(false),
	win(false)
{
}

void Level::generate_test()
{
	segments.push_back(new Segment());
	segments.push_back(new Segment());
	segments.push_back(new Segment());
	segments.push_back(new Segment());
	segments.push_back(new Segment());
	segments.push_back(new Segment());

	float i = 0;
	for (Segment *seg : segments) {
		seg->generate_test(Vector3 {i * 50, 0, 0}, Vector3());
		i++;
	}
}

void Level::update(float dt)
{
	if (win || dead) {
		return;
	}

	time += dt;
	currentSegmentTime += dt;

	if (currentSegmentTime >= MAX_TIME_PER_SEGMENT) {
		LOGINFO << "timeout" << std::endl;
		die();
		return;
	}

	ship.update(dt);

	if (shipCollidesWithAsteroids()) {
		LOGINFO << "collision with asteroid" << std::endl;
		die();
		return;
	}

	if (shipCollidesWithCheckpoint()) {
		LOGINFO << "checkpoint reached" << std::endl;
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

	samplerMipmap.bind(1);
	samplerMipmap.bind(2);
	samplerMipmap.bind(3);
	samplerMipmap.bind(4);

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

	ship.draw(tp);

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

	tp.saveModel();
	tp.identity();
	tp.translation(10000, -30000, -3000);
	tp.scale(6000);
	planetShader.bind();
	samplerMipmap.bind(1);
	Registry::textures["planet"]->bind(1);
	planetShader["u_PvmMatrix"].setMatrix4(tp.getPVMMatrix());
	planetShader["u_NormalMatrix"].setMatrix3(tp.getNormalMatrix());
	planetShader["u_ViewMatrix"].setMatrix4(tp.getViewMatrix());
	planetShader["u_ViewModelMatrix"].setMatrix4(tp.getViewModelMatrix());
	planet.draw();
	planetShader.unbind();
	tp.restoreModel();
}

bool Level::shipCollidesWithAsteroids()
{
	InputHandler &input = InputHandler::getInstance();
	if (input.keyWasPressed(InputHandler::SpeedDown)) {
		reachCheckpoint();
	} else if (input.keyWasPressed(InputHandler::SpeedUp)) {
		currentSegmentIndex--;
	}

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
	if (currentSegmentIndex == segments.size()) {
		win = true;
		LOGINFO << "won" << std::endl;
	} else {
		currentSegmentIndex += 1;
		currentSegmentTime = 0;
		LOGINFO << "next segment" << std::endl;
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
