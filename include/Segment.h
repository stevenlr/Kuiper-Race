#ifndef SEGMENT_H
#define SEGMENT_H

#include <vector>

#include "maths/Vector.h"

#include "graphics/opengl/Buffer.h"

#include "Asteroid.h"

#define SEGMENT_LENGTH 11000
#define SEGMENT_HALF_WIDTH 2000
#define ASTEROID_NBR 280

class Segment {
	std::vector<Asteroid> asteroids;
	Vector3 checkpoint;
	Buffer buffer;

	void generatePlaneVectors(const Vector3 &direction,
								Vector3 &planeVector1,
								Vector3 &planeVector2) const;



	void generateAsteroidPosition(Asteroid & currentAsteroid,
									const Vector3 & direction,
									const Vector3 & width,
									const Vector3 & height,
									const Vector3 & start);

	bool detectOneAsteroidCollision(const Asteroid &a, const Asteroid &b) const;
	bool detectAsteroidsCollision(const Asteroid &a, unsigned short aIndex) const;

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

