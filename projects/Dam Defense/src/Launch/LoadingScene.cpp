//Dam Defense, by Atlas X Games
//SplashCard.cpp, the source file for the class that represents the team logo splash card at the beginning of the game

//import the class
#include "LoadingScene.h"

LoadingScene::LoadingScene()
	: TTN_Scene()
{
}

void LoadingScene::InitScene()
{
	//grab the references to the textures
	textureForLoadingText = TTN_AssetSystem::GetTexture2D("Loading-Text");
	textureForLoadingCircle = TTN_AssetSystem::GetTexture2D("Loading-Circle");
	bgText = TTN_AssetSystem::GetTexture2D("BG");

	//setup the entities
	//main camera
	{
		//create an entity in the scene for the camera
		cam = CreateEntity();
		SetCamEntity(cam);
		Attach<TTN_Transform>(cam);
		Attach<TTN_Camera>(cam);
		auto& camTrans = Get<TTN_Transform>(cam);
		camTrans.SetPos(glm::vec3(0.0f, 0.0f, 0.0f));
		camTrans.SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
		camTrans.LookAlong(glm::vec3(0.0, 0.0, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		Get<TTN_Camera>(cam).CalcOrtho(-960.0f, 960.0f, -540.0f, 540.0f, 0.0f, 10.0f);
		//Get<TTN_Camera>(cam).CalcPerspective(60.0f, 1.78f, 0.01f, 1000.f);
		Get<TTN_Camera>(cam).View();
	}

	//loading text
	{
		//create an entity in the scene for the background
		loadingText = CreateEntity();

		//create a transform for the logo, placing it in the center of the screen, covering the whole thing
		TTN_Transform textTrans = TTN_Transform(glm::vec3(90.0f, -180.0f, 1.0f), glm::vec3(0.0f), glm::vec3(550.0f, 150.0f, 1.0f));
		AttachCopy(loadingText, textTrans);

		//create a sprite renderer for the logo
		TTN_Renderer2D textRenderer2D = TTN_Renderer2D(textureForLoadingText);
		AttachCopy(loadingText, textRenderer2D);
	}

	//loading cirlce
	{
		//create an entity in the scene for the background
		loadingCircle = CreateEntity();

		//create a transform for the logo, placing it in the center of the screen, covering the whole thing
		TTN_Transform circleTrans = TTN_Transform(glm::vec3(90.0f - 285.0f - 80.0f, -180.0f, 2.0f), glm::vec3(0.0f), glm::vec3(160.0f, 160.0f, 1.0f));
		AttachCopy(loadingCircle, circleTrans);

		//create a sprite renderer for the logo
		TTN_Renderer2D cirlceRenderer2D = TTN_Renderer2D(textureForLoadingCircle);
		AttachCopy(loadingCircle, cirlceRenderer2D);

	}
	//background
	{
		//create an entity in the scene for the background
		bg = CreateEntity();

		//create a transform for the background, placing it in the center of the screen, covering the whole thing
		TTN_Transform bgTrans = TTN_Transform(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f), glm::vec3(1920.0f, 1080.0f, 1.0f));
		AttachCopy(bg, bgTrans);

		//create a sprite renderer for the background
		TTN_Renderer2D bgRenderer2D = TTN_Renderer2D(bgText, glm::vec4(1.0f), 1);
		AttachCopy(bg, bgRenderer2D);
	}
}

void LoadingScene::Update(float deltaTime)
{
	//rotate the loading cirlce
	Get<TTN_Transform>(loadingCircle).RotateFixed(glm::vec3(0.0f, 0.0f, 360.0f * deltaTime));

	//update the base scene class
	TTN_Scene::Update(deltaTime);
}