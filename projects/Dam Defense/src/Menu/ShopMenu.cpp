//Dam Defense, by Atlas X Games
//OptionMenu.cpp, the source file for the scene class representing the options menu in the game
#include "ShopMenu.h"

ShopMenu::ShopMenu() : TTN_Scene()
{
}

void ShopMenu::InitScene()
{
	shouldShop = false;
	shouldBack = false;
	textureButton1 = TTN_AssetSystem::GetTexture2D("Button Base");
	textureButton2 = TTN_AssetSystem::GetTexture2D("Button Hovering");

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

	//background
	{
		//create an entity in the scene for the background
		background = CreateEntity();

		//create a transform for the background, placing it in the center of the screen, covering the whole thing
		TTN_Transform bgTrans = TTN_Transform(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f), glm::vec3(1920.0f, 1080.0f, 1.0f));
		AttachCopy(background, bgTrans);

		//create a sprite renderer for the background
		TTN_Renderer2D bgRenderer2D = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("BG"));
		AttachCopy(background, bgRenderer2D);
	}

	{
		buttonHealth = CreateEntity();

		//create a transform for the button
		TTN_Transform buttonTrans;
		buttonTrans = TTN_Transform(glm::vec3(650.0f, -220.0f, 0.9f), glm::vec3(0.0f), glm::vec3(250.0f, 150.0, 1.0f));

		AttachCopy(buttonHealth, buttonTrans);

		//create a 2D renderer for the button
		TTN_Renderer2D buttonRenderer = TTN_Renderer2D(textureButton1);
		AttachCopy(buttonHealth, buttonRenderer);
	}
}

void ShopMenu::Update(float deltaTime)
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

	KeyDownChecks();
	//std::cout << "Diff" << diff << std::endl;
	//update the base scene
	TTN_Scene::Update(deltaTime);
}

void ShopMenu::MouseButtonDownChecks()
{
	if (TTN_Application::TTN_Input::GetMouseButton(TTN_MouseButton::Left)) {
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
	}
}

void ShopMenu::KeyDownChecks()
{
	if (TTN_Application::TTN_Input::GetKeyDown(TTN_KeyCode::L)) {
		shouldShop = true;
	}

	if (TTN_Application::TTN_Input::GetKeyDown(TTN_KeyCode::Esc)) {
		shouldBack = true;
	}

}