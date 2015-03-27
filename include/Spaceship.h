#ifndef SPACESHIP_H
#define SPACESHIP_H

#include "maths/Vector.h"

class Spaceship {
public:
	void draw();
	void update(float dt);

	Vector3 getPosition();
	Vector3 getSpeed();
	void applyLookAt(TransformPipeline&);
};

#endif

