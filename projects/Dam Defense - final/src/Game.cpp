//Dam Defense, by Atlas X Games
//Game.cpp, the source file for the class that represents the main gameworld scene

//import the class
#include "Game.h"

//default constructor
Game::Game()
	: TTN_Scene()
{}

//sets up the scene
void Game::InitScene()
{
	//load in the scene's assets
	SetUpAssets();

	//create the entities
	SetUpEntities();

	//set up the other data
	SetUpOtherData();
}

//updates the scene every frame
void Game::Update(float deltaTime)
{
	//allow the player to rotate
	PlayerRotate(deltaTime);

	//switch to the cannon's normal static animation if it's firing animation has ended
	StopFiring();

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
	if (Get<TTN_MorphAnimator>(cannon).getActiveAnim() == 0 &&
		TTN_Application::TTN_Input::GetMouseButton(TTN_MouseButton::Left)) {
		//fire from the cannon
		Get<TTN_MorphAnimator>(cannon).SetActiveAnim(1);
		Get<TTN_MorphAnimator>(cannon).getActiveAnimRef().Restart();
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

	///TEXTURES////
	cannonText = TTN_Texture2D::LoadFromFile("textures/metal.png");
	skyboxText = TTN_TextureCubeMap::LoadFromImages("textures/skybox/ocean.jpg");

	////MATERIALS////
	cannonMat = TTN_Material::Create();
	cannonMat->SetAlbedo(cannonText);
	cannonMat->SetShininess(128.0f);
	skyboxMat = TTN_Material::Create();
	skyboxMat->SetSkybox(skyboxText);
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
		camTrans.SetPos(glm::vec3(0.0f, 5.0f, 0.0f));
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
		TTN_Renderer cannonRenderer = TTN_Renderer(cannonMesh, shaderProgramAnimatedTextured);
		cannonRenderer.SetMat(cannonMat);
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

	//set the cannon to be a child of the camera
	Get<TTN_Transform>(cannon).SetParent(&Get<TTN_Transform>(camera), &camera);
}

//sets up any other data the game needs to store
void Game::SetUpOtherData()
{
	rotAmmount = glm::vec2(0.0f);
	mousePos = TTN_Application::TTN_Input::GetMousePosition();
	playerDir = glm::vec3(0.0f, 0.0f, 1.0f);
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
	playerDir = glm::vec3(glm::toMat4(glm::quat(glm::vec3(rotAmmount.y, -rotAmmount.x, 0.0f))) * glm::vec4(playerDir, 1.0f));

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
