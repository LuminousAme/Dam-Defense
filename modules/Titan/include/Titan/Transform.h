//Titan Engine, by Atlas X Games 
// Transform.h - header for the class that defines the transform component 
#pragma once

//include require GLM features 
#define GLM_ENABLE_EXPERIMENTAL
#include "GLM/glm.hpp"
#include "GLM/gtx/quaternion.hpp"

namespace Titan {

	//transform class, defines the transform component 
	class TTN_Transform {
	public:
		//constructor 
		TTN_Transform();

		//destructor 
		~TTN_Transform();

		//SETTERS  
		//position 
		void SetPos(glm::vec3 pos);
		//scale
		void SetScale(glm::vec3 scale);
		//rotation
		void SetRot(glm::vec3 rotation);

		//GETTERS
		//position
		glm::vec3 GetPos();
		//scale 
		glm::vec3 GetScale();
		//rotation
		glm::vec3 GetRotation();
		//transform matrix 
		glm::mat4 GetMatrix();

	private:
		//function that will recalculate the transformation matrix, call from the setters (so whenever a change is made to the object) 
		void Recompute();

	private:
		//stores the position
		glm::vec3 m_pos; 
		//stores the scale
		glm::vec3 m_scale; 
		//stores the rotation
		glm::quat m_rotation;
		//stores a 4x4 matrix that represents the combination of all the above elements, used for rendering 
		glm::mat4 m_transform;
	};
}