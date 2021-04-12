//Titan Engine, by Atlas X Games 
// Random.cpp - source file for the class that gives static templates for random number generation

//precompile header, this file uses stdio.h, stdlib.h, and time.h
#include "Titan/ttn_pch.h"
//include the header
#include "Titan/Utilities/Random.h"

namespace Titan {
	//generates a pseudo-random integer between a min and max value
	int TTN_Random::RandomInt(int min, int max)
	{
		if (firstCall) {
			srand(time(NULL));
			firstCall = false;
		}
		
		//generate number
		int randomInt = rand() % (max - min + 1) + min;
		//return number
		return randomInt;
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