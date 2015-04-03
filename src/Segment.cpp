#include <random>

#include "Segment.h"
#include <utility.h>

#include <cmath>

Segment::Segment() : buffer(Buffer::Array, Buffer::StaticDraw)
{
	asteroids.resize(ASTEROID_NBR);
}

Segment::~Segment()
{
}

void Segment::generatePlaneVectors(const Vector3 &direction,
									Vector3 &planeVector1,
									Vector3 &planeVector2) const
{
	planeVector1[0] = direction[0];
	planeVector1[1] = direction[1];

	planeVector1[2]= (-planeVector1[0] * planeVector1[0]
						-planeVector1[1] * planeVector1[1]) / direction[2];

	planeVector1.normalize();
	planeVector1 *= SEGMENT_HALF_WIDTH;

	planeVector2 = direction.cross(planeVector1);
	planeVector2.normalize();
	planeVector2 *= SEGMENT_HALF_WIDTH;
}

bool Segment::detectOneAsteroidCollision(const Asteroid &a, const Asteroid &b) const
{
	Vector3 vectDistance = a.position - b.position;
	return vectDistance.length() < a.scale + b.scale;
}

bool Segment::detectAsteroidsCollision(const Asteroid &a, unsigned short aIndex) const
{
	for (int i = 1; i < aIndex; ++i) {
			if (detectOneAsteroidCollision(a, asteroids[i]))
				return true;
	}

	return false;
}

void Segment::generateAsteroidPosition(Asteroid & currentAsteroid,
										 const Vector3 & direction,
										 const Vector3 & width,
										 const Vector3 & height,
										 const Vector3 & start)
{
	currentAsteroid.position = start;
	currentAsteroid.position += direction * Utility::generate0_1Value();
	currentAsteroid.position += width * Utility::generateMinus1_1Value();
	currentAsteroid.position += height * Utility::generateMinus1_1Value();
}




void Segment::generate(Vector3 start, Vector3 end)
{
	checkpoint = end;

	Vector3 direction = end - start,
	width, height;

	generatePlaneVectors(direction, width, height);

	for (int i = 0; i < ASTEROID_NBR; ++i)
	{
		asteroids[i].scale = Utility::generate0_1Value() * (ASTEROID_MAX_RADIUS - ASTEROID_MIN_RADIUS) + ASTEROID_MIN_RADIUS;


		for(int j = 0; j < 3; ++j)
			asteroids[i].rotation[j] = Utility::generateMinus1_1Value() * ASTEROID_MAX_SPEED;

		do {
			generateAsteroidPosition(asteroids[i], direction, width, height, start);
		} while (detectAsteroidsCollision(asteroids[i], i));
	}

	buffer.data(asteroids.size() * sizeof(Asteroid), reinterpret_cast<const void*>(&asteroids[0]));
}

Vector3 Segment::getCheckpoint() const
{
	return checkpoint;
}

void Segment::generate_test(Vector3 start, Vector3 end)
{
	float range = 10.f;

	for (int i = 0; i < 200; ++i) {
		Asteroid ast;
		ast.position = Vector3 {
			start[0] + sinf(rand()) * range,
			start[1] + sinf(rand()) * range,
			start[2] + sinf(rand()) * range
		};

		ast.scale = 0.5 + 2 * ((float) rand()) / RAND_MAX;
		ast.rotation = Vector3 {sinf(rand()), sinf(rand()), sinf(rand())} / 7;
		asteroids.push_back(ast);
	}

	buffer.data(asteroids.size() * sizeof(Asteroid), reinterpret_cast<const void*>(&asteroids[0]));
}

const std::vector<Asteroid>& Segment::getAsteroids() const
{
	return asteroids;
}

Buffer& Segment::getDataBuffer()
{
	return buffer;
}

