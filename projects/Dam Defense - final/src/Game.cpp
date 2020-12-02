//Dam Defense, by Atlas X Games
//Game.cpp, the source file for the class that represents the main gameworld scene

//import the class
#include "Game.h"

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
	if(playerShootCooldownTimer >= 0.0f) playerShootCooldownTimer -= deltaTime;

	printf("fps: %f\n", 1.0f/deltaTime);
	//don't forget to call the base class' update
	TTN_Scene::Update(deltaTime);
}

//function to use to check for when a key is being pressed down for the first frame
void Game::KeyDownChecks()
{
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
		Get<TTN_Transform>(smokePS).SetPos(glm::vec3(0.0f, -0.1f, 0.0f) + 0.9f * playerDir);
		Get<TTN_ParticeSystemComponent>(smokePS).GetParticleSystemPointer()->
			SetEmitterRotation(glm::vec3(rotAmmount.y, -rotAmmount.x, 0.0f));
		Get<TTN_ParticeSystemComponent>(smokePS).GetParticleSystemPointer()->Burst(500);
	}
}

//function to check for when a mouse button has been released
void Game::MouseButtonUpChecks()
{

}

//sets up all the assets in the scene
void Game::SetUpAssets()
{
	//// SHADERS ////
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

	////MESHES////
	cannonMesh = TTN_ObjLoader::LoadAnimatedMeshFromFiles("models/cannon/cannon", 7);
	skyboxMesh = TTN_ObjLoader::LoadFromFile("models/SkyboxMesh.obj");
	sphereMesh = TTN_ObjLoader::LoadFromFile("models/IcoSphereMesh.obj");

	///TEXTURES////
	cannonText = TTN_Texture2D::LoadFromFile("textures/metal.png");
	skyboxText = TTN_TextureCubeMap::LoadFromImages("textures/skybox/ocean.jpg");

	////MATERIALS////
	cannonMat = TTN_Material::Create();
	cannonMat->SetAlbedo(cannonText);
	cannonMat->SetShininess(128.0f);
	skyboxMat = TTN_Material::Create();
	skyboxMat->SetSkybox(skyboxText);
	smokeMat = TTN_Material::Create();
	smokeMat->SetAlbedo(nullptr); //do this to be sure titan uses it's default white texture for the particle
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
		Get<TTN_Camera>(camera).CalcPerspective(60.0f, 1.78f, 0.01f, 100.f);
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
		TTN_MorphAnimation notFiringAnim = TTN_MorphAnimation({0}, {3.0f/24}, true); //anim 0
		//create an animation for the cannon when it is firing
		std::vector<int> firingFrameIndices = std::vector<int>();
		std::vector<float> firingFrameLenghts = std::vector<float>();
		for (int i = 0; i < 7; i++) firingFrameIndices.push_back(i);
		firingFrameLenghts.push_back(3.0f/24.0f);
		firingFrameLenghts.push_back(1.0f/24.0f);
		firingFrameLenghts.push_back(1.0f/24.0f);
		firingFrameLenghts.push_back(1.0f/24.0f);
		firingFrameLenghts.push_back(1.0f/24.0f);
		firingFrameLenghts.push_back(2.0f/24.0f);
		firingFrameLenghts.push_back(3.0f/24.0f);
		TTN_MorphAnimation firingAnim = TTN_MorphAnimation(firingFrameIndices, firingFrameLenghts, true); //anim 1
		//add both animatons to the animator
		cannonAnimator.AddAnim(notFiringAnim);
		cannonAnimator.AddAnim(firingAnim);
		//start on the not firing anim
		cannonAnimator.SetActiveAnim(0);
		//attach that animator to the entity
		AttachCopy(cannon, cannonAnimator);
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

	//prepare the vector of cannonballs
	cannonBalls = std::vector<entt::entity>();

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
	cannonBallForce = 1250.0f;
	playerShootCooldown = 1.0f;
	playerShootCooldownTimer = playerShootCooldown;

	//make the scene have gravity
	TTN_Scene::SetGravity(glm::vec3(0.0f, -9.81f, 0.0f));

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
		cannonBallTrans.SetScale(glm::vec3(0.25f));
		//attach that transform to the entity
		AttachCopy(cannonBalls[cannonBalls.size() - 1], cannonBallTrans);

		//set up a physics body for the cannonball
		TTN_Physics cannonBallPhysBod = TTN_Physics(cannonBallTrans.GetPos(), glm::vec3(0.0f), cannonBallTrans.GetScale(),
			cannonBalls[cannonBalls.size() - 1]);
		cannonBallPhysBod.SetHasGravity(true); //be explict about the cannonball being affected by gravity
		//attach that physics body to the entity
		AttachCopy(cannonBalls[cannonBalls.size() - 1], cannonBallPhysBod);
	}

	//after the cannonball has been created, get the physics body and apply a force along the player's direction
	Get<TTN_Physics>(cannonBalls[cannonBalls.size() - 1]).AddForce(cannonBallForce * playerDir);
}

//function that will check the positions of the cannonballs each frame and delete any that're too low
void Game::DeleteCannonballs()
{
	//iterate through the vector of cannonballs, deleting the cannonball if it is at or below y = -50
	std::vector<entt::entity>::iterator it = cannonBalls.begin();
	while (it != cannonBalls.end()) {
		if (Get<TTN_Transform>(*it).GetGlobalPos().y > -50.0f)
			it++;
		else
			it = cannonBalls.erase(it);
	}
}
