//Titan Engine, by Atlas X Games 

//precompile header, this file uses GLM/gtx/transform.hpp
#include "Titan/ttn_pch.h"

// Transform.cpp - source file for the class that defines the transform component 
#include "Titan/Utilities/Transform.h"

namespace Titan {
	//default constructor, sets all of the variables to default values 
	TTN_Transform::TTN_Transform()
	{
		m_pos = glm::vec3(0.0f, 0.0f, 0.0f);
		m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
		m_rotation = glm::quat(glm::radians(glm::vec3(0.0f, 0.0f, 0.0f)));
		m_Parent = nullptr;
		m_parentEntity = nullptr;
		m_Children = std::vector<TTN_Transform*>();
		Recompute();
	}

	//constructor that takes all the data and makes a transform out of it
	TTN_Transform::TTN_Transform(glm::vec3 pos, glm::vec3 rotation, glm::vec3 scale, TTN_Transform* parent, entt::entity* parentEntity)
	{
		m_pos = pos;
		m_rotation = glm::quat(glm::radians(rotation));
		m_scale = scale;

		m_Parent = parent;
		m_parentEntity = parentEntity;
		m_Children = std::vector<TTN_Transform*>();

		Recompute();
	}

	TTN_Transform::~TTN_Transform()
	{
		SetParent(nullptr, nullptr);
	}

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

	void TTN_Transform::SetRotationQuat(glm::quat rotationQuat)
	{
		//copy the rotation
		m_rotation = rotationQuat;
		//recompute the matrix represneting the overall transform
		Recompute();
	}

	//sets the transform that acts as this object's parent
	void TTN_Transform::SetParent(TTN_Transform* parent, entt::entity* parentEntity)
	{
		//if this tranform already had a parent
		if (m_Parent != nullptr) {
			//remove this object from it's child list
			m_Parent->RemoveChild(this);
		}

		//set the new parent
		m_Parent = parent;
		m_parentEntity = parentEntity;

		if (m_Parent != nullptr) {
			//add this object as a child of the new parent
			m_Parent->AddChild(this);
			Recompute();
		}

	}

	//returns the position value
	glm::vec3 TTN_Transform::GetPos()
	{
		return m_pos;
	}

	glm::vec3 TTN_Transform::GetGlobalPos()
	{
		return m_global * glm::vec4(0,0,0,1);
	}

	//returns the scale value
	glm::vec3 TTN_Transform::GetScale()
	{
		return m_scale;
	}

	glm::vec3 TTN_Transform::GetRotation()
	{
		return glm::vec3(glm::degrees(glm::eulerAngles(m_rotation)));
	}

	//returns the 4x4 matrix representing the combiation of all other elements
	glm::mat4 TTN_Transform::GetMatrix()
	{
		return m_transform;
	}

	//returns the 4x4 matrix reprensenting the combination of all other elements in global space
	glm::mat4 TTN_Transform::GetGlobal()
	{
		return m_global;
	}

	//returns a pointer to this object's parent
	TTN_Transform* TTN_Transform::GetParent()
	{
		return m_Parent;
	}

	void TTN_Transform::RotateRelative(glm::vec3 rotation)
	{
		m_rotation = m_rotation * glm::quat(glm::radians(rotation));
		Recompute();
	}

	void TTN_Transform::RotateFixed(glm::vec3 rotation)
	{
		m_rotation = glm::quat(glm::radians(rotation)) * m_rotation;
		Recompute();
	}

	void TTN_Transform::LookAt(glm::vec3 target, glm::vec3 up)
	{
		m_rotation = glm::quatLookAt(target - m_pos, up);
		Recompute();
	}

	void TTN_Transform::LookAlong(glm::vec3 direction, glm::vec3 up)
	{
		m_rotation = glm::quatLookAt(direction, up);
		Recompute();
	}

	void TTN_Transform::Recompute()
	{
		//convert the position, rotation, and scale into their matrix forms and multiplys them together into the overall local transform matrix
		m_transform = glm::translate(m_pos) *
			glm::toMat4(m_rotation) *
			glm::scale(m_scale);

		//check if the object has a parent
		if (m_Parent != nullptr) {
			//if it does then use it to caculate the global transformation matrix
			m_global = m_Parent->GetGlobal() * m_transform;

			//repeat on all the children (this is will do forward kinematics) 
			for (int i = 0; i < m_Children.size(); i++) {
				m_Children[i]->Recompute();
			}
		}
		else {
			//if it doesn't then the global and local transforms must be the same
			m_global = m_transform;
		}
	}

	//adds a child to this object
	void TTN_Transform::AddChild(TTN_Transform* child)
	{
		m_Children.push_back(child);
	}

	//removes a child from the object
	void TTN_Transform::RemoveChild(TTN_Transform* child)
	{
		//go through all the children of the object
		for (int i = 0; i < m_Children.size(); i++) {
			//and check for the one that matches the given child
			if (m_Children[i] == child) {
				//when it's found erase it from the vector and break
				m_Children.erase(m_Children.begin() + i);
			}
		}
	}
}