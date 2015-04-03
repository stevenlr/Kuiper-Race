#ifndef LEVEL_H
#define LEVEL_H

#include <vector>

#include "maths/Vector.h"
#include "maths/TransformPipeline.h"

#include "Segment.h"
#include "Spaceship.h"

#define MAX_TIME_PER_SEGMENT 30.f
#define CHECKPOINT_RADIUS 500.f

#define SEGMENT_NBR 2

class Level {
	Spaceship ship;
	std::vector<Segment*> segments;
	int currentSegmentIndex;
	float currentSegmentTime;
	float time;
	bool dead;
	bool win;

	void generateRandomDirection(Vector3 & direction) const;
	void generateTurn(Vector3 & direction) const;

public:

	Level();
	~Level();

	float getShipSpeed();

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
