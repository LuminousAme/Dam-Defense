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

	float GetMouseSen() { return acutal_mouse_sen; }
	void SetMouseSen(float sensitive) { mouse_sen = sensitive; }

	float GetVolume() { return acutal_volume; }
	void SetVolume(float vol) { volume = vol; }

	float GetVolumeMusic() { return acutal_volumeMuisc; }
	void SetVolumeMusic(float vol) { volumeMusic = vol; }

	float GetVolumeSFX() { return acutal_volumeSFX; }
	void SetVolumeSFX(float vol) { volumeSFX = vol; }

	void SetNoLut(bool lut) { Off = lut; }
	void SetColor1(bool lut) { color = lut; }

	bool GetOff() { return acutal_Off; }
	bool GetColor() { return acutal_color; }

	void SetDiff(float d) { diff = d; }
	float GetDiff() { return acutal_Diff; }

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

	float numScale = 0.4f;

	//mouse sensitivity bar border and bg
	entt::entity mouseBar;
	entt::entity mouseBarBg;
	//mouse sensitivity bar
	entt::entity mouseSensitivity;
	float mouseScale = 0.3f;
	//mouse sensitivity numbers
	std::vector<entt::entity> mouseNums;
	float mouseNumScale = 0.4f;

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

	float buttonScale = 0.3f;//scale of button
	//color correction border and bg
	entt::entity OffBarBorder;
	entt::entity OffBarBg;
	// color correction button
	entt::entity OffBar;

	//color correction border and bg
	entt::entity ColorBarBorder;
	entt::entity ColorBarBg;
	// color correction button
	entt::entity ColorBar;

	/// buttons /////
	//off color correction
	entt::entity buttonOff;
	entt::entity OffText;
	//color blind 1
	entt::entity buttonColor1;
	entt::entity colorText;
	// other colorblind/color correction
	entt::entity buttonColor2;
	entt::entity colorText2;

	//color correction border and bg
	entt::entity diffBarBorder;
	entt::entity diffBarBg;
	// color correction button
	entt::entity diffBar;
	//nums representation
	std::vector<entt::entity> diffNums;

	//easy border and bg
	entt::entity EasyDiffBorder;
	entt::entity EasyDiffBarBg;
	// easy inidicator
	entt::entity EasyDiffBar;

	//regular diff. border and bg
	entt::entity RegDiffBorder;
	entt::entity RegDiffBarBg;
	// regular diff inidicator
	entt::entity RegDiffBar;

	//hard border and bg
	entt::entity HardDiffBorder;
	entt::entity HardDiffBarBg;
	// hard inidicator
	entt::entity HardDiffBar;

	/// difficulty buttons /////
	//off color correction
	entt::entity buttonEasy;
	entt::entity EasyText;
	//color blind 1
	entt::entity buttonReg;
	entt::entity RegText;
	// other colorblind/color correction
	entt::entity buttonHard;
	entt::entity HardText;

	//buttons for applying, undoing, and setting default
	entt::entity buttonApply;
	entt::entity buttonDefault;
	entt::entity buttonUndo;

	//assets
	TTN_Texture2D::st2dptr textureOptions;//options text/title
	TTN_Texture2D::st2dptr textureButton1;
	TTN_Texture2D::st2dptr textureButton2;

	void MakeMouseNumEntity();
	void MakeVolumeNumEntity();
	void MakeMusicNumEntity();
	void MakeSFXNumEntity();
	void MakeDiffNumEntity();

	void WriteToFile();
	void ReadFromFile();

	//settings
	int mouse_sen;
	int volume;
	int volumeSFX;
	int volumeMusic;
	int diff;
	bool Off;
	bool color;
	bool easy;
	bool reg;
	bool hard;

	//acutal settings
	int acutal_mouse_sen;
	int acutal_volume;
	int acutal_volumeSFX;
	int acutal_volumeMuisc;
	int acutal_Diff;
	bool acutal_Off;
	bool acutal_color;
	bool acutal_easy;
	bool acutal_reg;
	bool acutal_hard;

	//should go back to pause
	bool shouldBack;
	//should go back to mainmenu
	bool shouldMenu;

	//the time remaining before it accepts player input, used when the player is moving in and out of the scene
	float m_InputDelay; 
};