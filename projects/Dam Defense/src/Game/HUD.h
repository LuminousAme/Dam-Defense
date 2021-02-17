//Dam Defense by Atlas X Games
//HUD.h, the header file for the scene hclass representing the UI in the main game
#pragma once

//include required features from titan
#include "Titan/Application.h"
#include "Titan/Utilities/Interpolation.h"

using namespace Titan;

class GameUI : public TTN_Scene {
	friend class Game;
public:
	//default constructor
	GameUI();

	//default destrcutor 
	~GameUI() = default;

	//sets up the scene
	void InitScene();

	//update the scene
	void Update(float deltaTime);

	//setters
	void SetScore(unsigned score) { m_score = score; }
	void SetDamHP(float health) { m_DamHealth = health; }
	//getters
	unsigned GetScore() { return m_score; }
	float GetDamHealth() { return m_DamHealth; }

private:
	//entities
	entt::entity cam;
	//healthbar
	entt::entity healthBar;
	entt::entity healthBarBg;
	//health
	entt::entity healthDam;
	float healthScale = 0.3f;
	//health numbers
	std::vector<entt::entity> healthNums;
	float healthTextScale = 0.4f;
	//score text
	entt::entity scoreText;
	float scoreTextScale = 0.5f;
	//score numbers
	std::vector<entt::entity> scoreNums;

	//assets
	TTN_Texture2D::st2dptr textureScore;

	//dam health
	float m_DamHealth;
	//the player's score
	unsigned m_score;

	void MakeScoreNumEntity();
	void MakeHealthNumEntity();
};

//get the number of digits in a number
inline unsigned GetNumOfDigits(unsigned int number) {
	//if we're on the last digit, return 1
	if (number < 10) {
		return 1;
	}
	//otherwise add 1, remove a digit, and call the function recursively
	else
		return GetNumOfDigits(number / 10) + 1;
}

//gets a given digit from a number
inline unsigned GetDigit(unsigned number, unsigned digit) {
	unsigned divisor = 1;

	for (unsigned i = 0; i < digit; i++)
		divisor *= 10;

	return (number / divisor % 10);
}