#include "Asteroid.h"

#include "Logger.h"

Asteroid::~Asteroid()
{
	//Audio::stop(source);
}

void Asteroid::updateSound(const Spaceship &ship)
{
	Vector3 offset = position - ship.getPosition();
	float dist = offset.length();

	//offset.normalize();

	if (source.version > 0) {
		if (dist > 1000) {
			Audio::setGain(source, 0);
			Audio::stop(source);
		} else {
			Audio::setPosition(source, offset * -1);
			Audio::setGain(source, 60 / dist);
		}
	} else {
		if (dist <= 1000) {
			source = Audio::play("asteroid", 1.0, 60 / dist, offset * -1, false);
		}
	}
}
