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

	Vector3 forwardCam;
	Vector3 upCam;
	Vector3 rightCam;

public:
	Spaceship();
	void restart();

	void draw(TransformPipeline& tp);
	void update(float dt);
	void applyLookAt(TransformPipeline& tp);
	void applyModelMatrix(TransformPipeline& tp);
	Matrix4 getBaseMatrix();

	const Vector3& getPosition() const;
	Vector3 getSpeed() const;
	float getRadius() const;
	float getRotationSpeed() const;
};

#endif

