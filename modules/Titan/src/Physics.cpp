#include "Titan/Physics.h"
#include <iostream>

namespace Titan {
	

	TTN_Physics::TTN_Physics()
	{
		center = glm::vec3(0.0f, 0.0f, 0.f);
		min = glm::vec3(-1.0f, -1.0f, -1.f);
		max = glm::vec3(1.0f, 1.0f, 1.f);
	}

	TTN_Physics::TTN_Physics(glm::vec3 minimum, glm::vec3 maximum)
	{
		min = minimum;
		max = maximum;
	}

	Titan::TTN_Physics::TTN_Physics(glm::vec3 c, glm::vec3 minimum, glm::vec3 maximum)
	{
		center = c;
		min = minimum;
		max = maximum;
	}

	void TTN_Physics::Projections()
	{


	}

	void TTN_Physics::GetNormals()
	{
		


	}

	void TTN_Physics::Intersects(TTN_Physics b1, TTN_Physics b2)
	{
		if ((b1.min.x <= b2.max.x && b1.max.x >= b2.min.x) &&
			(b1.min.y <= b2.max.y && b1.max.y >= b2.min.y) &&
			(b1.min.z <= b2.max.z && b1.max.z >= b2.min.z))
		{

			std::cout << "Hello" << std::endl;
		}

	}

	bool TTN_Physics::Inter(TTN_Physics b1, TTN_Physics b2)
	{
		/*if ((b1.min.x >= b2.min.x && b1.max.x <= b2.max.x) &&
			(b1.min.y >= b2.min.y && b1.max.y <= b2.max.y) &&
			(b1.min.z >= b2.min.z && b1.max.z <= b2.max.z))*/
		if ((b1.min.x <= b2.max.x && b1.max.x >= b2.min.x) &&
			(b1.min.y <= b2.max.y && b1.max.y >= b2.min.y) &&
			(b1.min.z <= b2.max.z && b1.max.z >= b2.min.z))
		{
			return true;
		}

		else
			return false;
	}

	void TTN_Physics::SetMin(glm::vec3 m)
	{
		min = m;
	}

	void TTN_Physics::SetMax(glm::vec3 m)
	{
		max = m;
	}






}