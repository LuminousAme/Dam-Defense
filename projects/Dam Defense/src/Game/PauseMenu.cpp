//Dam Defense, by Atlas X Games
//MainMenu.cpp, the source file for the class that represents the main menu

//import the class
#include "PauseMenu.h"
#include "glm/ext.hpp"

PauseMenu::PauseMenu()
{
	shouldQuit = false;
	shouldResume = false;
	shouldMenu = false;
	shouldOptions = false;

	TTN_Scene();
}

void PauseMenu::InitScene()
{
	//grab the textures
	textureBackground = TTN_AssetSystem::GetTexture2D("BG");
	texturePaused = TTN_AssetSystem::GetTexture2D("Paused-Text");
	textureButton1 = TTN_AssetSystem::GetTexture2D("Button Base");
	textureButton2 = TTN_AssetSystem::GetTexture2D("Button Hovering");
	textureResume = TTN_AssetSystem::GetTexture2D("Resume-Text");
	textureOptions = TTN_AssetSystem::GetTexture2D("Options-Text");
	textureQuit = TTN_AssetSystem::GetTexture2D("Quit-Text");
	textureMainMenu = TTN_AssetSystem::GetTexture2D("Main Menu");

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
		Get<TTN_Camera>(cam).View();
	}

	//paused text
	{
		entt::entity paused = CreateEntity();

		//create a transform for the text
		TTN_Transform textTrans = TTN_Transform(glm::vec3(0.0f, 360.0f, 0.5f), glm::vec3(0.0f), glm::vec3(550.0f, 150.0f, 1.0f));
		AttachCopy(paused, textTrans);

		//create a 2D renderer for the button
		TTN_Renderer2D textRenderer = TTN_Renderer2D(texturePaused);
		AttachCopy(paused, textRenderer);
	}

	//text
	for (int i = 0; i < 4; i++) {
		entt::entity temp = CreateEntity();
		if (i == 0) resumeText = temp;
		else if (i == 1) optionsText = temp;
		else if (i == 2) menuText = temp;
		else if (i == 3) quitText = temp;

		//create a transform for the text
		TTN_Transform textTrans;

		if (i == 0) textTrans = TTN_Transform(glm::vec3(657.75f, -75.0f, 0.5f), glm::vec3(0.0f), glm::vec3(550.0f / 2.75f, 150.0f / 2.75f, 1.0f));
		else if (i == 1) textTrans = TTN_Transform(glm::vec3(254.75, -75.0f, 0.5f), glm::vec3(0.0f), glm::vec3(550.0f / 2.75f, 150.0f / 2.75f, 1.0f));
		else if (i == 2) textTrans = TTN_Transform(glm::vec3(-148.25, -75.0f, 0.5f), glm::vec3(0.0f), glm::vec3(550.0f / 2.0f, 150.0f / 2.0f, 1.0f));
		else if (i == 3) textTrans = TTN_Transform(glm::vec3(-551.25, -75.0f, 0.5f), glm::vec3(0.0f), glm::vec3(550.0f / 2.75f, 150.0f / 2.75f, 1.0f));
		AttachCopy(temp, textTrans);

		//create a 2D renderer for the button
		TTN_Renderer2D textRenderer;
		if (i == 0) textRenderer = TTN_Renderer2D(textureResume);
		else if (i == 1) textRenderer = TTN_Renderer2D(textureOptions);
		else if (i == 2) textRenderer = TTN_Renderer2D(textureMainMenu);
		else if (i == 3) textRenderer = TTN_Renderer2D(textureQuit);
		AttachCopy(temp, textRenderer);
	}

	//buttons
	for (int i = 0; i < 4; i++) {
		entt::entity temp = CreateEntity();
		if (i == 0) resumeButton = temp;
		else if (i == 1) optionsButton = temp;
		else if (i == 2) menuButton = temp;
		else if (i == 3) quitButton = temp;

		//create a transform for the button
		TTN_Transform buttonTrans;
		/*if (i == 0) buttonTrans = TTN_Transform(glm::vec3(300.0f, -75.0f, 2.0f), glm::vec3(0.0f), glm::vec3(322.75f, 201.5, 1.0f));
		else if (i == 1) buttonTrans = TTN_Transform(glm::vec3(0.0f, -75.0f, 2.0f), glm::vec3(0.0f), glm::vec3(322.75f, 201.5, 1.0f));
		else if (i == 2) buttonTrans = TTN_Transform(glm::vec3(-300.0f, -75.0f, 2.0f), glm::vec3(0.0f), glm::vec3(322.75f, 201.5, 1.0f));
		else if (i == 3) buttonTrans = TTN_Transform(glm::vec3(-600.0f, -75.0f, 2.0f), glm::vec3(0.0f), glm::vec3(322.75f, 201.5, 1.0f));*/
		if (i == 0) buttonTrans = TTN_Transform(glm::vec3(657.75f, -75.0f, 0.8f), glm::vec3(0.0f), glm::vec3(322.75f, 201.5, 1.0f));
		else if (i == 1) buttonTrans = TTN_Transform(glm::vec3(254.75, -75.0f, 0.8f), glm::vec3(0.0f), glm::vec3(322.75f, 201.5, 1.0f));
		else if (i == 2) buttonTrans = TTN_Transform(glm::vec3(-148.25, -75.0f, 0.8f), glm::vec3(0.0f), glm::vec3(322.75f, 201.5, 1.0f));
		else if (i == 3) buttonTrans = TTN_Transform(glm::vec3(-551.25, -75.0f, 0.8f), glm::vec3(0.0f), glm::vec3(322.75f, 201.5, 1.0f));

		AttachCopy(temp, buttonTrans);

		//create a 2D renderer for the button
		TTN_Renderer2D buttonRenderer = TTN_Renderer2D(textureButton1);
		AttachCopy(temp, buttonRenderer);
	}

	//background
	{
		//create an entity in the scene for the background
		background = CreateEntity();

		//create a transform for the background, placing it in the center of the screen, covering the whole thing
		TTN_Transform bgTrans = TTN_Transform(glm::vec3(0.0f, 0.0f, 0.85f), glm::vec3(0.0f), glm::vec3(1920.0f, 1080.0f, 1.0f));
		AttachCopy(background, bgTrans);

		//create a sprite renderer for the background
		TTN_Renderer2D bgRenderer2D = TTN_Renderer2D(textureBackground, glm::vec4(1.0f, 1.0f, 1.0f, 0.5f), 1);
		AttachCopy(background, bgRenderer2D);
	}
}

void PauseMenu::Update(float deltaTime)
{
	//get the mouse position
	glm::vec2 mousePos = TTN_Application::TTN_Input::GetMousePosition();
	//convert it to worldspace
	glm::vec3 mousePosWorldSpace;
	{
		float tx = TTN_Interpolation::InverseLerp(0.0f, 1920.0f, mousePos.x);
		float ty = TTN_Interpolation::InverseLerp(0.0f, 1080.0f, mousePos.y);

		float newX = TTN_Interpolation::Lerp(960.0f, -960.0f, tx);
		float newY = TTN_Interpolation::Lerp(540.0f, -540.0f, ty);

		mousePosWorldSpace = glm::vec3(newX, newY, 2.0f);
	}

	//get resume buttons transform
	TTN_Transform resumeButtonTrans = Get<TTN_Transform>(resumeButton);
	if (mousePosWorldSpace.x < resumeButtonTrans.GetPos().x + 0.5f * abs(resumeButtonTrans.GetScale().x) &&
		mousePosWorldSpace.x > resumeButtonTrans.GetPos().x - 0.5f * abs(resumeButtonTrans.GetScale().x) &&
		mousePosWorldSpace.y < resumeButtonTrans.GetPos().y + 0.5f * abs(resumeButtonTrans.GetScale().y) &&
		mousePosWorldSpace.y > resumeButtonTrans.GetPos().y - 0.5f * abs(resumeButtonTrans.GetScale().y)) {
		Get<TTN_Renderer2D>(resumeButton).SetSprite(textureButton2);
	}
	else {
		Get<TTN_Renderer2D>(resumeButton).SetSprite(textureButton1);
	}

	//get options buttons transform
	TTN_Transform optionsButtonTrans = Get<TTN_Transform>(optionsButton);
	if (mousePosWorldSpace.x < optionsButtonTrans.GetPos().x + 0.5f * abs(optionsButtonTrans.GetScale().x) &&
		mousePosWorldSpace.x > optionsButtonTrans.GetPos().x - 0.5f * abs(optionsButtonTrans.GetScale().x) &&
		mousePosWorldSpace.y < optionsButtonTrans.GetPos().y + 0.5f * abs(optionsButtonTrans.GetScale().y) &&
		mousePosWorldSpace.y > optionsButtonTrans.GetPos().y - 0.5f * abs(optionsButtonTrans.GetScale().y)) {
		Get<TTN_Renderer2D>(optionsButton).SetSprite(textureButton2);
	}
	else {
		Get<TTN_Renderer2D>(optionsButton).SetSprite(textureButton1);
	}

	//get options buttons transform
	TTN_Transform menuButtonTrans = Get<TTN_Transform>(menuButton);
	if (mousePosWorldSpace.x < menuButtonTrans.GetPos().x + 0.5f * abs(menuButtonTrans.GetScale().x) &&
		mousePosWorldSpace.x > menuButtonTrans.GetPos().x - 0.5f * abs(menuButtonTrans.GetScale().x) &&
		mousePosWorldSpace.y < menuButtonTrans.GetPos().y + 0.5f * abs(menuButtonTrans.GetScale().y) &&
		mousePosWorldSpace.y > menuButtonTrans.GetPos().y - 0.5f * abs(menuButtonTrans.GetScale().y)) {
		Get<TTN_Renderer2D>(menuButton).SetSprite(textureButton2);
	}
	else {
		Get<TTN_Renderer2D>(menuButton).SetSprite(textureButton1);
	}

	//get quit buttons transform
	TTN_Transform quitButtonTrans = Get<TTN_Transform>(quitButton);
	if (mousePosWorldSpace.x < quitButtonTrans.GetPos().x + 0.5f * abs(quitButtonTrans.GetScale().x) &&
		mousePosWorldSpace.x > quitButtonTrans.GetPos().x - 0.5f * abs(quitButtonTrans.GetScale().x) &&
		mousePosWorldSpace.y < quitButtonTrans.GetPos().y + 0.5f * abs(quitButtonTrans.GetScale().y) &&
		mousePosWorldSpace.y > quitButtonTrans.GetPos().y - 0.5f * abs(quitButtonTrans.GetScale().y)) {
		Get<TTN_Renderer2D>(quitButton).SetSprite(textureButton2);
	}
	else {
		Get<TTN_Renderer2D>(quitButton).SetSprite(textureButton1);
	}
}

void PauseMenu::MouseButtonDownChecks()
{
	if (TTN_Application::TTN_Input::GetMouseButtonDown(TTN_MouseButton::Left)) {
		//get the mouse position
		glm::vec2 mousePos = TTN_Application::TTN_Input::GetMousePosition();
		//convert it to worldspace
		glm::vec3 mousePosWorldSpace;
		{
			float tx = TTN_Interpolation::InverseLerp(0.0f, 1920.0f, mousePos.x);
			float ty = TTN_Interpolation::InverseLerp(0.0f, 1080.0f, mousePos.y);

			float newX = TTN_Interpolation::Lerp(960.0f, -960.0f, tx);
			float newY = TTN_Interpolation::Lerp(540.0f, -540.0f, ty);

			mousePosWorldSpace = glm::vec3(newX, newY, 2.0f);
		}

		//get resume buttons transform
		TTN_Transform resumeButtonTrans = Get<TTN_Transform>(resumeButton);
		if (mousePosWorldSpace.x < resumeButtonTrans.GetPos().x + 0.5f * abs(resumeButtonTrans.GetScale().x) &&
			mousePosWorldSpace.x > resumeButtonTrans.GetPos().x - 0.5f * abs(resumeButtonTrans.GetScale().x) &&
			mousePosWorldSpace.y < resumeButtonTrans.GetPos().y + 0.5f * abs(resumeButtonTrans.GetScale().y) &&
			mousePosWorldSpace.y > resumeButtonTrans.GetPos().y - 0.5f * abs(resumeButtonTrans.GetScale().y)) {
			shouldResume = true;
		}

		//get options buttons transform
		TTN_Transform optionsButtonTrans = Get<TTN_Transform>(optionsButton);
		if (mousePosWorldSpace.x < optionsButtonTrans.GetPos().x + 0.5f * abs(optionsButtonTrans.GetScale().x) &&
			mousePosWorldSpace.x > optionsButtonTrans.GetPos().x - 0.5f * abs(optionsButtonTrans.GetScale().x) &&
			mousePosWorldSpace.y < optionsButtonTrans.GetPos().y + 0.5f * abs(optionsButtonTrans.GetScale().y) &&
			mousePosWorldSpace.y > optionsButtonTrans.GetPos().y - 0.5f * abs(optionsButtonTrans.GetScale().y)) {
			//do something
			shouldOptions = true;
		}

		//get menu buttons transform
		TTN_Transform menuButtonTrans = Get<TTN_Transform>(menuButton);
		if (mousePosWorldSpace.x < menuButtonTrans.GetPos().x + 0.5f * abs(menuButtonTrans.GetScale().x) &&
			mousePosWorldSpace.x > menuButtonTrans.GetPos().x - 0.5f * abs(menuButtonTrans.GetScale().x) &&
			mousePosWorldSpace.y < menuButtonTrans.GetPos().y + 0.5f * abs(menuButtonTrans.GetScale().y) &&
			mousePosWorldSpace.y > menuButtonTrans.GetPos().y - 0.5f * abs(menuButtonTrans.GetScale().y)) {
			//do something
			shouldMenu = true;
		}

		//get quit buttons transform
		TTN_Transform quitButtonTrans = Get<TTN_Transform>(quitButton);
		if (mousePosWorldSpace.x < quitButtonTrans.GetPos().x + 0.5f * abs(quitButtonTrans.GetScale().x) &&
			mousePosWorldSpace.x > quitButtonTrans.GetPos().x - 0.5f * abs(quitButtonTrans.GetScale().x) &&
			mousePosWorldSpace.y < quitButtonTrans.GetPos().y + 0.5f * abs(quitButtonTrans.GetScale().y) &&
			mousePosWorldSpace.y > quitButtonTrans.GetPos().y - 0.5f * abs(quitButtonTrans.GetScale().y)) {
			shouldQuit = true;
		}
	}
}