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

	float GetVolumeMusic() { return volumeMusic; }
	void SetVolumeMusic(float vol) { volumeMusic = vol; }

	float GetVolumeSFX() { return volumeSFX; }
	void SetVolumeSFX(float vol) { volumeSFX = vol; }

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

	//master volume bar border and bg
	entt::entity volumeBarBorder;
	entt::entity volumeBarBg;
	//master volume bar
	entt::entity volumeBar;
	float volumeScale = 0.3f;//scale of bar
	//master volume numbers
	std::vector<entt::entity> volumeNums;
	float volumeNumScale = 0.4f;

	//sfx volume bar border and bg
	entt::entity SFXvolumeBarBorder;
	entt::entity SFXvolumeBarBg;
	//sfx volume bar
	entt::entity SFXvolumeBar;
	//sfx volume numbers
	std::vector<entt::entity> SFXvolumeNums;

	//music volume bar border and bg
	entt::entity MusicVolumeBarBorder;
	entt::entity MusicVolumeBarBg;
	//music volume bar
	entt::entity MusicVolumeBar;
	//music volume numbers
	std::vector<entt::entity> MusicVolumeNums;

	//volume controls
	float volume;
	float volumeSFX;
	float volumeMusic;

	//assets
	TTN_Texture2D::st2dptr textureOptions;//options text/title
	
	void MakeMouseNumEntity();
	void MakeVolumeNumEntity();
	void MakeMusicNumEntity();
	void MakeSFXNumEntity();

	//should go back to pause
	bool shouldBack;
	//should go back to mainmenu
	bool shouldMenu;
};
