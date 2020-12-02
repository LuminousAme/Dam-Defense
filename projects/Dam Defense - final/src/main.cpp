//Dam Defense, by Atlas X Games
//main.cpp, the source file that runs the game

//import required titan features
#include "Titan/Application.h"
//include the other headers in dam defense
#include "Game.h"

using namespace Titan;

//main function, runs the program
int main() { 
	Logger::Init(); //initliaze otter's base logging system
	TTN_Application::Init("Dam Defense", 1920, 1080); //initliaze titan's application

	//lock the cursor while focused in the application window
	TTN_Application::TTN_Input::SetCursorLocked(true);

	//create the scenes
	TTN_Scene* gameScene = new Game;

	//initliaze them
	gameScene->InitScene();

	//add them to the application
	TTN_Application::scenes.push_back(gameScene);

	//while the application is running
	while (!TTN_Application::GetIsClosing()) {
		//update the scenes and render the screen
		TTN_Application::Update();
	}
	
	//when the application has ended, exit the program with no errors
	return 0; 
} 