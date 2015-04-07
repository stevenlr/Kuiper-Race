#include "InputHandler.h"

#include "maths/TransformPipeline.h"
#include "graphics/Registry.h"
#include "graphics/opengl/ShaderProgram.h"
#include "graphics/opengl/Sampler.h"

#include "Spaceship.h"

Spaceship::Spaceship()
{
	restart();
}

void Spaceship::restart() {
	forward = {0, 1, 0};
	up = {0, 0, 1};
	right = {1, 0, 0};
	forwardCam = {0, 1, 0};
	upCam = {0, 0, 1};
	rightCam = {1, 0, 0};
	position = {0, -20, 0};
	speed = 0;
	rotationSpeedY = 0;
	rotationSpeedX = 0;
	rotationSpeedZ = 0;
}

void Spaceship::update(float dt)
{
	static const float acceleration = 70;
	static const float rotationAccelerationY = 5;
	static const float rotationAccelerationX = 5;
	static const float rotationAccelerationZ = 5;
	InputHandler &input = InputHandler::getInstance();

	speed *= 0.96; // Friction in space, suuuure...

	if (input.keyIsDown(InputHandler::SpeedUp)) {
		speed += dt * acceleration;
	}

	if (input.keyIsDown(InputHandler::SpeedDown)) {
		speed -= dt * acceleration;
	}

	rotationSpeedY *= 0.95;
	rotationSpeedX *= 0.95;
	rotationSpeedZ *= 0.90;

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

	if(input.keyIsDown(InputHandler::YawLeft)){
		rotationSpeedZ -= dt * rotationAccelerationZ;
	}

	if(input.keyIsDown(InputHandler::YawRight)){
		rotationSpeedZ += dt * rotationAccelerationZ;
	}

	float rotationY = dt * rotationSpeedY;
	float rotationX = dt * rotationSpeedX;
	float rotationZ = dt * rotationSpeedZ;

	forward = forward * cosf(rotationZ) + right * sinf(rotationZ);
	right = forward.cross(up);

	right = right * cosf(rotationY) + up * sinf(rotationY);
	right.normalize();
	up = right.cross(forward);
	up.normalize();

	forward = forward * cosf(rotationX) + up * sinf(rotationX);
	forward.normalize();
	up = right.cross(forward);
	up.normalize();

	position += forward * speed;

	static const float interpFactor = 0.12;

	forwardCam = forward * interpFactor + forwardCam * (1 - interpFactor);
	forwardCam.normalize();

	rightCam = right * interpFactor + rightCam * (1 - interpFactor);
	rightCam.normalize();

	upCam = up * interpFactor + upCam * (1 - interpFactor);
	upCam.normalize();
}

void Spaceship::draw(TransformPipeline& tp)
{
	static Sampler samplerMipmap(Sampler::MinLinearMipmapLinear, Sampler::MagLinear, Sampler::Repeat);
	ShaderProgram &shader = *Registry::shaders["ship"];

	shader.bind();

	tp.saveModel();
	tp.identity();

	tp.translation(position);
	applyModelMatrix(tp);

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
	static const float offsetUpward = 1;

	Vector3 cameraPosition = position + forwardCam * offsetForward + upCam * offsetUpward;
	Vector3 centerPosition = position + upCam * 1;

	tp.lookAt(cameraPosition, centerPosition, upCam);
}

void Spaceship::applyModelMatrix(TransformPipeline& tp)
{
	tp.setModel(tp.getModelMatrix() * getBaseMatrix());
}

Matrix4 Spaceship::getBaseMatrix()
{
	Matrix4 m;

	for (int i = 0; i < 3; ++i) {
		m(i, 0) = right[i];
		m(i, 1) = forward[i];
		m(i, 2) = up[i];
	}

	return m;
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

float Spaceship::getRotationSpeed() const
{
	return fabs(rotationSpeedX) + fabs(rotationSpeedY);
}
