//Dam Defense, by Atlas X Games
//GameOverMenu.cpp, the source file for the class that represents the game over menu

#include "GameOverMenu.h"
#include "glm/ext.hpp"

GameOverMenu::GameOverMenu()
	: TTN_Scene()
{
}

void GameOverMenu::InitScene()
{
	//load in the scene's assets
	SetUpAssets();

	//set up the other data
	SetUpOtherData();

	//create the entities
	SetUpEntities();

	TTN_Scene::InitScene();
}

void GameOverMenu::Update(float deltaTime)
{
	//increase the total time of the scene to make the water animated correctly
	time += deltaTime / 2.5f;

	//call imgui's update for this scene
	ImGui();

	//control to make the CG controls appear or disappear
	if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::C) && TTN_Application::TTN_Input::GetKeyDown(TTN_KeyCode::G)) {
		showCGControls = !showCGControls;
	}

	//don't forget to call the base class' update
	TTN_Scene::Update(deltaTime);
}

void GameOverMenu::PostRender()
{
	//disable blending so the gBuffer can draw properlly
	glDisable(GL_BLEND);

	//bind the geometry buffer
	gBuffer->Bind();

	//render the terrain
	{
		//bind the shader
		shaderProgramTerrain->Bind();

		//vert shader
		//bind the height map texture
		terrainMap->Bind(0);
		TTN_AssetSystem::GetTexture2D("Normal Map")->Bind(1);

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
		sandText->Bind(2);
		rockText->Bind(3);
		grassText->Bind(4);

		m_mats[0]->GetDiffuseRamp()->Bind(10);
		m_mats[0]->GetSpecularMap()->Bind(11);

		//set if the albedo textures should be used
		shaderProgramTerrain->SetUniform("u_UseDiffuse", (int)m_mats[0]->GetUseAlbedo());

		//render the terrain
		terrainPlain->GetVAOPointer()->Render();
	}

	//render the water
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
		float time = WaterManager::GetTime();
		float steepness = WaterManager::GetSteepness();
		int waveNums = WaterManager::GetNumberOfWaves();
		float speed = WaterManager::GetSpeed();

		shaderProgramWater->SetUniform("time", time);
		shaderProgramWater->SetUniform("u_Q", steepness);
		shaderProgramWater->SetUniform("u_numOfWaves", waveNums);
		shaderProgramWater->SetUniform("u_speed", speed);

		float amplitudes[16];
		float freqs[16];
		glm::vec3 directions[16];

		for (int i = 0; i < 16; i++) {
			if (i < WaterManager::GetNumberOfWaves()) {
				amplitudes[i] = WaterManager::m_waveAcutalAmplitude[i];
				freqs[i] = WaterManager::m_waveFrequency[i];
				directions[i] = WaterManager::m_waveDirection[i];
			}
			else {
				amplitudes[i] = 0.0f;
				freqs[i] = 0.0f;
				directions[i] = glm::vec3(0.0f);
			}
		}

		shaderProgramWater->SetUniform("u_amplitude", amplitudes[0], 16);
		shaderProgramWater->SetUniform("u_frequency", freqs[0], 16);
		shaderProgramWater->SetUniform("u_direction", directions[0], 16);

		//frag shader
		//bind the textures
		waterText->Bind(0);
		WaterManager::BindVoronoiAsColor(1);

		//send lighting from the scene
		shaderProgramWater->SetUniform("u_UseDiffuse", (int)m_mats[0]->GetUseAlbedo());

		//render the water (just use the same plane as the terrain)
		terrainPlain->GetVAOPointer()->Render();

		WaterManager::UnbindVoronoi(1);
	}

	//unbind the geometry buffer
	gBuffer->Unbind();

	//enable blending again so it works on everything else
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//and do the rest of the titan side rendering
	TTN_Scene::PostRender();
}

void GameOverMenu::SetUpAssets()
{
	m_DialougeGameOver = TTN_AudioEventHolder::Create("Game over dialogue", "{6aad0ce5-e274-4e93-afe2-ae6fcf1c28e4}", 1);

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
	m_mats.push_back(cannonMat);

	flamethrowerMat = TTN_Material::Create();
	flamethrowerMat->SetAlbedo(flamethrowerText);
	flamethrowerMat->SetShininess(128.0f);
	m_mats.push_back(flamethrowerMat);

	skyboxMat = TTN_Material::Create();
	skyboxMat->SetSkybox(skyboxText);

	damMat = TTN_Material::Create();
	damMat->SetAlbedo(damText);
	m_mats.push_back(damMat);

	m_mats.push_back(TTN_AssetSystem::GetMaterial("boat1Mat"));
	m_mats.push_back(TTN_AssetSystem::GetMaterial("boat2Mat"));
	m_mats.push_back(TTN_AssetSystem::GetMaterial("boat3Mat"));
	m_mats.push_back(TTN_AssetSystem::GetMaterial("enemyCannonMat"));
	m_mats.push_back(TTN_AssetSystem::GetMaterial("smokeMat"));
	m_mats.push_back(TTN_AssetSystem::GetMaterial("fireMat"));
	m_mats.push_back(TTN_AssetSystem::GetMaterial("birdMat"));
	m_mats.push_back(TTN_AssetSystem::GetMaterial("LightHouseMat"));
	m_mats.push_back(TTN_AssetSystem::GetMaterial("TreeMat"));

	for (int i = 0; i < m_mats.size(); i++) {
		m_mats[i]->SetDiffuseRamp(TTN_AssetSystem::GetTexture2D("blue ramp"));
		m_mats[i]->SetSpecularRamp(TTN_AssetSystem::GetTexture2D("blue ramp"));
		m_mats[i]->SetUseDiffuseRamp(m_useDiffuseRamp);
		m_mats[i]->SetUseSpecularRamp(m_useSpecularRamp);
	}
}

void GameOverMenu::SetUpEntities()
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
		TTN_Transform terrainTrans = TTN_Transform(glm::vec3(0.0f, -15.0f, 35.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(100.0f));
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

void GameOverMenu::SetUpOtherData()
{
	//init some scene data
	terrainScale = 0.15f;
	time = 0.0f;
	waveSpeed = -2.5f;
	waveBaseHeightIncrease = 0.0f;
	waveHeightMultiplier = 0.005f;
	waveLenghtMultiplier = -10.0f;

	//setup up the color correction effect
	glm::ivec2 windowSize = TTN_Backend::GetWindowSize();
	m_colorCorrectEffect = TTN_ColorCorrect::Create();
	m_colorCorrectEffect->Init(windowSize.x, windowSize.y);
	//set to custom
	m_colorCorrectEffect->SetShouldApply(true);
	m_colorCorrectEffect->SetCube(TTN_AssetSystem::GetLUT("Custom LUT"));
	//and add it to this scene's list of effects
	m_PostProcessingEffects.push_back(m_colorCorrectEffect);

	//set all 3 effects to false
	m_applyWarmLut = false;
	m_applyCoolLut = false;
	m_applyCustomLut = false;

	//set the lighting bools
	m_noLighting = false;
	m_ambientOnly = false;
	m_specularOnly = false;
	m_ambientAndSpecular = true;
	m_ambientSpecularAndOutline = false;

	//setup the bloom effect
	m_bloomThreshold = m_bloomEffect->GetThreshold();
	m_numOfBloomPasses = m_bloomEffect->GetNumOfPasses();
	m_bloomBufferDivisor = m_bloomEffect->GetBlurDownScale();
	m_bloomRadius = m_bloomEffect->GetRadius();
	m_bloomStrength = m_bloomEffect->GetStrength();
	m_PostProcessingEffects.push_back(m_bloomEffect);

	for (int i = 0; i < m_mats.size(); i++)
		m_mats[i]->SetOutlineSize(m_outlineSize);
}

void GameOverMenu::ImGui()
{
	if (showCGControls) {
		//ImGui controls for the CG requirements
		ImGui::Begin("CG Controls");

		if (ImGui::CollapsingHeader("Lighting and Shadow Mapping Controls")) {
			TTN_DirectionalLight tempSun = illBuffer->GetSunRef();

			if (ImGui::SliderFloat3("Directional Light Direction", glm::value_ptr(tempSun.m_lightDirection), -50.0f, 0.0f)) {
				SetSun(tempSun);
			}

			if (ImGui::ColorPicker3("Directional Light Color", glm::value_ptr(tempSun.m_lightColor))) {
				SetSun(tempSun);
			}

			if (ImGui::ColorPicker3("Directional Light Ambient Color", glm::value_ptr(tempSun.m_ambientColor))) {
				SetSun(tempSun);
			}

			if (ImGui::SliderFloat("Directional Light Ambient Power", &tempSun.m_ambientPower, 0.0f, 5.0f)) {
				SetSun(tempSun);
			}

			if (ImGui::SliderFloat("Directional Light - Light Ambient Power", &tempSun.m_lightAmbientPower, 0.0f, 5.0f)) {
				SetSun(tempSun);
			}

			if (ImGui::SliderFloat("Directional Light - Light Specular Power", &tempSun.m_lightSpecularPower, 0.0f, 5.0f)) {
				SetSun(tempSun);
			}

			if (ImGui::SliderFloat("Directional Light Min Shadow Bias", &tempSun.m_minShadowBias, 0.0f, 0.005f)) {
				SetSun(tempSun);
			}

			if (ImGui::SliderFloat("Directional Light Max Shadow Bias", &tempSun.m_maxShadowBias, 0.0f, 0.005f)) {
				SetSun(tempSun);
			}

			if (ImGui::SliderInt("Directional Light PCF Filter Passes", &tempSun.m_pcfFilterSamples, 1, 20)) {
				SetSun(tempSun);
			}

			float emissiveStr = illBuffer->GetEmissiveStrenght();
			if (ImGui::SliderFloat("Emissive Strenght", &emissiveStr, 0.0f, 10.0f)) {
				illBuffer->SetEmissiveStrenght(emissiveStr);
			}

			bool AmbientOn = illBuffer->GetUseAmbient();
			if (ImGui::Checkbox("Ambient Lighting On", &AmbientOn)) {
				illBuffer->SetUseAmbient(AmbientOn);
			}

			bool SpecularOn = illBuffer->GetUseSpecular();
			if (ImGui::Checkbox("Specular Lighting On", &SpecularOn)) {
				illBuffer->SetUseSpecular(SpecularOn);
			}

			bool ShadowingOn = illBuffer->GetuseShadowMapping();
			if (ImGui::Checkbox("Shadow Mapping On", &ShadowingOn)) {
				illBuffer->SetUseShadowMapping(ShadowingOn);
			}

			bool EmissiveOn = TTN_AssetSystem::GetMaterial("LightHouseMat")->GetUseEmissive();
			if (ImGui::Checkbox("Emissive Lighting On", &EmissiveOn)) {
				TTN_AssetSystem::GetMaterial("LightHouseMat")->SetUseEmissive(EmissiveOn);
			}

			bool RimOn = TTN_AssetSystem::GetMaterial("boat1Mat")->GetHasRimLighting();
			if (ImGui::Checkbox("Rim Lighting On", &RimOn)) {
				TTN_AssetSystem::GetMaterial("boat1Mat")->SetHasRimLighting(RimOn);
				TTN_AssetSystem::GetMaterial("boat1Mat")->SetHasRimLighting(RimOn);
				TTN_AssetSystem::GetMaterial("boat1Mat")->SetHasRimLighting(RimOn);
			}
		}

		if (ImGui::CollapsingHeader("Texture Controls")) {
			bool  texturesOn = m_mats[0]->GetUseAlbedo();
			if (ImGui::Checkbox("Use Textures", &texturesOn)) {
				for (int i = 0; i < m_mats.size(); i++) {
					m_mats[i]->SetUseAlbedo(texturesOn);
				}
			}
		}

		if (ImGui::CollapsingHeader("Color Correction Controls")) {
			bool mainlutIsActive = m_colorCorrectEffect->GetShouldApply();
			if (ImGui::Checkbox("Game Over Correction On", &mainlutIsActive)) {
				m_colorCorrectEffect->SetShouldApply(mainlutIsActive);
			}

			ImGui::Text("Others Color correction effects can be toggled from the scenes they affect, and the options menu");
		}

		if (ImGui::CollapsingHeader("Bloom Controls")) {
			bool bloomShouldApply = m_bloomEffect->GetShouldApply();
			if (ImGui::Checkbox("Bloom On", &bloomShouldApply)) {
				m_bloomEffect->SetShouldApply(bloomShouldApply);
			}

			if (ImGui::SliderFloat("Threshold", &m_bloomThreshold, 0.0f, 1.0f)) {
				m_bloomEffect->SetThreshold(m_bloomThreshold);
			}

			if (ImGui::SliderInt("Number of blur passes", &m_numOfBloomPasses, 1, 20)) {
				m_bloomEffect->SetNumOfPasses(m_numOfBloomPasses);
			}

			int tempInt = m_bloomBufferDivisor;
			if (ImGui::SliderInt("Downscale divisor", &tempInt, 1, 20)) {
				m_bloomBufferDivisor = tempInt;
				m_bloomEffect->SetBlurDownScale(m_bloomBufferDivisor);
			}

			if (ImGui::SliderFloat("radius", &m_bloomRadius, 0.1f, 20.0f)) {
				m_bloomEffect->SetRadius(m_bloomRadius);
			}

			if (ImGui::SliderFloat("Strength (radial)", &m_bloomStrength, 0.1f, 20.0f)) {
				m_bloomEffect->SetStrength(m_bloomStrength);
			}

			if (ImGui::Button("Make Gaussian Blur")) {
				m_bloomEffect->SetBlurMode(TTN_BloomBlurModes::GAUSSIAN);
			}

			if (ImGui::Button("Make Box Blur")) {
				m_bloomEffect->SetBlurMode(TTN_BloomBlurModes::BOX);
			}

			if (ImGui::Button("Make Radial Blur")) {
				m_bloomEffect->SetBlurMode(TTN_BloomBlurModes::RADIAL);
			}
		}

		if (ImGui::CollapsingHeader("Toon Shading Controls")) {
			bool diffuseRampIsOn = illBuffer->GetUseDiffuseRamp();
			if (ImGui::Checkbox("Diffuse Ramp On", &diffuseRampIsOn)) {
				illBuffer->SetUseDiffuseRamp(diffuseRampIsOn);
			}

			bool specularRampIsOn = illBuffer->GetUseSpecularRamp();
			if (ImGui::Checkbox("Specular Ramp On", &specularRampIsOn)) {
				illBuffer->SetUseSpecularRamp(specularRampIsOn);
			}
		}

		ImGui::End();
	}
}

GameOverMenuUI::GameOverMenuUI()
	: TTN_Scene()
{
	//set set the navigation bools
	shouldPlay = false;
	shouldQuit = false;
	shouldMenu = false;
}

void GameOverMenuUI::InitScene()
{
	//grab the textures
	textureGameLogo = TTN_AssetSystem::GetTexture2D("Game logo");
	textureGameOver = TTN_AssetSystem::GetTexture2D("Game Over");
	textureButton1 = TTN_AssetSystem::GetTexture2D("Button Base");
	textureButton2 = TTN_AssetSystem::GetTexture2D("Button Hovering");
	texturePlayAgain = TTN_AssetSystem::GetTexture2D("Play Again");
	textureQuit = TTN_AssetSystem::GetTexture2D("Quit-Text");
	textureScore = TTN_AssetSystem::GetTexture2D("Score");
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
		//Get<TTN_Camera>(cam).CalcPerspective(60.0f, 1.78f, 0.01f, 1000.f);
		Get<TTN_Camera>(cam).View();
	}

	//game over
	{
		//create an entity in the scene for the logo
		gameOver = CreateEntity();

		//create a transform for the logo
		TTN_Transform logoTrans = TTN_Transform(glm::vec3(0.0f, 360.0f, 1.0f), glm::vec3(0.0f), glm::vec3(550 * 2.0f, 150.0f * 2.0f, 1.0f));
		AttachCopy(gameOver, logoTrans);

		//create a sprite renderer for the logo
		TTN_Renderer2D logoRenderer = TTN_Renderer2D(textureGameOver);
		AttachCopy(gameOver, logoRenderer);
	}

	//text
	for (int i = 0; i < 3; i++) {
		entt::entity temp = CreateEntity();
		if (i == 0) playText = temp;
		else if (i == 1) menuText = temp;
		else if (i == 2) quitText = temp;

		//create a transform for the text
		TTN_Transform textTrans;
		if (i == 0) textTrans = TTN_Transform(glm::vec3(500.0f, -180.0f, 1.0f), glm::vec3(0.0f), glm::vec3(550.0f / 2.75f, 150.0f / 2.75f, 1.0f));
		else if (i == 1) textTrans = TTN_Transform(glm::vec3(10.0f, -180.0f, 1.0f), glm::vec3(0.0f), glm::vec3(550.0f / 2.75f, 150.0f / 2.75f, 1.0f));
		else if (i == 2) textTrans = TTN_Transform(glm::vec3(-500.0f, -180.0f, 1.0f), glm::vec3(0.0f), glm::vec3(550.0f / 2.75f, 150.0f / 2.75f, 1.0f));
		AttachCopy(temp, textTrans);

		//create a 2D renderer for the button
		TTN_Renderer2D textRenderer;
		if (i == 0) textRenderer = TTN_Renderer2D(texturePlayAgain);
		else if (i == 1) textRenderer = TTN_Renderer2D(textureMainMenu);
		else if (i == 2) textRenderer = TTN_Renderer2D(textureQuit);
		AttachCopy(temp, textRenderer);
	}

	//buttons
	for (int i = 0; i < 3; i++) {
		entt::entity temp = CreateEntity();
		if (i == 0) playButton = temp;
		else if (i == 1) menuButton = temp;
		else if (i == 2) quitButton = temp;

		//create a transform for the button
		TTN_Transform buttonTrans;
		if (i == 0) buttonTrans = TTN_Transform(glm::vec3(500.0f, -180.0f, 2.0f), glm::vec3(0.0f), glm::vec3(322.75f, 201.5, 1.0f));
		else if (i == 1) buttonTrans = TTN_Transform(glm::vec3(10.0f, -180.0f, 2.0f), glm::vec3(0.0f), glm::vec3(322.75f, 201.5, 1.0f));
		else if (i == 2) buttonTrans = TTN_Transform(glm::vec3(-500.0f, -180.0f, 2.0f), glm::vec3(0.0f), glm::vec3(322.75f, 201.5, 1.0f));
		AttachCopy(temp, buttonTrans);

		//create a 2D renderer for the button
		TTN_Renderer2D buttonRenderer = TTN_Renderer2D(textureButton1);
		AttachCopy(temp, buttonRenderer);
	}

	TTN_Scene::InitScene();
}

void GameOverMenuUI::Update(float deltaTime)
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
	if (mousePosWorldSpace.x < playButtonTrans.GetPos().x + 0.5f * abs(playButtonTrans.GetScale().x) &&
		mousePosWorldSpace.x > playButtonTrans.GetPos().x - 0.5f * abs(playButtonTrans.GetScale().x) &&
		mousePosWorldSpace.y < playButtonTrans.GetPos().y + 0.5f * abs(playButtonTrans.GetScale().y) &&
		mousePosWorldSpace.y > playButtonTrans.GetPos().y - 0.5f * abs(playButtonTrans.GetScale().y)) {
		Get<TTN_Renderer2D>(playButton).SetSprite(textureButton2);
	}
	else {
		Get<TTN_Renderer2D>(playButton).SetSprite(textureButton1);
	}

	//get play buttons transform
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

void GameOverMenuUI::MouseButtonDownChecks()
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

		//get menu buttons transform
		TTN_Transform menuButtonTrans = Get<TTN_Transform>(menuButton);
		if (mousePosWorldSpace.x < menuButtonTrans.GetPos().x + 0.5f * abs(menuButtonTrans.GetScale().x) &&
			mousePosWorldSpace.x > menuButtonTrans.GetPos().x - 0.5f * abs(menuButtonTrans.GetScale().x) &&
			mousePosWorldSpace.y < menuButtonTrans.GetPos().y + 0.5f * abs(menuButtonTrans.GetScale().y) &&
			mousePosWorldSpace.y > menuButtonTrans.GetPos().y - 0.5f * abs(menuButtonTrans.GetScale().y)) {
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