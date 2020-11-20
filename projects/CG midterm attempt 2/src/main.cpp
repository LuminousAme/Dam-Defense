//INFR 1350 Pratical Midterm - Battle Tanks by Ame Gilham (100741352) and Jackie Zhou (100750922)
//The project makes use of the Titan Engine, the engine we developed together through this class and GDW (team: Atlas X Games)
//all classes with the prefix TTN_ are classes from the Titan engine

//main.cpp the source code file to run the program

//include the scenes (they already include titan features)
#include "TanksScene.h"

using namespace Titan;

int main() { 
	//set up warning and error logging
	Logger::Init();
	//set up titan
	TTN_Application::Init("CG midterm, Ame Gilham and Jackie Zhou", 1920, 1080);

	//create a new scene
	TanksScene* game = new TanksScene;

	//set up that scene
	game->InitScene();

	//add that scene to the application
	TTN_Application::scenes.push_back(game);

	/// Game Loop ///
	while (!TTN_Application::GetIsClosing()) {
		//update all the scenes and render to the screen
		TTN_Application::Update();
	}
	
	//exit the program with no errors
	return 0; 
} 