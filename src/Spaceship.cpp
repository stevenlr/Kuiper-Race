#include "InputHandler.h"

#include "maths/TransformPipeline.h"

#include "Spaceship.h"

void Spaceship::update(float dt)
{
	InputHandler &input = InputHandler::getInstance();

	//if (input.keyIsDown(InputHandler::SpeedUp)) {
	//	augmenter la vitesse
	//}

	// etc.
}

void Spaceship::draw(TransformPipeline& tp)
{
}

void Spaceship::applyLookAt(TransformPipeline& tp)
{
	static Camera camera({-3, 0.5, 0.5});
	camera.update();
	tp.lookAt(camera);
}

const Vector3& Spaceship::getPosition() const
{
	return position;
}

const Vector3& Spaceship::getSpeed() const
{
	return speed;
}

float Spaceship::getRadius() const
{
	return 1.f;
}

