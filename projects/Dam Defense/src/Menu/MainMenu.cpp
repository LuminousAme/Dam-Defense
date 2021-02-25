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

	//call imgui's update for this scene
	ImGui();

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

		//send lighting from the scene
		shaderProgramTerrain->SetUniform("u_AmbientCol", TTN_Scene::GetSceneAmbientColor());
		shaderProgramTerrain->SetUniform("u_AmbientStrength", TTN_Scene::GetSceneAmbientLightStrength());
		shaderProgramTerrain->SetUniform("u_Shininess", 128.0f);
		shaderProgramTerrain->SetUniform("u_hasAmbientLighting", (int)m_mats[0]->GetHasAmbient());
		shaderProgramTerrain->SetUniform("u_hasSpecularLighting", (int)m_mats[0]->GetHasSpecular());
		shaderProgramTerrain->SetUniform("u_hasOutline", (int)m_mats[0]->GetHasOutline());
		shaderProgramTerrain->SetUniform("u_useDiffuseRamp", m_mats[0]->GetUseDiffuseRamp());
		shaderProgramTerrain->SetUniform("u_useSpecularRamp", (int)m_mats[0]->GetUseSpecularRamp());
		//stuff from the light
		glm::vec3 lightPositions[16];
		glm::vec3 lightColor[16];
		float lightAmbientStr[16];
		float lightSpecStr[16];
		float lightAttenConst[16];
		float lightAttenLinear[16];
		float lightAttenQuadartic[16];

		for (int i = 0; i < 16 && i < m_Lights.size(); i++) {
			auto& light = Get<TTN_Light>(m_Lights[i]);
			auto& lightTrans = Get<TTN_Transform>(m_Lights[i]);
			lightPositions[i] = lightTrans.GetPos();
			lightColor[i] = light.GetColor();
			lightAmbientStr[i] = light.GetAmbientStrength();
			lightSpecStr[i] = light.GetSpecularStrength();
			lightAttenConst[i] = light.GetConstantAttenuation();
			lightAttenLinear[i] = light.GetConstantAttenuation();
			lightAttenQuadartic[i] = light.GetQuadraticAttenuation();
		}

		//send all the data about the lights to glsl
		shaderProgramTerrain->SetUniform("u_LightPos", lightPositions[0], 16);
		shaderProgramTerrain->SetUniform("u_LightCol", lightColor[0], 16);
		shaderProgramTerrain->SetUniform("u_AmbientLightStrength", lightAmbientStr[0], 16);
		shaderProgramTerrain->SetUniform("u_SpecularLightStrength", lightSpecStr[0], 16);
		shaderProgramTerrain->SetUniform("u_LightAttenuationConstant", lightAttenConst[0], 16);
		shaderProgramTerrain->SetUniform("u_LightAttenuationLinear", lightAttenLinear[0], 16);
		shaderProgramTerrain->SetUniform("u_LightAttenuationQuadratic", lightAttenQuadartic[0], 16);

		//and tell it how many lights there actually are
		shaderProgramTerrain->SetUniform("u_NumOfLights", (int)m_Lights.size());

		//stuff from the camera
		shaderProgramTerrain->SetUniform("u_CamPos", Get<TTN_Transform>(camera).GetPos());

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

	for (int i = 0; i < m_mats.size(); i++) {
		m_mats[i]->SetDiffuseRamp(TTN_AssetSystem::GetTexture2D("blue ramp"));
		m_mats[i]->SetSpecularRamp(TTN_AssetSystem::GetTexture2D("blue ramp"));
		m_mats[i]->SetUseDiffuseRamp(m_useDiffuseRamp);
		m_mats[i]->SetUseSpecularRamp(m_useSpecularRamp);
	}
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

void MainMenu::SetUpOtherData()
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
	//set it so it doesn't render
	m_colorCorrectEffect->SetShouldApply(false);
	m_colorCorrectEffect->SetCube(TTN_AssetSystem::GetLUT("Warm LUT"));
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

	for (int i = 0; i < m_mats.size(); i++)
		m_mats[i]->SetOutlineSize(m_outlineSize);
}

//imgui update for this scene and frame
void MainMenu::ImGui()
{
	ImGui::Begin("Editor");

	if (ImGui::CollapsingHeader("Light Controls")) {
		ImGui::Text("Maximum number of lights: 16");

		//scene level lighting
		float sceneAmbientLight[3], sceneAmbientStr;
		sceneAmbientLight[0] = GetSceneAmbientColor().r;
		sceneAmbientLight[1] = GetSceneAmbientColor().g;
		sceneAmbientLight[2] = GetSceneAmbientColor().b;
		sceneAmbientStr = GetSceneAmbientLightStrength();

		//scene level ambient strenght
		if (ImGui::SliderFloat("Scene level ambient strenght", &sceneAmbientStr, 0.0f, 1.0f)) {
			SetSceneAmbientLightStrength(sceneAmbientStr);
		}

		//scene level ambient color
		if (ImGui::ColorPicker3("Scene level ambient color", sceneAmbientLight)) {
			SetSceneAmbientColor(glm::vec3(sceneAmbientLight[0], sceneAmbientLight[1], sceneAmbientLight[2]));
		}

		//loop through all the lights 
		int i = 0;
		std::vector<entt::entity>::iterator it = m_Lights.begin();
		while (it != m_Lights.end()) {
			//make temp floats for their data
			float color[3], pos[3], ambientStr, specularStr, attenConst, attenLine, attenQuad;
			TTN_Light& tempLightRef = Get<TTN_Light>(*it);
			TTN_Transform& tempLightTransRef = Get<TTN_Transform>(*it);
			color[0] = tempLightRef.GetColor().r;
			color[1] = tempLightRef.GetColor().g;
			color[2] = tempLightRef.GetColor().b;
			pos[0] = tempLightTransRef.GetPos().x;
			pos[1] = tempLightTransRef.GetPos().y;
			pos[2] = tempLightTransRef.GetPos().z;
			ambientStr = tempLightRef.GetAmbientStrength();
			specularStr = tempLightRef.GetSpecularStrength();
			attenConst = tempLightRef.GetConstantAttenuation();
			attenLine = tempLightRef.GetLinearAttenuation();
			attenQuad = tempLightRef.GetQuadraticAttenuation();

			//position
			std::string tempPosString = "Light " + std::to_string(i) + " Position";
			if (ImGui::SliderFloat3(tempPosString.c_str(), pos, -100.0f, 100.0f)) {
				tempLightTransRef.SetPos(glm::vec3(pos[0], pos[1], pos[2]));
			}

			//color
			std::string tempColorString = "Light " + std::to_string(i) + " Color";
			if (ImGui::ColorPicker3(tempColorString.c_str(), color)) {
				tempLightRef.SetColor(glm::vec3(color[0], color[1], color[2]));
			}

			//strenghts
			std::string tempAmbientStrString = "Light " + std::to_string(i) + " Ambient strenght";
			if (ImGui::SliderFloat(tempAmbientStrString.c_str(), &ambientStr, 0.0f, 10.0f)) {
				tempLightRef.SetAmbientStrength(ambientStr);
			}

			std::string tempSpecularStrString = "Light " + std::to_string(i) + " Specular strenght";
			if (ImGui::SliderFloat(tempSpecularStrString.c_str(), &specularStr, 0.0f, 10.0f)) {
				tempLightRef.SetSpecularStrength(specularStr);
			}

			//attenutaition
			std::string tempAttenConst = "Light " + std::to_string(i) + " Constant Attenuation";
			if (ImGui::SliderFloat(tempAttenConst.c_str(), &attenConst, 0.0f, 100.0f)) {
				tempLightRef.SetConstantAttenuation(attenConst);
			}

			std::string tempAttenLine = "Light " + std::to_string(i) + " Linear Attenuation";
			if (ImGui::SliderFloat(tempAttenLine.c_str(), &attenLine, 0.0f, 100.0f)) {
				tempLightRef.SetLinearAttenuation(attenLine);
			}

			std::string tempAttenQuad = "Light " + std::to_string(i) + " Quadratic Attenuation";
			if (ImGui::SliderFloat(tempAttenQuad.c_str(), &attenQuad, 0.0f, 100.0f)) {
				tempLightRef.SetQuadraticAttenuation(attenQuad);
			}

			std::string tempButton = "Remove Light " + std::to_string(i);
			if (ImGui::Button(tempButton.c_str())) {
				DeleteEntity(*it);
				it = m_Lights.erase(it);
			}

			i++;
			it++;
		}

		//if there are less than 16 lights, give a button that allows the user to add a new light
		if (i < 15) {
			if (ImGui::Button("Add New Light")) {
				m_Lights.push_back(CreateEntity());

				TTN_Transform newTrans = TTN_Transform();
				TTN_Light newLight = TTN_Light();

				AttachCopy(m_Lights[m_Lights.size() - 1], newTrans);
				AttachCopy(m_Lights[m_Lights.size() - 1], newLight);
			}
		}
	}

	if (ImGui::CollapsingHeader("Effect Controls")) {
		//Lighting controls
		//size of the outline
		if (ImGui::SliderFloat("Outline Size", &m_outlineSize, 0.0f, 1.0f)) {
			//set the size of the outline in the materials
			for (int i = 0; i < m_mats.size(); i++)
				m_mats[i]->SetOutlineSize(m_outlineSize);
		}

		//No ligthing
		if (ImGui::Checkbox("No Lighting", &m_noLighting)) {
			//set no lighting to true
			m_noLighting = true;
			//change all the other lighting settings to false
			m_ambientOnly = false;
			m_specularOnly = false;
			m_ambientAndSpecular = false;
			m_ambientSpecularAndOutline = false;

			//set that data in the materials
			for (int i = 0; i < m_mats.size(); i++) {
				m_mats[i]->SetHasAmbient(false);
				m_mats[i]->SetHasSpecular(false);
				m_mats[i]->SetHasOutline(false);
			}
		}

		//Ambient only
		if (ImGui::Checkbox("Ambient Lighting Only", &m_ambientOnly)) {
			//set ambient only to true
			m_ambientOnly = true;
			//change all the other lighting settings to false
			m_noLighting = false;
			m_specularOnly = false;
			m_ambientAndSpecular = false;
			m_ambientSpecularAndOutline = false;

			//set that data in the materials
			for (int i = 0; i < m_mats.size(); i++) {
				m_mats[i]->SetHasAmbient(true);
				m_mats[i]->SetHasSpecular(false);
				m_mats[i]->SetHasOutline(false);
			}
		}

		//Specular only
		if (ImGui::Checkbox("Specular Lighting Only", &m_specularOnly)) {
			//set Specular only to true
			m_specularOnly = true;
			//change all the other lighting settings to false
			m_noLighting = false;
			m_ambientOnly = false;
			m_ambientAndSpecular = false;
			m_ambientSpecularAndOutline = false;

			//set that data in the materials
			for (int i = 0; i < m_mats.size(); i++) {
				m_mats[i]->SetHasAmbient(false);
				m_mats[i]->SetHasSpecular(true);
				m_mats[i]->SetHasOutline(false);
			}
		}

		//Ambient and specular
		if (ImGui::Checkbox("Ambient and Specular Lighting", &m_ambientAndSpecular)) {
			//set ambient and specular to true
			m_ambientAndSpecular = true;
			//change all the other lighting settings to false
			m_noLighting = false;
			m_ambientOnly = false;
			m_specularOnly = false;
			m_ambientSpecularAndOutline = false;

			//set that data in the materials
			for (int i = 0; i < m_mats.size(); i++) {
				m_mats[i]->SetHasAmbient(true);
				m_mats[i]->SetHasSpecular(true);
				m_mats[i]->SetHasOutline(false);
			}
		}

		//Ambient, specular, and lineart outline 
		if (ImGui::Checkbox("Ambient, Specular, and custom(outline) Lighting", &m_ambientSpecularAndOutline)) {
			//set ambient, specular, and outline to true
			m_ambientSpecularAndOutline = true;
			//change all the other lighting settings to false
			m_noLighting = false;
			m_ambientOnly = false;
			m_specularOnly = false;
			m_ambientAndSpecular = false;

			//set that data in the materials
			for (int i = 0; i < m_mats.size(); i++) {
				m_mats[i]->SetHasAmbient(true);
				m_mats[i]->SetHasSpecular(true);
				m_mats[i]->SetHasOutline(true);
			}
		}

		//Ramp controls

		//diffuse ramp
		if (ImGui::Checkbox("Use Diffuse Ramp", &m_useDiffuseRamp)) {
			for (int i = 0; i < m_mats.size(); i++) {
				m_mats[i]->SetUseDiffuseRamp(m_useDiffuseRamp);
			}
		}

		//specular ramp
		if (ImGui::Checkbox("Use Specular Ramp", &m_useSpecularRamp)) {
			for (int i = 0; i < m_mats.size(); i++) {
				m_mats[i]->SetUseSpecularRamp(m_useSpecularRamp);
			}
		}

		//Lut controls

		//toogles the warm color correction effect on or off
		if (ImGui::Checkbox("Warm Color Correction", &m_applyWarmLut)) {
			switch (m_applyWarmLut)
			{
			case true:
				//if it's been turned on set the effect to render
				m_colorCorrectEffect->SetShouldApply(true);
				m_colorCorrectEffect->SetCube(TTN_AssetSystem::GetLUT("Warm LUT"));
				//and make sure the cool and customs luts are set not to render
				m_applyCoolLut = false;
				m_applyCustomLut = false;
				break;
			case false:
				//if it's been turned of set the effect not to render
				m_colorCorrectEffect->SetShouldApply(false);
				break;
			}
		}

		//toogles the cool color correction effect on or off
		if (ImGui::Checkbox("Cool Color Correction", &m_applyCoolLut)) {
			switch (m_applyCoolLut)
			{
			case true:
				//if it's been turned on set the effect to render
				m_colorCorrectEffect->SetShouldApply(true);
				m_colorCorrectEffect->SetCube(TTN_AssetSystem::GetLUT("Cool LUT"));
				//and make sure the warm and customs luts are set not to render
				m_applyWarmLut = false;
				m_applyCustomLut = false;
				break;
			case false:
				m_colorCorrectEffect->SetShouldApply(false);
				break;
			}
		}

		//toogles the custom color correction effect on or off
		if (ImGui::Checkbox("Custom Color Correction", &m_applyCustomLut)) {
			switch (m_applyCustomLut)
			{
			case true:
				//if it's been turned on set the effect to render
				m_colorCorrectEffect->SetShouldApply(true);
				m_colorCorrectEffect->SetCube(TTN_AssetSystem::GetLUT("Custom LUT"));
				//and make sure the warm and cool luts are set not to render
				m_applyWarmLut = false;
				m_applyCoolLut = false;
				break;
			case false:
				m_colorCorrectEffect->SetShouldApply(false);
				break;
			}
		}

		//texture controls
		if (ImGui::Checkbox("Use Textures", &m_useTextures)) {
			for (int i = 0; i < m_mats.size(); i++) {
				m_mats[i]->SetUseAlbedo(m_useTextures);
			}
		}
	}

	ImGui::End();
}

MainMenuUI::MainMenuUI()
	: TTN_Scene()
{
	//set set the navigation bools
	shouldPlay = false;
	shouldQuit = false;
	shouldArcade = false;
}

void MainMenuUI::InitScene()
{
	//set set the navigation bools
	shouldPlay = false;
	shouldQuit = false;
	shouldArcade = false;

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
		TTN_Transform logoTrans = TTN_Transform(glm::vec3(0.0f, 360.0f, 1.0f), glm::vec3(0.0f), glm::vec3(1603.0f, 185.5f, 1.0f));
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
		if (i == 0) textTrans = TTN_Transform(glm::vec3(657.75f, -180.0f, 1.0f), glm::vec3(0.0f), glm::vec3(550.0f / 2.75f, 150.0f / 2.75f, 1.0f));
		else if (i == 1) textTrans = TTN_Transform(glm::vec3(254.75, -360.0f, 1.0f), glm::vec3(0.0f), glm::vec3(550.0f / 2.75f, 150.0f / 2.75f, 1.0f));
		else if (i == 2) textTrans = TTN_Transform(glm::vec3(-148.25, -180.0f, 1.0f), glm::vec3(0.0f), glm::vec3(550.0f / 2.75f, 150.0f / 2.75f, 1.0f));
		else if (i == 3) textTrans = TTN_Transform(glm::vec3(-551.25, -360.0f, 1.0f), glm::vec3(0.0f), glm::vec3(550.0f / 2.75f, 150.0f / 2.75f, 1.0f));
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
		if (i == 0) buttonTrans = TTN_Transform(glm::vec3(657.75f, -180.0f, 2.0f), glm::vec3(0.0f), glm::vec3(322.75f, 201.5, 1.0f));
		else if (i == 1) buttonTrans = TTN_Transform(glm::vec3(254.75, -360.0f, 2.0f), glm::vec3(0.0f), glm::vec3(322.75f, 201.5, 1.0f));
		else if (i == 2) buttonTrans = TTN_Transform(glm::vec3(-148.25, -180.0f, 2.0f), glm::vec3(0.0f), glm::vec3(322.75f, 201.5, 1.0f));
		else if (i == 3) buttonTrans = TTN_Transform(glm::vec3(-551.25, -360.0f, 2.0f), glm::vec3(0.0f), glm::vec3(322.75f, 201.5, 1.0f));
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
	if (mousePosWorldSpace.x < playButtonTrans.GetPos().x + 0.5f * abs(playButtonTrans.GetScale().x) &&
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

	ImGui::Begin("Ui Editor");

	if (ImGui::CollapsingHeader("Sprite Masking Testing")) {
		//grab a refernece ot the logo's renderer
		TTN_Renderer2D& logoRenderer = Get<TTN_Renderer2D>(gameLogo);

		float hori = logoRenderer.GetHoriMask();
		if (ImGui::SliderFloat("Horizontal Mask", &hori, 0.0f, 1.0f)) {
			logoRenderer.SetHoriMask(hori);
		}

		float vert = logoRenderer.GetVertMask();
		if (ImGui::SliderFloat("Vertical Mask", &vert, 0.0f, 1.0f)) {
			logoRenderer.SetVertMask(vert);
		}

		bool right = logoRenderer.GetHoriMaskStartsAtRight();
		if (ImGui::Checkbox("Start Hori mask on right", &right)) {
			logoRenderer.StartHoriMaskAtRight(right);
		}

		bool bottom = logoRenderer.GetVertMaskStartsAtBottom();
		if (ImGui::Checkbox("Start Vert mask at bottom", &bottom)) {
			logoRenderer.StartVertMaskAtBottom(bottom);
		}
	}

	ImGui::End();
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
			shouldPlay = true;
			shouldArcade = true;
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