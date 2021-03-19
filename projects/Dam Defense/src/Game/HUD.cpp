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
	m_waveCompleteTime = 10.0f;

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
		TTN_Transform healthTrans = TTN_Transform(glm::vec3(750.0f, -420.0f, 0.9f), glm::vec3(0.0f), glm::vec3(1228.0f * healthScale, 239.0f * healthScale, 1.0f));
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
		TTN_Transform healthTrans = TTN_Transform(glm::vec3(750.0f, -420.0f, 1.0f), glm::vec3(0.0f), glm::vec3(1228.0f * healthScale, 239.0f * healthScale, 1.0f));
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
		TTN_Transform healthTrans = TTN_Transform(glm::vec3(750.0f, -420.0f, 1.1f), glm::vec3(0.0f), glm::vec3(1228.0f * healthScale, 239.0f * healthScale, 1.0f));
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
		TTN_Transform progressTrans = TTN_Transform(glm::vec3(0.0f, 480.0f, 0.9f), glm::vec3(0.0f), glm::vec3(1228.0f * progressScale.x, 239.0f * progressScale.y, 1.0f));
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
		TTN_Transform progressTrans = TTN_Transform(glm::vec3(0.0f, 480.0f, 1.0f), glm::vec3(0.0f), glm::vec3(1228.0f * progressScale.x, 239.0f * progressScale.y, 1.0f));
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
		TTN_Transform progressTrans = TTN_Transform(glm::vec3(0.0f, 480.0f, 1.1f), glm::vec3(0.0f), glm::vec3(1228.0f * progressScale.x, 239.0f * progressScale.y, 1.0f));
		AttachCopy(progressBarBg, progressTrans);

		//create a sprite renderer for the progress bar background
		TTN_Renderer2D progressRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Bar BG"), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		AttachCopy(progressBarBg, progressRenderer);
	}

	//crosshair top
	{
		//create an entity
		crosshairCross = CreateEntity();

		//make a transform for it 
		TTN_Transform Trans = TTN_Transform(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f), glm::vec3(60.0f * crosshairScale, 60.0f * crosshairScale, 1.0f));
		AttachCopy(crosshairCross, Trans);

		//make a 2D renderer for it 
		TTN_Renderer2D renderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Crosshair Cross"), crosshairColor);
		AttachCopy(crosshairCross, renderer);
	}

	//crosshair bars
	for (int i = 0; i < 4; i++) {	
		//create an entity
		crosshairHoriLines.push_back(std::pair(CreateEntity(), 1.0f));

		//make a transform for it 
		TTN_Transform Trans = TTN_Transform(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f), glm::vec3(40.0f * crosshairScale, 40.0f * crosshairScale, 1.0f));
		if (i == 0) {
			Trans.SetPos(glm::vec3(0.0f, -22.5f, 2.0f));
			Trans.SetScale(Trans.GetScale() * glm::vec3(0.8f, 1.0f, 1.0f));
		}
		if (i == 1) {
			Trans.SetPos(glm::vec3(0.0f, -45.0f, 2.0f));
			Trans.SetScale(Trans.GetScale() * glm::vec3(0.6f, 1.0f, 1.0f));
		}		
		if (i == 2) {
			Trans.SetPos(glm::vec3(0.0f, -67.5f, 2.0f));
			Trans.SetScale(Trans.GetScale() * glm::vec3(0.4f, 1.0f, 1.0f));
		}		
		if (i == 3) {
			Trans.SetPos(glm::vec3(0.0f, -90.0f, 2.0f));
			Trans.SetScale(Trans.GetScale() * glm::vec3(0.2f, 1.0f, 1.0f));
		}
		
		AttachCopy(crosshairHoriLines[crosshairHoriLines.size() - 1].first, Trans);

		//make a 2D renderer for it 
		TTN_Renderer2D renderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Crosshair Hori Line"), crosshairColor);
		AttachCopy(crosshairHoriLines[crosshairHoriLines.size() - 1].first, renderer);
	}

	//crosshair vertical bar
	{
		crosshairVertLine = CreateEntity();

		//make a transform for it 
		TTN_Transform Trans = TTN_Transform(glm::vec3(0.0f, -56.25, 2.5f), glm::vec3(0.0f), glm::vec3(60.0f * crosshairScale, 75.0f, 1.0f));
		AttachCopy(crosshairVertLine, Trans);

		//make a 2D renderer for it 
		TTN_Renderer2D renderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Crosshair Vert Line"), crosshairColor);
		AttachCopy(crosshairVertLine, renderer);
	}

	//score
	{
		//create an entity in the scene for the logo
		scoreText = CreateEntity();

		//create a transform for the logo
		TTN_Transform logoTrans = TTN_Transform(glm::vec3(825.0f, 480.0f, 1.0f), glm::vec3(0.0f), 
			glm::vec3(scoreTextScale * 550.0f, scoreTextScale * 150.0f, 1.0f));
		AttachCopy(scoreText, logoTrans);

		//create a sprite renderer for the logo
		TTN_Renderer2D logoRenderer = TTN_Renderer2D(textureScore);
		AttachCopy(scoreText, logoRenderer);
	}

	//wave complete 
	for (int i = 0; i < 3; i++) {
		//create an entity in the scene 
		entt::entity entity = CreateEntity();
		if (i == 0) waveText = entity;
		else if (i == 1) waveNums.push_back(entity);
		else if (i == 2) completeText = entity;

		//create a transform 
		TTN_Transform Trans;
		if (i == 0)
			Trans = TTN_Transform(glm::vec3(1500.0f, 0.0f, 1.0f), glm::vec3(0.0f), glm::vec3(200.0f * waveCompleteScale, 150.0f * waveCompleteScale, 1.0f));
		else if (i == 1)
			Trans = TTN_Transform(glm::vec3(Get<TTN_Transform>(waveText).GetGlobalPos().x - 0.5f * std::abs(Get<TTN_Transform>(waveText).GetScale().x) - 0.5f * waveCompleteScale * 150.0f, 0.0f, 1.0f),
				glm::vec3(0.0f), glm::vec3(100.0f * waveCompleteScale, 100.0f * waveCompleteScale, 1.0f));
		else if (i == 2) 
			Trans = TTN_Transform(glm::vec3(Get<TTN_Transform>(waveNums[waveNums.size()-1]).GetGlobalPos().x - 
				0.5f * std::abs(Get<TTN_Transform>(waveNums[waveNums.size() - 1]).GetScale().x) - 0.5f * waveCompleteScale * 350.0f, 0.0f, 1.0f),
				glm::vec3(0.0f), glm::vec3(350.0f * waveCompleteScale, 150.0f * waveCompleteScale, 1.0f));
		AttachCopy(entity, Trans);

		//create a sprite renderer for the logo
		TTN_Renderer2D Renderer;
		if (i == 0)
			Renderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Wave-Text"));
		else if (i == 1)
			Renderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("0-Text"));
		else if (i == 2)
			Renderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Complete-Text"));
		AttachCopy(entity, Renderer);
	}

	//flamethrower background
	{
		//create an entity
		flameThrowerBG = CreateEntity();

		//create a transform 
		TTN_Transform Trans = TTN_Transform(glm::vec3(0.0f, 0.0f, 1.2f), glm::vec3(0.0f), glm::vec3(1000.0f * specialAbilityScale, 1000.0f * specialAbilityScale, 1.0f));
		Trans.SetPos(glm::vec3(-960.0f + 0.5f * std::abs(Trans.GetScale().x), -400.0f, 1.1f));
		AttachCopy(flameThrowerBG, Trans);

		//create a sprite renderer 
		TTN_Renderer2D Renderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Special Ability Background"));
		AttachCopy(flameThrowerBG, Renderer);
	}

	//flamethrower bar
	{
		//create an entity
		flameThrowerBar = CreateEntity();

		//get a copy of the background's transform
		TTN_Transform bgTrans = Get<TTN_Transform>(flameThrowerBG);
		
		//create a transform 
		TTN_Transform Trans = TTN_Transform(glm::vec3(bgTrans.GetPos().x - 0.175f * std::abs(bgTrans.GetScale().x) , bgTrans.GetPos().y - 0.25f * bgTrans.GetScale().y, 1.1f),
			glm::vec3(0.0f), glm::vec3(bgTrans.GetScale().x * 0.65f, bgTrans.GetScale().y * 0.1f, 1.0f));
		AttachCopy(flameThrowerBar, Trans);

		//create a sprite renderer 
		TTN_Renderer2D Renderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Special Ability Bar"), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		AttachCopy(flameThrowerBar, Renderer);
	}

	//flamethrower overlay
	{
		//create an entity
		flameThrowerOverlay = CreateEntity();

		//create a transform 
		TTN_Transform Trans = TTN_Transform(glm::vec3(0.0f, 0.0f, 0.9f), glm::vec3(0.0f), glm::vec3(1000.0f * specialAbilityScale, 1000.0f * specialAbilityScale, 1.0f));
		Trans.SetPos(glm::vec3(-960.0f + 0.5f * std::abs(Trans.GetScale().x), -400.0f, 1.1f));
		AttachCopy(flameThrowerOverlay, Trans);

		//create a sprite renderer 
		TTN_Renderer2D Renderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Special Ability Overlay"));
		AttachCopy(flameThrowerOverlay, Renderer);
	}

	//flamethrower icon
	{
		//create an entity
		flameThrowerIcon = CreateEntity();

		//create a transform 
		TTN_Transform Trans = TTN_Transform(glm::vec3(0.0f, 0.0f, 0.8f), glm::vec3(0.0f), glm::vec3(1000.0f * specialAbilityScale, 1000.0f * specialAbilityScale, 1.0f));
		Trans.SetPos(glm::vec3(-960.0f + 0.5f * std::abs(Trans.GetScale().x), -400.0f, 1.1f));
		AttachCopy(flameThrowerIcon, Trans);

		//create a sprite renderer 
		TTN_Renderer2D Renderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Flamethrower Icon"));
		AttachCopy(flameThrowerIcon, Renderer);
	}

	//flamethrower key
	{
		//create an entity
		flameThrowerKey = CreateEntity();

		//get a copy of the background's transform
		TTN_Transform bgTrans = Get<TTN_Transform>(flameThrowerBG);

		//create a transform 
		TTN_Transform Trans = TTN_Transform(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(bgTrans.GetScale().x * 0.25f, bgTrans.GetScale().y * 0.25f, 1.0f));
		Trans.SetPos(glm::vec3(bgTrans.GetPos().x + 0.4f * std::abs(bgTrans.GetScale().x) + 0.5f * std::abs(Trans.GetScale().x), 
			bgTrans.GetPos().y + 0.025f * bgTrans.GetScale().y, 0.5f));
		AttachCopy(flameThrowerKey, Trans);

		//create a sprite renderer 
		TTN_Renderer2D Renderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Flamethrower Key"));
		AttachCopy(flameThrowerKey, Renderer);
	}

	//bird bomb background
	{
		//create an entity
		birdBombBG = CreateEntity();

		//create a transform 
		TTN_Transform Trans = TTN_Transform(glm::vec3(0.0f, 0.0f, 1.2f), glm::vec3(0.0f), glm::vec3(1000.0f * specialAbilityScale, 1000.0f * specialAbilityScale, 1.0f));
		Trans.SetPos(glm::vec3(-960.0f + 0.5f * std::abs(Trans.GetScale().x), -400.0f + 0.75 * 1000.0f * specialAbilityScale, 1.1f));
		AttachCopy(birdBombBG, Trans);

		//create a sprite renderer 
		TTN_Renderer2D Renderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Special Ability Background"));
		AttachCopy(birdBombBG, Renderer);
	}

	//bird bomb bar
	{
		//create an entity
		birdBombBar = CreateEntity();

		//get a copy of the background's transform
		TTN_Transform bgTrans = Get<TTN_Transform>(birdBombBG);

		//create a transform 
		TTN_Transform Trans = TTN_Transform(glm::vec3(bgTrans.GetPos().x - 0.175f * std::abs(bgTrans.GetScale().x), bgTrans.GetPos().y - 0.25f * bgTrans.GetScale().y, 1.1f),
			glm::vec3(0.0f), glm::vec3(bgTrans.GetScale().x * 0.65f, bgTrans.GetScale().y * 0.1f, 1.0f));
		AttachCopy(birdBombBar, Trans);

		//create a sprite renderer 
		TTN_Renderer2D Renderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Special Ability Bar"), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		AttachCopy(birdBombBar, Renderer);
	}

	//bird bomb overlay
	{
		//create an entity
		birdBombOverlay = CreateEntity();

		//create a transform 
		TTN_Transform Trans = TTN_Transform(glm::vec3(0.0f, 0.0f, 0.9f), glm::vec3(0.0f), glm::vec3(1000.0f * specialAbilityScale, 1000.0f * specialAbilityScale, 1.0f));
		Trans.SetPos(glm::vec3(-960.0f + 0.5f * std::abs(Trans.GetScale().x), -400.0f + 0.75 * 1000.0f * specialAbilityScale, 1.1f));
		AttachCopy(birdBombOverlay, Trans);

		//create a sprite renderer 
		TTN_Renderer2D Renderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Special Ability Overlay"));
		AttachCopy(birdBombOverlay, Renderer);
	}

	//bird bomb icon
	{
		//create an entity
		birdBombIcon = CreateEntity();

		//create a transform 
		TTN_Transform Trans = TTN_Transform(glm::vec3(0.0f, 0.0f, 0.8f), glm::vec3(0.0f), glm::vec3(1000.0f * specialAbilityScale, 1000.0f * specialAbilityScale, 1.0f));
		Trans.SetPos(glm::vec3(-960.0f + 0.5f * std::abs(Trans.GetScale().x), -400.0f + 0.75 * 1000.0f * specialAbilityScale, 1.1f));
		AttachCopy(birdBombIcon, Trans);

		//create a sprite renderer 
		TTN_Renderer2D Renderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Bird Bomb Icon"));
		AttachCopy(birdBombIcon, Renderer);
	}

	//bird bomb key
	{
		//create an entity
		birdBombKey = CreateEntity();

		//get a copy of the background's transform
		TTN_Transform bgTrans = Get<TTN_Transform>(birdBombBG);

		//create a transform 
		TTN_Transform Trans = TTN_Transform(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(bgTrans.GetScale().x * 0.25f, bgTrans.GetScale().y * 0.25f, 1.0f));
		Trans.SetPos(glm::vec3(bgTrans.GetPos().x + 0.4f * std::abs(bgTrans.GetScale().x) + 0.5f * std::abs(Trans.GetScale().x),
			bgTrans.GetPos().y + 0.025f * bgTrans.GetScale().y, 0.5f));
		AttachCopy(birdBombKey, Trans);

		//create a sprite renderer 
		TTN_Renderer2D Renderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Bird Bomb Key"));
		AttachCopy(birdBombKey, Renderer);
	}

	TTN_Scene::InitScene();
}

void GameUI::Update(float deltaTime)
{
	if (!m_paused) {
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
		{
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
	

		//update the wave complete
		{
			while (GetNumOfDigits(m_currentWave) < waveNums.size()) {
				DeleteEntity(waveNums[waveNums.size() - 1]);
				waveNums.pop_back();
			}

			if (GetNumOfDigits(m_currentWave) > waveNums.size())
				MakeWaveNumEntity();

			for (int i = 0; i < waveNums.size(); i++) {
				Get<TTN_Renderer2D>(waveNums[i]).SetSprite(TTN_AssetSystem::GetTexture2D(std::to_string(GetDigit(m_currentWave, waveNums.size() - i - 1)) + "-Text"));
			}

			//update time
			m_waveCompleteTime += deltaTime;
			if (m_waveProgress == 1.0f && waveDone && m_waveCompleteTime > 10.0f) {
				m_waveCompleteTime = 0.0f;
			}

			//update position
			float t = waveCompleteLerpParamter(m_waveCompleteTime / m_waveCompleteTotalTime);
			glm::vec3 centerPos = TTN_Interpolation::Lerp(glm::vec3(1500.0f, 0.0f, 1.0f), glm::vec3(-1500.0f, 0.0f, 1.0f), t);
			int offset = std::ceil((float)waveNums.size() / 2.0f);
			//position of the numbers
			for (int i = 0; i < waveNums.size(); i++) {
				TTN_Transform& trans = Get<TTN_Transform>(waveNums[i]);
				if (i < offset) {
					//places the numbers to the left of the center
					trans.SetPos(centerPos + glm::vec3((float)(offset - i) * 0.5f * std::abs(trans.GetScale().x), 0.0f, 0.0f));
				}
				else {
					//places the numbers on and to the right of the center
					trans.SetPos(centerPos - glm::vec3((float)(i - offset) * 0.5f * std::abs(trans.GetScale().x), 0.0f, 0.0f));
				}
			}
			TTN_Transform& firstNumTrans = Get<TTN_Transform>(waveNums[0]);
			//places the wave text
			Get<TTN_Transform>(waveText).SetPos(firstNumTrans.GetGlobalPos() + glm::vec3(0.5f * std::abs(firstNumTrans.GetScale().x) + 0.4f * 
			std::abs(Get<TTN_Transform>(waveText).GetScale().x), 0.0f, 0.0f));

			TTN_Transform& lastNumTrans = Get<TTN_Transform>(waveNums[waveNums.size() - 1]);
			//places the complete text
			Get<TTN_Transform>(completeText).SetPos(lastNumTrans.GetGlobalPos() - glm::vec3(0.6f * std::abs(firstNumTrans.GetScale().x) + 0.4f *
				std::abs(Get<TTN_Transform>(completeText).GetScale().x), 0.0f, 0.0f));
		}

		//update the flame thrower 
		{
			//normalized cooldown
			flameThrowerCoolDownPercent = TTN_Interpolation::ReMap(flameThrowerMaxCoolDownTime, 0.0f, 0.0f, 1.0f, flameThrowerCoolDownTime);

			//if there is no more cooldown but the numbers are still displayed, delete all the numbers
			while (flameThrowerRealCoolDownTime <= 0.0f && flamethrowerNums.size() > 0) {
				DeleteEntity(flamethrowerNums[flamethrowerNums.size() - 1]);
				flamethrowerNums.pop_back();
			}
			if (flameThrowerRealCoolDownTime > 0.0f) {
				//get the time as an int
				unsigned time = std::ceil(flameThrowerRealCoolDownTime);
				//make sure there are the correct number of digits
				while (GetNumOfDigits(flameThrowerRealCoolDownTime) < flamethrowerNums.size()) {
					DeleteEntity(flamethrowerNums[flamethrowerNums.size() - 1]);
					flamethrowerNums.pop_back();
				}

				if (GetNumOfDigits(time) > flamethrowerNums.size())
					MakeFlamethrowerNumEntity();

				//update each digit approriately
				TTN_Transform bgTrans = Get<TTN_Transform>(flameThrowerBG);
				glm::vec3 centerPos = glm::vec3(bgTrans.GetPos().x - 0.15f * std::abs(bgTrans.GetScale().x),
					bgTrans.GetPos().y + 0.025f * bgTrans.GetScale().y, 0.5f);
				int offset = std::ceil((float)flamethrowerNums.size() / 2.0f);
				for (int i = 0; i < flamethrowerNums.size(); i++) {
					//update position
					TTN_Transform& trans = Get<TTN_Transform>(flamethrowerNums[i]);
					if (i < offset) {
						//places the numbers to the left of the center
						trans.SetPos(centerPos + glm::vec3((float)(offset - i) * 0.5f * std::abs(trans.GetScale().x), 0.0f, 0.0f));
					}
					else {
						//places the numbers on and to the right of the center
						trans.SetPos(centerPos - glm::vec3((float)(i - offset) * 0.5f * std::abs(trans.GetScale().x), 0.0f, 0.0f));
					}

					//update renderer
					Get<TTN_Renderer2D>(flamethrowerNums[i]).SetSprite(TTN_AssetSystem::GetTexture2D(std::to_string(GetDigit(time, flamethrowerNums.size() - i - 1)) + "-Text"));

					//make the renderers of the icon, overlay, and backdrop darker
					Get<TTN_Renderer2D>(flameThrowerIcon).SetColor(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
					Get<TTN_Renderer2D>(flameThrowerBG).SetColor(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
					Get<TTN_Renderer2D>(flameThrowerOverlay).SetColor(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
					//set the colour of the bar
					glm::vec3 color = glm::vec3(1.0f);
					if (flameThrowerCoolDownPercent <= 0.75) {
						color = TTN_Interpolation::Lerp(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), TTN_Interpolation::ReMap(0.0f, 0.75f, 0.0f, 1.0f, flameThrowerCoolDownPercent));
					}
					else {
						color = TTN_Interpolation::Lerp(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), TTN_Interpolation::ReMap(0.75f, 1.0f, 0.0f, 1.0f, flameThrowerCoolDownPercent));
					}
					Get<TTN_Renderer2D>(flameThrowerBar).SetColor(glm::vec4(color, 1.0f));
				}
			}
			else {
				//make the renderers of the icon, overlay, bar and backdrop to their regular color
				Get<TTN_Renderer2D>(flameThrowerIcon).SetColor(glm::vec4(1.0f));
				Get<TTN_Renderer2D>(flameThrowerBG).SetColor(glm::vec4(1.0f));
				Get<TTN_Renderer2D>(flameThrowerOverlay).SetColor(glm::vec4(1.0f));
				Get<TTN_Renderer2D>(flameThrowerBar).SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
			}

			//set the ammount the bar should render
			Get<TTN_Renderer2D>(flameThrowerBar).SetHoriMask(flameThrowerCoolDownPercent);
		}

		//update the bird bomb
		{
			//normalized cooldown
			birdBombCoolDownPercent = TTN_Interpolation::ReMap(birdBombMaxCoolDownTime, 0.0f, 0.0f, 1.0f, birdBombCoolDownTime);

			//if there is no more cooldown but the numbers are still displayed, delete all the numbers
			while (birdBombRealCoolDownTime <= 0.0f && birdBombNums.size() > 0) {
				DeleteEntity(birdBombNums[birdBombNums.size() - 1]);
				birdBombNums.pop_back();
			}
			if (birdBombRealCoolDownTime > 0.0f) {
				//get the time as an int
				unsigned time = std::ceil(birdBombRealCoolDownTime);
				//make sure there are the correct number of digits
				while (GetNumOfDigits(birdBombRealCoolDownTime) < birdBombNums.size()) {
					DeleteEntity(birdBombNums[birdBombNums.size() - 1]);
					birdBombNums.pop_back();
				}

				if (GetNumOfDigits(time) > birdBombNums.size())
					MakeBirdBombNumEntity();

				//update each digit approriately
				TTN_Transform bgTrans = Get<TTN_Transform>(birdBombBG);
				glm::vec3 centerPos = glm::vec3(bgTrans.GetPos().x - 0.15f * std::abs(bgTrans.GetScale().x),
					bgTrans.GetPos().y + 0.025f * bgTrans.GetScale().y, 0.5f);
				int offset = std::ceil((float)birdBombNums.size() / 2.0f);
				for (int i = 0; i < birdBombNums.size(); i++) {
					//update position
					TTN_Transform& trans = Get<TTN_Transform>(birdBombNums[i]);
					if (i < offset) {
						//places the numbers to the left of the center
						trans.SetPos(centerPos + glm::vec3((float)(offset - i) * 0.5f * std::abs(trans.GetScale().x), 0.0f, 0.0f));
					}
					else {
						//places the numbers on and to the right of the center
						trans.SetPos(centerPos - glm::vec3((float)(i - offset) * 0.5f * std::abs(trans.GetScale().x), 0.0f, 0.0f));
					}

					//update renderer
					Get<TTN_Renderer2D>(birdBombNums[i]).SetSprite(TTN_AssetSystem::GetTexture2D(std::to_string(GetDigit(time, birdBombNums.size() - i - 1)) + "-Text"));

					//make the renderers of the icon, overlay, and backdrop darker
					Get<TTN_Renderer2D>(birdBombBG).SetColor(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
					Get<TTN_Renderer2D>(birdBombOverlay).SetColor(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
					Get<TTN_Renderer2D>(birdBombIcon).SetColor(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
					//set the colour of the bar
					glm::vec3 color = glm::vec3(1.0f);
					if (birdBombCoolDownPercent <= 0.75) {
						color = TTN_Interpolation::Lerp(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), TTN_Interpolation::ReMap(0.0f, 0.75f, 0.0f, 1.0f, birdBombCoolDownPercent));
					}
					else {
						color = TTN_Interpolation::Lerp(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), TTN_Interpolation::ReMap(0.75f, 1.0f, 0.0f, 1.0f, birdBombCoolDownPercent));
					}
					Get<TTN_Renderer2D>(birdBombBar).SetColor(glm::vec4(color, 1.0f));
				}
			}
			else {
				//make the renderers of the icon, overlay, bar and backdrop to their regular color
				Get<TTN_Renderer2D>(birdBombBG).SetColor(glm::vec4(1.0f));
				Get<TTN_Renderer2D>(birdBombOverlay).SetColor(glm::vec4(1.0f));
				Get<TTN_Renderer2D>(birdBombIcon).SetColor(glm::vec4(1.0f));
				Get<TTN_Renderer2D>(birdBombBar).SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
			}

			//set the ammount the bar should render
			Get<TTN_Renderer2D>(birdBombBar).SetHoriMask(birdBombCoolDownPercent);
		}
	}

	//update the base scene
	TTN_Scene::Update(deltaTime);
}

void GameUI::MakeScoreNumEntity()
{
	scoreNums.push_back(CreateEntity());

	//reference to the base score text's transform
	TTN_Transform& scoreTrans = Get<TTN_Transform>(scoreText);

	//setup a transform for the new entity
	TTN_Transform numTrans = TTN_Transform(glm::vec3(scoreTrans.GetGlobalPos().x - 0.3f * std::abs(scoreTrans.GetScale().x) - 
		(float)scoreNums.size() * 0.5f * scoreTextScale * 150.0f, scoreTrans.GetGlobalPos().y, scoreTrans.GetGlobalPos().z),
		glm::vec3(0.0f), glm::vec3(scoreTextScale * 150.0f, scoreTextScale * 150.0f, 1.0f));
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
		glm::vec3(0.0f), glm::vec3(healthTextScale * 150.0f, healthTextScale * 150.0f, 1.0f));
	AttachCopy(healthNums[healthNums.size() - 1], numTrans);

	//setup a 2D renderer for the new entity
			//create a sprite renderer for the logo
	TTN_Renderer2D numRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("0-Text"));
	AttachCopy(healthNums[healthNums.size() - 1], numRenderer);
}

void GameUI::MakeWaveNumEntity()
{
	waveNums.push_back(CreateEntity());

	//reference to the wave's transform
	TTN_Transform& Trans = Get<TTN_Transform>(waveText);

	//setup a transform for the new entity
	TTN_Transform numTrans = TTN_Transform(glm::vec3(Trans.GetGlobalPos().x + 0.5f * std::abs(Trans.GetScale().x) -
		(float)waveNums.size() * 0.5f * waveCompleteScale * 150.0f, Trans.GetGlobalPos().y, Trans.GetGlobalPos().z),
		glm::vec3(0.0f), glm::vec3(waveCompleteScale * 100.0f, waveCompleteScale * 100.0f, 1.0f));
	AttachCopy(waveNums[waveNums.size() - 1], numTrans);

	//setup a 2D renderer for the new entity
			//create a sprite renderer for the logo
	TTN_Renderer2D numRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("0-Text"));
	AttachCopy(waveNums[waveNums.size() - 1], numRenderer);
}

void GameUI::MakeFlamethrowerNumEntity()
{
	flamethrowerNums.push_back(CreateEntity());

	//reference to the icon's transform
	TTN_Transform& Trans = Get<TTN_Transform>(flameThrowerIcon);

	//setup a transform for the new entity
	TTN_Transform numTrans = TTN_Transform(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(150.0f * scoreTextScale, 150.0f * scoreTextScale, 1.0f));
	AttachCopy(flamethrowerNums[flamethrowerNums.size() - 1], numTrans);

	//setup a 2D renderer for the new entity
			//create a sprite renderer for the logo
	TTN_Renderer2D numRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("0-Text"));
	AttachCopy(flamethrowerNums[flamethrowerNums.size() - 1], numRenderer);
}

void GameUI::MakeBirdBombNumEntity()
{
	birdBombNums.push_back(CreateEntity());

	//reference to the bf's transform
	TTN_Transform& Trans = Get<TTN_Transform>(birdBombBG);

	//setup a transform for the new entity
	TTN_Transform numTrans = TTN_Transform(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(150.0f * scoreTextScale, 150.0f * scoreTextScale, 1.0f));
	AttachCopy(birdBombNums[birdBombNums.size() - 1], numTrans);

	//setup a 2D renderer for the new entity
			//create a sprite renderer for the logo
	TTN_Renderer2D numRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("0-Text"));
	AttachCopy(birdBombNums[birdBombNums.size() - 1], numRenderer);
}