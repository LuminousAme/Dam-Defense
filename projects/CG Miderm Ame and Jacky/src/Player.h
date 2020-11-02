//INFR 1350 Pratical Midterm - Battle Tanks by Ame Gilham (100741352) and Jackie Zhou (100750922)
//Player.h, header file for the class representing components for player data in tanks
#pragma once

//import required features
#include "GLM/glm.hpp"

class PlayerComponent {
public:
	//default contructor
	PlayerComponent();

	~PlayerComponent() = default;

	//copy, move, and assingment constrcutors for ENTT
	PlayerComponent(const PlayerComponent&) = default;
	PlayerComponent(PlayerComponent&&) = default;
	PlayerComponent& operator=(PlayerComponent&) = default;

	//sets the direction the player is facing
	void SetDirection(glm::vec3 dir);

	//returns the direction the player is facing
	glm::vec3 GetDirection() { return m_Direction; }

private:
	//vector representing the direction the player is currently facing
	glm::vec3 m_Direction;
};
