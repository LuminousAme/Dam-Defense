//Dam Defense, by Atlas X Games
//MainMenu.cpp, the source file for the class that represents the main menu

//import the class
#include "MainMenu.h"
#include "glm/ext.hpp"

MainMenu::MainMenu()
	: TTN_Scene()
{
}

void MainMenu::InitScene()
{
	//load in the scene's assets
	SetUpAssets();

	//set up the other data
	SetUpOtherData();

	//create the entities
	SetUpEntities();
}

void MainMenu::Update(float deltaTime)
{
	//increase the total time of the scene to make the water animated correctly
	time += deltaTime;

	//don't forget to call the base class' update
	TTN_Scene::Update(deltaTime);
}

void MainMenu::PostRender()
{
	//terrain
	{
		//bind the shader
		shaderProgramTerrain->Bind();

		//vert shader
		//bind the height map texture
		terrainMap->Bind(0);

		//pass the scale uniform
		shaderProgramTerrain->SetUniform("u_scale", terrainScale);
		//pass the mvp uniform
		glm::mat4 mvp = Get<TTN_Camera>(camera).GetProj();
		glm::mat4 viewMat = glm::inverse(Get<TTN_Transform>(camera).GetGlobal());
		mvp *= viewMat;
		mvp *= Get<TTN_Transform>(terrain).GetGlobal();
		shaderProgramTerrain->SetUniformMatrix("MVP", mvp);
		//pass the model uniform
		shaderProgramTerrain->SetUniformMatrix("Model", Get<TTN_Transform>(terrain).GetGlobal());
		//and pass the normal matrix uniform
		shaderProgramTerrain->SetUniformMatrix("NormalMat",
			glm::mat3(glm::inverse(glm::transpose(Get<TTN_Transform>(terrain).GetGlobal()))));

		//frag shader
		//bind the textures
		sandText->Bind(1);
		rockText->Bind(2);
		grassText->Bind(3);

		//send lighting from the scene
		shaderProgramTerrain->SetUniform("u_AmbientCol", TTN_Scene::GetSceneAmbientColor());
		shaderProgramTerrain->SetUniform("u_AmbientStrength", TTN_Scene::GetSceneAmbientLightStrength());

		//render the terrain
		terrainPlain->GetVAOPointer()->Render();
	}

	//water
	{
		//bind the shader
		shaderProgramWater->Bind();

		//vert shader
		//pass the mvp uniform
		glm::mat4 mvp = Get<TTN_Camera>(camera).GetProj();
		glm::mat4 viewMat = glm::inverse(Get<TTN_Transform>(camera).GetGlobal());
		mvp *= viewMat;
		mvp *= Get<TTN_Transform>(water).GetGlobal();
		shaderProgramWater->SetUniformMatrix("MVP", mvp);
		//pass the model uniform
		shaderProgramWater->SetUniformMatrix("Model", Get<TTN_Transform>(water).GetGlobal());
		//and pass the normal matrix uniform
		shaderProgramWater->SetUniformMatrix("NormalMat",
			glm::mat3(glm::inverse(glm::transpose(Get<TTN_Transform>(water).GetGlobal()))));

		//pass in data about the water animation
		shaderProgramWater->SetUniform("time", time);
		shaderProgramWater->SetUniform("speed", waveSpeed);
		shaderProgramWater->SetUniform("baseHeight", waveBaseHeightIncrease);
		shaderProgramWater->SetUniform("heightMultiplier", waveHeightMultiplier);
		shaderProgramWater->SetUniform("waveLenghtMultiplier", waveLenghtMultiplier);

		//frag shader
		//bind the textures
		waterText->Bind(0);

		//send lighting from the scene
		shaderProgramWater->SetUniform("u_AmbientCol", TTN_Scene::GetSceneAmbientColor());
		shaderProgramWater->SetUniform("u_AmbientStrength", TTN_Scene::GetSceneAmbientLightStrength());

		//render the water (just use the same plane as the terrain)
		terrainPlain->GetVAOPointer()->Render();
	}

	TTN_Scene::PostRender();
}

void MainMenu::SetUpAssets()
{
	//grab the shaders
	shaderProgramTextured = TTN_AssetSystem::GetShader("Basic textured shader");
	shaderProgramSkybox = TTN_AssetSystem::GetShader("Skybox shader");
	shaderProgramTerrain = TTN_AssetSystem::GetShader("Terrain shader");
	shaderProgramWater = TTN_AssetSystem::GetShader("Water shader");
	shaderProgramAnimatedTextured = TTN_AssetSystem::GetShader("Animated textured shader");

	//grab the meshes
	cannonMesh = TTN_AssetSystem::GetMesh("Cannon mesh");
	skyboxMesh = TTN_AssetSystem::GetMesh("Skybox mesh");
	sphereMesh = TTN_AssetSystem::GetMesh("Sphere");
	flamethrowerMesh = TTN_AssetSystem::GetMesh("Flamethrower mesh");
	terrainPlain = TTN_AssetSystem::GetMesh("Terrain plane");
	damMesh = TTN_AssetSystem::GetMesh("Dam mesh");

	//grab textures
	cannonText = TTN_AssetSystem::GetTexture2D("Cannon texture");
	skyboxText = TTN_AssetSystem::GetSkybox("Skybox texture");
	terrainMap = TTN_AssetSystem::GetTexture2D("Terrain height map");
	sandText = TTN_AssetSystem::GetTexture2D("Sand texture");
	rockText = TTN_AssetSystem::GetTexture2D("Rock texture");
	grassText = TTN_AssetSystem::GetTexture2D("Grass texture");
	waterText = TTN_AssetSystem::GetTexture2D("Water texture");
	flamethrowerText = TTN_AssetSystem::GetTexture2D("Flamethrower texture");
	damText = TTN_AssetSystem::GetTexture2D("Dam texture");

	////MATERIALS////
	cannonMat = TTN_Material::Create();
	cannonMat->SetAlbedo(cannonText);
	cannonMat->SetShininess(128.0f);

	flamethrowerMat = TTN_Material::Create();
	flamethrowerMat->SetAlbedo(flamethrowerText);
	flamethrowerMat->SetShininess(128.0f);

	skyboxMat = TTN_Material::Create();
	skyboxMat->SetSkybox(skyboxText);

	damMat = TTN_Material::Create();
	damMat->SetAlbedo(damText);
}

void MainMenu::SetUpEntities()
{
	//entity for the camera
	{
		//create an entity in the scene for the camera
		camera = CreateEntity();
		SetCamEntity(camera);
		Attach<TTN_Transform>(camera);
		Attach<TTN_Camera>(camera);
		auto& camTrans = Get<TTN_Transform>(camera);
		camTrans.SetPos(glm::vec3(0.0f, 0.0f, 115.0f));
		camTrans.SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
		camTrans.LookAlong(glm::vec3(0.0, 0.0, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		Get<TTN_Camera>(camera).CalcPerspective(60.0f, 1.78f, 0.01f, 1000.f);
		Get<TTN_Camera>(camera).View();
	}

	//entity for the light
	{
		//create an entity in the scene for a light
		light = CreateEntity();

		m_Lights.push_back(light);

		//set up a trasnform for the light
		TTN_Transform lightTrans = TTN_Transform();
		lightTrans.SetPos(glm::vec3(0.0f, 3.0f, 5.0f));
		//attach that transform to the light entity
		AttachCopy<TTN_Transform>(light, lightTrans);

		//set up a light component for the light
		TTN_Light lightLight = TTN_Light(glm::vec3(1.0f), 0.6f, 2.0f, 0.3f, 0.3f, 0.3f);
		//attach that light to the light entity
		AttachCopy<TTN_Light>(light, lightLight);
	}

	//entity for the skybox
	{
		skybox = CreateEntity();

		//setup a mesh renderer for the skybox
		TTN_Renderer skyboxRenderer = TTN_Renderer(skyboxMesh, shaderProgramSkybox);
		skyboxRenderer.SetMat(skyboxMat);
		skyboxRenderer.SetRenderLayer(100);
		//attach that renderer to the entity
		AttachCopy<TTN_Renderer>(skybox, skyboxRenderer);

		//setup a transform for the skybox
		TTN_Transform skyboxTrans = TTN_Transform(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f));
		//attach that transform to the entity
		AttachCopy<TTN_Transform>(skybox, skyboxTrans);
	}

	//entity for the cannon
	{
		cannon = CreateEntity();

		//setup a mesh renderer for the cannon
		TTN_Renderer cannonRenderer = TTN_Renderer(cannonMesh, shaderProgramAnimatedTextured, cannonMat);
		//attach that renderer to the entity
		AttachCopy(cannon, cannonRenderer);

		//setup a transform for the cannon
		TTN_Transform cannonTrans = TTN_Transform(glm::vec3(0.0f, -0.4f, -0.25f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.40f));
		cannonTrans.RotateFixed(glm::vec3(0.0f, 90.0f, 0.0f));
		//attach that transform to the entity
		AttachCopy(cannon, cannonTrans);

		//setup an animator for the cannon
		TTN_MorphAnimator cannonAnimator = TTN_MorphAnimator();
		//create an animation for the cannon when it's not firing
		TTN_MorphAnimation notFiringAnim = TTN_MorphAnimation({ 0 }, { 3.0f / 24 }, true); //anim 0
		//create an animation for the cannon when it is firing
		std::vector<int> firingFrameIndices = std::vector<int>();
		std::vector<float> firingFrameLenghts = std::vector<float>();
		for (int i = 0; i < 7; i++) firingFrameIndices.push_back(i);
		firingFrameLenghts.push_back(3.0f / 24.0f);
		firingFrameLenghts.push_back(1.0f / 24.0f);
		firingFrameLenghts.push_back(1.0f / 24.0f);
		firingFrameLenghts.push_back(1.0f / 24.0f);
		firingFrameLenghts.push_back(1.0f / 24.0f);
		firingFrameLenghts.push_back(2.0f / 24.0f);
		firingFrameLenghts.push_back(3.0f / 24.0f);
		TTN_MorphAnimation firingAnim = TTN_MorphAnimation(firingFrameIndices, firingFrameLenghts, true); //anim 1
		//add both animatons to the animator
		cannonAnimator.AddAnim(notFiringAnim);
		cannonAnimator.AddAnim(firingAnim);
		//start on the not firing anim
		cannonAnimator.SetActiveAnim(0);
		//attach that animator to the entity
		AttachCopy(cannon, cannonAnimator);
	}

	//entity for the dam
	{
		dam = CreateEntity();

		//setup a mesh renderer for the dam
		TTN_Renderer damRenderer = TTN_Renderer(damMesh, shaderProgramTextured, damMat);
		//attach that renderer to the entity
		AttachCopy(dam, damRenderer);

		//setup a transform for the dam
		TTN_Transform damTrans = TTN_Transform(glm::vec3(0.0f, -10.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.7f, 0.7f, 0.3f));
		//attach that transform to the entity
		AttachCopy(dam, damTrans);
	}

	flamethrowers = std::vector<entt::entity>();
	//entities for flamethrowers
	for (int i = 0; i < 6; i++) {
		//flamethrower entities
		{
			flamethrowers.push_back(CreateEntity());

			//setup a mesh renderer for the cannon
			TTN_Renderer ftRenderer = TTN_Renderer(flamethrowerMesh, shaderProgramTextured);
			ftRenderer.SetMat(flamethrowerMat);
			//attach that renderer to the entity
			AttachCopy<TTN_Renderer>(flamethrowers[i], ftRenderer);

			//setup a transform for the flamethrower
			TTN_Transform ftTrans = TTN_Transform(glm::vec3(5.0f, -6.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.40f));
			if (i == 0) {
				ftTrans.SetPos(glm::vec3(-5.0f, -6.0f, 2.0f));
			}
			else if (i == 1) {
				ftTrans.SetPos(glm::vec3(15.0f, -6.0f, 2.0f));
			}
			else if (i == 2) {
				ftTrans.SetPos(glm::vec3(-15.0f, -6.0f, 2.0f));
			}
			else if (i == 3) {
				ftTrans.SetPos(glm::vec3(40.0f, -6.0f, 2.0f));
			}
			else if (i == 4) {
				ftTrans.SetPos(glm::vec3(-40.0f, -6.0f, 2.0f));
			}
			else {}

			//attach that transform to the entity
			AttachCopy<TTN_Transform>(flamethrowers[i], ftTrans);
		}
	}

	//terrain entity
	{
		terrain = CreateEntity();

		//setup a transform for the terrain
		TTN_Transform terrainTrans = TTN_Transform(glm::vec3(0.0f, -10.0f, 35.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(100.0f));
		//attach that transform to the entity
		AttachCopy(terrain, terrainTrans);
	}

	//water
	{
		water = CreateEntity();

		//setup a transform for the water
		TTN_Transform waterTrans = TTN_Transform(glm::vec3(0.0f, -8.0f, 35.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(93.0f));
		//attach that transform to the entity
		AttachCopy(water, waterTrans);
	}
}

void MainMenu::SetUpOtherData()
{
	//init some scene data
	terrainScale = 0.1f;
	time = 0.0f;
	waveSpeed = -2.5f;
	waveBaseHeightIncrease = 0.0f;
	waveHeightMultiplier = 0.005f;
	waveLenghtMultiplier = -10.0f;
}

MainMenuUI::MainMenuUI()
	: TTN_Scene()
{
	//set set the navigation bools
	shouldPlay = false;
	shouldQuit = false;
}

void MainMenuUI::InitScene()
{
	//grab the textures
	textureGameLogo = TTN_AssetSystem::GetTexture2D("Game logo");
	textureButton1 = TTN_AssetSystem::GetTexture2D("Button Base");
	textureButton2 = TTN_AssetSystem::GetTexture2D("Button Hovering");
	texturePlay = TTN_AssetSystem::GetTexture2D("Play-Text");
	textureArcade = TTN_AssetSystem::GetTexture2D("Arcade-Text");
	textureOptions = TTN_AssetSystem::GetTexture2D("Options-Text");
	textureQuit = TTN_AssetSystem::GetTexture2D("Quit-Text");

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
		//create an entity in the scene for the logo
		gameLogo = CreateEntity();

		//create a transform for the logo
		TTN_Transform logoTrans = TTN_Transform(glm::vec3(0.0f, 360.0f, 1.0f), glm::vec3(0.0f), glm::vec3(-1603.0f, 185.5f, 1.0f));
		AttachCopy(gameLogo, logoTrans);

		//create a sprite renderer for the logo
		TTN_Renderer2D logoRenderer = TTN_Renderer2D(textureGameLogo);
		AttachCopy(gameLogo, logoRenderer);
	}

	//text
	for (int i = 0; i < 4; i++) {
		entt::entity temp = CreateEntity();
		if (i == 0) playText = temp;
		else if (i == 1) arcadeText = temp;
		else if (i == 2) optionsText = temp;
		else if (i == 3) quitText = temp;

		//create a transform for the text
		TTN_Transform textTrans;
		if (i == 0) textTrans = TTN_Transform(glm::vec3(657.75f, -180.0f, 1.0f), glm::vec3(0.0f), glm::vec3(-550.0f / 2.75f, 150.0f / 2.75f, 1.0f));
		else if (i == 1) textTrans = TTN_Transform(glm::vec3(254.75, -360.0f, 1.0f), glm::vec3(0.0f), glm::vec3(-550.0f / 2.75f, 150.0f / 2.75f, 1.0f));
		else if (i == 2) textTrans = TTN_Transform(glm::vec3(-148.25, -180.0f, 1.0f), glm::vec3(0.0f), glm::vec3(-550.0f / 2.75f, 150.0f / 2.75f, 1.0f));
		else if (i == 3) textTrans = TTN_Transform(glm::vec3(-551.25, -360.0f, 1.0f), glm::vec3(0.0f), glm::vec3(-550.0f / 2.75f, 150.0f / 2.75f, 1.0f));
		AttachCopy(temp, textTrans);

		//create a 2D renderer for the button
		TTN_Renderer2D textRenderer;
		if (i == 0) textRenderer = TTN_Renderer2D(texturePlay);
		else if (i == 1) textRenderer = TTN_Renderer2D(textureArcade);
		else if (i == 2) textRenderer = TTN_Renderer2D(textureOptions);
		else if (i == 3) textRenderer = TTN_Renderer2D(textureQuit);
		AttachCopy(temp, textRenderer);

	}

	//buttons
	for (int i = 0; i < 4; i++) {
		entt::entity temp = CreateEntity();
		if (i == 0) playButton = temp;
		else if (i == 1) arcadeButton = temp;
		else if (i == 2) optionsButton = temp;
		else if (i == 3) quitButton = temp;

		//create a transform for the button
		TTN_Transform buttonTrans;
		if (i == 0) buttonTrans = TTN_Transform(glm::vec3(657.75f, -180.0f, 2.0f), glm::vec3(0.0f), glm::vec3(-322.75f, 201.5, 1.0f));
		else if (i == 1) buttonTrans = TTN_Transform(glm::vec3(254.75, -360.0f, 2.0f), glm::vec3(0.0f), glm::vec3(-322.75f, 201.5, 1.0f));
		else if (i == 2) buttonTrans = TTN_Transform(glm::vec3(-148.25, -180.0f, 2.0f), glm::vec3(0.0f), glm::vec3(-322.75f, 201.5, 1.0f));
		else if (i == 3) buttonTrans = TTN_Transform(glm::vec3(-551.25, -360.0f, 2.0f), glm::vec3(0.0f), glm::vec3(-322.75f, 201.5, 1.0f));
		AttachCopy(temp, buttonTrans);

		//create a 2D renderer for the button
		TTN_Renderer2D buttonRenderer = TTN_Renderer2D(textureButton1);
		AttachCopy(temp, buttonRenderer);
	}
}

void MainMenuUI::Update(float deltaTime)
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

	//get play buttons transform
	TTN_Transform playButtonTrans = Get<TTN_Transform>(playButton);
	if (mousePosWorldSpace.x < playButtonTrans.GetPos().x + 0.5f  * abs(playButtonTrans.GetScale().x) && 
		mousePosWorldSpace.x > playButtonTrans.GetPos().x - 0.5f * abs(playButtonTrans.GetScale().x) &&
		mousePosWorldSpace.y < playButtonTrans.GetPos().y + 0.5f * abs(playButtonTrans.GetScale().y) &&
		mousePosWorldSpace.y > playButtonTrans.GetPos().y - 0.5f * abs(playButtonTrans.GetScale().y)) {
		Get<TTN_Renderer2D>(playButton).SetSprite(textureButton2);
	}
	else {
		Get<TTN_Renderer2D>(playButton).SetSprite(textureButton1);
	}

	//get aracde buttons transform
	TTN_Transform arcadeButtonTrans = Get<TTN_Transform>(arcadeButton);
	if (mousePosWorldSpace.x < arcadeButtonTrans.GetPos().x + 0.5f * abs(arcadeButtonTrans.GetScale().x) &&
		mousePosWorldSpace.x > arcadeButtonTrans.GetPos().x - 0.5f * abs(arcadeButtonTrans.GetScale().x) &&
		mousePosWorldSpace.y < arcadeButtonTrans.GetPos().y + 0.5f * abs(arcadeButtonTrans.GetScale().y) &&
		mousePosWorldSpace.y > arcadeButtonTrans.GetPos().y - 0.5f * abs(arcadeButtonTrans.GetScale().y)) {
		Get<TTN_Renderer2D>(arcadeButton).SetSprite(textureButton2);
	}
	else {
		Get<TTN_Renderer2D>(arcadeButton).SetSprite(textureButton1);
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

void MainMenuUI::MouseButtonDownChecks()
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

		//get play buttons transform
		TTN_Transform playButtonTrans = Get<TTN_Transform>(playButton);
		if (mousePosWorldSpace.x < playButtonTrans.GetPos().x + 0.5f * abs(playButtonTrans.GetScale().x) &&
			mousePosWorldSpace.x > playButtonTrans.GetPos().x - 0.5f * abs(playButtonTrans.GetScale().x) &&
			mousePosWorldSpace.y < playButtonTrans.GetPos().y + 0.5f * abs(playButtonTrans.GetScale().y) &&
			mousePosWorldSpace.y > playButtonTrans.GetPos().y - 0.5f * abs(playButtonTrans.GetScale().y)) {
			shouldPlay = true;
		}

		//get aracde buttons transform
		TTN_Transform arcadeButtonTrans = Get<TTN_Transform>(arcadeButton);
		if (mousePosWorldSpace.x < arcadeButtonTrans.GetPos().x + 0.5f * abs(arcadeButtonTrans.GetScale().x) &&
			mousePosWorldSpace.x > arcadeButtonTrans.GetPos().x - 0.5f * abs(arcadeButtonTrans.GetScale().x) &&
			mousePosWorldSpace.y < arcadeButtonTrans.GetPos().y + 0.5f * abs(arcadeButtonTrans.GetScale().y) &&
			mousePosWorldSpace.y > arcadeButtonTrans.GetPos().y - 0.5f * abs(arcadeButtonTrans.GetScale().y)) {
			//do something
		}

		//get options buttons transform
		TTN_Transform optionsButtonTrans = Get<TTN_Transform>(optionsButton);
		if (mousePosWorldSpace.x < optionsButtonTrans.GetPos().x + 0.5f * abs(optionsButtonTrans.GetScale().x) &&
			mousePosWorldSpace.x > optionsButtonTrans.GetPos().x - 0.5f * abs(optionsButtonTrans.GetScale().x) &&
			mousePosWorldSpace.y < optionsButtonTrans.GetPos().y + 0.5f * abs(optionsButtonTrans.GetScale().y) &&
			mousePosWorldSpace.y > optionsButtonTrans.GetPos().y - 0.5f * abs(optionsButtonTrans.GetScale().y)) {
			//do something
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
