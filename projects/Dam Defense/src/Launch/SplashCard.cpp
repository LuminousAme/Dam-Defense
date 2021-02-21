//Dam Defense, by Atlas X Games
//SplashCard.cpp, the source file for the class that represents the team logo splash card at the beginning of the game

//import the class
#include "SplashCard.h"

SplashCard::SplashCard()
	: TTN_Scene()
{
}

void SplashCard::InitScene()
{
	//set the time to a starting value
	totalSceneTime = 0.0f;

	//grab the references to the textures
	logoText = TTN_AssetSystem::GetTexture2D("AtlasXLogo");
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

	//logo
	{
		//create an entity in the scene for the background
		logo = CreateEntity();

		//create a transform for the logo, placing it in the center of the screen, covering the whole thing
		TTN_Transform logoTrans = TTN_Transform(glm::vec3(0.0f, 0.0f, 3.5f), glm::vec3(0.0f), glm::vec3(500.0f, 500.0f, 1.0f));
		AttachCopy(logo, logoTrans);

		//create a sprite renderer for the logo
		TTN_Renderer2D logoRenderer2D = TTN_Renderer2D(logoText, glm::vec4(1.0f, 1.0f, 1.0f, 0.0f), 1);
		AttachCopy(logo, logoRenderer2D);

	}

	//background
	{
		//create an entity in the scene for the background
		background = CreateEntity();

		//create a transform for the background, placing it in the center of the screen, covering the whole thing
		TTN_Transform bgTrans = TTN_Transform(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f), glm::vec3(1920.0f, 1080.0f, 1.0f));
		AttachCopy(background, bgTrans);

		//create a sprite renderer for the background
		TTN_Renderer2D bgRenderer2D = TTN_Renderer2D(bgText, glm::vec4(1.0f), 1);
		AttachCopy(background, bgRenderer2D);
	}	
}

//update function, runs once every frame
void SplashCard::Update(float deltaTime)
{
	//update the total scenetime
	totalSceneTime += deltaTime;

	//have the logo fade in and out over the course of 4 seconds, holding for 2 seconds in the middle 
	if (totalSceneTime < 1.0f)
		Get<TTN_Renderer2D>(logo).SetColor(glm::vec4(1.0f, 1.0f, 1.0f, TTN_Interpolation::InverseLerp(0.0f, 1.0f, totalSceneTime)));
	else if (totalSceneTime >= 1.0f && totalSceneTime < 3.0f) 
		Get<TTN_Renderer2D>(logo).SetColor(glm::vec4(1.0f));
	else if (totalSceneTime >= 3.0f && totalSceneTime < 4.0f)
		Get<TTN_Renderer2D>(logo).SetColor(glm::vec4(1.0f, 1.0f, 1.0f, TTN_Interpolation::InverseLerp(4.0f, 3.0f, totalSceneTime)));
	else 
		Get<TTN_Renderer2D>(logo).SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));

	//always call the base scene's update at the end
	TTN_Scene::Update(deltaTime);
}
