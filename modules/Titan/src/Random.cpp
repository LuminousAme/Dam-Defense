//Titan Engine, by Atlas X Games 
// Random.cpp - source file for the class that gives static templates for random number generation

#include "Titan/Random.h"

namespace Titan {
	//generates a pseudo-random integer between a min and max value
	int TTN_Random::RandomInt(int min, int max)
	{

		if (firstCall) {
			srand(time(NULL));
			firstCall = false;
		}

		int randomInt = rand() / RAND_MAX;

		//convert to range
		int intInRange = (randomInt * (max - min)) + min;
		//handle if the range is zero
		if (max - min == 0) intInRange = min;
		//return converted number
		return intInRange;

	}

	//generates a pseudo-random float between a min and max value
	float TTN_Random::RandomFloat(float min, float max)
	{
		if (firstCall) {
			srand(time(NULL));
			firstCall = false;
		}

		float randomFloat = (float)(rand()) / (float)(RAND_MAX);

		//convert to range
		float floatInRange = (randomFloat * (max - min)) + min;
		//handle if the range is zero
		if (max - min == 0.0f) floatInRange = min;
		//return converted number
		return floatInRange;
	}
}