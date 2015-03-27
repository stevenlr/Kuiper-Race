#ifndef LEVEL_H
#define LEVEL_H

#include <vector>

#include "maths/Vector.h"
#include "maths/TransformPipeline.h"

#include "Segment.h"

class Level {
	float time;
	std::vector<Segment*> segments;

public:
	void generate();
	void generate_test();

	int getSegmentCount() const;
	int getCurrentSegment() const;
	Vector3 getCheckpoint(int i) const;

	void draw(TransformPipeline& tp);
	void update(float dt);

	bool isDead();
	void reachCheckpoint();
};

#endif
