//Dam Defense, by Atlas X Games
//Game.cpp, the source file for the class that represents the main gameworld scene

//import the class
#include "Game.h"
#include "glm/ext.hpp"

//default constructor
Game::Game()
	: TTN_Scene()
{
}

//sets up the scene
void Game::InitScene()
{
	//load in the scene's assets
	SetUpAssets();

	//set up the other data
	SetUpOtherData();

	//create the entities
	SetUpEntities();
}

//updates the scene every frame
void Game::Update(float deltaTime)
{
	if (!m_paused) {
		//allow the player to rotate
		PlayerRotate(deltaTime);

		//switch to the cannon's normal static animation if it's firing animation has ended
		StopFiring();

		//delete any cannonballs that're way out of range
		DeleteCannonballs();

		//if the player is on shoot cooldown, decrement the time remaining on the cooldown
		if (playerShootCooldownTimer >= 0.0f) playerShootCooldownTimer -= deltaTime;

		//update the enemy wave spawning
		WaveUpdate(deltaTime);

		//goes through the boats vector
		for (int i = 0; i < boats.size(); i++) {
			//sets gravity to 0
			Get<TTN_Physics>(boats[i]).GetRigidBody()->setGravity(btVector3(0.0f, 0.0f, 0.0f));
		}

		//go through all the entities with enemy compontents
		auto enemyView = GetScene()->view<EnemyComponent>();
		for (auto entity : enemyView) {
			//and run their update
			Get<EnemyComponent>(entity).Update(deltaTime);
		}

		//updates the flamethrower logic
		FlamethrowerUpdate(deltaTime);

		Collisions(); //collision check
		Damage(deltaTime); //damage function, contains cooldoown

		BirdUpate(deltaTime);

		//increase the total time of the scene to make the water animated correctly
		water_time += deltaTime;
	}

	//game over stuff
	if (Dam_health <= 0.0f) {
		m_gameOver = true;
		printf("GAME OVER");
	}

	TTN_AudioEvent& music = engine.GetEvent("music");

	//get ref to bus
	TTN_AudioBus& musicBus = engine.GetBus("MusicBus");

	//get ref to listener
	TTN_AudioListener& listener = engine.GetListener();
	engine.Update();

	//call the update on ImGui
	ImGui();

	//don't forget to call the base class' update
	TTN_Scene::Update(deltaTime);
}

//render the terrain and water
void Game::PostRender()
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
		shaderProgramWater->SetUniform("time", water_time);
		shaderProgramWater->SetUniform("speed", water_waveSpeed);
		shaderProgramWater->SetUniform("baseHeight", water_waveBaseHeightIncrease);
		shaderProgramWater->SetUniform("heightMultiplier", water_waveHeightMultiplier);
		shaderProgramWater->SetUniform("waveLenghtMultiplier", water_waveLenghtMultiplier);

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

#pragma region INPUTS
//function to use to check for when a key is being pressed down for the first frame
void Game::KeyDownChecks()
{
	//if the game is not paused
	if (!m_paused) {
		//and they press the 2 key, try to activate the flamethrower
		if (TTN_Application::TTN_Input::GetKeyDown(TTN_KeyCode::Two)) {
			Flamethrower();
		}
	}

	//if they try to press the escape key, pause or unpause the game
	if (TTN_Application::TTN_Input::GetKeyDown(TTN_KeyCode::Esc)) {
		m_paused = !m_paused;
		TTN_Scene::SetPaused(m_paused);
	}

	//just some temp controls to let us access the mouse for ImGUI, remeber to remove these in the final game
	if (TTN_Application::TTN_Input::GetKeyDown(TTN_KeyCode::P)) {
		TTN_Application::TTN_Input::SetCursorLocked(true);
	}

	if (TTN_Application::TTN_Input::GetKeyDown(TTN_KeyCode::O)) {
		TTN_Application::TTN_Input::SetCursorLocked(false);
	}
}

//function to cehck for when a key is being pressed
void Game::KeyChecks()
{
	auto& a = Get<TTN_Transform>(camera);
	/// CAMERA MOVEMENT FOR A2 ///
	if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::W)) {
		a.SetPos(glm::vec3(a.GetPos().x, a.GetPos().y, a.GetPos().z + 2.0f));
	}

	if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::S)) {
		a.SetPos(glm::vec3(a.GetPos().x, a.GetPos().y, a.GetPos().z - 2.0f));
	}

	if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::A)) {
		a.SetPos(glm::vec3(a.GetPos().x + 2.0f, a.GetPos().y, a.GetPos().z));
	}
	if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::D)) {
		a.SetPos(glm::vec3(a.GetPos().x - 2.0f, a.GetPos().y, a.GetPos().z));
	}

	if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::LeftControl)) {
		a.SetPos(glm::vec3(a.GetPos().x - 2.0f, a.GetPos().y - 2.0f, a.GetPos().z));
	}
	if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::Space)) {
		a.SetPos(glm::vec3(a.GetPos().x - 2.0f, a.GetPos().y + 2.0f, a.GetPos().z));
	}

}

//function to check for when a key has been released
void Game::KeyUpChecks()
{
}

//function to check for when a mouse button has been pressed down for the first frame
void Game::MouseButtonDownChecks()
{
}

//function to check for when a mouse button is being pressed
void Game::MouseButtonChecks()
{
	//if the game is not paused
	if (!m_paused) {
		//if the cannon is not in the middle of firing, fire when the player is pressing the left mouse button
		if (Get<TTN_MorphAnimator>(cannon).getActiveAnim() == 0 && playerShootCooldownTimer <= 0.0f &&
			TTN_Application::TTN_Input::GetMouseButton(TTN_MouseButton::Left)) {
			//play the firing animation
			Get<TTN_MorphAnimator>(cannon).SetActiveAnim(1);
			Get<TTN_MorphAnimator>(cannon).getActiveAnimRef().Restart();
			//create a new cannonball
			CreateCannonball();
			//reset the cooldown
			playerShootCooldownTimer = playerShootCooldown;
			//and play the smoke particle effect
			Get<TTN_Transform>(smokePS).SetPos(glm::vec3(0.0f, -0.2f, 0.0f) + 1.5f * playerDir);
			Get<TTN_ParticeSystemComponent>(smokePS).GetParticleSystemPointer()->
				SetEmitterRotation(glm::vec3(rotAmmount.y, -rotAmmount.x, 0.0f));
			Get<TTN_ParticeSystemComponent>(smokePS).GetParticleSystemPointer()->Burst(500);
		}
	}
}

//function to check for when a mouse button has been released
void Game::MouseButtonUpChecks()
{
}
#pragma endregion

#pragma region SetUP STUFF
//sets up all the assets in the scene
void Game::SetUpAssets()
{
	//// SOUNDS ////
	engine.LoadBank("Master");
	engine.LoadBus("MusicBus", "{a5b53ded-d7b3-4e6b-a920-0b241ef6f268}");

	TTN_AudioEvent& music = engine.CreateEvent("music", "{b56cb9d2-1d47-4099-b80e-7d257b99a823}");
	music.Play();

	//// SHADERS ////
#pragma region SHADERS
	//grab the shaders
	shaderProgramTextured = TTN_AssetSystem::GetShader("Basic textured shader");
	shaderProgramSkybox = TTN_AssetSystem::GetShader("Skybox shader");
	shaderProgramTerrain = TTN_AssetSystem::GetShader("Terrain shader");
	shaderProgramWater = TTN_AssetSystem::GetShader("Water shader");
	shaderProgramAnimatedTextured = TTN_AssetSystem::GetShader("Animated textured shader");

#pragma endregion

	////MESHES////
	cannonMesh = TTN_ObjLoader::LoadAnimatedMeshFromFiles("models/cannon/cannon", 7);
	skyboxMesh = TTN_ObjLoader::LoadFromFile("models/SkyboxMesh.obj");
	sphereMesh = TTN_ObjLoader::LoadFromFile("models/IcoSphereMesh.obj");
	flamethrowerMesh = TTN_ObjLoader::LoadFromFile("models/Flamethrower.obj");
	flamethrowerMesh->SetUpVao();
	boat1Mesh = TTN_ObjLoader::LoadFromFile("models/Boat 1.obj");
	boat2Mesh = TTN_ObjLoader::LoadFromFile("models/Boat 2.obj");
	boat3Mesh = TTN_ObjLoader::LoadFromFile("models/Boat 3.obj");
	terrainPlain = TTN_ObjLoader::LoadFromFile("models/terrainPlain.obj");
	terrainPlain->SetUpVao();
	birdMesh = TTN_ObjLoader::LoadAnimatedMeshFromFiles("models/bird/bird", 2);
	treeMesh[0] = TTN_ObjLoader::LoadFromFile("models/Tree1.obj");
	treeMesh[1] = TTN_ObjLoader::LoadFromFile("models/Tree2.obj");
	treeMesh[2] = TTN_ObjLoader::LoadFromFile("models/Tree3.obj");
	damMesh = TTN_ObjLoader::LoadFromFile("models/Dam.obj");

	//grab the meshes
	cannonMesh = TTN_AssetSystem::GetMesh("Cannon mesh");
	skyboxMesh = TTN_AssetSystem::GetMesh("Skybox mesh");
	sphereMesh = TTN_AssetSystem::GetMesh("Sphere");
	flamethrowerMesh = TTN_AssetSystem::GetMesh("Flamethrower mesh");
	boat1Mesh = TTN_AssetSystem::GetMesh("Boat 1");
	boat2Mesh = TTN_AssetSystem::GetMesh("Boat 2");
	boat3Mesh = TTN_AssetSystem::GetMesh("Boat 3");
	terrainPlain = TTN_AssetSystem::GetMesh("Terrain plane");
	birdMesh = TTN_AssetSystem::GetMesh("Bird mesh");
	damMesh = TTN_AssetSystem::GetMesh("Dam mesh");

	///TEXTURES////
	cannonText = TTN_Texture2D::LoadFromFile("textures/metal.png");
	skyboxText = TTN_TextureCubeMap::LoadFromImages("textures/skybox/sky.png");
	terrainMap = TTN_Texture2D::LoadFromFile("textures/Game Map Long.jpg");
	sandText = TTN_Texture2D::LoadFromFile("textures/SandTexture.jpg");
	rockText = TTN_Texture2D::LoadFromFile("textures/RockTexture.jpg");
	grassText = TTN_Texture2D::LoadFromFile("textures/GrassTexture.jpg");
	waterText = TTN_Texture2D::LoadFromFile("textures/water.png");
	boat1Text = TTN_Texture2D::LoadFromFile("textures/Boat 1 Texture.png");
	boat2Text = TTN_Texture2D::LoadFromFile("textures/Boat 2 Texture.png");
	boat3Text = TTN_Texture2D::LoadFromFile("textures/Boat 3 Texture.png");
	flamethrowerText = TTN_Texture2D::LoadFromFile("textures/FlamethrowerTexture.png");
	birdText = TTN_Texture2D::LoadFromFile("textures/BirdTexture.png");
	treeText = TTN_Texture2D::LoadFromFile("textures/Trees Texture.png");
	damText = TTN_Texture2D::LoadFromFile("textures/Dam.png");

	healthBar = TTN_Texture2D::LoadFromFile("textures/health.png");
	//grab textures
	cannonText = TTN_AssetSystem::GetTexture2D("Cannon texture");
	skyboxText = TTN_AssetSystem::GetSkybox("Skybox texture");
	terrainMap = TTN_AssetSystem::GetTexture2D("Terrain height map");
	sandText = TTN_AssetSystem::GetTexture2D("Sand texture");
	rockText = TTN_AssetSystem::GetTexture2D("Rock texture");
	grassText = TTN_AssetSystem::GetTexture2D("Grass texture");
	waterText = TTN_AssetSystem::GetTexture2D("Water texture");
	boat1Text = TTN_AssetSystem::GetTexture2D("Boat texture 1");
	boat2Text = TTN_AssetSystem::GetTexture2D("Boat texture 2");
	boat3Text = TTN_AssetSystem::GetTexture2D("Boat texture 3");
	flamethrowerText = TTN_AssetSystem::GetTexture2D("Flamethrower texture");
	birdText = TTN_AssetSystem::GetTexture2D("Bird texture");
	damText = TTN_AssetSystem::GetTexture2D("Dam texture");

	////MATERIALS////
	cannonMat = TTN_Material::Create();
	cannonMat->SetAlbedo(cannonText);
	cannonMat->SetShininess(128.0f);
	m_mats.push_back(cannonMat);

	boat1Mat = TTN_Material::Create();
	boat1Mat->SetAlbedo(boat1Text);
	boat1Mat->SetShininess(128.0f);
	m_mats.push_back(boat1Mat);
	boat2Mat = TTN_Material::Create();
	boat2Mat->SetAlbedo(boat2Text);
	boat2Mat->SetShininess(128.0f);
	m_mats.push_back(boat2Mat);
	boat3Mat = TTN_Material::Create();
	boat3Mat->SetAlbedo(boat3Text);
	boat3Mat->SetShininess(128.0f);
	m_mats.push_back(boat3Mat);

	flamethrowerMat = TTN_Material::Create();
	flamethrowerMat->SetAlbedo(flamethrowerText);
	flamethrowerMat->SetShininess(128.0f);
	m_mats.push_back(flamethrowerMat);

	skyboxMat = TTN_Material::Create();
	skyboxMat->SetSkybox(skyboxText);
	smokeMat = TTN_Material::Create();
	smokeMat->SetAlbedo(nullptr); //do this to be sure titan uses it's default white texture for the particle

	fireMat = TTN_Material::Create();
	fireMat->SetAlbedo(nullptr); //do this to be sure titan uses it's default white texture for the particle

	birdMat = TTN_Material::Create();
	birdMat->SetAlbedo(birdText);
	m_mats.push_back(birdMat);

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

//create the scene's initial entities
void Game::SetUpEntities()
{
	//entity for the camera
	{
		//create an entity in the scene for the camera
		camera = CreateEntity();
		SetCamEntity(camera);
		Attach<TTN_Transform>(camera);
		Attach<TTN_Camera>(camera);
		auto& camTrans = Get<TTN_Transform>(camera);
		camTrans.SetPos(glm::vec3(0.0f, 0.0f, 0.0f));
		camTrans.SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
		camTrans.LookAlong(glm::vec3(0.0, 0.0, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
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

			//attach that transform to the entity
			AttachCopy<TTN_Transform>(flamethrowers[i], ftTrans);
		}
	}

	//entity for the smoke particle system (rather than recreating whenever we need it, we'll just make one
	//and burst again when we need to)
	{
		smokePS = CreateEntity();

		//setup a transfrom for the particle system
		TTN_Transform smokePSTrans = TTN_Transform(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f));
		//attach that transform to the entity
		AttachCopy(smokePS, smokePSTrans);

		//setup a particle system for the particle system
		TTN_ParticleSystem::spsptr ps = std::make_shared<TTN_ParticleSystem>(5000, 0, smokeParticle, 0.0f, false);
		ps->MakeCircleEmitter(glm::vec3(0.0f));
		ps->VelocityReadGraphCallback(FastStart);
		ps->ColorReadGraphCallback(SlowStart);
		//setup a particle system component
		TTN_ParticeSystemComponent psComponent = TTN_ParticeSystemComponent(ps);
		//attach the particle system component to the entity
		AttachCopy(smokePS, psComponent);
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

	//birds
	for (int i = 0; i < 3; i++) {
		birds[i] = CreateEntity();

		//create a renderer

		TTN_Renderer birdRenderer = TTN_Renderer(birdMesh, shaderProgramAnimatedTextured, birdMat);

		//attach that renderer to the entity

		AttachCopy(birds[i], birdRenderer);

		//create an animator

		TTN_MorphAnimator birdAnimator = TTN_MorphAnimator();

		//create an animation for the bird flying
		TTN_MorphAnimation flyingAnim = TTN_MorphAnimation({ 0, 1 }, { 10.0f / 24.0f, 10.0f / 24.0f }, true); //anim 0
		birdAnimator.AddAnim(flyingAnim);
		birdAnimator.SetActiveAnim(0);

		//attach that animator to the entity

		AttachCopy(birds[i], birdAnimator);

		//create a transform
		TTN_Transform birdTrans = TTN_Transform(birdBase, glm::vec3(0.0f), glm::vec3(1.0f));
		if (i == 1) birdTrans.SetPos(birdBase + glm::vec3(3.0f, -3.0f, 3.0f));
		if (i == 2) birdTrans.SetPos(birdBase + glm::vec3(-3.0f, -3.0f, -3.0f));
		birdTrans.RotateFixed(glm::vec3(0.0f, -45.0f + 180.0f, 0.0f));

		//attach that transform to the entity
		AttachCopy(birds[i], birdTrans);
	}

	//prepare the vector of cannonballs
	cannonBalls = std::vector<entt::entity>();
	//vector of boats
	boats = std::vector<entt::entity>();

	//vector for flamethrower models and flame particles
	flames = std::vector<entt::entity>();

	//set the cannon to be a child of the camera
	Get<TTN_Transform>(cannon).SetParent(&Get<TTN_Transform>(camera), &camera);
}

//sets up any other data the game needs to store
void Game::SetUpOtherData()
{
	//call the restart data function
	RestartData();

	//create the particle templates
	//smoke particle
	{
		smokeParticle = TTN_ParticleTemplate();
		smokeParticle.SetMat(smokeMat);
		smokeParticle.SetMesh(sphereMesh);
		smokeParticle.SetTwoLifetimes((playerShootCooldown - 0.1f), playerShootCooldown);
		smokeParticle.SetOneStartColor(glm::vec4(0.1f, 0.1f, 0.1f, 0.8f));
		smokeParticle.SetOneEndColor(glm::vec4(0.5f, 0.5f, 0.5f, 0.1f));
		smokeParticle.SetOneStartSize(0.05f);
		smokeParticle.SetOneEndSize(0.05f);
		smokeParticle.SetTwoStartSpeeds(1.5f, 1.0f);
		smokeParticle.SetOneEndSpeed(0.05f);
	}

	//fire particle template
	{
		fireParticle = TTN_ParticleTemplate();
		fireParticle.SetMat(fireMat);
		fireParticle.SetMesh(sphereMesh);
		fireParticle.SetOneEndColor(glm::vec4(1.0f, 0.2f, 0.0f, 0.0f));
		fireParticle.SetOneEndSize(4.0f);
		fireParticle.SetOneEndSpeed(6.0f);
		fireParticle.SetOneLifetime(2.0f);
		fireParticle.SetTwoStartColors(glm::vec4(1.0f, 0.35f, 0.0f, 1.0f), glm::vec4(1.0f, 0.60f, 0.0f, 1.0f));
		fireParticle.SetOneStartSize(0.5f);
		fireParticle.SetOneStartSpeed(8.5f);
	}

	//expolsion particle template
	{
		expolsionParticle = TTN_ParticleTemplate();
		expolsionParticle.SetMat(fireMat);
		expolsionParticle.SetMesh(sphereMesh);
		expolsionParticle.SetTwoEndColors(glm::vec4(0.5f, 0.1f, 0.0f, 0.2f), glm::vec4(0.8f, 0.3f, 0.0f, 0.2f));
		expolsionParticle.SetOneEndSize(4.5f);
		expolsionParticle.SetOneEndSpeed(0.05f);
		expolsionParticle.SetTwoLifetimes(1.8f, 2.0f);
		expolsionParticle.SetTwoStartColors(glm::vec4(1.0f, 0.35f, 0.0f, 1.0f), glm::vec4(1.0f, 0.60f, 0.0f, 1.0f));
		expolsionParticle.SetOneStartSize(1.0f);
		expolsionParticle.SetOneStartSpeed(4.5f);
	}

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

//restarts the game
void Game::RestartData()
{
	//player data
	rotAmmount = glm::vec2(0.0f);
	mousePos = TTN_Application::TTN_Input::GetMousePosition();
	playerDir = glm::vec3(0.0f, 0.0f, 1.0f);
	playerShootCooldownTimer = playerShootCooldown;

	//water and terrain data setup
	water_time = 0.0f;
	water_waveSpeed = -2.5f;
	water_waveBaseHeightIncrease = 0.0f;
	water_waveHeightMultiplier = 0.005f;
	water_waveLenghtMultiplier = -10.0f;

	//dam and flamethrower data setup
	Flaming = false;
	FlameTimer = 0.0f;
	FlameAnim = 0.0f;
	Dam_health = Dam_MaxHealth;

	//bird data setup
	birdTimer = 0.0f;

	//scene data steup
	TTN_Scene::SetGravity(glm::vec3(0.0f, -9.8f, 0.0f));
	m_paused = false;
	m_gameOver = false;
	gameWin = false;

	//enemy and wave data setup
	m_currentWave = 1;
	m_timeTilNextWave = m_timeBetweenEnemyWaves;
	m_timeUntilNextSpawn = m_timeBetweenEnemySpawns;
	m_boatsRemainingThisWave = m_enemiesPerWave;
	m_boatsStillNeedingToSpawnThisWave = m_boatsRemainingThisWave;
	m_rightSideSpawn = (bool)(rand() % 2);
}

#pragma endregion

#pragma region Player and CANNON Stuff
//called by update once a frame, allows the player to rotate
void Game::PlayerRotate(float deltaTime)
{
	//get the mouse position
	glm::vec2 tempMousePos = TTN_Application::TTN_Input::GetMousePosition();

	//figure out how much the cannon and camera should be rotated
	rotAmmount += (tempMousePos - mousePos) * 5.0f * deltaTime;

	//clamp the rotation to within 85 degrees of the base rotation in all the directions
	if (rotAmmount.x > 85.0f) rotAmmount.x = 85.0f;
	else if (rotAmmount.x < -85.0f) rotAmmount.x = -85.0f;
	if (rotAmmount.y > 85.0f) rotAmmount.y = 85.0f;
	else if (rotAmmount.y < -85.0f) rotAmmount.y = -85.0f;

	//reset the rotation
	Get<TTN_Transform>(camera).LookAlong(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//and rotate it by the ammount it should be rotated
	Get<TTN_Transform>(camera).RotateFixed(glm::vec3(rotAmmount.y, -rotAmmount.x, 0.0f));
	//clear the direction the player is facing, and rotate it to face the same along
	playerDir = glm::vec3(0.0f, 0.0f, 1.0f);
	playerDir = glm::vec3(glm::toMat4(glm::quat(glm::radians(glm::vec3(rotAmmount.y, -rotAmmount.x, 0.0f)))) * glm::vec4(playerDir, 1.0f));
	playerDir = glm::normalize(playerDir);

	//save the next position to rotate properly next frame
	mousePos = tempMousePos;
}

//called by update, makes the cannon switch back to it's not firing animation when it's firing animation has ended
void Game::StopFiring()
{
	if (Get<TTN_MorphAnimator>(cannon).getActiveAnim() == 1 &&
		Get<TTN_MorphAnimator>(cannon).getActiveAnimRef().getIsDone()) {
		Get<TTN_MorphAnimator>(cannon).SetActiveAnim(0);
	}
}

//function to create a cannonball, used when the player fires
void Game::CreateCannonball()
{
	//create the cannonball
	{
		//create the entity
		cannonBalls.push_back(CreateEntity());

		//set up a renderer for the cannonball
		TTN_Renderer cannonBallRenderer = TTN_Renderer(sphereMesh, shaderProgramTextured, cannonMat);
		//attach that renderer to the entity
		AttachCopy(cannonBalls[cannonBalls.size() - 1], cannonBallRenderer);

		//set up a transform for the cannonball
		TTN_Transform cannonBallTrans = TTN_Transform();
		cannonBallTrans.SetPos(Get<TTN_Transform>(cannon).GetGlobalPos());
		cannonBallTrans.SetScale(glm::vec3(0.35f));
		//attach that transform to the entity
		AttachCopy(cannonBalls[cannonBalls.size() - 1], cannonBallTrans);

		//set up a physics body for the cannonball
		TTN_Physics cannonBallPhysBod = TTN_Physics(cannonBallTrans.GetPos(), glm::vec3(0.0f), cannonBallTrans.GetScale(),
			cannonBalls[cannonBalls.size() - 1]);

		//attach that physics body to the entity
		AttachCopy(cannonBalls[cannonBalls.size() - 1], cannonBallPhysBod);

		TTN_Tag ballTag = TTN_Tag("Ball"); //sets boat path number to ttn_tag
		AttachCopy<TTN_Tag>(cannonBalls[cannonBalls.size() - 1], ballTag);
	}
	//TTN_Physics& tt = Get<TTN_Physics>(cannonBalls[cannonBalls.size() - 1]);

	//after the cannonball has been created, get the physics body and apply a force along the player's direction
	Get<TTN_Physics>(cannonBalls[cannonBalls.size() - 1]).AddForce((cannonBallForce * playerDir));
}

//function that will check the positions of the cannonballs each frame and delete any that're too low
void Game::DeleteCannonballs()
{
	//iterate through the vector of cannonballs, deleting the cannonball if it is at or below y = -50
	std::vector<entt::entity>::iterator it = cannonBalls.begin();
	while (it != cannonBalls.end()) {
		if (Get<TTN_Transform>(*it).GetGlobalPos().y > -40.0f) {
			it++;
		}
		else {
			DeleteEntity(*it);
			it = cannonBalls.erase(it);
		}
	}
}

//function that will create an expolsion particle effect at a given input location
void Game::CreateExpolsion(glm::vec3 location)
{
	//we don't really need to save the entity number for any reason, so we just make the variable local
	entt::entity newExpolsion = CreateEntity(2.0f);

	//setup a transfrom for the particle system
	TTN_Transform PSTrans = TTN_Transform(location, glm::vec3(0.0f), glm::vec3(1.0f));
	//attach that transform to the entity
	AttachCopy(newExpolsion, PSTrans);
	glm::vec3 tempLoc = Get<TTN_Transform>(newExpolsion).GetGlobalPos();

	//setup a particle system for the particle system
	TTN_ParticleSystem::spsptr ps = std::make_shared<TTN_ParticleSystem>(500, 0, expolsionParticle, 0.0f, false);
	ps->MakeSphereEmitter();
	ps->VelocityReadGraphCallback(FastStart);
	ps->ColorReadGraphCallback(SlowStart);
	ps->ScaleReadGraphCallback(ZeroOneZero);
	//setup a particle system component
	TTN_ParticeSystemComponent psComponent = TTN_ParticeSystemComponent(ps);
	//attach the particle system component to the entity
	AttachCopy(newExpolsion, psComponent);

	//get a reference to that particle system and burst it
	Get<TTN_ParticeSystemComponent>(newExpolsion).GetParticleSystemPointer()->Burst(500);
}

//creates the flames for the flamethrower
void Game::Flamethrower() {
	//if the cooldown has ended
	if (FlameTimer <= 0.0f) {
		//reset cooldown
		FlameTimer = FlameThrowerCoolDown;
		//set the active flag to true
		Flaming = true;
		//and through and create the fire particle systems
		for (int i = 0; i < 6; i++) {
			//fire particle entities
			{
				flames.push_back(CreateEntity(3.0f));

				//setup a transfrom for the particle system
				TTN_Transform firePSTrans = TTN_Transform(Get<TTN_Transform>(flamethrowers[i]).GetGlobalPos() + glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(1.0f));

				//attach that transform to the entity
				AttachCopy(flames[i], firePSTrans);

				//setup a particle system for the particle system
				TTN_ParticleSystem::spsptr ps = std::make_shared<TTN_ParticleSystem>(1200, 300, fireParticle, 2.0f, true);
				ps->MakeConeEmitter(15.0f, glm::vec3(90.0f, 0.0f, 0.0f));

				//setup a particle system component
				TTN_ParticeSystemComponent psComponent = TTN_ParticeSystemComponent(ps);
				//attach the particle system component to the entity
				AttachCopy(flames[i], psComponent);
			}
		}
	}
	//otherwise nothing happens
	else {
		Flaming = false;
	}
}

//function to update the flamethrower logic
void Game::FlamethrowerUpdate(float deltaTime)
{
	//reduce the cooldown timer on the flamethrower
	FlameTimer -= deltaTime;

	//if the flamethrowers are active
	if (Flaming) {
		//increment flamethrower anim timer
		FlameAnim += deltaTime;

		//if it's reached the end of the animation
		if (FlameAnim >= FlameActiceTime) {
			//get rid of all the flames, reset the timer and set the active flag to false
			flames.clear();
			FlameAnim = 0.0f;
			Flaming = false;
		}

		//while it's flaming, iterate through the vector of boats, deleting the boat if it is at or below z = 20
		std::vector<entt::entity>::iterator it = boats.begin();
		while (it != boats.end()) {
			if (Get<TTN_Transform>(*it).GetPos().z >= 27.0f) {
				it++;
			}
			else {
				m_boatsRemainingThisWave--;
				DeleteEntity(*it);
				it = boats.erase(it);
			}
		}
	}
}
#pragma endregion

#pragma region Enemy spawning and wave stuff
//spawn a boat on the left side of the map
void Game::SpawnBoatLeft()
{
	//create the entity
	boats.push_back(CreateEntity());
	int randomBoat = rand() % 3;

	//create a renderer
	TTN_Renderer boatRenderer = TTN_Renderer(boat1Mesh, shaderProgramTextured, boat1Mat);
	//setup renderer for green boat
	if (randomBoat == 0) {
		boatRenderer = TTN_Renderer(boat1Mesh, shaderProgramTextured, boat1Mat);
	}
	//setup renderer for red boat
	else if (randomBoat == 1) {
		boatRenderer = TTN_Renderer(boat2Mesh, shaderProgramTextured, boat2Mat);
	}
	//set up renderer for yellow boat
	else if (randomBoat == 2) {
		boatRenderer = TTN_Renderer(boat3Mesh, shaderProgramTextured, boat3Mat);
	}
	//attach the renderer to the boat
	AttachCopy<TTN_Renderer>(boats[boats.size() - 1], boatRenderer);

	//create a transform for the boat
	TTN_Transform boatTrans = TTN_Transform(glm::vec3(21.0f, 10.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f));
	//set up the transform for the green boat
	if (randomBoat == 0) {
		boatTrans.RotateFixed(glm::vec3(0.0f, 180.0f, 0.0f));
		boatTrans.SetScale(glm::vec3(0.25f, 0.25f, 0.25f));
		boatTrans.SetPos(glm::vec3(90.0f, -8.5f, 115.0f));
	}
	//setup transform for the red boat
	else if (randomBoat == 1) {
		boatTrans.RotateFixed(glm::vec3(0.0f, -90.0f, 0.0f));
		boatTrans.SetScale(glm::vec3(0.05f, 0.05f, 0.05f));
		boatTrans.SetPos(glm::vec3(90.0f, -8.0f, 115.0f));
	}
	//set up transform for the yellow boat
	else if (randomBoat == 2) {
		boatTrans.RotateFixed(glm::vec3(0.0f, 90.0f, 0.0f));
		boatTrans.SetScale(glm::vec3(0.15f, 0.15f, 0.15f));
		boatTrans.SetPos(glm::vec3(90.0f, -7.5f, 115.0f));
	}
	//attach the transform
	AttachCopy<TTN_Transform>(boats[boats.size() - 1], boatTrans);

	//create an attach a transform
	TTN_Physics pbody = TTN_Physics(boatTrans.GetPos(), glm::vec3(0.0f), glm::vec3(2.0f, 4.0f, 8.95f), boats[boats.size() - 1], TTN_PhysicsBodyType::DYNAMIC);
	pbody.SetLinearVelocity(glm::vec3(-25.0f, 0.0f, 0.0f));//-2.0f
	AttachCopy<TTN_Physics>(boats[boats.size() - 1], pbody);

	//creates and attaches a tag to the boat
	TTN_Tag boatTag = TTN_Tag("Boat");
	AttachCopy<TTN_Tag>(boats[boats.size() - 1], boatTag);

	//create and attach the enemy component to the boat
	int randPath = rand() % 3; // generates path number between 0-2 (left side paths, right side path nums are 3-5)
	EnemyComponent en = EnemyComponent(boats[boats.size() - 1], this, randomBoat, randPath, 0.0f);
	AttachCopy(boats[boats.size() - 1], en);
}

//spawn a boat on the right side of the map
void Game::SpawnBoatRight()
{
	boats.push_back(CreateEntity());

	//gets the type of boat
	int randomBoat = rand() % 3;

	//create a renderer
	TTN_Renderer boatRenderer = TTN_Renderer(boat1Mesh, shaderProgramTextured, boat1Mat);
	//set up renderer for green boat
	if (randomBoat == 0) {
		boatRenderer = TTN_Renderer(boat1Mesh, shaderProgramTextured, boat1Mat);
	}
	//set up renderer for red boat
	else if (randomBoat == 1) {
		boatRenderer = TTN_Renderer(boat2Mesh, shaderProgramTextured, boat2Mat);
	}
	//set up renderer for yellow boat
	else if (randomBoat == 2) {
		boatRenderer = TTN_Renderer(boat3Mesh, shaderProgramTextured, boat3Mat);
	}
	//attach the renderer to the entity
	AttachCopy<TTN_Renderer>(boats[boats.size() - 1], boatRenderer);

	//create a transform for the boat
	TTN_Transform boatTrans = TTN_Transform();
	//set up the transform for the green boat
	if (randomBoat == 0) {
		boatTrans.RotateFixed(glm::vec3(0.0f, 0.0f, 0.0f));
		boatTrans.SetScale(glm::vec3(0.25f, 0.25f, 0.25f));
		boatTrans.SetPos(glm::vec3(-90.0f, -8.5f, 115.0f));
	}
	//set up the transform for the red boat
	else if (randomBoat == 1) {
		boatTrans.RotateFixed(glm::vec3(0.0f, 90.0f, 0.0f));
		boatTrans.SetScale(glm::vec3(0.05f, 0.05f, 0.05f));
		boatTrans.SetPos(glm::vec3(-90.0f, -8.0f, 115.0f));
	}
	//set up the transform the yellow boat
	else if (randomBoat == 2) {
		boatTrans.RotateFixed(glm::vec3(0.0f, -90.0f, 0.0f));
		boatTrans.SetScale(glm::vec3(0.15f, 0.15f, 0.15f));
		boatTrans.SetPos(glm::vec3(-90.0f, -7.5f, 115.0f));
	}
	//attach the transform
	AttachCopy<TTN_Transform>(boats[boats.size() - 1], boatTrans);

	//create and attach a physics body to the boats
	TTN_Physics pbody = TTN_Physics(boatTrans.GetPos(), glm::vec3(0.0f), glm::vec3(2.0f, 4.0f, 8.95f), boats[boats.size() - 1]);
	pbody.SetLinearVelocity(glm::vec3(25.0f, 0.0f, 0.0f));//-2.0f
	AttachCopy<TTN_Physics>(boats[boats.size() - 1], pbody);

	//creates and attaches a tag to the boat
	TTN_Tag boatTag = TTN_Tag("Boat");
	AttachCopy<TTN_Tag>(boats[boats.size() - 1], boatTag);

	//create and attach the enemy component to the boat
	int randPath = rand() % 3 + 3; // generates path number between 3-5 (right side paths, left side path nums are 0-2)
	EnemyComponent en = EnemyComponent(boats[boats.size() - 1], this, randomBoat, randPath, 0.0f);
	AttachCopy(boats[boats.size() - 1], en);
}

//updates the waves
void Game::WaveUpdate(float deltaTime)
{
	//if there are no more boats in this wave, begin the countdown to the next wave
	if (m_boatsRemainingThisWave == 0 && m_timeTilNextWave <= 0.0f) {
		m_timeTilNextWave = m_timeBetweenEnemyWaves;
		m_currentWave++;
		m_boatsRemainingThisWave = m_enemiesPerWave * m_currentWave;
		m_boatsStillNeedingToSpawnThisWave = m_boatsRemainingThisWave;
		m_timeUntilNextSpawn = m_timeBetweenEnemySpawns;
	}

	//if it is in the cooldown between waves, reduce the cooldown by deltaTime
	if (m_timeTilNextWave >= 0.0f) {
		m_timeTilNextWave -= deltaTime;
	}
	//otherwise, check if it should spawn
	else {
		m_timeUntilNextSpawn -= deltaTime;
		//if it's time for the next enemy spawn
		if (m_timeUntilNextSpawn <= 0.0f && m_boatsStillNeedingToSpawnThisWave > 0) {
			//then spawn a new enemy and reset the timer
			if (m_rightSideSpawn)
				SpawnBoatRight();
			else
				SpawnBoatLeft();

			m_rightSideSpawn = !m_rightSideSpawn;
			m_timeUntilNextSpawn = m_timeBetweenEnemySpawns;
			m_boatsStillNeedingToSpawnThisWave--;
		}
	}
}
#pragma endregion

#pragma region Collisions and Damage Stuff
//collision check
void Game::Collisions()
{
	//collision checks
	//get the collisions from the base scene
	std::vector<TTN_Collision::scolptr> collisionsThisFrame = TTN_Scene::GetCollisions();

	//iterate through the collisions
	for (int i = 0; i < collisionsThisFrame.size(); i++) {
		//grab the entity numbers of the colliding entities
		entt::entity entity1Ptr = collisionsThisFrame[i]->GetBody1();
		entt::entity entity2Ptr = collisionsThisFrame[i]->GetBody2();

		//check if both entities still exist
		if (TTN_Scene::GetScene()->valid(entity1Ptr) && TTN_Scene::GetScene()->valid(entity2Ptr)) {
			bool cont = true;
			//if they do, then check they both have tags
			if (TTN_Scene::Has<TTN_Tag>(entity1Ptr) && TTN_Scene::Has<TTN_Tag>(entity2Ptr)) {
				//if they do, then do tag comparisons

				//if one is a boat and the other is a cannonball
				if (cont && ((Get<TTN_Tag>(entity1Ptr).getLabel() == "Boat" && Get<TTN_Tag>(entity2Ptr).getLabel() == "Ball") ||
					(Get<TTN_Tag>(entity1Ptr).getLabel() == "Ball" && Get<TTN_Tag>(entity2Ptr).getLabel() == "Boat"))) {
					//then iterate through the list of cannonballs until you find the one that's collided
					std::vector<entt::entity>::iterator it = cannonBalls.begin();
					while (it != cannonBalls.end()) {
						if (entity1Ptr == *it || entity2Ptr == *it) {
							//and delete it
							DeleteEntity(*it);
							it = cannonBalls.erase(it);
						}
						else {
							it++;
						}
					}

					//and do the same with the boats, iteratoring through all of them until you find matching entity numbers
					std::vector<entt::entity>::iterator itt = boats.begin();
					while (itt != boats.end()) {
						if (entity1Ptr == *itt || entity2Ptr == *itt) {
							//play an expolsion at it's location
							glm::vec3 loc = Get<TTN_Transform>(*itt).GetGlobalPos();
							CreateExpolsion(loc);
							//remove the physics from it
							Remove<TTN_Physics>(*itt);
							//add a countdown until it deletes
							TTN_DeleteCountDown countdown = TTN_DeleteCountDown(2.5f);
							AttachCopy(*itt, countdown);
							//mark it as dead
							Get<EnemyComponent>(*itt).SetAlive(false);

							//and remove it from the list of boats as it will be deleted soon
							itt = boats.erase(itt);
							m_boatsRemainingThisWave--;
						}
						else {
							itt++;
						}
					}
				}
			}
		}
	}
}

//damage cooldown and stuff
void Game::Damage(float deltaTime) {
	//iterator through all the boats
	std::vector<entt::entity>::iterator it = boats.begin();
	while (it != boats.end()) {
		//check if the boat is close enough to the dam to damage it
		if (Get<TTN_Transform>(*it).GetPos().z <= EnemyComponent::GetZTarget() + 2.0f * EnemyComponent::GetZTargetDistance()) {
			//if they are check if they're through the cooldown
			if (Get<EnemyComponent>(*it).GetCooldown() <= 0.f) {
				//if they are do damage
				Get<EnemyComponent>(*it).SetCooldown(3.0f);
				Dam_health--;
				std::cout << Dam_health << std::endl;
			}
			//otherwise lower the remaining damage cooldown
			else {
				Get<EnemyComponent>(*it).SetCooldown(Get<EnemyComponent>(*it).GetCooldown() - deltaTime);
			}
			//and move to the next boat
			it++;
		}
		//otherwise just move to the next boat
		else {
			it++;
		}
	}
}
#pragma endregion

void Game::BirdUpate(float deltaTime)
{
	//move the birds
	birdTimer += deltaTime;
	birdTimer = fmod(birdTimer, 20);
	float t = TTN_Interpolation::InverseLerp(0.0f, 20.0f, birdTimer);
	for (int i = 0; i < 3; i++) {
		if (i == 0) Get<TTN_Transform>(birds[i]).SetPos(TTN_Interpolation::Lerp(birdBase, birdTarget, t));

		if (i == 1) Get<TTN_Transform>(birds[i]).SetPos(TTN_Interpolation::Lerp
		(birdBase + glm::vec3(3.0f, -3.0f, 3.0f), birdTarget + glm::vec3(3.0f, -3.0f, 3.0f), t));

		if (i == 2) Get<TTN_Transform>(birds[i]).SetPos(TTN_Interpolation::Lerp
		(birdBase + glm::vec3(-3.0f, -3.0f, -3.0f), birdTarget + glm::vec3(-3.0f, -3.0f, -3.0f), t));
	}
}

void Game::ImGui()
{
	//ImGui controller for the camera
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
		while(it != m_Lights.end()) {
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

	if (ImGui::CollapsingHeader("Camera Controls")) {
		//control the x axis position
		auto& a = Get<TTN_Transform>(camera);
		float b = a.GetPos().x;
		if (ImGui::SliderFloat("Camera Test X-Axis", &b, -100.0f, 100.0f)) {
			a.SetPos(glm::vec3(b, a.GetPos().y, a.GetPos().z));
		}

		//control the y axis position
		float c = a.GetPos().y;
		if (ImGui::SliderFloat("Camera Test Y-Axis", &c, -100.0f, 100.0f)) {
			a.SetPos(glm::vec3(a.GetPos().x, c, a.GetPos().z));
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
	}

	ImGui::End();
}