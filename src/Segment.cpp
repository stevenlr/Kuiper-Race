#include "Segment.h"

Segment::Segment() :
	buffer(Buffer::Array, Buffer::StaticDraw)
{
}

Segment::~Segment()
{
}

void Segment::generate_test(Vector3 start, Vector3 end)
{
	float range = 30.f;
	for (int i = 0; i < 200; ++i) {
		Asteroid ast;
		ast.position = Vector3 {
			sinf(rand()) * range,
			sinf(rand()) * range,
			sinf(rand()) * range
		};
		ast.scale = 0.5 + 2 * ((float) rand()) / RAND_MAX;
		ast.rotation = Vector3 { sinf(rand()), sinf(rand()), sinf(rand()) } / 7;
		asteroids.push_back(ast);
	}

	buffer.data(asteroids.size() * sizeof(Asteroid),
				reinterpret_cast<const void*>(&asteroids[0]));
}

const std::vector<Asteroid>& Segment::getAsteroids() const
{
	return asteroids;
}

Buffer& Segment::getDataBuffer()
{
	return buffer;
}

