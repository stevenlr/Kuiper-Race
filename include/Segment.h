#ifndef SEGMENT_H
#define SEGMENT_H

#include <vector>

#include "maths/Vector.h"

#include "graphics/opengl/Buffer.h"

#include "Asteroid.h"

class Segment {
	std::vector<Asteroid> asteroids;
	Vector3 checkpoint;
	Buffer buffer;

public:
	Segment();
	~Segment();

	void generate(Vector3 start, Vector3 end);
	void generate_test(Vector3 start, Vector3 end);

	Vector3 getCheckpoint() const;
	const std::vector<Asteroid>& getAsteroids() const;

	Buffer& getDataBuffer();
};

#endif

