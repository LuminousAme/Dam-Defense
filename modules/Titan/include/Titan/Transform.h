//Titan Engine, by Atlas X Games 
// Transform.h - header for the class that defines the transform component 
#pragma once

//include require GLM features 
#define GLM_ENABLE_EXPERIMENTAL
#include "GLM/glm.hpp"
#include "GLM/gtx/quaternion.hpp"
//include other required features
#include <vector>

namespace Titan {

	//transform class, defines the transform component 
	class TTN_Transform {
	public:
		//constructor 
		TTN_Transform();

		//constructor that takes all the data and makes a transform out of it
		TTN_Transform(glm::vec3 pos, glm::vec3 rotation, glm::vec3 scale, TTN_Transform* parent = nullptr);

		//destructor 
		~TTN_Transform();

		//SETTERS  
		//position 
		void SetPos(glm::vec3 pos);
		//scale
		void SetScale(glm::vec3 scale);
		//rotation
		void SetRotationQuat(glm::quat rotationQuat);
		//parent
		void SetParent(TTN_Transform* parent);


		//GETTERS
		//position
		glm::vec3 GetPos();
		//scale 
		glm::vec3 GetScale();
		//rotation
		glm::vec3 GetRotation();
		glm::quat GetRotQuat() { return m_rotation; }
		//transform matrix 
		glm::mat4 GetMatrix();
		//global transform matrix
		glm::mat4 GetGlobal();
		//parent 
		TTN_Transform* GetParent();

		//rotates by inputed value
		void RotateRelative(glm::vec3 rotation);
		void RotateFixed(glm::vec3 rotation);

		//rotates to towards a certain point
		void LookAt(glm::vec3 target, glm::vec3 up);
		//rotates to look towards along a certain direction
		void LookAlong(glm::vec3 direction, glm::vec3 up);

	protected:
		//function that will recalculate the transformation matrix, call from the setters (so whenever a change is made to the object) 
		void Recompute();

		//functions to add or remove children (handled by setParent)
		void AddChild(TTN_Transform* child);
		void RemoveChild(TTN_Transform* child);

	private:
		/// Hierararchy ///
		TTN_Transform* m_Parent;
		std::vector<TTN_Transform*> m_Children;

		/// LOCAL /// 
		//stores the position
		glm::vec3 m_pos; 
		//stores the scale
		glm::vec3 m_scale; 
		//stores the rotation
		glm::quat m_rotation;
		
		//stores a 4x4 matrix that represents the combination of all the above elements
		glm::mat4 m_transform;

		/// GLOBAL ///
		glm::mat4 m_global; //stores a 4x4 matrix that represents the transform of the object relative to global space, used for rendering
	};
}