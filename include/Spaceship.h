#ifndef SPACESHIP_H
#define SPACESHIP_H

#include "maths/Vector.h"

class TransformPipeline;

class Spaceship {
	Vector3 position;
	float speed;
	float rotationSpeedY;
	float rotationSpeedX;

	Vector3 forward;
	Vector3 up;
	Vector3 right;

public:
	Spaceship();

	void draw(TransformPipeline&);
	void update(float dt);
	void applyLookAt(TransformPipeline&);

	const Vector3& getPosition() const;
	Vector3 getSpeed() const;
	float getRadius() const;
};

#endif

