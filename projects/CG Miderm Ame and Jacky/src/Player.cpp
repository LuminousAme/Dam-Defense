//INFR 1350 Pratical Midterm - Battle Tanks by Ame Gilham (100741352) and Jackie Zhou (100750922)
//Player.cpp, source file for the class representing components for player data in tanks

//include the header
#include "Player.h"

//default constructor
PlayerComponent::PlayerComponent()
{
	m_Direction = glm::vec3(0.0f, 0.0f, 1.0f);
}


//sets the direction the player is facing
void PlayerComponent::SetDirection(glm::vec3 dir)
{
	m_Direction = dir;
}
