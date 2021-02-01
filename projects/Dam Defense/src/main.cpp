//Dam Defense, by Atlas X Games
//main.cpp, the source file that runs the game

//import required titan features
#include "Titan/Application.h"
//include the other headers in dam defense
#include "Game.h"
#include "SplashCard.h"
#include "LoadingScene.h"
#include "MainMenu.h"
#include "PauseMenu.h"

using namespace Titan;

//asset setup function
void PrepareAssetLoading();

//main function, runs the program
int main() { 
	Logger::Init(); //initliaze otter's base logging system
	TTN_Application::Init("Dam Defense", 1920, 1080); //initliaze titan's application

	//data to track loading progress
	bool set1Loaded = false;
	bool set2Loaded = false;

	//lock the cursor while focused in the application window
	TTN_Application::TTN_Input::SetCursorLocked(false);

	//prepare the assets
	PrepareAssetLoading();

	//load set 0 assets
	TTN_AssetSystem::LoadSetNow(0);

	//create the scenes
	SplashCard* splash = new SplashCard;
	LoadingScene* loadingScreen = new LoadingScene;
	Game* gameScene = new Game;
	MainMenu* titleScreen = new MainMenu;
	MainMenuUI* titleScreenUI = new MainMenuUI;
	PauseMenu* paused = new PauseMenu;

	//initliaze them
	//gameScene->InitScene();
	splash->InitScene();
	loadingScreen->InitScene();
	loadingScreen->SetShouldRender(false);
	gameScene->SetShouldRender(false);
	titleScreen->SetShouldRender(false);
	titleScreenUI->SetShouldRender(false);
	paused->SetShouldRender(false);

	//add them to the application
	TTN_Application::scenes.push_back(splash);
	TTN_Application::scenes.push_back(loadingScreen);
	TTN_Application::scenes.push_back(gameScene);
	TTN_Application::scenes.push_back(paused);
	TTN_Application::scenes.push_back(titleScreen);
	TTN_Application::scenes.push_back(titleScreenUI);

	// init's the configs and contexts for imgui
	TTN_Application::InitImgui();

	//while the application is running
	while (!TTN_Application::GetIsClosing()) {
		
		//check if the splash card is done playing
		if (splash->GetShouldRender() && splash->GetTotalSceneTime() > 4.0f) {
			//if it is move to the loading screen
			splash->SetShouldRender(false);
			loadingScreen->SetShouldRender(true);
			//and start up the queue to load the main menu assets in
			TTN_AssetSystem::LoadSetInBackground(1);
			TTN_AssetSystem::LoadSetInBackground(2);
			TTN_AssetSystem::LoadSetInBackground(3);
		}


		//check if the loading is done
		if (loadingScreen->GetShouldRender() && set1Loaded) {
			//if it is, go to the main menu
			loadingScreen->SetShouldRender(false);
			titleScreen->InitScene();
			titleScreen->SetShouldRender(true);
			titleScreenUI->InitScene();
			titleScreenUI->SetShouldRender(true);
		}

		//check if the loading is done and the menu should be going to the game
		if (titleScreenUI->GetShouldRender() && titleScreenUI->GetShouldPlay() && set2Loaded) {
			//if it is, go to the main menu
			titleScreen->SetShouldRender(false);
			titleScreenUI->SetShouldRender(false);
			TTN_Application::TTN_Input::SetCursorLocked(true);
			gameScene->InitScene();
			gameScene->SetShouldRender(true);
			paused->InitScene();
		}

		//check if the game should quit
		if (titleScreenUI->GetShouldQuit() || paused->GetShouldQuit()) {
			TTN_Application::Quit();
			break;
		}

		//pause menu rendering
		//if the player has paused but the menu hasn't appeared yet
		if (gameScene->GetShouldRender() && !paused->GetShouldRender() && gameScene->GetPaused()) {
			TTN_Application::TTN_Input::SetCursorLocked(false);
			paused->SetShouldResume(false);
			paused->SetShouldRender(true);
		}
		//if the menu has appeared but the player has unpaused with the esc key
		else if (gameScene->GetShouldRender() && paused->GetShouldRender() && !gameScene->GetPaused()) {
			TTN_Application::TTN_Input::SetCursorLocked(true);
			paused->SetShouldResume(false);
			paused->SetShouldRender(false);
		}
		//if the menu has appeared and the player has unpaused from the menu button
		else if (gameScene->GetShouldRender() && paused->GetShouldRender() && paused->GetShouldResume()) {
			TTN_Application::TTN_Input::SetCursorLocked(true);
			gameScene->SetGameIsPaused(false);
			gameScene->SetPaused(false);
			paused->SetShouldResume(false);
			paused->SetShouldRender(false);
		}

		if (!set1Loaded && TTN_AssetSystem::GetSetLoaded(1) && TTN_AssetSystem::GetCurrentSet() == 1)
			set1Loaded = true;
		if (!set2Loaded && TTN_AssetSystem::GetSetLoaded(2) && TTN_AssetSystem::GetCurrentSet() == 2)
			set2Loaded = true;
		
		//update the scenes and render the screen
		TTN_Application::Update();
	}
	
	//when the application has ended, exit the program with no errors
	return 0; 
} 

void PrepareAssetLoading() {
	//Set 0 assets that get loaded right as the program begins after Titan and Logger init 
	TTN_AssetSystem::AddTexture2DToBeLoaded("BG", "textures/Background.png", 0); //dark grey background for splash card, loading screen and pause menu
	TTN_AssetSystem::AddTexture2DToBeLoaded("AtlasXLogo", "textures/Atlas X Games Logo.png", 0); //team logo for splash card
	TTN_AssetSystem::AddTexture2DToBeLoaded("Loading-Text", "textures/text/loading.png", 0); //loading text for loading screen
	TTN_AssetSystem::AddTexture2DToBeLoaded("Loading-Circle", "textures/loading-circle.png", 0); //circle to rotate while loading

	//Set 1 assets to be loaded while the splash card and loading screen play
	TTN_AssetSystem::AddMeshToBeLoaded("Skybox mesh", "models/SkyboxMesh.obj", 1); //mesh for the skybox
	TTN_AssetSystem::AddSkyboxToBeLoaded("Skybox texture", "textures/skybox/sky.png", 1); //texture for the skybox
	TTN_AssetSystem::AddMeshToBeLoaded("Dam mesh", "models/Dam.obj", 1); //mesh for the dam 
	TTN_AssetSystem::AddTexture2DToBeLoaded("Dam texture", "textures/Dam.png", 1); //texture for the dam
	TTN_AssetSystem::AddMorphAnimationMeshesToBeLoaded("Cannon mesh", "models/cannon/cannon", 7, 1); //mesh for the cannon
	TTN_AssetSystem::AddTexture2DToBeLoaded("Cannon texture", "textures/metal.png", 1); //texture for the cannon
	TTN_AssetSystem::AddMeshToBeLoaded("Flamethrower mesh", "models/Flamethrower.obj", 1); //mesh for the flamethrowers
	TTN_AssetSystem::AddTexture2DToBeLoaded("Flamethrower texture", "textures/FlamethrowerTexture.png", 1); //texture for the flamethrower
	TTN_AssetSystem::AddMeshToBeLoaded("Terrain plane", "models/terrainPlain.obj", 1); //large plane with lots of subdivisions for the terrain and water
	TTN_AssetSystem::AddTexture2DToBeLoaded("Terrain height map", "textures/Game Map Long.jpg", 1); //height map for the terrain
	TTN_AssetSystem::AddTexture2DToBeLoaded("Sand texture", "textures/SandTexture.jpg", 1); //sand texture
	TTN_AssetSystem::AddTexture2DToBeLoaded("Rock texture", "textures/RockTexture.jpg", 1); //rock texture
	TTN_AssetSystem::AddTexture2DToBeLoaded("Grass texture", "textures/GrassTexture.jpg", 1); //grass texture
	TTN_AssetSystem::AddTexture2DToBeLoaded("Water texture", "textures/water.png", 1); //water texture
	TTN_AssetSystem::AddDefaultShaderToBeLoaded("Basic textured shader", TTN_DefaultShaders::VERT_NO_COLOR, TTN_DefaultShaders::FRAG_BLINN_PHONG_ALBEDO_ONLY, 1);
	TTN_AssetSystem::AddDefaultShaderToBeLoaded("Skybox shader", TTN_DefaultShaders::VERT_SKYBOX, TTN_DefaultShaders::FRAG_SKYBOX, 1);
	TTN_AssetSystem::AddDefaultShaderToBeLoaded("Animated textured shader", TTN_DefaultShaders::VERT_MORPH_ANIMATION_NO_COLOR, TTN_DefaultShaders::FRAG_BLINN_PHONG_ALBEDO_ONLY, 1);
	TTN_AssetSystem::AddShaderToBeLoaded("Terrain shader", "shaders/terrain_vert.glsl", "shaders/terrain_frag.glsl", 1);
	TTN_AssetSystem::AddShaderToBeLoaded("Water shader", "shaders/water_vert.glsl", "shaders/water_frag.glsl", 1);
	TTN_AssetSystem::AddTexture2DToBeLoaded("Button Base", "textures/Button_1.png", 1); //button when not being hovered over
	TTN_AssetSystem::AddTexture2DToBeLoaded("Button Hovering", "textures/Button_2.png", 1); //button when being hovered over
	TTN_AssetSystem::AddTexture2DToBeLoaded("Play-Text", "textures/text/play.png", 1); //rendered text of word Play
	TTN_AssetSystem::AddTexture2DToBeLoaded("Arcade-Text", "textures/text/Arcade.png", 1); //rendered text of word Arcade
	TTN_AssetSystem::AddTexture2DToBeLoaded("Options-Text", "textures/text/Options.png", 1); //rendered text of word Options
	TTN_AssetSystem::AddTexture2DToBeLoaded("Quit-Text", "textures/text/Quit.png", 1); //rendered text of word Quit
	TTN_AssetSystem::AddTexture2DToBeLoaded("Game logo", "textures/Dam Defense logo.png", 1); //logo for the game
	TTN_AssetSystem::AddMeshToBeLoaded("Sphere", "models/IcoSphereMesh.obj", 1);

	//set 2, the game (excluding things already loaded into set 1)
	for(int i = 0; i < 10; i++)
		TTN_AssetSystem::AddTexture2DToBeLoaded(std::to_string(i) + "-Text", "textures/text/" + std::to_string(i) + ".png", 2); //numbers for health and score
	for (int i = 1; i < 4; i++) {
		TTN_AssetSystem::AddMeshToBeLoaded("Boat " + std::to_string(i), "models/Boat " + std::to_string(i) + ".obj", 2); //enemy boat meshes
		TTN_AssetSystem::AddTexture2DToBeLoaded("Boat texture " + std::to_string(i), "textures/Boat " + std::to_string(i) + " Texture.png", 2); //enemy boat textures 
	}
	TTN_AssetSystem::AddMorphAnimationMeshesToBeLoaded("Bird mesh", "models/bird/bird", 2, 2); //bird mesh
	TTN_AssetSystem::AddTexture2DToBeLoaded("Bird texture", "textures/BirdTexture.png", 2); //bird texture
	TTN_AssetSystem::AddTexture2DToBeLoaded("Paused-Text", "textures/text/Paused.png", 2); //rendered text of the word paused
	TTN_AssetSystem::AddTexture2DToBeLoaded("Resume-Text", "textures/text/Resume.png", 2); //rendered text of the word resume
	TTN_AssetSystem::AddTexture2DToBeLoaded("Score-Text", "textures/text/Score.png", 2); //rendered text of the word Score

	//set 3, win/lose screen
	TTN_AssetSystem::AddTexture2DToBeLoaded("You Win-Text", "textures/text/You win!.png", 3); //rendered text of the pharse "You Win!" 
	TTN_AssetSystem::AddTexture2DToBeLoaded("Game Over-Text", "textures/text/Game over.png", 3); //rendered text of the phrase "Game Over..." 
	TTN_AssetSystem::AddTexture2DToBeLoaded("Play Again-Text", "textures/text/Play again.png", 3); //rendered text of the phrase "Play Again" 
}