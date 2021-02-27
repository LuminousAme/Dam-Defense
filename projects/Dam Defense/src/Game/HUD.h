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
	void SetWaveProgress(float waveProgress) { m_waveProgress = waveProgress; }
	void SetGamePaused(bool paused) { m_paused = paused; }
	void SetWave(unsigned wave) { m_currentWave = wave; }
	void SetWaveOver(bool waveOver) { waveDone = waveOver; }
	void SetFlameThrowerCoolDownTime(float cooldownRemaining) { flameThrowerCoolDownTime = cooldownRemaining; }
	void SetFlameThrowerMaxCoolDown(float cooldown) { flameThrowerMaxCoolDownTime = cooldown; }
	void SetFlameThrowerRealCoolDown(float realCoolDownTime) { flameThrowerRealCoolDownTime = realCoolDownTime; }
	//getters
	unsigned GetScore() { return m_score; }
	float GetDamHealth() { return m_DamHealth; }
	float GetWaveProgress() { return m_waveProgress; }
	bool GetGamePaused() { return m_paused; }

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
	//progress bar
	entt::entity progressBar;
	entt::entity progressBarBg;
	entt::entity progressRepresentation;
	glm::vec2 progressScale = glm::vec2(0.6f, 0.25f);
	//wave complete
	entt::entity waveText;
	entt::entity completeText;
	std::vector<entt::entity> waveNums;
	float waveCompleteScale = 2.0f;
	//flamethrower
	entt::entity flameThrowerBG;
	entt::entity flameThrowerOverlay;
	entt::entity flameThrowerBar;
	entt::entity flameThrowerIcon;
	entt::entity flameThrowerKey;
	std::vector<entt::entity> flamethrowerNums;
	float specialAbilityScale = 0.2f;

	//assets
	TTN_Texture2D::st2dptr textureScore;

	//dam health
	float m_DamHealth;
	//progess
	float m_waveProgress;
	float m_displayedWaveProgress;
	//the player's score
	unsigned m_score;
	//if the game is paused
	bool m_paused;
	//the current wave the player is on
	unsigned m_currentWave = 0;

	//wave complete time tracker
	float m_waveCompleteTime = 10.0f;
	float m_waveCompleteTotalTime = 4.0f;
	bool waveDone = false;

	//special ability variables
	float flameThrowerCoolDownTime = 0.0f;
	float flameThrowerRealCoolDownTime = 0.0f;
	float flameThrowerMaxCoolDownTime;
	float flameThrowerCoolDownPercent = 1.0f;

	void MakeScoreNumEntity();
	void MakeHealthNumEntity();
	void MakeWaveNumEntity();
	void MakeFlamethrowerNumEntity();
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

//interpolation parameter for the wave complete pass by lerp
inline float waveCompleteLerpParamter(float t) {
	float paramater = 0;

	if (t < 0.3f) {
		paramater = TTN_Interpolation::ReMap(0.0f, 0.3f, 0.0f, 0.5f, t);
	}
	else if (t >= 0.3f && t <= 0.7f) {
		paramater = 0.5f;
	}
	else {
		paramater = TTN_Interpolation::ReMap(0.7f, 1.0f, 0.5f, 1.0f, t);
	}

	return paramater;
}