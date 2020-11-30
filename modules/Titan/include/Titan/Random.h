//Titan Engine, by Atlas X Games 
// Random.h - header for the class that gives static templates for random number generation
#pragma once

//import required features
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>

namespace Titan {
	class TTN_Random {
	public:
		//generates a pseudo-random integer between a min and max value
		static int RandomInt(int min, int max);

		//generates a pseudo-random float between a min and max value
		static float RandomFloat(float min, float max);

	private:
		//boolean for checking if it needs to set the seed
		inline static bool firstCall = false;
	};
}
