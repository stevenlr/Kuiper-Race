#ifndef LEVEL_H
#define LEVEL_H

#include <vector>

#include "maths/Vector.h"
#include "Segment.h"

class Level {
	std::vector<Segment> segments;

public:
	void generate();
	int getSegmentCount() const;
	int getCurrentSegment() const;
	Vector3 getCheckpoint(int i) const;

	void draw();
	void update(float dt);

	bool isDead();
	void reachCheckpoint();
};

#endif
