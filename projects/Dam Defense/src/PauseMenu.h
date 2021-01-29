//Dam Defense, by Atlas X Games
//PauseMenu.h, the header file for the class that represents the main menu
#pragma once

//include required features from titan
#include "Titan/Application.h"
#include "Titan/ObjLoader.h"
#include "Titan/Interpolation.h"

using namespace Titan;

class PauseMenu : public TTN_Scene {
public:
	//default constructor
	PauseMenu();

	//default destrcutor 
	~PauseMenu() = default;

	//sets up the scene
	void InitScene();

	//update the scene
	void Update(float deltaTime);

	//mouse input
	void MouseButtonDownChecks();

	bool GetShouldQuit() { return shouldQuit; }
	bool GetShouldResume() { return shouldResume; }
	void SetShouldResume(bool resume) { shouldResume = resume; }

private:
	//entities
	entt::entity cam;
	entt::entity background;
	//resume
	entt::entity resumeButton;
	entt::entity resumeText;
	//options
	entt::entity optionsButton;
	entt::entity optionsText;
	//quit
	entt::entity quitButton;
	entt::entity quitText;

	//assets
	TTN_Texture2D::st2dptr textureBackground;
	TTN_Texture2D::st2dptr texturePaused;
	TTN_Texture2D::st2dptr textureButton1;
	TTN_Texture2D::st2dptr textureButton2;
	TTN_Texture2D::st2dptr textureResume;
	TTN_Texture2D::st2dptr textureOptions;
	TTN_Texture2D::st2dptr textureQuit;

	//should quit the game
	bool shouldQuit;
	bool shouldResume;
};