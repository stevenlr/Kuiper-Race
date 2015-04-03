#include "InputHandler.h"

#include "maths/TransformPipeline.h"
#include "graphics/Registry.h"
#include "graphics/opengl/ShaderProgram.h"
#include "graphics/opengl/Sampler.h"

#include "Spaceship.h"

Spaceship::Spaceship() :
		forward({0, 1, 0}),
		up({0, 0, 1}),
		right({1, 0, 0}),
		position({0, -50, 0}),
		speed(0),
		rotationSpeedY(0),
		rotationSpeedX(0)
{
}

void Spaceship::update(float dt)
{
	static const float acceleration = 120;
	static const float rotationAccelerationY = 20;
	static const float rotationAccelerationX = 20;
	InputHandler &input = InputHandler::getInstance();

	speed *= 0.96; // Friction in space, suuuure...

	if (input.keyIsDown(InputHandler::SpeedUp)) {
		speed += dt * acceleration;
	}

	if (input.keyIsDown(InputHandler::SpeedDown)) {
		speed -= dt * acceleration;
	}

	rotationSpeedY *= 0.7;
	rotationSpeedX *= 0.7;

	if (input.keyIsDown(InputHandler::Left)) {
		rotationSpeedY += dt * rotationAccelerationY;
	}

	if (input.keyIsDown(InputHandler::Right)) {
		rotationSpeedY -= dt * rotationAccelerationY;
	}

	if (input.keyIsDown(InputHandler::Forward)) {
		rotationSpeedX -= dt * rotationAccelerationX;
	}

	if (input.keyIsDown(InputHandler::Backward)) {
		rotationSpeedX += dt * rotationAccelerationX;
	}

	float rotationY = dt * rotationSpeedY;
	float rotationX = dt * rotationSpeedX;

	right = right * cosf(rotationY) + up * sinf(rotationY);
	right.normalize();
	up = right.cross(forward);
	up.normalize();

	forward = forward * cosf(rotationX) + up * sinf(rotationX);
	forward.normalize();
	up = right.cross(forward);
	up.normalize(); 

	position += forward * speed;
}

void Spaceship::draw(TransformPipeline& tp)
{
	static Sampler samplerMipmap(Sampler::MinLinearMipmapLinear, Sampler::MagLinear, Sampler::Repeat);
	ShaderProgram &shader = *Registry::shaders["ship"];

	shader.bind();

	tp.saveModel();
	tp.identity();

	tp.translation(position);

	samplerMipmap.bind(1);
	samplerMipmap.bind(2);
	samplerMipmap.bind(3);
	samplerMipmap.bind(4);
	Registry::textures["ship-diffuse"]->bind(1);
	Registry::textures["ship-normal"]->bind(2);
	Registry::textures["ship-emit"]->bind(3);
	Registry::textures["ship-specular"]->bind(4);

	shader["u_PvmMatrix"].setMatrix4(tp.getPVMMatrix());
	shader["u_NormalMatrix"].setMatrix3(tp.getNormalMatrix());
	shader["u_ViewMatrix"].setMatrix4(tp.getViewMatrix());
	shader["u_ViewModelMatrix"].setMatrix4(tp.getViewModelMatrix());
	Registry::models["ship"]->draw();

	tp.restoreModel();

	shader.unbind();
}

void Spaceship::applyLookAt(TransformPipeline& tp)
{
	static const float offsetForward = -7;
	static const float offsetUpward = 2;

	Vector3 cameraPosition = position + forward * offsetForward + up * offsetUpward;
	Vector3 centerPosition = position + up * 1;

	tp.lookAt(cameraPosition, centerPosition, up);
}

const Vector3& Spaceship::getPosition() const
{
	return position;
}

Vector3 Spaceship::getSpeed() const
{
	return forward * speed;
}

float Spaceship::getRadius() const
{
	return 1.f;
}
