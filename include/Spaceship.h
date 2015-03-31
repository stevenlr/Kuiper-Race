#ifndef SPACESHIP_H
#define SPACESHIP_H

#include "maths/Vector.h"

class TransformPipeline;

class Spaceship {
	Vector3 position;
	Vector3 speed;

public:
	void draw(TransformPipeline&);
	void update(float dt);
	void applyLookAt(TransformPipeline&);

	const Vector3& getPosition() const;
	const Vector3& getSpeed() const;
	float getRadius() const;
};

#endif

