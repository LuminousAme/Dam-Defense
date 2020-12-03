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
	//allow the player to rotate
	PlayerRotate(deltaTime);

	//switch to the cannon's normal static animation if it's firing animation has ended
	StopFiring();

	//delete any cannonballs that're way out of range
	DeleteCannonballs();

	//if the player is on shoot cooldown, decrement the time remaining on the cooldown
	if (playerShootCooldownTimer >= 0.0f) playerShootCooldownTimer -= deltaTime;

	//parameters: number of waves, rest time between waves, length of waves, deltatime
	Waves(6, 10.f, 40.0f, deltaTime); //first wave is shorter because delta time starts incrementing before scene loads in
	SpawnerLS(deltaTime, 2.5f);//sets the spawner and gives the interval of time the spawner should spawn boats
	SpawnerRS(deltaTime, 2.5f);//sets the spawner and gives the interval of time the spawner should spawn boats

	//goes through the boats vector
	for (int i = 0; i < boats.size(); i++) {
		//std::cout << "Path: " << Get<TTN_Tag>(boats[i]).getPath() << std::endl;
		int p = Get<TTN_Tag>(boats[i]).getPath(); //gets the boats randomized path num
		int n = Get<TTN_Tag>(boats[i]).getNum(); //gets the boats randomized path num
		Get<TTN_Physics>(boats[i]).GetRigidBody()->setGravity(btVector3(0.0f, 0.0f, 0.0f)); //sets gravity to 0
		BoatPathing(boats[i], p, n); //updates the pathing for the boat
	}

	if (FlameTimer <= 0) FlameTimer = 0.0f;
	else FlameTimer -= deltaTime;

	if (Flaming) {// if the flamethorwers are spewing flame particles
		FlameAnim += deltaTime;//increment flamethrower anim timer

		if (FlameAnim >= 3.0f) {//flame particles last for 3 seconds
			DeleteFlamethrowers(); //delete the flamethrowers and particles
			FlameAnim = 0.0f; //reset timer
			Flaming = false; //set flaming to false
		}

		//while it's flaming, iterate through the vector of boats, deleting the boat if it is at or below z = 20
		std::vector<entt::entity>::iterator it = boats.begin();
		while (it != boats.end()) {
			if (Get<TTN_Transform>(*it).GetPos().z >= 27.0f) {
				//std::cout <<"Global Pos:"<< Get<TTN_Transform>(*it).GetGlobalPos().z << std::endl;
				//std::cout << "Pos:" << Get<TTN_Transform>(*it).GetPos().z << std::endl;
				it++;
			}
			else {
				DeleteEntity(*it);
				it = boats.erase(it);
				std::cout << "ERASED " << std::endl;
			}
		}
	}

	Collisions(); //collision check

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

	//increase the total time of the scene to make the water animated correctly
	time += deltaTime;

	//printf("fps: %f\n", 1.0f / deltaTime);
	//don't forget to call the base class' update
	TTN_Scene::Update(deltaTime);
}

void Game::PostRender()
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

#pragma region INPUTS
//function to use to check for when a key is being pressed down for the first frame
void Game::KeyDownChecks()
{
	if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::Two)) {
		if (FlameTimer == 0.0f) { //cooldown is zero
			Flamethrower();
		}
	}
}

//function to cehck for when a key is being pressed
void Game::KeyChecks()
{
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
		//Get<TTN_Transform>(smokePS).SetPos((Get<TTN_Transform>(cannon).GetGlobalPos() -
			//0.5f * (Get<TTN_Transform>(cannon).GetGlobalPos() - Get<TTN_Transform>(cannon).GetPos())) + 0.8f * playerDir);
		Get<TTN_Transform>(smokePS).SetPos(glm::vec3(0.0f, -0.2f, 0.0f) + 0.9f * playerDir);
		Get<TTN_ParticeSystemComponent>(smokePS).GetParticleSystemPointer()->
			SetEmitterRotation(glm::vec3(rotAmmount.y, -rotAmmount.x, 0.0f));
		Get<TTN_ParticeSystemComponent>(smokePS).GetParticleSystemPointer()->Burst(500);
	}
}

//function to check for when a mouse button has been released
void Game::MouseButtonUpChecks()
{
}

#pragma endregion

//sets up all the assets in the scene
void Game::SetUpAssets()
{
	//// SHADERS ////
#pragma region SHADERS
	//create a shader program object
	shaderProgramUnTextured = TTN_Shader::Create();
	//load the shaders into the shader program
	shaderProgramUnTextured->LoadDefaultShader(TTN_DefaultShaders::VERT_NO_COLOR);
	shaderProgramUnTextured->LoadDefaultShader(TTN_DefaultShaders::FRAG_BLINN_PHONG_NO_TEXTURE);
	shaderProgramUnTextured->Link();

	//create a shader program object for textured objects
	shaderProgramTextured = TTN_Shader::Create();
	//load the shaders into the shader program
	shaderProgramTextured->LoadDefaultShader(TTN_DefaultShaders::VERT_NO_COLOR);
	shaderProgramTextured->LoadDefaultShader(TTN_DefaultShaders::FRAG_BLINN_PHONG_ALBEDO_ONLY);
	shaderProgramTextured->Link();

	//create a shader program object for the skybox
	shaderProgramSkybox = TTN_Shader::Create();
	//load the shaders into the shader program
	shaderProgramSkybox->LoadDefaultShader(TTN_DefaultShaders::VERT_SKYBOX);
	shaderProgramSkybox->LoadDefaultShader(TTN_DefaultShaders::FRAG_SKYBOX);
	shaderProgramSkybox->Link();

	//create a shader program for animationed textured objects
	shaderProgramAnimatedTextured = TTN_Shader::Create();
	//load the shaders into the shader program
	shaderProgramAnimatedTextured->LoadDefaultShader(TTN_DefaultShaders::VERT_MORPH_ANIMATION_NO_COLOR);
	shaderProgramAnimatedTextured->LoadDefaultShader(TTN_DefaultShaders::FRAG_BLINN_PHONG_ALBEDO_ONLY);
	shaderProgramAnimatedTextured->Link();

	//create a shader program for the terrain
	shaderProgramTerrain = TTN_Shader::Create();
	//load the shaders into the shader program
	shaderProgramTerrain->LoadShaderStageFromFile("shaders/terrain_vert.glsl", GL_VERTEX_SHADER);
	shaderProgramTerrain->LoadShaderStageFromFile("shaders/terrain_frag.glsl", GL_FRAGMENT_SHADER);
	shaderProgramTerrain->Link();

	//create a shader program for the water
	shaderProgramWater = TTN_Shader::Create();
	//load the shaders into the shader program
	shaderProgramWater->LoadShaderStageFromFile("shaders/water_vert.glsl", GL_VERTEX_SHADER);
	shaderProgramWater->LoadShaderStageFromFile("shaders/water_frag.glsl", GL_FRAGMENT_SHADER);
	shaderProgramWater->Link();

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
	treeMesh[0] = TTN_ObjLoader::LoadFromFile("models/Tree.obj");
	treeMesh[1] = TTN_ObjLoader::LoadFromFile("models/Tree2.obj");
	treeMesh[2] = TTN_ObjLoader::LoadFromFile("models/Tree3.obj");
	damMesh = TTN_ObjLoader::LoadFromFile("models/Dam.obj");

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

	////MATERIALS////
	cannonMat = TTN_Material::Create();
	cannonMat->SetAlbedo(cannonText);
	cannonMat->SetShininess(128.0f);

	boat1Mat = TTN_Material::Create();
	boat1Mat->SetAlbedo(boat1Text);
	boat1Mat->SetShininess(128.0f);
	boat2Mat = TTN_Material::Create();
	boat2Mat->SetAlbedo(boat2Text);
	boat2Mat->SetShininess(128.0f);
	boat3Mat = TTN_Material::Create();
	boat3Mat->SetAlbedo(boat3Text);
	boat3Mat->SetShininess(128.0f);

	flamethrowerMat = TTN_Material::Create();
	flamethrowerMat->SetAlbedo(flamethrowerText);
	flamethrowerMat->SetShininess(128.0f);

	skyboxMat = TTN_Material::Create();
	skyboxMat->SetSkybox(skyboxText);
	smokeMat = TTN_Material::Create();
	smokeMat->SetAlbedo(nullptr); //do this to be sure titan uses it's default white texture for the particle

	fireMat = TTN_Material::Create();
	fireMat->SetAlbedo(nullptr); //do this to be sure titan uses it's default white texture for the particle

	birdMat = TTN_Material::Create();
	birdMat->SetAlbedo(birdText);

	treeMat = TTN_Material::Create();
	treeMat->SetAlbedo(treeText);

	damMat = TTN_Material::Create();
	damMat->SetAlbedo(damText);
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
	flamethrowers = std::vector<entt::entity>();

	//set the cannon to be a child of the camera
	Get<TTN_Transform>(cannon).SetParent(&Get<TTN_Transform>(camera), &camera);
}

//sets up any other data the game needs to store
void Game::SetUpOtherData()
{
	//init some scene data
	rotAmmount = glm::vec2(0.0f);
	mousePos = TTN_Application::TTN_Input::GetMousePosition();
	playerDir = glm::vec3(0.0f, 0.0f, 1.0f);
	cannonBallForce = 3600.0f;
	playerShootCooldown = 0.7f;
	playerShootCooldownTimer = playerShootCooldown;
	terrainScale = 0.1f;
	time = 0.0f;
	waveSpeed = -2.5f;
	waveBaseHeightIncrease = 0.0f;
	waveHeightMultiplier = 0.005f;
	waveLenghtMultiplier = -10.0f;
	birdTimer = 0.0f;

	birdBase = glm::vec3(100, 10, 135);

	birdTarget = glm::vec3(-100, 10, -65);

	//make the scene have gravity

	TTN_Scene::SetGravity(glm::vec3(0.0f, -9.8f, 0.0f));

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
		fireParticle.SetOneEndColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		fireParticle.SetOneEndSize(4.0f);
		fireParticle.SetOneEndSpeed(6.0f);
		fireParticle.SetOneLifetime(2.0f);
		fireParticle.SetOneStartColor(glm::vec4(1.0f, 0.65f, 0.0f, 1.0f));
		fireParticle.SetOneStartSize(0.5f);
		fireParticle.SetOneStartSpeed(8.5f);
	}
}

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
	playerDir = glm::vec3(glm::toMat4(glm::quat(glm::radians(glm::vec3(rotAmmount.y, -rotAmmount.x, 0.0f))))
		* glm::vec4(playerDir, 1.0f));
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

//sets the pathing the boat entity should take based on the path integer (1-3 is left side, 4-6 is right side)
void Game::BoatPathing(entt::entity boatt, int path, int boatNum)
{
	auto& pBoat = Get<TTN_Physics>(boatt);
	auto& tBoat = Get<TTN_Transform>(boatt);

	//left side middle path
	if (path == 1) {
		if (tBoat.GetPos().x <= 65.f) {
			if (tBoat.GetRotation().y <= 75.0f && boatNum == 1) {
				tBoat.RotateFixed(glm::vec3(0.0f, -0.55f, 0.0f));
			}

			else if (tBoat.GetRotation().y <= -20.0f && boatNum == 2) {
				tBoat.RotateFixed(glm::vec3(0.0f, -0.55f, 0.0f));
			}

			else if (tBoat.GetRotation().y >= 20.0f && boatNum == 3) {
				tBoat.RotateFixed(glm::vec3(0.0f, -0.55f, 0.0f));
			}

			pBoat.AddForce(Seek(glm::vec3(8.0f, -8.0f, 1.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));
		}
	}

	//far left path
	if (path == 2) {
		if (tBoat.GetPos().x <= 75.f) {
			if (tBoat.GetRotation().y <= 83.0f && boatNum == 1) {
				tBoat.RotateFixed(glm::vec3(0.0f, -0.75f, 0.0f));
			}

			else if (tBoat.GetRotation().y <= -1.0f && boatNum == 2) {
				tBoat.RotateFixed(glm::vec3(0.0f, -0.7f, 0.0f));
			}

			else if (tBoat.GetRotation().y >= 1.0f && boatNum == 3) {
				tBoat.RotateFixed(glm::vec3(0.0f, -0.7f, 0.0f));
			}

			pBoat.AddForce(Seek(glm::vec3(40.0f, -8.0f, 1.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));
		}
	}

	//center left
	if (path == 3) {
		if (tBoat.GetPos().x <= 65.f && !(tBoat.GetPos().x <= 5.f)) {
			if (tBoat.GetRotation().y <= 65.0f && boatNum == 1) {
				tBoat.RotateFixed(glm::vec3(0.0f, -0.6f, 0.0f));}

			else if (tBoat.GetRotation().y <= 15.0f && boatNum == 2) { //carrier rotation
				tBoat.RotateFixed(glm::vec3(0.0f, -0.15f, 0.0f));
			}

			else if (tBoat.GetRotation().y >= 15.0f && boatNum == 3) { //submarine rotation
				tBoat.RotateFixed(glm::vec3(0.0f, -0.15f, 0.0f));
			}

			pBoat.AddForce(Seek(glm::vec3(5.0f, -8.0f, 55.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));
		}
	
		if (tBoat.GetPos().x <= 5.f) {
			if (tBoat.GetRotation().y <= 89.0f && boatNum == 1) {
				tBoat.RotateFixed(glm::vec3(0.0f, -0.95f, 0.0f));
			}

			else if (tBoat.GetRotation().y <= 15.0f && boatNum == 2) {
				tBoat.RotateFixed(glm::vec3(0.0f, -0.08f, 0.0f));
			}

			else if (tBoat.GetRotation().y >= 1.0f && boatNum == 3) {
				tBoat.RotateFixed(glm::vec3(0.0f, -0.08f, 0.0f));
			}

			pBoat.AddForce(Seek(glm::vec3(4.0f, -8.0f, 1.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));
		}
	}

	//right middle path
	if (path == 4) {
		if (tBoat.GetPos().x >= -65.F) {
			if (tBoat.GetRotation().y <= 69.0f && boatNum == 1) {
				tBoat.RotateFixed(glm::vec3(0.0f, 0.5f, 0.0f));
			}

			else if (tBoat.GetRotation().y >= 20.0f && boatNum == 2) {
				tBoat.RotateFixed(glm::vec3(0.0f, 0.55f, 0.0f));
			}

			else if (tBoat.GetRotation().y <= -20.0f && boatNum == 3) {
				tBoat.RotateFixed(glm::vec3(0.0f, 0.55f, 0.0f));
			}

			pBoat.AddForce(Seek(glm::vec3(-8.0f, -8.0f, 1.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));
		}
	}

	//far right path
	if (path == 5) {
		if (tBoat.GetPos().x >= -75.f) {
			if (tBoat.GetRotation().y <= 83.0f && boatNum == 1) {
				tBoat.RotateFixed(glm::vec3(0.0f, 0.75f, 0.0f));
			}

			else if (tBoat.GetRotation().y >= 1.0f && boatNum == 2) {
				tBoat.RotateFixed(glm::vec3(0.0f, 0.7f, 0.0f));
				std::cout << glm::to_string(tBoat.GetRotation()) << std::endl;
			}

			else if (tBoat.GetRotation().y <= -1.0f && boatNum == 3) {
				tBoat.RotateFixed(glm::vec3(0.0f, 0.7f, 0.0f));
			}

			pBoat.AddForce(Seek(glm::vec3(-40.0f, -8.0f, 1.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));
		}
	}

	//right center path
	if (path == 6) {
		if (tBoat.GetPos().x >= -65.f && !(tBoat.GetPos().x >= -5.F)) {
			if (tBoat.GetRotation().y <= 65.0f && boatNum == 1) {
				tBoat.RotateFixed(glm::vec3(0.0f, 0.6f, 0.0f));}

			else if (tBoat.GetRotation().y >= 15.0f && boatNum == 2) { //carrier rotation
				tBoat.RotateFixed(glm::vec3(0.0f, 0.20f, 0.0f));}

			else if (tBoat.GetRotation().y <= -15.0f && boatNum == 3) { //submarine rotation
				tBoat.RotateFixed(glm::vec3(0.0f, 0.15f, 0.0f));}

			pBoat.AddForce(Seek(glm::vec3(-5.0f, -8.0f, 55.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));
		}

		if (tBoat.GetPos().x >= -5.f) {
			if (tBoat.GetRotation().y <= 89.0f && boatNum == 1) {
				tBoat.RotateFixed(glm::vec3(0.0f, 0.95f, 0.0f));}

			else if (tBoat.GetRotation().y <= 15.0f && boatNum == 2) {
				tBoat.RotateFixed(glm::vec3(0.0f, 0.08f, 0.0f));}

			else if (tBoat.GetRotation().y <= -1.0f && boatNum == 3) {
				tBoat.RotateFixed(glm::vec3(0.0f, 0.08f, 0.0f));}

			pBoat.AddForce(Seek(glm::vec3(-4.0f, -8.0f, 1.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));
		}
	}

}

//spawn left side boats
void Game::SpawnerLS(float deltatime, float SpawnTime) {
	//increment timer
	Timer += deltatime;

	//if the timer is >= spawn time then it will spawn the boat
	if (Timer >= SpawnTime && Spawning) {
		// timer = 0, boat spawn code
		Timer = 0.F;//reset timer

		boats.push_back(CreateEntity());
		int randomBoat = rand() % 3 + 1; // generates number between 1-3
		//randomBoat = 3;

		TTN_Renderer boatRenderer = TTN_Renderer(boat1Mesh, shaderProgramTextured, boat1Mat);
		if (randomBoat == 1) {
			boatRenderer = TTN_Renderer(boat1Mesh, shaderProgramTextured, boat1Mat);
		}

		else if (randomBoat == 2) { //large carrier
			boatRenderer = TTN_Renderer(boat2Mesh, shaderProgramTextured, boat2Mat);
		}

		else if (randomBoat == 3) { // submarine lookking
			boatRenderer = TTN_Renderer(boat3Mesh, shaderProgramTextured, boat3Mat);
		}

		AttachCopy<TTN_Renderer>(boats[boats.size() - 1], boatRenderer);

		TTN_Transform boatTrans = TTN_Transform(glm::vec3(21.0f, 10.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f));
		boatTrans.SetPos(glm::vec3(90.0f, -8.0f, 115.0f));

		if (randomBoat == 1) { //small regular boat
			boatTrans.RotateFixed(glm::vec3(0.0f, 180.0f, 0.0f));
			boatTrans.SetScale(glm::vec3(0.25f, 0.25f, 0.25f));
		}

		else if (randomBoat == 2) { //large carrier
			boatTrans.RotateFixed(glm::vec3(0.0f, -90.0f, 0.0f));
			boatTrans.SetScale(glm::vec3(0.05f, 0.05f, 0.05f));
		}

		else if (randomBoat == 3) { // submarine lookking
			boatTrans.RotateFixed(glm::vec3(0.0f, 90.0f, 0.0f));
			boatTrans.SetScale(glm::vec3(0.15f, 0.15f, 0.15f));
		}

		AttachCopy<TTN_Transform>(boats[boats.size() - 1], boatTrans);

		TTN_Physics pbody = TTN_Physics(boatTrans.GetPos(), glm::vec3(0.0f), glm::vec3(2.0f, 4.0f, 8.95f), boats[boats.size() - 1], TTN_PhysicsBodyType::DYNAMIC);
		pbody.SetLinearVelocity(glm::vec3(-25.0f, 0.0f, 0.0f));//-2.0f
		AttachCopy<TTN_Physics>(boats[boats.size() - 1], pbody);

		int r = rand() % 3 + 1; // generates path number between 1-3 (left side paths, right side path nums are 4-6)

		//if (randomBoat == 2 && r == 3) r = 2; //if it's the carrier, make sure it doesnt go through the center
		TTN_Tag boatTag = TTN_Tag("Boat", r, randomBoat); //sets boat path number to ttn_tag
		AttachCopy<TTN_Tag>(boats[boats.size() - 1], boatTag);
	}
}

//spawn right side boats
void Game::SpawnerRS(float deltatime, float SpawnTime)
{
	Timer2 += deltatime;

	if (Timer2 >= SpawnTime && Spawning) {
		// timer = 0, boat spawn code
		Timer2 = 0.F;//reset timer

		boats.push_back(CreateEntity());

		int randomBoat = rand() % 3 + 1; // generates number between 1-3
	//	randomBoat = 3;

		TTN_Renderer boatRenderer = TTN_Renderer(boat1Mesh, shaderProgramTextured, boat1Mat);

		if (randomBoat == 1) {
			boatRenderer = TTN_Renderer(boat1Mesh, shaderProgramTextured, boat1Mat);
		}

		else if (randomBoat == 2) { //large carrier
			boatRenderer = TTN_Renderer(boat2Mesh, shaderProgramTextured, boat2Mat);
		}

		else if (randomBoat == 3) { // submarine lookking
			boatRenderer = TTN_Renderer(boat3Mesh, shaderProgramTextured, boat3Mat);
		}

		AttachCopy<TTN_Renderer>(boats[boats.size() - 1], boatRenderer);

		TTN_Transform boatTrans = TTN_Transform();
		boatTrans.SetPos(glm::vec3(-90.0f, -8.0f, 115.0f));
		if (randomBoat == 1) { //small regular boat
			boatTrans.RotateFixed(glm::vec3(0.0f, 0.0f, 0.0f));
			boatTrans.SetScale(glm::vec3(0.25f, 0.25f, 0.25f));
		}

		else if (randomBoat == 2) { //large carrier
			boatTrans.RotateFixed(glm::vec3(0.0f, 90.0f, 0.0f));
			boatTrans.SetScale(glm::vec3(0.05f, 0.05f, 0.05f));
		}

		else if (randomBoat == 3) { // submarine lookking
			boatTrans.RotateFixed(glm::vec3(0.0f, -90.0f, 0.0f));
			boatTrans.SetScale(glm::vec3(0.15f, 0.15f, 0.15f));
		}

		AttachCopy<TTN_Transform>(boats[boats.size() - 1], boatTrans);

		TTN_Physics pbody = TTN_Physics(boatTrans.GetPos(), glm::vec3(0.0f), glm::vec3(2.0f, 4.0f, 8.95f), boats[boats.size() - 1]);

		pbody.SetLinearVelocity(glm::vec3(25.0f, 0.0f, 0.0f));//-2.0f
		AttachCopy<TTN_Physics>(boats[boats.size() - 1], pbody);

		int r = rand() % 3 + 4; // generates path number between 4-6 (left side paths 1-3, right side path nums are 4-6)

		TTN_Tag boatTag = TTN_Tag("Boat", r, randomBoat); //sets boat path number to ttn_tag
		AttachCopy<TTN_Tag>(boats[boats.size() - 1], boatTag);
	}
}

//wave function: parameters: number of waves, rest time between waves, length of waves, deltatime
void Game::Waves(int num, float restTime, float waveTime, float deltaTime)
{
	if (num > wave) {
		//printf("Wave now!\n");
		waveTimer += deltaTime;

		if (waveTimer >= waveTime) {
			Spawning = false;
			printf("wavetimer over!\n");
			waveTimer = 0;
		}

		if (!Spawning) {
			//printf("resting!\n");
			restTimer += deltaTime;
			if (restTimer >= restTime) {
				Spawning = true;
				restTimer = 0;
				printf("Spawn now!\n");
				wave++;
			}
		}
	}

	else {
		printf("GAME over!\n");
	}
}

glm::vec3 Game::Seek(glm::vec3 target, glm::vec3 velo, glm::vec3 pos)
{
	//std::cout << glm::to_string(target) << std::endl;
	//std::cout << glm::to_string(velo) << std::endl;
	//std::cout << glm::to_string(pos) << std::endl;
	glm::vec3 maxVelo(-10.0f, 9.81f, -10.0f);
	glm::vec3 desired = (pos - target); //gets the desired vector

	desired = glm::normalize(desired) * maxVelo;
	glm::vec3 steering = desired - velo;

	glm::vec3 moveVelo = steering;  //clamp(moveVelo,velo+steering, maxVelo);

	return moveVelo;
}

//cooldown is set in this function, change flame timer
void Game::Flamethrower() {
	if (FlameTimer == 0.0f) { //cooldown is zero
		FlameTimer = 2.f; // set cooldown
		Flaming = true;// set flaming to true

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

			//fire particle entities
			{
				flames.push_back(CreateEntity());

				//setup a transfrom for the particle system
				TTN_Transform firePSTrans = TTN_Transform(glm::vec3(2.5f, -3.0f, 1.8f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(1.0f)); //close left
				if (i == 0) {
					firePSTrans.SetPos(glm::vec3(-2.5f, -3.0f, 1.8f));//close right
				}
				else if (i == 1) {
					firePSTrans.SetPos(glm::vec3(20.f, -3.0f, 1.8f));//far left
				}
				else if (i == 2) {
					firePSTrans.SetPos(glm::vec3(-20.0f, -3.0f, 1.8f));//far right
				}
				else if (i == 3) {
					firePSTrans.SetPos(glm::vec3(7.5f, -3.0f, 1.8f));
				}
				else if (i == 4) {
					firePSTrans.SetPos(glm::vec3(-7.5f, -3.0f, 1.8f));
				}
				else {}

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

	else { //otherwise nothing happens
		Flaming = false;
	}
}

void Game::Collisions()
{
	//collision checks
	//get the collisions from the base scene
	std::vector<TTN_Collision::scolptr> collisionsThisFrame = TTN_Scene::GetCollisions();

	//iterate through the collisions
	for (int i = 0; i < collisionsThisFrame.size(); i++) {
		//get both the rigidbodies
		//const btRigidBody* b1 = collisionsThisFrame[i]->GetBody1();
		//const btRigidBody* b2 = collisionsThisFrame[i]->GetBody2();
		//get the entity number from each body
		//entt::entity entity1Ptr = static_cast<entt::entity>(reinterpret_cast<uint32_t>(b1->getUserPointer()));
		//entt::entity entity2Ptr = static_cast<entt::entity>(reinterpret_cast<uint32_t>(b2->getUserPointer()));
		//entt::entity entity1Ptr = *static_cast<entt::entity*>(b1->getUserPointer());
		//entt::entity entity2Ptr = *static_cast<entt::entity*>(b2->getUserPointer());
		entt::entity entity1Ptr = collisionsThisFrame[i]->GetBody1();
		entt::entity entity2Ptr = collisionsThisFrame[i]->GetBody2();

		//entt::entity tempTankRed = redPlayer;
		//entt::entity tempTankBlue = bluePlayer;

		//check if both entities still exist
		if (TTN_Scene::GetScene()->valid(entity1Ptr) && TTN_Scene::GetScene()->valid(entity2Ptr)) {
			bool cont = true;
			//if they do, then check they both have tags
			if (TTN_Scene::Has<TTN_Tag>(entity1Ptr) && TTN_Scene::Has<TTN_Tag>(entity2Ptr)) {
				//if they do, then do tag comparisons

				//if one is a boat and the other is a cannonball
				if (cont && ((Get<TTN_Tag>(entity1Ptr).getName() == "Boat" && Get<TTN_Tag>(entity2Ptr).getName() == "Ball") ||
					(Get<TTN_Tag>(entity1Ptr).getName() == "Ball" && Get<TTN_Tag>(entity2Ptr).getName() == "Boat"))) {
					std::vector<entt::entity>::iterator it = cannonBalls.begin();
					while (it != cannonBalls.end()) {
						if (entity1Ptr == *it || entity2Ptr == *it) {
							DeleteEntity(*it);
							it = cannonBalls.erase(it);
						}
						else {
							it++;
						}
					}

					std::vector<entt::entity>::iterator itt = boats.begin();
					while (itt != boats.end()) {
						if (entity1Ptr == *itt || entity2Ptr == *itt) {
							DeleteEntity(*itt);
							itt = boats.erase(itt);
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

//called to delete particle system and flamethrower models
void Game::DeleteFlamethrowers() {
	std::vector<entt::entity>::iterator it = flamethrowers.begin();
	while (it != flamethrowers.end()) {
		DeleteEntity(*it);
		it = flamethrowers.erase(it);
		//it++;
	}

	std::vector<entt::entity>::iterator itt = flames.begin();
	while (itt != flames.end()) {
		DeleteEntity(*itt);
		itt = flames.erase(itt);
	}
}