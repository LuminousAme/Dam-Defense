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
		
		//TTN_Physics();
		~TTN_Physics() = default;


		static void Projections();

		void GetNormals();


		static void Intersects(TTN_Physics b1, TTN_Physics b2);

		// return true if 2 boxs are intersecting
		static bool Inter(TTN_Physics b1, TTN_Physics b2);

		void SetMin(glm::vec3 min);
		void SetMax(glm::vec3 max);

		glm::vec3 GetCenter() { return center; };
		glm::vec3 GetMin() { return min; };
		glm::vec3 GetMax() { return max; };

		glm::vec3 center; //center of object
		glm::vec3 min; //corner of smallest  coord
		glm::vec3 max; //corner of largest coord

		glm::vec3 axis;
		glm::vec3 normals;
		glm::vec3 points;



	};

}

