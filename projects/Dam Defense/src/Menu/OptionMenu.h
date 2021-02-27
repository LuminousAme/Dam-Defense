//Dam Defense, by Atlas X Games
//OptionMenu.h, the header file for the class that represents the options menu
#pragma once

//include required features from titan
#include "Titan/Application.h"
#include "Titan/Utilities/Interpolation.h"
#include "Game/HUD.h"

using namespace Titan;

class OptionsMenu : public TTN_Scene {
public:
	//default constructor
	OptionsMenu();

	//default destructor
	~OptionsMenu() = default;

	//sets up the scene
	void InitScene();

	//update the scene
	void Update(float deltaTime);

	//mouse input
	void MouseButtonDownChecks();
	void KeyDownChecks();

	float GetMouseSen() { return mouse_sen; }
	void SetMouseSen(float sensitive) { mouse_sen = sensitive; }

	float GetVolume() { return volume; }
	void SetVolume(float vol) { volume = vol; }


	bool GetShouldBack() { return shouldBack; }
	//bool GetShouldResume() { return shouldResume; }
	bool GetShouldMenu() { return shouldMenu; }

	//void SetShouldResume(bool resume) { shouldResume = resume; }
	void SetShouldMenu(bool menu) { shouldMenu = menu; }
	void SetShouldBack(bool back) { shouldBack = back; }

private:
	//entities
	entt::entity cam;
	entt::entity background;

	//mouse sensitivity bar border and bg
	entt::entity mouseBar;
	entt::entity mouseBarBg;
	//mouse sensitivity bar
	entt::entity mouseSensitivity;
	float mouseScale = 0.3f;
	//mouse sensitivity numbers
	std::vector<entt::entity> mouseNums;
	float mouseNumScale = 0.4f;

	float mouse_sen;

	//mouse sensitivity bar border and bg
	entt::entity volumeBarBorder;
	entt::entity volumeBarBg;
	//mouse sensitivity bar
	entt::entity volumeBar;
	float volumeScale = 0.3f;
	//mouse sensitivity numbers
	std::vector<entt::entity> volumeNums;
	float volumeNumScale = 0.4f;

	float volume;


	//assets
	TTN_Texture2D::st2dptr textureOptions;
	//TTN_Texture2D::st2dptr texturePaused;
	//TTN_Texture2D::st2dptr textureSensitivity;

	void MakeMouseNumEntity();
	void MakeVolumeNumEntity();


	//should go back to pause
	bool shouldBack;
	//should go back to mainmenu
	bool shouldMenu;
};

//
////get the number of digits in a number
//inline unsigned GetNumOfDigits(unsigned int number) {
//	//if we're on the last digit, return 1
//	if (number < 10) {
//		return 1;
//	}
//	//otherwise add 1, remove a digit, and call the function recursively
//	else
//		return GetNumOfDigits(number / 10) + 1;
//}
//
////gets a given digit from a number
//inline unsigned GetDigit(unsigned number, unsigned digit) {
//	unsigned divisor = 1;
//
//	for (unsigned i = 0; i < digit; i++)
//		divisor *= 10;
//
//	return (number / divisor % 10);
//}
