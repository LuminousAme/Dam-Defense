//Dam Defense, by Atlas X Games
//LoadingScene.h, the header file for the class that represents the loading screens in the game
#pragma once

//include required features from titan
#include "Titan/Application.h"
#include "Titan/Utilities/Interpolation.h"

using namespace Titan;

class LoadingScene : public TTN_Scene {
public:
	//default constructor
	LoadingScene();

	//default destrcutor 
	~LoadingScene() = default;

	//sets up the scene
	void InitScene();

	//update the scene
	void Update(float deltaTime);

private:
	//entities
	entt::entity cam;
	entt::entity bg;
	entt::entity loadingText;
	entt::entity loadingCircle;

	//assets
	TTN_Texture2D::st2dptr textureForLoadingText;
	TTN_Texture2D::st2dptr textureForLoadingCircle;
	TTN_Texture2D::st2dptr bgText;
};
