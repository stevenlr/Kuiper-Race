#ifndef LEVEL_H
#define LEVEL_H

#include <vector>

#include "maths/Vector.h"
#include "maths/TransformPipeline.h"

#include "Segment.h"
#include "Spaceship.h"

#define MAX_TIME_PER_SEGMENT 12.f
#define CHECKPOINT_RADIUS 5.f

class Level {
	Spaceship ship;
	std::vector<Segment*> segments;
	int currentSegmentIndex;
	float currentSegmentTime;
	float time;
	bool dead;
	bool win;

public:
	Level();

	void generate();
	void generate_test();

	int getSegmentCount() const;
	int getCurrentSegment() const;
	Vector3 getCheckpoint(int i) const;

	void draw(TransformPipeline& tp);
	void drawHUD();
	void update(float dt);

	bool shipCollidesWithAsteroids();
	bool shipCollidesWithCheckpoint();

	void die();
	void reachCheckpoint();

	bool isDead() const;
	bool isWin() const;
};

#endif
