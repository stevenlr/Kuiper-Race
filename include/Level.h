#ifndef LEVEL_H
#define LEVEL_H

#include <vector>

#include "maths/Vector.h"
#include "maths/TransformPipeline.h"

#include "Segment.h"
#include "Spaceship.h"

#define MAX_TIME_PER_SEGMENT 8.f
#define CHECKPOINT_RADIUS 500.f

#define SEGMENT_NBR 16

class Level {
	Spaceship ship;
	std::vector<Segment*> segments;
	int currentSegmentIndex;
	float currentSegmentTime;
	float currentTime;
	float time;
	bool dead;
	bool win;
	bool hasStarted;

	void generateRandomDirection(Vector3 & direction) const;
	void generateTurn(Vector3 & direction) const;

public:

	Level();
	~Level();

	float getShipSpeed();

	void generate();
	void generate_test();
	void restart();

	void draw(TransformPipeline& tp);
	void drawHUD(int windowWidth, int windowHeight);
	void update(float dt);

	bool shipCollidesWithAsteroids();
	bool shipCollidesWithCheckpoint();

	void die();
	void reachCheckpoint();

	bool isDead() const;
	bool isWin() const;
	bool hasCrashed() const;
};

#endif
