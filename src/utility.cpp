#include "utility.h"

float Utility::generate0_1Value()
{
	return ((float) std::rand()) / RAND_MAX;
}

float Utility::generateMinus1_1Value()
{
	return generate0_1Value() * 2 - 1;
}
