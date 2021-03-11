//Dam Defense, by Atlas X Games
//shopmenu.h, the header file for the class that represents the shop
#pragma once

//include required features from titan
#include "Titan/Application.h"
#include "Titan/Utilities/Interpolation.h"
#include "Game/HUD.h"

using namespace Titan;

class ShopMenu : public TTN_Scene {
public:
	//default constructor
	ShopMenu();

	//default destructor
	~ShopMenu() = default;

	//sets up the scene
	void InitScene();

	//update the scene
	void Update(float deltaTime);

	//mouse input
	void MouseButtonDownChecks();
	void KeyDownChecks();

	
	bool GetShouldShop() { return shouldShop; };
	bool GetShouldBack() { return shouldBack; };

	void SetShouldShop(bool menu) { shouldShop = menu; }
	void SetShouldBack(bool back) { shouldBack = back; }

private:
	//entities
	entt::entity cam;
	entt::entity background;

	entt::entity buttonHealth;


	//assets
	TTN_Texture2D::st2dptr textureShop;//options text/title
	TTN_Texture2D::st2dptr textureButton1;
	TTN_Texture2D::st2dptr textureButton2;

	//should go back to game
	bool shouldBack;
	//should go to shop menu
	bool shouldShop;

};
