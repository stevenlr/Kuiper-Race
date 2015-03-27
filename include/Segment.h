#ifndef SEGMENT_H
#define SEGMENT_H

#include "maths/Vector.h"
#include "Asteroid.h"

class Segment {
	std::vector<Asteroid> asteroids;
	Vector3 checkpoint;

public:
	void generate(Vector3 start, Vector3 end);

	Vector3 getCheckpoint() const;
	int getAsteroidCount() const;
	const Asteroid& getAsteroid(int i) const;
};

#endif

