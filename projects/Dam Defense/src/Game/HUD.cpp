//Dam Defense, by Atlas X Games
//HUD.cpp, the source file for the scene hclass representing the UI in the main game

#include "HUD.h"

GameUI::GameUI() : TTN_Scene()
{
}

void GameUI::InitScene()
{
	textureHealth = TTN_AssetSystem::GetTexture2D("Health Bar");
	textureHealthDam = TTN_AssetSystem::GetTexture2D("Health Bar Dam");
	textureScore = TTN_AssetSystem::GetTexture2D("Score");
	m_DamHealth = 100.0f;

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

	//health bar
	{
		//create an entity in the scene for the logo
		healthBar = CreateEntity();

		//create a transform for the logo
		TTN_Transform healthTrans = TTN_Transform(glm::vec3(700.0f, -380.0f, 1.0f), glm::vec3(0.0f), glm::vec3(-350.0f, 100.0f, 1.0f));
		AttachCopy(healthBar, healthTrans);

		//create a sprite renderer for the logo
		TTN_Renderer2D healthRenderer = TTN_Renderer2D(textureHealth);
		AttachCopy(healthBar, healthRenderer);
	}

	//health of dam
	{
		//create an entity
		healthDam = CreateEntity();

		//create a transform for the logo
		//TTN_Transform healthDamTrans = TTN_Transform(glm::vec3(700.0f, -380.0f, 1.0f), glm::vec3(0.0f), glm::vec3(-950.0f, 100.0f, 1.0f));

		TTN_Transform healthDamTrans = TTN_Transform(glm::vec3(700.0f, -380.0f, 10.0f), glm::vec3(0.0f), glm::vec3(m_DamHealth * -3.50f, 100.0f, 1.0f));
		AttachCopy(healthDam, healthDamTrans);

		//create a sprite renderer for the logo
		TTN_Renderer2D healthDamRenderer = TTN_Renderer2D(textureHealthDam);
		AttachCopy(healthDam, healthDamRenderer);
	}

	//score
	{
		//create an entity in the scene for the logo
		scoreText = CreateEntity();

		//create a transform for the logo
		TTN_Transform logoTrans = TTN_Transform(glm::vec3(800.0f, 450.0f, 1.0f), glm::vec3(0.0f), 
			glm::vec3(-scoreTextScale * 550.0f, scoreTextScale * 150.0f, 1.0f));
		AttachCopy(scoreText, logoTrans);

		//create a sprite renderer for the logo
		TTN_Renderer2D logoRenderer = TTN_Renderer2D(textureScore);
		AttachCopy(scoreText, logoRenderer);
	}
}

void GameUI::Update(float deltaTime)
{
	//update the score number
	while (GetNumOfDigits(m_score) < scoreNums.size()) {
		DeleteEntity(scoreNums[scoreNums.size() - 1]);
		scoreNums.pop_back();
	}

	if (GetNumOfDigits(m_score) > scoreNums.size())
		MakeScoreNumEntity();

	for (int i = 0; i < scoreNums.size(); i++) {
		Get<TTN_Renderer2D>(scoreNums[i]).SetSprite(TTN_AssetSystem::GetTexture2D(std::to_string(GetDigit(m_score, scoreNums.size() - i - 1)) + "-Text"));
	}

	//update the health number
	unsigned health = std::ceil(m_DamHealth);
	while (GetNumOfDigits(health) < healthNums.size()) {
		DeleteEntity(healthNums[healthNums.size() - 1]);
		healthNums.pop_back();
	}

	if (GetNumOfDigits(health) > healthNums.size())
		MakeHealthNumEntity();

	for (int i = 0; i < healthNums.size(); i++) {
		Get<TTN_Renderer2D>(healthNums[i]).SetSprite(TTN_AssetSystem::GetTexture2D(std::to_string(GetDigit(health, healthNums.size() - i - 1)) + "-Text"));
	}

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

	//DamHealth = Game::GetDamHealth();
	std::cout << m_DamHealth << " UIIIII" << std::endl;
	Get<TTN_Transform>(healthDam).SetScale(glm::vec3(m_DamHealth * -3.50f, 100.0f, 1.0f));
	//Get<TTN_Transform>(healthDam).SetPos(glm::vec3(DamHealth * -4.50f, -380.0f, 1.0f));
}

void GameUI::MakeScoreNumEntity()
{
	scoreNums.push_back(CreateEntity());

	//reference to the base score text's transform
	TTN_Transform& scoreTrans = Get<TTN_Transform>(scoreText);

	//setup a transform for the new entity
	TTN_Transform numTrans = TTN_Transform(glm::vec3(scoreTrans.GetGlobalPos().x - 0.3f * std::abs(scoreTrans.GetScale().x) - 
		(float)scoreNums.size() * 0.5f * scoreTextScale * 150.0f, scoreTrans.GetGlobalPos().y, scoreTrans.GetGlobalPos().z),
		glm::vec3(0.0f), glm::vec3(-scoreTextScale * 150.0f, scoreTextScale * 150.0f, 1.0f));
	AttachCopy(scoreNums[scoreNums.size() - 1], numTrans);

	//setup a 2D renderer for the new entity
			//create a sprite renderer for the logo
	TTN_Renderer2D numRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("0-Text"));
	AttachCopy(scoreNums[scoreNums.size() - 1], numRenderer);
}

void GameUI::MakeHealthNumEntity()
{
	healthNums.push_back(CreateEntity());

	//reference to the health bar's transform
	TTN_Transform& healthTrans = Get<TTN_Transform>(healthBar);

	//setup a transform for the new entity
	TTN_Transform numTrans = TTN_Transform(glm::vec3(healthTrans.GetGlobalPos().x + 0.3f * std::abs(healthTrans.GetScale().x)  -
		(float)healthNums.size() * 0.5f * healthTextScale * 150.0f, healthTrans.GetGlobalPos().y, healthTrans.GetGlobalPos().z),
		glm::vec3(0.0f), glm::vec3(-healthTextScale * 150.0f, healthTextScale * 150.0f, 1.0f));
	AttachCopy(healthNums[healthNums.size() - 1], numTrans);

	//setup a 2D renderer for the new entity
			//create a sprite renderer for the logo
	TTN_Renderer2D numRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("0-Text"));
	AttachCopy(healthNums[healthNums.size() - 1], numRenderer);
}
