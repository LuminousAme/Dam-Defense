//Dam Defense, by Atlas X Games
//OptionMenu.cpp, the source file for the scene class representing the options menu in the game
#include "OptionMenu.h"

OptionsMenu::OptionsMenu() : TTN_Scene()
{
}

void OptionsMenu::InitScene()
{
	mouse_sen = 50.f;
	volume = 100.f;
	volumeMusic = 20.f;
	volumeSFX = 5.0f;

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

#pragma region MOUSE SENSITIVITY

	//mouse sensitivity bar border
	{
		//create an entity in the scene for the mouse sensitivity bar overlay
		mouseBar = CreateEntity();

		//create a transform for the mouse sensitivity bar overlay
		TTN_Transform healthTrans = TTN_Transform(glm::vec3(0.0f, 420.0f, 0.9f), glm::vec3(0.0f), glm::vec3(4200.0f * mouseScale, 239.0f * mouseScale, 1.0f));
		AttachCopy(mouseBar, healthTrans);

		//create a sprite renderer for the mouse sensitivity bar overlay
		TTN_Renderer2D healthRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Bar Border"));
		AttachCopy(mouseBar, healthRenderer);
	}

	//mouse sensitivity bar
	{
		//create an entity for the health bar
		mouseSensitivity = CreateEntity();

		//create a transform for the health bar
		TTN_Transform healthTrans = TTN_Transform(glm::vec3(0.0f, 420.0f, 1.0f), glm::vec3(0.0f), glm::vec3(4200.0f * mouseScale, 239.0f * mouseScale, 1.0f));
		AttachCopy(mouseSensitivity, healthTrans);

		//create a sprite renderer for the health bar
		TTN_Renderer2D healthRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Bar"), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		float normalizedMouseSen = mouse_sen / 100.f;
		healthRenderer.SetHoriMask(normalizedMouseSen);
		AttachCopy(mouseSensitivity, healthRenderer);
	}

	// bar background
	{
		//create an entity for the mouse bar background
		mouseBarBg = CreateEntity();

		//create a transform for the mouse bar background
		TTN_Transform healthTrans = TTN_Transform(glm::vec3(0.0f, 420.0f, 1.1f), glm::vec3(0.0f), glm::vec3(4200.0f * mouseScale, 239.0f * mouseScale, 1.0f));
		AttachCopy(mouseBarBg, healthTrans);

		//create a sprite renderer for the mouse bar background
		TTN_Renderer2D healthRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Bar BG"), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

		AttachCopy(mouseBarBg, healthRenderer);
	}

#pragma endregion

#pragma region VOLUME

	//master volume
	{
		//volume bar border
		{
			//create an entity in the scene for the mouse sensitivity bar overlay
			volumeBarBorder = CreateEntity();

			//create a transform for the mouse sensitivity bar overlay
			TTN_Transform healthTrans = TTN_Transform(glm::vec3(0.0f, 320.0f, 0.9f), glm::vec3(0.0f), glm::vec3(4200.0f * mouseScale, 239.0f * mouseScale, 1.0f));
			AttachCopy(volumeBarBorder, healthTrans);

			//create a sprite renderer for the mouse sensitivity bar overlay
			TTN_Renderer2D healthRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Bar Border"));
			AttachCopy(volumeBarBorder, healthRenderer);
		}

		//volume bar
		{
			//create an entity for the health bar
			volumeBar = CreateEntity();

			//create a transform for the health bar
			TTN_Transform healthTrans = TTN_Transform(glm::vec3(0.0f, 320.0f, 1.0f), glm::vec3(0.0f), glm::vec3(4200.0f * mouseScale, 239.0f * mouseScale, 1.0f));
			AttachCopy(volumeBar, healthTrans);

			//create a sprite renderer for the health bar
			TTN_Renderer2D healthRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Bar"), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			float normalizedVolume = volume / 100.f;
			healthRenderer.SetHoriMask(normalizedVolume);
			AttachCopy(volumeBar, healthRenderer);
		}

		// volume bar background
		{
			//create an entity for the mouse bar background
			volumeBarBg = CreateEntity();

			//create a transform for the mouse bar background
			TTN_Transform healthTrans = TTN_Transform(glm::vec3(0.0f, 320.0f, 1.1f), glm::vec3(0.0f), glm::vec3(4200.0f * mouseScale, 239.0f * mouseScale, 1.0f));
			AttachCopy(volumeBarBg, healthTrans);

			//create a sprite renderer for the mouse bar background
			TTN_Renderer2D healthRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Bar BG"), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

			AttachCopy(volumeBarBg, healthRenderer);
		}
	}

	//music volume
	{
		//music volume bar border
		{
			//create an entity in the scene for the bar border
			MusicVolumeBarBorder = CreateEntity();

			//create a transform for the mouse sensitivity bar overlay
			TTN_Transform healthTrans = TTN_Transform(glm::vec3(0.0f, 220.0f, 0.9f), glm::vec3(0.0f), glm::vec3(4200.0f * mouseScale, 239.0f * mouseScale, 1.0f));
			AttachCopy(MusicVolumeBarBorder, healthTrans);

			//create a sprite renderer for the mouse sensitivity bar overlay
			TTN_Renderer2D healthRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Bar Border"));
			AttachCopy(MusicVolumeBarBorder, healthRenderer);
		}

		//volume bar
		{
			//create an entity for the health bar
			MusicVolumeBar = CreateEntity();

			//create a transform for the health bar
			TTN_Transform healthTrans = TTN_Transform(glm::vec3(0.0f, 220.0f, 1.0f), glm::vec3(0.0f), glm::vec3(4200.0f * mouseScale, 239.0f * mouseScale, 1.0f));
			AttachCopy(MusicVolumeBar, healthTrans);

			//create a sprite renderer for the health bar
			TTN_Renderer2D healthRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Bar"), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			float normalizedVolumeMusic = volumeMusic / 100.f;
			healthRenderer.SetHoriMask(normalizedVolumeMusic);
			AttachCopy(MusicVolumeBar, healthRenderer);
		}

		// volume bar background
		{
			//create an entity for the mouse bar background
			MusicVolumeBarBg = CreateEntity();

			//create a transform for the mouse bar background
			TTN_Transform healthTrans = TTN_Transform(glm::vec3(0.0f, 220.0f, 1.1f), glm::vec3(0.0f), glm::vec3(4200.0f * mouseScale, 239.0f * mouseScale, 1.0f));
			AttachCopy(MusicVolumeBarBg, healthTrans);

			//create a sprite renderer for the mouse bar background
			TTN_Renderer2D healthRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Bar BG"), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

			AttachCopy(MusicVolumeBarBg, healthRenderer);
		}
	}

	//SFX volume
	{
		//sfx volume bar border
		{
			//create an entity in the scene for the bar border
			SFXvolumeBarBorder = CreateEntity();

			//create a transform for the mouse sensitivity bar overlay
			TTN_Transform healthTrans = TTN_Transform(glm::vec3(0.0f, 20.0f, 0.9f), glm::vec3(0.0f), glm::vec3(4200.0f * mouseScale, 239.0f * mouseScale, 1.0f));
			AttachCopy(SFXvolumeBarBorder, healthTrans);

			//create a sprite renderer for the mouse sensitivity bar overlay
			TTN_Renderer2D healthRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Bar Border"));
			AttachCopy(SFXvolumeBarBorder, healthRenderer);
		}

		// sfx volume bar
		{
			//create an entity for the health bar
			SFXvolumeBar = CreateEntity();

			//create a transform for the health bar
			TTN_Transform healthTrans = TTN_Transform(glm::vec3(0.0f, 20.0f, 1.0f), glm::vec3(0.0f), glm::vec3(4200.0f * mouseScale, 239.0f * mouseScale, 1.0f));
			AttachCopy(SFXvolumeBar, healthTrans);

			//create a sprite renderer for the health bar
			TTN_Renderer2D healthRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Bar"), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			float normalizedSFXMusic = volumeSFX / 100.f;
			healthRenderer.SetHoriMask(normalizedSFXMusic);
			AttachCopy(SFXvolumeBar, healthRenderer);
		}

		// sfx volume bar background
		{
			//create an entity for the mouse bar background
			SFXvolumeBarBg = CreateEntity();

			//create a transform for the mouse bar background
			TTN_Transform healthTrans = TTN_Transform(glm::vec3(0.0f, 20.0f, 1.1f), glm::vec3(0.0f), glm::vec3(4200.0f * mouseScale, 239.0f * mouseScale, 1.0f));
			AttachCopy(SFXvolumeBarBg, healthTrans);

			//create a sprite renderer for the mouse bar background
			TTN_Renderer2D healthRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Bar BG"), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

			AttachCopy(SFXvolumeBarBg, healthRenderer);
		}
	}

#pragma endregion
}

void OptionsMenu::Update(float deltaTime)
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

	//update the mouse sensitivity number
	{
		unsigned mouse = std::ceil(mouse_sen);
		while (GetNumOfDigits(mouse) < mouseNums.size()) {
			DeleteEntity(mouseNums[mouseNums.size() - 1]);
			mouseNums.pop_back();
		}

		if (GetNumOfDigits(mouse) > mouseNums.size())
			MakeMouseNumEntity();

		for (int i = 0; i < mouseNums.size(); i++) {
			Get<TTN_Renderer2D>(mouseNums[i]).SetSprite(TTN_AssetSystem::GetTexture2D(std::to_string(GetDigit(mouse, mouseNums.size() - i - 1)) + "-Text"));
		}
	}

	//update the volume number
	{
		unsigned volumeN = std::ceil(volume);
		while (GetNumOfDigits(volumeN) < volumeNums.size()) {
			DeleteEntity(volumeNums[volumeNums.size() - 1]);
			volumeNums.pop_back();
		}

		if (GetNumOfDigits(volumeN) > volumeNums.size())
			MakeVolumeNumEntity();

		for (int i = 0; i < volumeNums.size(); i++) {
			Get<TTN_Renderer2D>(volumeNums[i]).SetSprite(TTN_AssetSystem::GetTexture2D(std::to_string(GetDigit(volumeN, volumeNums.size() - i - 1)) + "-Text"));
		}
	}

	//update the music volume number
	{
		unsigned volumeM = std::ceil(volumeMusic);
		while (GetNumOfDigits(volumeM) < MusicVolumeNums.size()) {
			DeleteEntity(MusicVolumeNums[MusicVolumeNums.size() - 1]);
			MusicVolumeNums.pop_back();
		}

		if (GetNumOfDigits(volumeM) > MusicVolumeNums.size())
			MakeMusicNumEntity();

		for (int i = 0; i < MusicVolumeNums.size(); i++) {
			Get<TTN_Renderer2D>(MusicVolumeNums[i]).SetSprite(TTN_AssetSystem::GetTexture2D(std::to_string(GetDigit(volumeM, MusicVolumeNums.size() - i - 1)) + "-Text"));
		}
	}

	//update the sfx volume number
	{
		unsigned volumeS = std::ceil(volumeSFX);
		while (GetNumOfDigits(volumeS) < SFXvolumeNums.size()) {
			DeleteEntity(SFXvolumeNums[SFXvolumeNums.size() - 1]);
			SFXvolumeNums.pop_back();
		}

		if (GetNumOfDigits(volumeS) > SFXvolumeNums.size())
			MakeSFXNumEntity();

		for (int i = 0; i < SFXvolumeNums.size(); i++) {
			Get<TTN_Renderer2D>(SFXvolumeNums[i]).SetSprite(TTN_AssetSystem::GetTexture2D(std::to_string(GetDigit(volumeS, SFXvolumeNums.size() - i - 1)) + "-Text"));
		}
	}


	//update the base scene
	TTN_Scene::Update(deltaTime);
}

void OptionsMenu::MouseButtonDownChecks()
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

		//get play buttons transform
		TTN_Transform playButtonTrans = Get<TTN_Transform>(mouseSensitivity);
		if (mousePosWorldSpace.x < playButtonTrans.GetPos().x + 0.5f * abs(playButtonTrans.GetScale().x) &&
			mousePosWorldSpace.x > playButtonTrans.GetPos().x - 0.5f * abs(playButtonTrans.GetScale().x) &&
			mousePosWorldSpace.y < playButtonTrans.GetPos().y + 0.5f * abs(playButtonTrans.GetScale().y) &&
			mousePosWorldSpace.y > playButtonTrans.GetPos().y - 0.5f * abs(playButtonTrans.GetScale().y)) {
			float normalizedMouseSen = mouse_sen / 100.f;;
			if (mousePosWorldSpace.x == 0.0f)
				normalizedMouseSen = 0.5f;

			else if (mousePosWorldSpace.x > 0.0f)
				normalizedMouseSen = abs(TTN_Interpolation::ReMap(0.0f, 100.0f, 0.0f, 1.0f, (abs(mousePosWorldSpace.x / playButtonTrans.GetScale().x) * 100.f)) - 0.5f);

			else if (mousePosWorldSpace.x < 0.0f)
				normalizedMouseSen = abs(TTN_Interpolation::ReMap(0.0f, 100.0f, 0.0f, 1.0f, (abs(mousePosWorldSpace.x / playButtonTrans.GetScale().x) * 100.f)) + 0.5f);
			else
				float normalizedMouseSen = mouse_sen / 100.f;

			Get<TTN_Renderer2D>(mouseSensitivity).SetHoriMask(normalizedMouseSen);

			mouse_sen = normalizedMouseSen * 100.f;
			/*std::cout << "SNESE: " << mouse_sen << std::endl;
			std::cout << normalizedMouseSen << std::endl;*/
			//std::cout << "mouse: " << mousePosWorldSpace.x << std::endl;
		}

		TTN_Transform volumeBarTrans = Get<TTN_Transform>(volumeBar);
		if (mousePosWorldSpace.x < volumeBarTrans.GetPos().x + 0.5f * abs(volumeBarTrans.GetScale().x) &&
			mousePosWorldSpace.x > volumeBarTrans.GetPos().x - 0.5f * abs(volumeBarTrans.GetScale().x) &&
			mousePosWorldSpace.y < volumeBarTrans.GetPos().y + 0.5f * abs(volumeBarTrans.GetScale().y) &&
			mousePosWorldSpace.y > volumeBarTrans.GetPos().y - 0.5f * abs(volumeBarTrans.GetScale().y)) {
			float normalizedVolume = volume / 100.f;;
			if (mousePosWorldSpace.x == 0.0f)
				normalizedVolume = 0.5f;

			else if (mousePosWorldSpace.x > 0.0f)
				normalizedVolume = abs(TTN_Interpolation::ReMap(0.0f, 100.0f, 0.0f, 1.0f, (abs(mousePosWorldSpace.x / volumeBarTrans.GetScale().x) * 100.f)) - 0.5f);

			else if (mousePosWorldSpace.x < 0.0f)
				normalizedVolume = abs(TTN_Interpolation::ReMap(0.0f, 100.0f, 0.0f, 1.0f, (abs(mousePosWorldSpace.x / volumeBarTrans.GetScale().x) * 100.f)) + 0.5f);
			else
				float normalizedVolume = volume / 100.f;

			Get<TTN_Renderer2D>(volumeBar).SetHoriMask(normalizedVolume);

			volume = normalizedVolume * 100.f;
			std::cout << "SNESE: " << volume << std::endl;
			std::cout << normalizedVolume << std::endl;
			//std::cout << "mouse: " << mousePosWorldSpace.x << std::endl;
		}

		TTN_Transform MusicVolumeBarTrans = Get<TTN_Transform>(MusicVolumeBar);
		if (mousePosWorldSpace.x < MusicVolumeBarTrans.GetPos().x + 0.5f * abs(MusicVolumeBarTrans.GetScale().x) &&
			mousePosWorldSpace.x > MusicVolumeBarTrans.GetPos().x - 0.5f * abs(MusicVolumeBarTrans.GetScale().x) &&
			mousePosWorldSpace.y < MusicVolumeBarTrans.GetPos().y + 0.5f * abs(MusicVolumeBarTrans.GetScale().y) &&
			mousePosWorldSpace.y > MusicVolumeBarTrans.GetPos().y - 0.5f * abs(MusicVolumeBarTrans.GetScale().y)) {
			float normalizedMusic = volumeMusic / 100.f;;
			if (mousePosWorldSpace.x == 0.0f)
				normalizedMusic = 0.5f;

			else if (mousePosWorldSpace.x > 0.0f)
				normalizedMusic = abs(TTN_Interpolation::ReMap(0.0f, 100.0f, 0.0f, 1.0f, (abs(mousePosWorldSpace.x / MusicVolumeBarTrans.GetScale().x) * 100.f)) - 0.5f);

			else if (mousePosWorldSpace.x < 0.0f)
				normalizedMusic = abs(TTN_Interpolation::ReMap(0.0f, 100.0f, 0.0f, 1.0f, (abs(mousePosWorldSpace.x / MusicVolumeBarTrans.GetScale().x) * 100.f)) + 0.5f);
			else
				float normalizedMusic = volumeMusic / 100.f;

			Get<TTN_Renderer2D>(MusicVolumeBar).SetHoriMask(normalizedMusic);
			volumeMusic = normalizedMusic * 100.f;
		}

		TTN_Transform SFXVolumeBarTrans = Get<TTN_Transform>(SFXvolumeBar);
		if (mousePosWorldSpace.x < SFXVolumeBarTrans.GetPos().x + 0.5f * abs(SFXVolumeBarTrans.GetScale().x) &&
			mousePosWorldSpace.x > SFXVolumeBarTrans.GetPos().x - 0.5f * abs(SFXVolumeBarTrans.GetScale().x) &&
			mousePosWorldSpace.y < SFXVolumeBarTrans.GetPos().y + 0.5f * abs(SFXVolumeBarTrans.GetScale().y) &&
			mousePosWorldSpace.y > SFXVolumeBarTrans.GetPos().y - 0.5f * abs(SFXVolumeBarTrans.GetScale().y)) {
			float normalizedSFX = volumeSFX / 100.f;;
			if (mousePosWorldSpace.x == 0.0f)
				normalizedSFX = 0.5f;

			else if (mousePosWorldSpace.x > 0.0f)
				normalizedSFX = abs(TTN_Interpolation::ReMap(0.0f, 100.0f, 0.0f, 1.0f, (abs(mousePosWorldSpace.x / MusicVolumeBarTrans.GetScale().x) * 100.f)) - 0.5f);

			else if (mousePosWorldSpace.x < 0.0f)
				normalizedSFX = abs(TTN_Interpolation::ReMap(0.0f, 100.0f, 0.0f, 1.0f, (abs(mousePosWorldSpace.x / MusicVolumeBarTrans.GetScale().x) * 100.f)) + 0.5f);
			else
				float normalizedSFX = volumeSFX / 100.f;

			Get<TTN_Renderer2D>(SFXvolumeBar).SetHoriMask(normalizedSFX);
			volumeSFX = normalizedSFX * 100.f;
		}


	}

}

void OptionsMenu::KeyDownChecks()
{
	if (TTN_Application::TTN_Input::GetKeyDown(TTN_KeyCode::Esc)) {
		shouldMenu = true;
	}

	if (TTN_Application::TTN_Input::GetKeyDown(TTN_KeyCode::B)) {
		shouldBack = true;
	}
}

void OptionsMenu::MakeMouseNumEntity()
{
	mouseNums.push_back(CreateEntity());

	//reference to the health bar's transform
	TTN_Transform& healthTrans = Get<TTN_Transform>(mouseBar);

	//setup a transform for the new entity
	TTN_Transform numTrans = TTN_Transform(glm::vec3(healthTrans.GetGlobalPos().x + 0.3f * std::abs(healthTrans.GetScale().x) -
		(float)mouseNums.size() * 0.5f * mouseNumScale * 150.0f, healthTrans.GetGlobalPos().y, healthTrans.GetGlobalPos().z),
		glm::vec3(0.0f), glm::vec3(mouseNumScale * 150.0f, mouseNumScale * 150.0f, 1.0f));
	AttachCopy(mouseNums[mouseNums.size() - 1], numTrans);

	//setup a 2D renderer for the new entity
			//create a sprite renderer for the logo
	TTN_Renderer2D numRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("0-Text"));
	AttachCopy(mouseNums[mouseNums.size() - 1], numRenderer);
}

void OptionsMenu::MakeVolumeNumEntity()
{
	volumeNums.push_back(CreateEntity());

	//reference to the health bar's transform
	TTN_Transform& volumeTrans = Get<TTN_Transform>(volumeBar);

	//setup a transform for the new entity
	TTN_Transform numTrans = TTN_Transform(glm::vec3(volumeTrans.GetGlobalPos().x + 0.3f * std::abs(volumeTrans.GetScale().x) -
		(float)volumeNums.size() * 0.5f * volumeNumScale * 150.0f, volumeTrans.GetGlobalPos().y, volumeTrans.GetGlobalPos().z),
		glm::vec3(0.0f), glm::vec3(volumeNumScale * 150.0f, volumeNumScale * 150.0f, 1.0f));
	AttachCopy(volumeNums[volumeNums.size() - 1], numTrans);

	//setup a 2D renderer for the new entity
			//create a sprite renderer for the logo
	TTN_Renderer2D numRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("0-Text"));
	AttachCopy(volumeNums[volumeNums.size() - 1], numRenderer);
}

void OptionsMenu::MakeMusicNumEntity()
{
	MusicVolumeNums.push_back(CreateEntity());

	//reference to the health bar's transform
	TTN_Transform& volumeTrans = Get<TTN_Transform>(MusicVolumeBar);

	//setup a transform for the new entity
	TTN_Transform numTrans = TTN_Transform(glm::vec3(volumeTrans.GetGlobalPos().x + 0.3f * std::abs(volumeTrans.GetScale().x) -
		(float)MusicVolumeNums.size() * 0.5f * volumeNumScale * 150.0f, volumeTrans.GetGlobalPos().y, volumeTrans.GetGlobalPos().z),
		glm::vec3(0.0f), glm::vec3(volumeNumScale * 150.0f, volumeNumScale * 150.0f, 1.0f));
	AttachCopy(MusicVolumeNums[MusicVolumeNums.size() - 1], numTrans);

	//setup a 2D renderer for the new entity
			//create a sprite renderer for the logo
	TTN_Renderer2D numRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("0-Text"));
	AttachCopy(MusicVolumeNums[MusicVolumeNums.size() - 1], numRenderer);
}

void OptionsMenu::MakeSFXNumEntity()
{
	SFXvolumeNums.push_back(CreateEntity());

	//reference to the health bar's transform
	TTN_Transform& volumeTrans = Get<TTN_Transform>(SFXvolumeBar);

	//setup a transform for the new entity
	TTN_Transform numTrans = TTN_Transform(glm::vec3(volumeTrans.GetGlobalPos().x + 0.3f * std::abs(volumeTrans.GetScale().x) -
		(float)SFXvolumeNums.size() * 0.5f * volumeNumScale * 150.0f, volumeTrans.GetGlobalPos().y, volumeTrans.GetGlobalPos().z),
		glm::vec3(0.0f), glm::vec3(volumeNumScale * 150.0f, volumeNumScale * 150.0f, 1.0f));
	AttachCopy(SFXvolumeNums[SFXvolumeNums.size() - 1], numTrans);

	//setup a 2D renderer for the new entity
	//create a sprite renderer for the logo
	TTN_Renderer2D numRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("0-Text"));
	AttachCopy(SFXvolumeNums[SFXvolumeNums.size() - 1], numRenderer);
}

