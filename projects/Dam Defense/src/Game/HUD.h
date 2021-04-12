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

	//restart the scene
	void RestartData();

	//update the scene
	void Update(float deltaTime);

	//mouse input
	void MouseButtonDownChecks();
	void KeyDownChecks();

	void UpdatePrices();

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
	void SetBirdBombCoolDownTime(float cooldownRemaining) { birdBombCoolDownTime = cooldownRemaining; }
	void SetBirdBombMaxCoolDown(float cooldown) { birdBombMaxCoolDownTime = cooldown; }
	void SetBirdBombRealCoolDown(float realCoolDownTime) { birdBombRealCoolDownTime = realCoolDownTime; }
	void SetArcade(bool arcade) { m_arcade = arcade; }
	//getters
	unsigned GetScore() { return m_score; }
	float GetDamHealth() { return m_DamHealth; }
	float GetWaveProgress() { return m_waveProgress; }
	bool GetGamePaused() { return m_paused; }

	void SetInputDelay(float delay) { m_InputDelay = delay; }
	//shop stuff
	bool GetShouldShop() { return shouldShop; }
	void SetShouldShop(bool shop) { shouldShop = shop; }
	bool GetShouldShopping() { return shopping; }
	void SetShouldShopping(bool shop) { shopping = shop; }
	bool GetWaveChange() { return waveChange; }
	void SetWaveChange(bool wave) { waveChange = wave; }
	//shop powers
	int GetHealCounter() { return healCounter; }
	void SetHealCounter(int heal) { healCounter = heal; }

	bool GetCannonPower() { return cannonPower; }
	void SetCannonPower(bool power) { cannonPower = power; }

	bool GetAbilityBuff() { return abilityCooldownBuff; }

	bool GetShouldExit() { return shouldExitShop; }
	void SetShouldExit(bool exit) { shouldExitShop = exit; }

	void SetLastWave(int wave) { lastWave = wave; }

	bool GetUpgradeBuff() { return upgradeAbilities; }

	void SetHealCost(int cost) { healCost = cost; }
	void SetCannonCost(int cost) { cannonCost = cost; }
	void SetCooldownCost(int cost) { cooldownCost = cost; }
	void SetUpgradeCost(int cost) { upgradeCost = cost; }

private:
#pragma region ENTITES AND STUFF
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
	float healthTextScale = 0.45f;

	//crosshair
	entt::entity crosshairCross;
	entt::entity crosshairVertLine;
	std::vector<std::pair<entt::entity, float>> crosshairHoriLines;
	float crosshairScale = 1.25f;
	glm::vec4 crosshairColor = glm::vec4(glm::vec3(0.0f), 1.0f);
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
	//bird bomb
	entt::entity birdBombBG;
	entt::entity birdBombOverlay;
	entt::entity birdBombBar;
	entt::entity birdBombIcon;
	entt::entity birdBombKey;
	std::vector<entt::entity> birdBombNums;
	//special ability general data
	float specialAbilityScale = 0.2f;

#pragma endregion

	//shop buttons
	entt::entity buttonHealth;
	entt::entity buttonCannon;
	entt::entity buttonAbilityCD;
	entt::entity buttonUpgrade;
	entt::entity shop;
	entt::entity Johnson;
	entt::entity buttonContinue;
	//entt::entity background;

	//assets
	TTN_Texture2D::st2dptr textureScore;
	TTN_Texture2D::st2dptr textureShop;//shop text/title
	TTN_Texture2D::st2dptr textureJohnson;//shop bird
	TTN_Texture2D::st2dptr textureHealButton1;
	TTN_Texture2D::st2dptr textureHealButton2;
	TTN_Texture2D::st2dptr textureFiringButton1;
	TTN_Texture2D::st2dptr textureFiringButton2;
	TTN_Texture2D::st2dptr textureCooldownButton1;
	TTN_Texture2D::st2dptr textureCooldownButton2;
	TTN_Texture2D::st2dptr textureUpgradeButton1;
	TTN_Texture2D::st2dptr textureUpgradeButton2;
	TTN_Texture2D::st2dptr textureContinue1;
	TTN_Texture2D::st2dptr textureContinue2;
	//price num enitites 
	std::vector<entt::entity> healPriceNums;
	std::vector<entt::entity> cannonPriceNums;
	std::vector<entt::entity> cooldownPriceNums;
	std::vector<entt::entity> upgradePriceNums;

	//arcade mode
	bool m_arcade;
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

	int lastWave; //last wave of the game

		//shop stuff
	bool shouldExitShop; //exit bool
	bool lerpAway; //lerp away bool
	bool shopOnce = false; //variable for only spawning the shop once per end of round

	float lerpTime = 10.0f;
	float lerpTotalTime = 4.0f;
	float lerpTime2 = 10.0f;
	float lerpTotalTime2 = 4.0f;

	bool shouldShop = false;
	bool shopPause = false;
	bool shopping = false;
	bool waveChange;
	int waveTracker = 0;
	//shop costs
	float healCost;
	int cannonCost;
	int cooldownCost;
	int upgradeCost;

	int healCounter;  //how many times to heal
	bool healOnce; // bool to keep track of whether the player can buy the heal

	bool cannonPower;//bool for cannon powerup

	//bool for whether faster ability cooldowns from the shop is active
	bool abilityCooldownBuff;

	//bool for the ability upgrade option
	bool upgradeAbilities;

	//the time remaining before it accepts player input, used for the shop
	float m_InputDelay = 0.3f;

	//special ability variables
	//flamethrower
	float flameThrowerCoolDownTime = 0.0f;
	float flameThrowerRealCoolDownTime = 0.0f;
	float flameThrowerMaxCoolDownTime;
	float flameThrowerCoolDownPercent = 1.0f;
	//bird bomb
	float birdBombCoolDownTime = 0.0f;
	float birdBombRealCoolDownTime = 0.0f;
	float birdBombMaxCoolDownTime;
	float birdBombCoolDownPercent = 1.0f;

	void MakeScoreNumEntity();
	void MakeHealthNumEntity();
	void MakeWaveNumEntity();
	void MakeFlamethrowerNumEntity();
	void MakeBirdBombNumEntity();
	void MakeHealPriceNumEntity();
	void MakeCannonPriceNumEntity();
	void MakeCooldownPriceNumEntity();
	void MakeUpgradePriceNumEntity();

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