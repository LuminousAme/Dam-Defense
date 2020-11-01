#pragma once
//include glm features
#include "GLM/glm.hpp"
//import other required features
#include <vector>

namespace Titan {
	class TTN_Physics
	{
	public:

		//default constructor
		TTN_Physics();

		TTN_Physics(glm::vec3 min, glm::vec3 max);
		TTN_Physics(glm::vec3 c, glm::vec3 min, glm::vec3 max);
		~TTN_Physics() = default;

		static void Intersects(TTN_Physics b1, TTN_Physics b2);

		static bool Inter(TTN_Physics b1, TTN_Physics b2);

		void SetCenter(glm::vec3 c);
		void SetMin(glm::vec3 min);
		void SetMax(glm::vec3 max);

		glm::vec3 GetCenter();
		glm::vec3 GetMin();
		glm::vec3 GetMax();

		glm::vec3 center; //center of object
		glm::vec3 min; //corner of smallest  coord
		glm::vec3 max; //corner of largest coord
	};

}