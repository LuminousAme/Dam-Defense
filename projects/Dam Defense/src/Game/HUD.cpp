//Dam Defense, by Atlas X Games
//HUD.cpp, the source file for the scene hclass representing the UI in the main game

#include "HUD.h"

GameUI::GameUI() : TTN_Scene()
{
}

void GameUI::InitScene()
{
	textureScore = TTN_AssetSystem::GetTexture2D("Score");
	m_DamHealth = 100.0f;
	m_displayedWaveProgress = 0.0f;
	m_waveProgress = 0.0f;

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
		//create an entity in the scene for the health bar overlay
		healthBar = CreateEntity();

		//create a transform for the health bar overlay
		TTN_Transform healthTrans = TTN_Transform(glm::vec3(750.0f, -420.0f, 0.9f), glm::vec3(0.0f), glm::vec3(-1228.0f * healthScale, 239.0f * healthScale, 1.0f));
		AttachCopy(healthBar, healthTrans);

		//create a sprite renderer for the health bar overlay
		TTN_Renderer2D healthRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Bar Border"));
		AttachCopy(healthBar, healthRenderer);
	}

	//health of dam
	{
		//create an entity for the health bar
		healthDam = CreateEntity();

		//create a transform for the health bar
		TTN_Transform healthTrans = TTN_Transform(glm::vec3(750.0f, -420.0f, 1.0f), glm::vec3(0.0f), glm::vec3(-1228.0f * healthScale, 239.0f * healthScale, 1.0f));
		AttachCopy(healthDam, healthTrans);

		//create a sprite renderer for the health bar
		TTN_Renderer2D healthRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Bar"), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		AttachCopy(healthDam, healthRenderer);
	}

	//health bar background
	{
		//create an entity for the health bar background
		healthBarBg = CreateEntity();

		//create a transform for the health bar background
		TTN_Transform healthTrans = TTN_Transform(glm::vec3(750.0f, -420.0f, 1.1f), glm::vec3(0.0f), glm::vec3(-1228.0f * healthScale, 239.0f * healthScale, 1.0f));
		AttachCopy(healthBarBg, healthTrans);

		//create a sprite renderer for the health bar background
		TTN_Renderer2D healthRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Bar BG"), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		AttachCopy(healthBarBg, healthRenderer);
	}

	//progress bar
	{
		//create an entity in the scene for the progress bar overlay
		progressBar = CreateEntity();

		//create a transform for the progress bar overlay
		TTN_Transform progressTrans = TTN_Transform(glm::vec3(0.0f, 480.0f, 0.9f), glm::vec3(0.0f), glm::vec3(-1228.0f * progressScale.x, 239.0f * progressScale.y, 1.0f));
		AttachCopy(progressBar, progressTrans);

		//create a sprite renderer for the progress bar overlay
		TTN_Renderer2D progressRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Bar Border"));
		AttachCopy(progressBar, progressRenderer);
	}

	//acutal progress
	{
		//create an entity for the progress bar
		progressRepresentation = CreateEntity();

		//create a transform for the progress bar
		TTN_Transform progressTrans = TTN_Transform(glm::vec3(0.0f, 480.0f, 1.0f), glm::vec3(0.0f), glm::vec3(-1228.0f * progressScale.x, 239.0f * progressScale.y, 1.0f));
		AttachCopy(progressRepresentation, progressTrans);

		//create a sprite renderer for the progress bar
		TTN_Renderer2D progressRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Bar"), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		AttachCopy(progressRepresentation, progressRenderer);
	}

	//progess bar background
	{
		//create an entity for the progress bar background
		progressBarBg = CreateEntity();

		//create a transform for the progress bar background
		TTN_Transform progressTrans = TTN_Transform(glm::vec3(0.0f, 480.0f, 1.1f), glm::vec3(0.0f), glm::vec3(-1228.0f * progressScale.x, 239.0f * progressScale.y, 1.0f));
		AttachCopy(progressBarBg, progressTrans);

		//create a sprite renderer for the progress bar background
		TTN_Renderer2D progressRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Bar BG"), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		AttachCopy(progressBarBg, progressRenderer);
	}

	//score
	{
		//create an entity in the scene for the logo
		scoreText = CreateEntity();

		//create a transform for the logo
		TTN_Transform logoTrans = TTN_Transform(glm::vec3(825.0f, 480.0f, 1.0f), glm::vec3(0.0f), 
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
	{
		while (GetNumOfDigits(m_score) < scoreNums.size()) {
			DeleteEntity(scoreNums[scoreNums.size() - 1]);
			scoreNums.pop_back();
		}

		if (GetNumOfDigits(m_score) > scoreNums.size())
			MakeScoreNumEntity();

		for (int i = 0; i < scoreNums.size(); i++) {
			Get<TTN_Renderer2D>(scoreNums[i]).SetSprite(TTN_AssetSystem::GetTexture2D(std::to_string(GetDigit(m_score, scoreNums.size() - i - 1)) + "-Text"));
		}
	}
	
	//update the health number
	{
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
	}

	//update the health bar
	float normalizedDamHealth = TTN_Interpolation::ReMap(0.0f, 100.0f, 0.0f, 1.0f, m_DamHealth);
	Get<TTN_Renderer2D>(healthDam).SetHoriMask(normalizedDamHealth);

	//update the progress bar
	if (m_displayedWaveProgress >= m_waveProgress + 0.01f || m_displayedWaveProgress <= m_waveProgress - 0.01f) {
		float sign = (m_waveProgress - m_displayedWaveProgress) / std::abs(m_waveProgress - m_displayedWaveProgress);
		m_displayedWaveProgress += sign * 0.5f * deltaTime;
		m_displayedWaveProgress = std::clamp(m_displayedWaveProgress, 0.0f, 1.0f);
	}

	Get<TTN_Renderer2D>(progressRepresentation).SetHoriMask(m_displayedWaveProgress);
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
