#ifndef ASTEROID_H
#define ASTEROID_H

#include "maths/Vector.h"

#define ASTEROID_MAX_SPEED 0.5
#define ASTEROID_MAX_RADIUS 100
#define ASTEROID_MIN_RADIUS 60

struct Asteroid {
	Vector3 position;
	float scale;
	Vector3 rotation;
};

#endif
