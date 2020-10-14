//Titan Engine, by Atlas X Games 
// Transform.cpp - source file for the class that defines the transform component 
#include "Titan/Transform.h"

//include required glm functions 
#include "GLM/gtx/transform.hpp"

namespace Titan {
	//default constructor, sets all of the variables to default values 
	TTN_Transform::TTN_Transform()
	{
		m_pos = glm::vec3(0.0f, 0.0f, 0.0f);
		m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
		m_rotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
		Recompute();
	}

	TTN_Transform::~TTN_Transform()
	{}

	//sets the position to the value passed in
	void TTN_Transform::SetPos(glm::vec3 pos)
	{
		//copy the position
		m_pos = pos;
		//recompute the matrix representing the overall transform
		Recompute();
	}

	//sets the scale to the value passed in 
	void TTN_Transform::SetScale(glm::vec3 scale)
	{
		//copy the scale 
		m_scale = scale;
		//recompute the matrix representing the overall transform
		Recompute();
	}

	//sets the rotation to the value passed in 
	void TTN_Transform::SetRot(glm::quat rotation)
	{
		//copy the rotation 
		m_rotation = rotation;
		//recompute the matrix representing the overall transform
		Recompute();
	}

	//returns the position value
	glm::vec3 TTN_Transform::GetPos()
	{
		return m_pos;
	}

	//returns the scale value
	glm::vec3 TTN_Transform::GetScale()
	{
		return m_scale;
	}

	//returns the rotation value 
	glm::quat TTN_Transform::GetRotation()
	{
		return m_rotation;
	}

	//returns the 4x4 matrix representing the combiation of all other elements
	glm::mat4 TTN_Transform::GetMatrix()
	{
		return m_transform;
	}

	void TTN_Transform::Recompute()
	{
		//convert the position, rotation, and scale into their matrix forms and multiplys them together into the overall transform matrix
		m_transform = glm::translate(m_pos) *
			glm::toMat4(m_rotation) *
			glm::scale(m_scale);
	}

}