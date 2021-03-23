//Dam Defense, by Atlas X Games
//OptionMenu.cpp, the source file for the scene class representing the options menu in the game
#include "OptionMenu.h"

// Taken from https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
#pragma region String Trimming

// trim from start (in place)
static inline void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch);
	}));
}

// trim from end (in place)
static inline void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !std::isspace(ch);
	}).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string& s) {
	ltrim(s);
	rtrim(s);
}

#pragma endregion 

OptionsMenu::OptionsMenu() : TTN_Scene()
{
}

void OptionsMenu::InitScene()
{
	textureButton1 = TTN_AssetSystem::GetTexture2D("Button Base");
	textureButton2 = TTN_AssetSystem::GetTexture2D("Button Hovering");
	m_InputDelay = 0.3f;

	previousSceneMainMenu = true;

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
			TTN_Transform healthTrans = TTN_Transform(glm::vec3(0.0f, 120.0f, 0.9f), glm::vec3(0.0f), glm::vec3(4200.0f * mouseScale, 239.0f * mouseScale, 1.0f));
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
			TTN_Transform healthTrans = TTN_Transform(glm::vec3(0.0f, 120.0f, 1.0f), glm::vec3(0.0f), glm::vec3(4200.0f * mouseScale, 239.0f * mouseScale, 1.0f));
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
			TTN_Transform healthTrans = TTN_Transform(glm::vec3(0.0f, 120.0f, 1.1f), glm::vec3(0.0f), glm::vec3(4200.0f * mouseScale, 239.0f * mouseScale, 1.0f));
			AttachCopy(SFXvolumeBarBg, healthTrans);

			//create a sprite renderer for the mouse bar background
			TTN_Renderer2D healthRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Bar BG"), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

			AttachCopy(SFXvolumeBarBg, healthRenderer);
		}
	}

#pragma endregion

	//indicator for no color correction
	{
		for (int i = 0; i < 2; i++) {
			//button border
			{
				//create an entity in the scene for the bar border
				entt::entity temp = CreateEntity();
				if (i == 0) OffBarBorder = temp;
				else if (i == 1) ColorBarBorder = temp;

				//create a transform for the mouse sensitivity bar overlay
				TTN_Transform buttonTrans;
				if (i == 0) buttonTrans = TTN_Transform(glm::vec3(450.0f, 20.0f, 0.9f), glm::vec3(0.0f), glm::vec3(249.0f * mouseScale, 239.0f * mouseScale, 1.0f));
				else if (i == 1) buttonTrans = TTN_Transform(glm::vec3(50.0f, 20.0f, 0.9f), glm::vec3(0.0f), glm::vec3(249.0f * mouseScale, 239.0f * mouseScale, 1.0f));
				AttachCopy(temp, buttonTrans);

				//create a sprite renderer for the bar overlay
				TTN_Renderer2D healthRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Bar Border"));
				AttachCopy(temp, healthRenderer);
			}

			// off button
			{
				//create an entity in the scene for off bar
				entt::entity temp = CreateEntity();
				if (i == 0) OffBar = temp;
				else if (i == 1) ColorBar = temp;

				//create a transform for the off button
				TTN_Transform buttonTrans;
				if (i == 0) buttonTrans = TTN_Transform(glm::vec3(450.0f, 20.0f, 0.1f), glm::vec3(0.0f), glm::vec3(249.0f * mouseScale, 239.0f * mouseScale, 1.0f));
				else if (i == 1) buttonTrans = TTN_Transform(glm::vec3(50.0f, 20.0f, 0.1f), glm::vec3(0.0f), glm::vec3(249.0f * mouseScale, 239.0f * mouseScale, 1.0f));
				AttachCopy(temp, buttonTrans);

				//create a sprite renderer for the health bar
				TTN_Renderer2D healthRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Bar"), glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
				AttachCopy(temp, healthRenderer);
			}

			// off button background
			{
				entt::entity temp = CreateEntity();
				if (i == 0) OffBarBg = temp;
				else if (i == 1) ColorBarBg = temp;

				//create a transform for the off button
				TTN_Transform buttonTrans;
				if (i == 0) buttonTrans = TTN_Transform(glm::vec3(450.0f, 20.0f, 0.9f), glm::vec3(0.0f), glm::vec3(249.0f * mouseScale, 239.0f * mouseScale, 1.0f));
				else if (i == 1) buttonTrans = TTN_Transform(glm::vec3(50.0f, 20.0f, 0.9f), glm::vec3(0.0f), glm::vec3(249.0f * mouseScale, 239.0f * mouseScale, 1.0f));
				AttachCopy(temp, buttonTrans);

				//create a sprite renderer for the mouse bar background
				TTN_Renderer2D healthRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Bar BG"), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

				AttachCopy(temp, healthRenderer);
			}
		}
	}

	//actual button
	for (int i = 0; i < 2; i++) {
		entt::entity temp = CreateEntity();
		if (i == 0) buttonOff = temp;
		else if (i == 1) buttonColor1 = temp;

		//create a transform for the button
		TTN_Transform buttonTrans;
		if (i == 0) buttonTrans = TTN_Transform(glm::vec3(650.0f, 0.0f, 2.0f), glm::vec3(0.0f), glm::vec3(250.0f, 150.0, 1.0f));
		else if (i == 1) buttonTrans = TTN_Transform(glm::vec3(250.0, 0.0f, 2.0f), glm::vec3(0.0f), glm::vec3(250.0f, 150.0, 1.0f));
		AttachCopy(temp, buttonTrans);

		//create a 2D renderer for the button
		TTN_Renderer2D buttonRenderer = TTN_Renderer2D(textureButton1);
		AttachCopy(temp, buttonRenderer);
	}

#pragma region DIFFICULTY BAR

	//difficulty bar border
	{
		//create an entity in the scene for the mouse sensitivity bar overlay
		diffBarBorder = CreateEntity();

		//create a transform for the mouse sensitivity bar overlay
		TTN_Transform healthTrans = TTN_Transform(glm::vec3(0.0f, -120.0f, 0.9f), glm::vec3(0.0f), glm::vec3(4200.0f * mouseScale, 239.0f * mouseScale, 1.0f));
		AttachCopy(diffBarBorder, healthTrans);

		//create a sprite renderer for the mouse sensitivity bar overlay
		TTN_Renderer2D healthRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Bar Border"));
		AttachCopy(diffBarBorder, healthRenderer);
	}

	// difficulty bar
	{
		//create an entity for the health bar
		diffBar = CreateEntity();

		//create a transform for the health bar
		TTN_Transform healthTrans = TTN_Transform(glm::vec3(0.0f, -120.0f, 1.0f), glm::vec3(0.0f), glm::vec3(4200.0f * mouseScale, 239.0f * mouseScale, 1.0f));
		AttachCopy(diffBar, healthTrans);

		//create a sprite renderer for the health bar
		TTN_Renderer2D healthRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Bar"), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		float normalizedDiff = diff / 200.f;
		healthRenderer.SetHoriMask(normalizedDiff);
		AttachCopy(diffBar, healthRenderer);
	}

	// difficulty bar background
	{
		//create an entity for the mouse bar background
		diffBarBg = CreateEntity();

		//create a transform for the mouse bar background
		TTN_Transform healthTrans = TTN_Transform(glm::vec3(0.0f, -120.0f, 1.1f), glm::vec3(0.0f), glm::vec3(4200.0f * mouseScale, 239.0f * mouseScale, 1.0f));
		AttachCopy(diffBarBg, healthTrans);

		//create a sprite renderer for the mouse bar background
		TTN_Renderer2D healthRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Bar BG"), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

		AttachCopy(diffBarBg, healthRenderer);
	}

#pragma endregion

	//indicator for difficulty
	{
		for (int i = 0; i < 3; i++) {
			//button border
			{
				//create an entity in the scene for the bar border
				entt::entity temp = CreateEntity();
				if (i == 0) EasyDiffBorder = temp;
				else if (i == 1) RegDiffBorder = temp;
				else if (i == 2) HardDiffBorder = temp;

				//create a transform for the mouse sensitivity bar overlay
				TTN_Transform buttonTrans;
				if (i == 0) buttonTrans = TTN_Transform(glm::vec3(450.0f, -220.0f, 0.9f), glm::vec3(0.0f), glm::vec3(249.0f * buttonScale, 239.0f * buttonScale, 1.0f));
				else if (i == 1) buttonTrans = TTN_Transform(glm::vec3(50.0f, -220.0f, 0.9f), glm::vec3(0.0f), glm::vec3(249.0f * buttonScale, 239.0f * buttonScale, 1.0f));
				else if (i == 2) buttonTrans = TTN_Transform(glm::vec3(-450.0f, -220.0f, 0.9f), glm::vec3(0.0f), glm::vec3(249.0f * buttonScale, 239.0f * buttonScale, 1.0f));
				AttachCopy(temp, buttonTrans);

				//create a sprite renderer for the bar overlay
				TTN_Renderer2D healthRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Bar Border"));
				AttachCopy(temp, healthRenderer);
			}

			// bar indicator
			{
				//create an entity in the scene for off bar
				entt::entity temp = CreateEntity();
				if (i == 0) EasyDiffBar = temp;
				else if (i == 1) RegDiffBar = temp;
				else if (i == 2) HardDiffBar = temp;

				//create a transform for the off button
				TTN_Transform buttonTrans;
				if (i == 0) buttonTrans = TTN_Transform(glm::vec3(450.0f, -220.0f, 0.1f), glm::vec3(0.0f), glm::vec3(249.0f * buttonScale, 239.0f * buttonScale, 1.0f));
				else if (i == 1) buttonTrans = TTN_Transform(glm::vec3(50.0f, -220.0f, 0.1f), glm::vec3(0.0f), glm::vec3(249.0f * buttonScale, 239.0f * buttonScale, 1.0f));
				else if (i == 2) buttonTrans = TTN_Transform(glm::vec3(-450.0f, -220.0f, 0.1f), glm::vec3(0.0f), glm::vec3(249.0f * buttonScale, 239.0f * buttonScale, 1.0f));
				AttachCopy(temp, buttonTrans);

				//create a sprite renderer for the health bar
				TTN_Renderer2D healthRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Bar"), glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
				AttachCopy(temp, healthRenderer);
			}

			// off button background
			{
				entt::entity temp = CreateEntity();
				if (i == 0) EasyDiffBarBg = temp;
				else if (i == 1) RegDiffBarBg = temp;
				else if (i == 2) HardDiffBarBg = temp;

				//create a transform for the off button
				TTN_Transform buttonTrans;
				if (i == 0) buttonTrans = TTN_Transform(glm::vec3(450.0f, -220.0f, 0.9f), glm::vec3(0.0f), glm::vec3(249.0f * buttonScale, 239.0f * buttonScale, 1.0f));
				else if (i == 1) buttonTrans = TTN_Transform(glm::vec3(50.0f, -220.0f, 0.9f), glm::vec3(0.0f), glm::vec3(249.0f * buttonScale, 239.0f * buttonScale, 1.0f));
				else if (i == 2) buttonTrans = TTN_Transform(glm::vec3(-450.0f, -220.0f, 0.9f), glm::vec3(0.0f), glm::vec3(249.0f * buttonScale, 239.0f * buttonScale, 1.0f));
				AttachCopy(temp, buttonTrans);

				//create a sprite renderer for the mouse bar background
				TTN_Renderer2D healthRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("Bar BG"), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
				AttachCopy(temp, healthRenderer);
			}
		}
	}

	//actual buttons for diffixulty
	for (int i = 0; i < 3; i++) {
		entt::entity temp = CreateEntity();
		if (i == 0) buttonEasy = temp;
		else if (i == 1) buttonReg = temp;
		else if (i == 2) buttonHard = temp;

		//create a transform for the button
		TTN_Transform buttonTrans;
		if (i == 0) buttonTrans = TTN_Transform(glm::vec3(650.0f, -220.0f, 0.9f), glm::vec3(0.0f), glm::vec3(250.0f, 150.0, 1.0f));
		else if (i == 1) buttonTrans = TTN_Transform(glm::vec3(250.0f, -220.0f, 0.9f), glm::vec3(0.0f), glm::vec3(250.0f, 150.0, 1.0f));
		else if (i == 2) buttonTrans = TTN_Transform(glm::vec3(-250.0f, -220.0f, 0.9f), glm::vec3(0.0f), glm::vec3(250.0f, 150.0, 1.0f));
		AttachCopy(temp, buttonTrans);

		//create a 2D renderer for the button
		TTN_Renderer2D buttonRenderer = TTN_Renderer2D(textureButton1);
		AttachCopy(temp, buttonRenderer);
	}


	//buttons for apply, undo, and default
	for (int i = 0; i < 3; i++) {
		entt::entity temp = CreateEntity();
		if (i == 0) buttonApply = temp;
		else if (i == 1) buttonUndo = temp;
		else if (i == 2) buttonDefault = temp;

		//create a transform for the button
		TTN_Transform buttonTrans;
		if (i == 0) buttonTrans = TTN_Transform(glm::vec3(-850.0f, -450.0f, 2.0f), glm::vec3(0.0f), glm::vec3(250.0f / 2.0f, 150.0 / 2.0f, 1.0f));
		else if (i == 1) buttonTrans = TTN_Transform(glm::vec3(700.0f, -450.0f, 2.0f), glm::vec3(0.0f), glm::vec3(250.0f / 2.0f, 150.0f / 2.0f, 1.0f));
		else if (i == 2) buttonTrans = TTN_Transform(glm::vec3(850.0f, -450.0f, 2.0f), glm::vec3(0.0f), glm::vec3(250.0f / 2.0f, 150.0f / 2.0f, 1.0f));
		AttachCopy(temp, buttonTrans);

		//create a 2D renderer for the button
		TTN_Renderer2D buttonRenderer = TTN_Renderer2D(textureButton1);
		AttachCopy(temp, buttonRenderer);
	}


	ReadFromFile();

	TTN_Scene::InitScene();
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
		unsigned mouse = std::round(mouse_sen);
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
		unsigned volumeN = std::round(volume);
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
		unsigned volumeM = std::round(volumeMusic);
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
		unsigned volumeS = std::round(volumeSFX);
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

	//update the difficulty number
	{
		unsigned difficult = std::round(diff);
		while (GetNumOfDigits(difficult) < diffNums.size()) {
			DeleteEntity(diffNums[diffNums.size() - 1]);
			diffNums.pop_back();
		}

		if (GetNumOfDigits(difficult) > diffNums.size())
			MakeDiffNumEntity();

		for (int i = 0; i < diffNums.size(); i++) {
			Get<TTN_Renderer2D>(diffNums[i]).SetSprite(TTN_AssetSystem::GetTexture2D(std::to_string(GetDigit(difficult, diffNums.size() - i - 1)) + "-Text"));
		}
	}

	//color correction buttons
	{
		//get buttons transform
		TTN_Transform buttonOffTrans = Get<TTN_Transform>(buttonOff);
		if (mousePosWorldSpace.x < buttonOffTrans.GetPos().x + 0.5f * abs(buttonOffTrans.GetScale().x) &&
			mousePosWorldSpace.x > buttonOffTrans.GetPos().x - 0.5f * abs(buttonOffTrans.GetScale().x) &&
			mousePosWorldSpace.y < buttonOffTrans.GetPos().y + 0.5f * abs(buttonOffTrans.GetScale().y) &&
			mousePosWorldSpace.y > buttonOffTrans.GetPos().y - 0.5f * abs(buttonOffTrans.GetScale().y)) {
			Get<TTN_Renderer2D>(buttonOff).SetSprite(textureButton2);
		}
		else {
			Get<TTN_Renderer2D>(buttonOff).SetSprite(textureButton1);
		}

		//get buttons transform
		TTN_Transform buttonColor1Trans = Get<TTN_Transform>(buttonColor1);
		if (mousePosWorldSpace.x < buttonColor1Trans.GetPos().x + 0.5f * abs(buttonColor1Trans.GetScale().x) &&
			mousePosWorldSpace.x > buttonColor1Trans.GetPos().x - 0.5f * abs(buttonColor1Trans.GetScale().x) &&
			mousePosWorldSpace.y < buttonColor1Trans.GetPos().y + 0.5f * abs(buttonColor1Trans.GetScale().y) &&
			mousePosWorldSpace.y > buttonColor1Trans.GetPos().y - 0.5f * abs(buttonColor1Trans.GetScale().y)) {
			Get<TTN_Renderer2D>(buttonColor1).SetSprite(textureButton2);
		}
		else {
			Get<TTN_Renderer2D>(buttonColor1).SetSprite(textureButton1);
		}

		if (Off) {
			Get<TTN_Renderer2D>(OffBar).SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			Get<TTN_Renderer2D>(ColorBar).SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
		}

		if (color) {
			Get<TTN_Renderer2D>(OffBar).SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
			Get<TTN_Renderer2D>(ColorBar).SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		}
	}
	
	//difficulty buttons
	{
		//button hovering update
		{
			TTN_Transform buttonEasyTrans = Get<TTN_Transform>(buttonEasy);
			if (mousePosWorldSpace.x < buttonEasyTrans.GetPos().x + 0.5f * abs(buttonEasyTrans.GetScale().x) &&
				mousePosWorldSpace.x > buttonEasyTrans.GetPos().x - 0.5f * abs(buttonEasyTrans.GetScale().x) &&
				mousePosWorldSpace.y < buttonEasyTrans.GetPos().y + 0.5f * abs(buttonEasyTrans.GetScale().y) &&
				mousePosWorldSpace.y > buttonEasyTrans.GetPos().y - 0.5f * abs(buttonEasyTrans.GetScale().y)) {
				Get<TTN_Renderer2D>(buttonEasy).SetSprite(textureButton2);
			}
			else {
				Get<TTN_Renderer2D>(buttonEasy).SetSprite(textureButton1);
			}

			TTN_Transform buttonRegTrans = Get<TTN_Transform>(buttonReg);
			if (mousePosWorldSpace.x < buttonRegTrans.GetPos().x + 0.5f * abs(buttonRegTrans.GetScale().x) &&
				mousePosWorldSpace.x > buttonRegTrans.GetPos().x - 0.5f * abs(buttonRegTrans.GetScale().x) &&
				mousePosWorldSpace.y < buttonRegTrans.GetPos().y + 0.5f * abs(buttonRegTrans.GetScale().y) &&
				mousePosWorldSpace.y > buttonRegTrans.GetPos().y - 0.5f * abs(buttonRegTrans.GetScale().y)) {
				Get<TTN_Renderer2D>(buttonReg).SetSprite(textureButton2);
			}
			else {
				Get<TTN_Renderer2D>(buttonReg).SetSprite(textureButton1);
			}

			TTN_Transform buttonHardTrans = Get<TTN_Transform>(buttonHard);
			if (mousePosWorldSpace.x < buttonHardTrans.GetPos().x + 0.5f * abs(buttonHardTrans.GetScale().x) &&
				mousePosWorldSpace.x > buttonHardTrans.GetPos().x - 0.5f * abs(buttonHardTrans.GetScale().x) &&
				mousePosWorldSpace.y < buttonHardTrans.GetPos().y + 0.5f * abs(buttonHardTrans.GetScale().y) &&
				mousePosWorldSpace.y > buttonHardTrans.GetPos().y - 0.5f * abs(buttonHardTrans.GetScale().y)) {
				Get<TTN_Renderer2D>(buttonHard).SetSprite(textureButton2);
			}
			else {
				Get<TTN_Renderer2D>(buttonHard).SetSprite(textureButton1);
			}
		}

		if (easy) {
			Get<TTN_Renderer2D>(EasyDiffBar).SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			Get<TTN_Renderer2D>(RegDiffBar).SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
			Get<TTN_Renderer2D>(HardDiffBar).SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
		}

		if (reg) {
			Get<TTN_Renderer2D>(EasyDiffBar).SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
			Get<TTN_Renderer2D>(RegDiffBar).SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			Get<TTN_Renderer2D>(HardDiffBar).SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
		}

		if (hard) {
			Get<TTN_Renderer2D>(EasyDiffBar).SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
			Get<TTN_Renderer2D>(RegDiffBar).SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
			Get<TTN_Renderer2D>(HardDiffBar).SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		}
	}

	//apply, undo, and default buttons
	{
		//get apply button transform and check if it's being hovered over
		TTN_Transform buttonApplyTrans = Get<TTN_Transform>(buttonApply);
		if (mousePosWorldSpace.x < buttonApplyTrans.GetPos().x + 0.5f * abs(buttonApplyTrans.GetScale().x) &&
			mousePosWorldSpace.x > buttonApplyTrans.GetPos().x - 0.5f * abs(buttonApplyTrans.GetScale().x) &&
			mousePosWorldSpace.y < buttonApplyTrans.GetPos().y + 0.5f * abs(buttonApplyTrans.GetScale().y) &&
			mousePosWorldSpace.y > buttonApplyTrans.GetPos().y - 0.5f * abs(buttonApplyTrans.GetScale().y)) {
			Get<TTN_Renderer2D>(buttonApply).SetSprite(textureButton2);
		}
		else
			Get<TTN_Renderer2D>(buttonApply).SetSprite(textureButton1);

		//get undo button transform and check if it's being hovered over
		TTN_Transform buttonUndoTrans = Get<TTN_Transform>(buttonUndo);
		if (mousePosWorldSpace.x < buttonUndoTrans.GetPos().x + 0.5f * abs(buttonUndoTrans.GetScale().x) &&
			mousePosWorldSpace.x > buttonUndoTrans.GetPos().x - 0.5f * abs(buttonUndoTrans.GetScale().x) &&
			mousePosWorldSpace.y < buttonUndoTrans.GetPos().y + 0.5f * abs(buttonUndoTrans.GetScale().y) &&
			mousePosWorldSpace.y > buttonUndoTrans.GetPos().y - 0.5f * abs(buttonUndoTrans.GetScale().y)) {
			Get<TTN_Renderer2D>(buttonUndo).SetSprite(textureButton2);
		}
		else
			Get<TTN_Renderer2D>(buttonUndo).SetSprite(textureButton1);

		//get default button transform and check if it's being hovered
		TTN_Transform buttonDefaultTrans = Get<TTN_Transform>(buttonDefault);
		if (mousePosWorldSpace.x < buttonDefaultTrans.GetPos().x + 0.5f * abs(buttonDefaultTrans.GetScale().x) &&
			mousePosWorldSpace.x > buttonDefaultTrans.GetPos().x - 0.5f * abs(buttonDefaultTrans.GetScale().x) &&
			mousePosWorldSpace.y < buttonDefaultTrans.GetPos().y + 0.5f * abs(buttonDefaultTrans.GetScale().y) &&
			mousePosWorldSpace.y > buttonDefaultTrans.GetPos().y - 0.5f * abs(buttonDefaultTrans.GetScale().y)) {
			Get<TTN_Renderer2D>(buttonDefault).SetSprite(textureButton2);
		}
		else
			Get<TTN_Renderer2D>(buttonDefault).SetSprite(textureButton1);
	}
	
	//update the input delay
	if (m_InputDelay >= 0.0f) {
		m_InputDelay -= deltaTime;
		Off = acutal_Off;
		color = acutal_color;
		easy = acutal_easy;
		reg = acutal_reg;
		hard = acutal_hard;
	}

	//update the base scene
	TTN_Scene::Update(deltaTime);
}

void OptionsMenu::MouseButtonDownChecks()
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

	//if the user is left clicking
	if (TTN_Application::TTN_Input::GetMouseButton(TTN_MouseButton::Left) && m_InputDelay <= 0.0f) {
		//mouse sensetivity bar
		{
			//grab the transform of the bar
			TTN_Transform playButtonTrans = Get<TTN_Transform>(mouseSensitivity);
			//if the player's mouse is over the bar
			if (mousePosWorldSpace.x < playButtonTrans.GetPos().x + 0.5f * abs(playButtonTrans.GetScale().x) &&
				mousePosWorldSpace.x > playButtonTrans.GetPos().x - 0.5f * abs(playButtonTrans.GetScale().x) &&
				mousePosWorldSpace.y < playButtonTrans.GetPos().y + 0.5f * abs(playButtonTrans.GetScale().y) &&
				mousePosWorldSpace.y > playButtonTrans.GetPos().y - 0.5f * abs(playButtonTrans.GetScale().y)) {
				//make a temp float mouse sensetivity
				float tempMouseSen = mouse_sen;
				//normalize it
				float normalizedMouseSen = tempMouseSen / 100.f;

				//if it's on the center than the mouse senestivity is 50% 
				if (mousePosWorldSpace.x == 0.0f)
					normalizedMouseSen = 0.5f;

				//if it's to the left than find the percentage
				else if (mousePosWorldSpace.x > 0.0f)
					normalizedMouseSen = abs(TTN_Interpolation::ReMap(0.0f, 100.0f, 0.0f, 1.0f, (abs(mousePosWorldSpace.x / playButtonTrans.GetScale().x) * 100.f)) - 0.5f);


				//if it's to the right than find the percentage
				else if (mousePosWorldSpace.x < 0.0f)
					normalizedMouseSen = abs(TTN_Interpolation::ReMap(0.0f, 100.0f, 0.0f, 1.0f, (abs(mousePosWorldSpace.x / playButtonTrans.GetScale().x) * 100.f)) + 0.5f);

				//otherwise it's just the existing value
				else
					float normalizedMouseSen = tempMouseSen / 100.f;

				//set the mask to the normalized mouse senestivity
				Get<TTN_Renderer2D>(mouseSensitivity).SetHoriMask(normalizedMouseSen);

				//take it out of the normalized range
				tempMouseSen = normalizedMouseSen * 100.f;
				//and round it back to an integer
				mouse_sen = std::round(tempMouseSen);
			}
		}

		//master volume bar
		{
			//grab the transform of the bar
			TTN_Transform volumeBarTrans = Get<TTN_Transform>(volumeBar);
			//if the player's mouse is over the bar
			if (mousePosWorldSpace.x < volumeBarTrans.GetPos().x + 0.5f * abs(volumeBarTrans.GetScale().x) &&
				mousePosWorldSpace.x > volumeBarTrans.GetPos().x - 0.5f * abs(volumeBarTrans.GetScale().x) &&
				mousePosWorldSpace.y < volumeBarTrans.GetPos().y + 0.5f * abs(volumeBarTrans.GetScale().y) &&
				mousePosWorldSpace.y > volumeBarTrans.GetPos().y - 0.5f * abs(volumeBarTrans.GetScale().y)) {

				//get a temp float for the volume
				float tempVolume = volume;
				//normalize it
				float normalizedVolume = tempVolume / 100.f;

				//if it's at the center than the volume is 50%
				if (mousePosWorldSpace.x == 0.0f)
					normalizedVolume = 0.5f;

				//if it's too the left find the percetange
				else if (mousePosWorldSpace.x > 0.0f)
					normalizedVolume = abs(TTN_Interpolation::ReMap(0.0f, 100.0f, 0.0f, 1.0f, (abs(mousePosWorldSpace.x / volumeBarTrans.GetScale().x) * 100.f)) - 0.5f);


				//if it's too the right find the percetange
				else if (mousePosWorldSpace.x < 0.0f)
					normalizedVolume = abs(TTN_Interpolation::ReMap(0.0f, 100.0f, 0.0f, 1.0f, (abs(mousePosWorldSpace.x / volumeBarTrans.GetScale().x) * 100.f)) + 0.5f);

				//otherwise just store the existing value
				else
					float normalizedVolume = tempVolume / 100.f;

				//set the horizontal mask to the normalized value
				Get<TTN_Renderer2D>(volumeBar).SetHoriMask(normalizedVolume);

				//take out of normalized space
				tempVolume = normalizedVolume * 100.f;
				//and round it back to an integer
				volume = std::round(tempVolume);
			}
		}
		
		//music volume bar
		{
			//get the bar's transform
			TTN_Transform MusicVolumeBarTrans = Get<TTN_Transform>(MusicVolumeBar);
			//if the player's mouse is over the bar
			if (mousePosWorldSpace.x < MusicVolumeBarTrans.GetPos().x + 0.5f * abs(MusicVolumeBarTrans.GetScale().x) &&
				mousePosWorldSpace.x > MusicVolumeBarTrans.GetPos().x - 0.5f * abs(MusicVolumeBarTrans.GetScale().x) &&
				mousePosWorldSpace.y < MusicVolumeBarTrans.GetPos().y + 0.5f * abs(MusicVolumeBarTrans.GetScale().y) &&
				mousePosWorldSpace.y > MusicVolumeBarTrans.GetPos().y - 0.5f * abs(MusicVolumeBarTrans.GetScale().y)) {

				//get a temp float for the volume
				float tempMusic = volumeMusic;
				//normalize it
				float normalizedMusic = tempMusic / 100.f;

				//if it's at the center than the volume is 50% 
				if (mousePosWorldSpace.x == 0.0f)
					normalizedMusic = 0.5f;

				//if it's too the left, figure out what percentage the volume is 
				else if (mousePosWorldSpace.x > 0.0f)
					normalizedMusic = abs(TTN_Interpolation::ReMap(0.0f, 100.0f, 0.0f, 1.0f, (abs(mousePosWorldSpace.x / MusicVolumeBarTrans.GetScale().x) * 100.f)) - 0.5f);

				//if it's too the right, figure out what percentage the volume is 
				else if (mousePosWorldSpace.x < 0.0f)
					normalizedMusic = abs(TTN_Interpolation::ReMap(0.0f, 100.0f, 0.0f, 1.0f, (abs(mousePosWorldSpace.x / MusicVolumeBarTrans.GetScale().x) * 100.f)) + 0.5f);

				//otherwise just save the previous value
				else
					float normalizedMusic = tempMusic / 100.f;

				//set the mask to the normalized value
				Get<TTN_Renderer2D>(MusicVolumeBar).SetHoriMask(normalizedMusic);
				
				//take it out of mormalized space
				tempMusic = normalizedMusic * 100.f;
				//and round it back to an integer
				volumeMusic = std::round(tempMusic);
			}
		}

		//sfx volume bar
		{
			//get the bar's transform
			TTN_Transform SFXVolumeBarTrans = Get<TTN_Transform>(SFXvolumeBar);
			//if the player's mouse is over the bar
			if (mousePosWorldSpace.x < SFXVolumeBarTrans.GetPos().x + 0.5f * abs(SFXVolumeBarTrans.GetScale().x) &&
				mousePosWorldSpace.x > SFXVolumeBarTrans.GetPos().x - 0.5f * abs(SFXVolumeBarTrans.GetScale().x) &&
				mousePosWorldSpace.y < SFXVolumeBarTrans.GetPos().y + 0.5f * abs(SFXVolumeBarTrans.GetScale().y) &&
				mousePosWorldSpace.y > SFXVolumeBarTrans.GetPos().y - 0.5f * abs(SFXVolumeBarTrans.GetScale().y)) {
				//make a temp float for the volume
				float tempSFX = volumeSFX;
				//normalize it 
				float normalizedSFX = tempSFX / 100.f;

				//if it's in the center than the volume is 50%
				if (mousePosWorldSpace.x == 0.0f)
					normalizedSFX = 0.5f;

				//if it's to the left then find out what percentage it is
				else if (mousePosWorldSpace.x > 0.0f)
					normalizedSFX = abs(TTN_Interpolation::ReMap(0.0f, 100.0f, 0.0f, 1.0f, (abs(mousePosWorldSpace.x / SFXVolumeBarTrans.GetScale().x) * 100.f)) - 0.5f);


				//if it's to the right then find out what percentage it is
				else if (mousePosWorldSpace.x < 0.0f)
					normalizedSFX = abs(TTN_Interpolation::ReMap(0.0f, 100.0f, 0.0f, 1.0f, (abs(mousePosWorldSpace.x / SFXVolumeBarTrans.GetScale().x) * 100.f)) + 0.5f);

				//otherwise just keep the existing value
				else
					float normalizedSFX = tempSFX / 100.f;

				//set the mask to the normalized value
				Get<TTN_Renderer2D>(SFXvolumeBar).SetHoriMask(normalizedSFX);

				//take it out of normalized space
				tempSFX = normalizedSFX * 100.f;
				//and round it back to an integer
				volumeSFX = std::round(tempSFX);
			}
		}

		//difficulty bar
		{
			//get the bar's transform
			TTN_Transform diffBarTrans = Get<TTN_Transform>(diffBar);
			//check if the player's mouse is over the bar
			if (mousePosWorldSpace.x < diffBarTrans.GetPos().x + 0.5f * abs(diffBarTrans.GetScale().x) &&
				mousePosWorldSpace.x > diffBarTrans.GetPos().x - 0.5f * abs(diffBarTrans.GetScale().x) &&
				mousePosWorldSpace.y < diffBarTrans.GetPos().y + 0.5f * abs(diffBarTrans.GetScale().y) &&
				mousePosWorldSpace.y > diffBarTrans.GetPos().y - 0.5f * abs(diffBarTrans.GetScale().y)) {

				//get a temp float for the difficulty
				float tempDiff = diff;
				//normalize it
				float normalizedDiff = tempDiff / 200.f;

				//if it's on the center than it's 50% 
				if (mousePosWorldSpace.x == 0.0f) {
					normalizedDiff = 0.5f;
				}

				//if it's to the left then figure out what percentage it is
				else if (mousePosWorldSpace.x > 0.0f) {
					normalizedDiff = abs(TTN_Interpolation::ReMap(0.0f, 200.0f, 0.0f, 1.0f, (abs((mousePosWorldSpace.x - 10.f) / diffBarTrans.GetScale().x) * 200.f)) - 0.5f);
					if (normalizedDiff < 0.050f) {
						normalizedDiff = 0.05f;
					}
				}

				//if it's to the right then figure out what percentage it is
				else if (mousePosWorldSpace.x < 0.0f) {
					normalizedDiff = abs(TTN_Interpolation::ReMap(0.0f, 200.0f, 0.0f, 1.0f, (abs((mousePosWorldSpace.x) / diffBarTrans.GetScale().x) * 200.f)) + 0.5f);
				}

				//otherwise keep the existing value
				else {
					float normalizedDiff = tempDiff / 200.f;
				}

				//set the mask to the normalized value
				Get<TTN_Renderer2D>(diffBar).SetHoriMask(normalizedDiff);
				
				//take it out of normalized space
				tempDiff = normalizedDiff * 200.f;
				//and round it back to an integer 
				diff = std::round(tempDiff);
			}
		}
	}

	//if the user has left clicked this frame
	if (TTN_Application::TTN_Input::GetMouseButtonDown(TTN_MouseButton::Left) && m_InputDelay <= 0.0f) {
		//color correction buttons
		{
			//get off button trans transform and check if it's being pressed
			TTN_Transform buttonOffTrans = Get<TTN_Transform>(buttonOff);
			if (mousePosWorldSpace.x < buttonOffTrans.GetPos().x + 0.5f * abs(buttonOffTrans.GetScale().x) &&
				mousePosWorldSpace.x > buttonOffTrans.GetPos().x - 0.5f * abs(buttonOffTrans.GetScale().x) &&
				mousePosWorldSpace.y < buttonOffTrans.GetPos().y + 0.5f * abs(buttonOffTrans.GetScale().y) &&
				mousePosWorldSpace.y > buttonOffTrans.GetPos().y - 0.5f * abs(buttonOffTrans.GetScale().y)) {
				\
					//if it is, turn off the colorbind modes
					Off = true;
				color = false;
			}

			//get the first colorbind mode button's transform and check if it's being pressed
			TTN_Transform buttonColor1Trans = Get<TTN_Transform>(buttonColor1);
			if (mousePosWorldSpace.x < buttonColor1Trans.GetPos().x + 0.5f * abs(buttonColor1Trans.GetScale().x) &&
				mousePosWorldSpace.x > buttonColor1Trans.GetPos().x - 0.5f * abs(buttonColor1Trans.GetScale().x) &&
				mousePosWorldSpace.y < buttonColor1Trans.GetPos().y + 0.5f * abs(buttonColor1Trans.GetScale().y) &&
				mousePosWorldSpace.y > buttonColor1Trans.GetPos().y - 0.5f * abs(buttonColor1Trans.GetScale().y)) {
				//if it is, turn it on
				color = true;
				Off = false;
			}
		}

		// difficulty buttons
		{
			//get the transform for the easy button and check if they are pressing it
			TTN_Transform buttonEasyTrans = Get<TTN_Transform>(buttonEasy);
			if (mousePosWorldSpace.x < buttonEasyTrans.GetPos().x + 0.5f * abs(buttonEasyTrans.GetScale().x) &&
				mousePosWorldSpace.x > buttonEasyTrans.GetPos().x - 0.5f * abs(buttonEasyTrans.GetScale().x) &&
				mousePosWorldSpace.y < buttonEasyTrans.GetPos().y + 0.5f * abs(buttonEasyTrans.GetScale().y) &&
				mousePosWorldSpace.y > buttonEasyTrans.GetPos().y - 0.5f * abs(buttonEasyTrans.GetScale().y)) {
				//if they are, set the difficulty to easy
				easy = true;
				reg = false;
				hard = false;
				diff = 50.f;
				//and update the bar as approriate
				float normalizedDiff = diff / 200.f;
				Get<TTN_Renderer2D>(diffBar).SetHoriMask(normalizedDiff);
			}

			//get the trnasform for the regular difficultly button, and check if they are pressing it
			TTN_Transform buttonRegTrans = Get<TTN_Transform>(buttonReg);
			if (mousePosWorldSpace.x < buttonRegTrans.GetPos().x + 0.5f * abs(buttonRegTrans.GetScale().x) &&
				mousePosWorldSpace.x > buttonRegTrans.GetPos().x - 0.5f * abs(buttonRegTrans.GetScale().x) &&
				mousePosWorldSpace.y < buttonRegTrans.GetPos().y + 0.5f * abs(buttonRegTrans.GetScale().y) &&
				mousePosWorldSpace.y > buttonRegTrans.GetPos().y - 0.5f * abs(buttonRegTrans.GetScale().y)) {
				//if they are, set the difficulty to regular
				easy = false;
				reg = true;
				hard = false;
				diff = 100.0f;
				//and update the bar as approriate
				float normalizedDiff = diff / 200.f;
				Get<TTN_Renderer2D>(diffBar).SetHoriMask(normalizedDiff);
			}

			//get the transform for the hard difficultly button, and check if they are pressing it 
			TTN_Transform buttonHardTrans = Get<TTN_Transform>(buttonHard);
			if (mousePosWorldSpace.x < buttonHardTrans.GetPos().x + 0.5f * abs(buttonHardTrans.GetScale().x) &&
				mousePosWorldSpace.x > buttonHardTrans.GetPos().x - 0.5f * abs(buttonHardTrans.GetScale().x) &&
				mousePosWorldSpace.y < buttonHardTrans.GetPos().y + 0.5f * abs(buttonHardTrans.GetScale().y) &&
				mousePosWorldSpace.y > buttonHardTrans.GetPos().y - 0.5f * abs(buttonHardTrans.GetScale().y)) {
				//if they are, set the difficulty to regular
				easy = false;
				reg = false;
				hard = true;
				diff = 150.f;
				//and update the bar as approriate
				float normalizedDiff = diff / 200.f;
				Get<TTN_Renderer2D>(diffBar).SetHoriMask(normalizedDiff);
			}
		}

		//apply, undo, and default buttons
		{
			//get apply button transform and check if it's being pressed
			TTN_Transform buttonApplyTrans = Get<TTN_Transform>(buttonApply);
			if (mousePosWorldSpace.x < buttonApplyTrans.GetPos().x + 0.5f * abs(buttonApplyTrans.GetScale().x) &&
				mousePosWorldSpace.x > buttonApplyTrans.GetPos().x - 0.5f * abs(buttonApplyTrans.GetScale().x) &&
				mousePosWorldSpace.y < buttonApplyTrans.GetPos().y + 0.5f * abs(buttonApplyTrans.GetScale().y) &&
				mousePosWorldSpace.y > buttonApplyTrans.GetPos().y - 0.5f * abs(buttonApplyTrans.GetScale().y)) {
				
				//if it is, apply the values and write them to the file
				acutal_mouse_sen = mouse_sen;
				acutal_volume = volume;
				acutal_volumeSFX = volumeSFX;
				acutal_volumeMuisc = volumeMusic;
				acutal_Diff = diff;
				acutal_Off = Off;
				acutal_color = color;
				acutal_easy = easy;
				acutal_reg = reg;
				acutal_hard = hard;

				WriteToFile();
			}

			//get undo button transform and check if it's being pressed
			TTN_Transform buttonUndoTrans = Get<TTN_Transform>(buttonUndo);
			if (mousePosWorldSpace.x < buttonUndoTrans.GetPos().x + 0.5f * abs(buttonUndoTrans.GetScale().x) &&
				mousePosWorldSpace.x > buttonUndoTrans.GetPos().x - 0.5f * abs(buttonUndoTrans.GetScale().x) &&
				mousePosWorldSpace.y < buttonUndoTrans.GetPos().y + 0.5f * abs(buttonUndoTrans.GetScale().y) &&
				mousePosWorldSpace.y > buttonUndoTrans.GetPos().y - 0.5f * abs(buttonUndoTrans.GetScale().y)) {

				//if it is, reset the values to their acutal numbers
				mouse_sen = acutal_mouse_sen;
				volume = acutal_volume;
				volumeSFX = acutal_volumeSFX;
				volumeMusic = acutal_volumeMuisc;
				diff = acutal_Diff;
				Off = acutal_Off;
				color = acutal_color;
				easy = acutal_easy;
				reg = acutal_reg;
				hard = acutal_hard;

				//and update the bars as approriate
				float normalized = mouse_sen / 100.0f;
				Get<TTN_Renderer2D>(mouseSensitivity).SetHoriMask(normalized);
				normalized = volume / 100.0f;
				Get<TTN_Renderer2D>(volumeBar).SetHoriMask(normalized);
				normalized = volumeSFX / 100.0f;
				Get<TTN_Renderer2D>(SFXvolumeBar).SetHoriMask(normalized);
				normalized = volumeMusic / 100.0f;
				Get<TTN_Renderer2D>(MusicVolumeBar).SetHoriMask(normalized);
				normalized = diff / 200.0f;
				Get<TTN_Renderer2D>(diffBar).SetHoriMask(normalized);
			}

			//get default button transform and check if it's being pressed
			TTN_Transform buttonDefaultTrans = Get<TTN_Transform>(buttonDefault);
			if (mousePosWorldSpace.x < buttonDefaultTrans.GetPos().x + 0.5f * abs(buttonDefaultTrans.GetScale().x) &&
				mousePosWorldSpace.x > buttonDefaultTrans.GetPos().x - 0.5f * abs(buttonDefaultTrans.GetScale().x) &&
				mousePosWorldSpace.y < buttonDefaultTrans.GetPos().y + 0.5f * abs(buttonDefaultTrans.GetScale().y) &&
				mousePosWorldSpace.y > buttonDefaultTrans.GetPos().y - 0.5f * abs(buttonDefaultTrans.GetScale().y)) {

				//if it is, reset the values to their default numbers
				mouse_sen = 50;
				volume = 100;
				volumeSFX = 5;
				volumeMusic = 20;
				diff = 100;
				Off = true;
				color = false;
				easy = false;
				reg = true;
				hard = false;

				//and update the bars as approriate
				float normalized = mouse_sen / 100.0f;
				Get<TTN_Renderer2D>(mouseSensitivity).SetHoriMask(normalized);
				normalized = volume / 100.0f;
				Get<TTN_Renderer2D>(volumeBar).SetHoriMask(normalized);
				normalized = volumeSFX / 100.0f;
				Get<TTN_Renderer2D>(SFXvolumeBar).SetHoriMask(normalized);
				normalized = volumeMusic / 100.0f;
				Get<TTN_Renderer2D>(MusicVolumeBar).SetHoriMask(normalized);
				normalized = diff / 200.0f;
				Get<TTN_Renderer2D>(diffBar).SetHoriMask(normalized);
			}
		}
	}
}

void OptionsMenu::KeyDownChecks()
{
	if (TTN_Application::TTN_Input::GetKeyDown(TTN_KeyCode::Esc)) {
		if (previousSceneMainMenu) shouldMenu = true;
		else shouldBack = true;
		m_InputDelay = 0.3f;

		//reset the values to their acutal numbers
		mouse_sen = acutal_mouse_sen;
		volume = acutal_volume;
		volumeSFX = acutal_volumeSFX;
		volumeMusic = acutal_volumeMuisc;
		diff = acutal_Diff;
		Off = acutal_Off;
		color = acutal_color;
		easy = acutal_easy;
		reg = acutal_reg;
		hard = acutal_hard;

		//and update the bars as approriate
		float normalized = mouse_sen / 100.0f;
		Get<TTN_Renderer2D>(mouseSensitivity).SetHoriMask(normalized);
		normalized = volume / 100.0f;
		Get<TTN_Renderer2D>(volumeBar).SetHoriMask(normalized);
		normalized = volumeSFX / 100.0f;
		Get<TTN_Renderer2D>(SFXvolumeBar).SetHoriMask(normalized);
		normalized = volumeMusic / 100.0f;
		Get<TTN_Renderer2D>(MusicVolumeBar).SetHoriMask(normalized);
		normalized = diff / 200.0f;
		Get<TTN_Renderer2D>(diffBar).SetHoriMask(normalized);
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

void OptionsMenu::MakeDiffNumEntity()
{
	diffNums.push_back(CreateEntity());

	//reference to the health bar's transform
	TTN_Transform& volumeTrans = Get<TTN_Transform>(diffBar);

	//setup a transform for the new entity
	TTN_Transform numTrans = TTN_Transform(glm::vec3(volumeTrans.GetGlobalPos().x + 0.3f * std::abs(volumeTrans.GetScale().x) -
		(float)diffNums.size() * 0.5f * numScale * 150.0f, volumeTrans.GetGlobalPos().y, volumeTrans.GetGlobalPos().z),
		glm::vec3(0.0f), glm::vec3(numScale * 150.0f, numScale * 150.0f, 1.0f));
	AttachCopy(diffNums[diffNums.size() - 1], numTrans);

	//setup a 2D renderer for the new entity
	//create a sprite renderer for the logo
	TTN_Renderer2D numRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("0-Text"));
	AttachCopy(diffNums[diffNums.size() - 1], numRenderer);
}

void OptionsMenu::MakeSFXNumEntity()
{
	SFXvolumeNums.push_back(CreateEntity());

	//reference to the health bar's transform
	TTN_Transform& volumeTrans = Get<TTN_Transform>(SFXvolumeBar);

	//setup a transform for the new entity
	TTN_Transform numTrans = TTN_Transform(glm::vec3(volumeTrans.GetGlobalPos().x + 0.3f * std::abs(volumeTrans.GetScale().x) -
		(float)SFXvolumeNums.size() * 0.5f * numScale * 150.0f, volumeTrans.GetGlobalPos().y, volumeTrans.GetGlobalPos().z),
		glm::vec3(0.0f), glm::vec3(numScale * 150.0f, numScale * 150.0f, 1.0f));
	AttachCopy(SFXvolumeNums[SFXvolumeNums.size() - 1], numTrans);

	//setup a 2D renderer for the new entity
	//create a sprite renderer for the logo
	TTN_Renderer2D numRenderer = TTN_Renderer2D(TTN_AssetSystem::GetTexture2D("0-Text"));
	AttachCopy(SFXvolumeNums[SFXvolumeNums.size() - 1], numRenderer);

}

void OptionsMenu::WriteToFile()
{
	//open the file
	std::ofstream file;
	file.open("Settings.txt", std::ios::out);

	if (!file) {
		LOG_ERROR("Settings file failed to open for writting.");
		throw std::runtime_error("Options menu failed to open settings file.");
	}

	//pass in each variable to the file
	file << acutal_mouse_sen << std::endl;
	file << acutal_volume << std::endl;
	file << acutal_volumeMuisc << std::endl;
	file << acutal_volumeSFX << std::endl;
	file << acutal_Diff << std::endl;
	file << (int)acutal_Off << std::endl;
	file << (int)acutal_color << std::endl;
	file << (int)acutal_easy << std::endl;
	file << (int)acutal_reg << std::endl;
	file << (int)acutal_hard << std::endl;

	//close the file
	file.close();

	//update the current values to the acutal values
	mouse_sen = acutal_mouse_sen;
	volume = acutal_volume;
	volumeSFX = acutal_volumeSFX;
	volumeMusic = acutal_volumeMuisc;
	diff = acutal_Diff;
	Off = acutal_Off;
	color = acutal_color;
	easy = acutal_easy;
	reg = acutal_reg;
	hard = acutal_hard;

	//and update the bars as approriate
	float normalized = mouse_sen / 100.0f;
	Get<TTN_Renderer2D>(mouseSensitivity).SetHoriMask(normalized);
	normalized = volume / 100.0f;
	Get<TTN_Renderer2D>(volumeBar).SetHoriMask(normalized);
	normalized = volumeSFX / 100.0f;
	Get<TTN_Renderer2D>(SFXvolumeBar).SetHoriMask(normalized);
	normalized = volumeMusic / 100.0f;
	Get<TTN_Renderer2D>(MusicVolumeBar).SetHoriMask(normalized);
	normalized = diff / 200.0f;
	Get<TTN_Renderer2D>(diffBar).SetHoriMask(normalized);
}

void OptionsMenu::ReadFromFile()
{
	//Open the file
	std::ifstream file;
	file.open("Settings.txt", std::ios::binary);

	//if it fails to open, throw an error
	if (!file) {
		LOG_ERROR("Settings file failed to load.");
		throw std::runtime_error("Options menu failed to open settings file.");
	}

	//get a string to represent each line of the file
	std::string line;

	int counter = 0;

	//parse each line of the file
	while (std::getline(file, line)) {
		//trim the line so there isn't any white space
		trim(line);

		//make a string stream of the line
		std::stringstream ss = std::stringstream(line);

		//copy the string stream into the variables based on the counter
		if (counter == 0)
			ss >> mouse_sen;
		else if (counter == 1)
			ss >> volume;
		else if (counter == 2)
			ss >> volumeMusic;
		else if (counter == 3)
			ss >> volumeSFX;
		else if (counter == 4)
			ss >> diff;
		else if (counter == 5) {
			int temp = 0;
			ss >> temp;
			Off = (bool)temp;
		}
		else if (counter == 6) {
			int temp = 0;
			ss >> temp;
			color = (bool)temp;
		}
		else if (counter == 7) {
			int temp = 0;
			ss >> temp;
			easy = (bool)temp;
		}
		else if (counter == 8) {
			int temp = 0;
			ss >> temp;
			reg = (bool)temp;
		}
		else if (counter == 9) {
			int temp = 0;
			ss >> temp;
			hard = (bool)temp;
		}

		//increment the counter
		counter++;
	}

	//close the file as we have finished reading from it
	file.close();

	//save the acutal values
	acutal_mouse_sen = mouse_sen;
	acutal_volume = volume;
	acutal_volumeMuisc = volumeMusic;
	acutal_volumeSFX = volumeSFX;
	acutal_Diff = diff;
	acutal_Off = Off;
	acutal_color = color;
	acutal_easy = easy;
	acutal_reg = reg;
	acutal_hard = hard;

	//and update the bars as approriate
	float normalized = mouse_sen / 100.0f;
	Get<TTN_Renderer2D>(mouseSensitivity).SetHoriMask(normalized);
	normalized = volume / 100.0f;
	Get<TTN_Renderer2D>(volumeBar).SetHoriMask(normalized);
	normalized = volumeSFX / 100.0f;
	Get<TTN_Renderer2D>(SFXvolumeBar).SetHoriMask(normalized);
	normalized = volumeMusic / 100.0f;
	Get<TTN_Renderer2D>(MusicVolumeBar).SetHoriMask(normalized);
	normalized = diff / 200.0f;
	Get<TTN_Renderer2D>(diffBar).SetHoriMask(normalized);
}