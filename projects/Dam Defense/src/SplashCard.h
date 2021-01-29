//Dam Defense, by Atlas X Games
//Splash.h, the header file for the class that represents the team logo splash card at the beginning of the game
#pragma once

//include required features from titan
#include "Titan/Application.h"
#include "Titan/ObjLoader.h"
#include "Titan/Interpolation.h"

using namespace Titan;

class SplashCard : public TTN_Scene {
public:
	//default constructor
	SplashCard();

	//default destrcutor 
	~SplashCard() = default;

	//sets up the scene
	void InitScene();

	//update the scene
	void Update(float deltaTime);

	//get the total scene time
	float GetTotalSceneTime() { return totalSceneTime; }

private:
	//total time 
	float totalSceneTime;

	//entities
	entt::entity cam;
	entt::entity logo;
	entt::entity background;

	//assets
	TTN_Texture2D::st2dptr logoText;
	TTN_Texture2D::st2dptr bgText;
};