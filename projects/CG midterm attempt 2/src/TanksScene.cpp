//INFR 1350 Pratical Midterm - Battle Tanks by Ame Gilham (100741352) and Jackie Zhou (100750922)
//The project makes use of the Titan Engine, the engine we developed together through this class and GDW (team: Atlas X Games)
//all classes with the prefix TTN_ are classes from the Titan engine

//TanksScene.cpp, the source file for the class for the scene with the game battle tanks
#include "TanksScene.h"

//constructor
TanksScene::TanksScene()
	: TTN_Scene()
{
}

//sets up the scene
void TanksScene::InitScene()
{
	//init resources (shaders, meshes, textures, materials, etc.)
	InitResources();

	//init entities
	InitEntities();

	//init other data
	InitOtherData();
}

//updates the scene every frame
void TanksScene::Update(float deltaTime)
{
	/// Camera Shake ///
	{
		//if the camera should be shaking
		if (shakeDuration > 0.0f) {
			//generate the random numbers
			float randomX = (2.0f * (float)(rand() / (float)(RAND_MAX))) - 1;
			float randomY = (2.0f * (float)(rand() / (float)(RAND_MAX))) - 1;
			float randomZ = (2.0f * (float)(rand() / (float)(RAND_MAX))) - 1;
			//add them to the camera's position
			Get<TTN_Transform>(camera).SetPos(originalCamPos + glm::vec3(randomX, randomY, randomZ) * shakeMagnitude);

			//reduce the remaining time on the screenshake
			shakeDuration -= deltaTime;
		}
	}

	/// Expolsion effect ///
	{
		//red
		if (redExpolsionDuration > 0.0f) {
			auto& redExplodeTrans = Get<TTN_Transform>(redExpolsion);
			redExplodeTrans.SetScale(redExplodeTrans.GetScale() + glm::vec3(deltaTime));
			redExplodeTrans.RotateFixed(glm::vec3(0.0f, 0.0f, 90.0f * deltaTime * 4.0f));

			redExpolsionDuration -= deltaTime;
		}
		else if (Get<TTN_Transform>(redExpolsion).GetPos() != originalExpolsionPos) {
			Get<TTN_Transform>(redExpolsion).SetPos(originalExpolsionPos);
			Get<TTN_Transform>(redExpolsion).SetScale(glm::vec3(0.1f));
		}
		//blue
		if (blueExpolsionDuration > 0.0f) {
			auto& blueExplodeTrans = Get<TTN_Transform>(blueExpolsion);
			blueExplodeTrans.SetScale(blueExplodeTrans.GetScale() + glm::vec3(deltaTime));
			blueExplodeTrans.RotateFixed(glm::vec3(0.0f, 0.0f, 90.0f * deltaTime * 4.0f));

			blueExpolsionDuration -= deltaTime;
		}
		else if (Get<TTN_Transform>(blueExpolsion).GetPos() != originalExpolsionPos) {
			Get<TTN_Transform>(blueExpolsion).SetPos(originalExpolsionPos);
			Get<TTN_Transform>(blueExpolsion).SetScale(glm::vec3(0.1f));
		}
	}

	//make sure bullet velocity is consistent 
	if (redBulletExists) {
		auto& redphysBodB = Get<TTN_Physics>(redBullet);
		redphysBodB.SetLinearVelocity(glm::normalize(redphysBodB.GetLinearVelocity()));
	}
	if (blueBulletExists) {
		auto& bluephysBodB = Get<TTN_Physics>(blueBullet);
		bluephysBodB.SetLinearVelocity(glm::normalize(bluephysBodB.GetLinearVelocity()));
	}

	//collision checks
	//get the collisions from the base scene
	std::vector<TTN_Collision::scolptr> collisionsThisFrame = TTN_Scene::GetCollisions();

	//make a vector of entites that need to be deleted
	std::vector<entt::entity> entitiesToDelete;
	bool deleteRed = false;
	bool deleteBlue = false;

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

		entt::entity tempTankRed = redPlayer;
		entt::entity tempTankBlue = bluePlayer;

		//check if both entities still exist
		if (TTN_Scene::GetScene()->valid(entity1Ptr) && TTN_Scene::GetScene()->valid(entity2Ptr)) {
			bool cont = true;
			//if they do, then check they both have tags
			if (TTN_Scene::Has<TTN_Tag>(entity1Ptr) && TTN_Scene::Has<TTN_Tag>(entity2Ptr)) {
				//if they do, then do tag comparisons 
				
				//if they're both bullets
				if (Get<TTN_Tag>(entity1Ptr).getName() == "bullet" && Get<TTN_Tag>(entity2Ptr).getName() == "bullet") {
					//if so delete both bullets
					bool shouldAdd = true;
					for (int j = 0; j < entitiesToDelete.size(); j++)
						if (entity1Ptr == entitiesToDelete[j]) shouldAdd = false;
					if (shouldAdd) entitiesToDelete.push_back(entity1Ptr);

					shouldAdd = true;
					for (int j = 0; j < entitiesToDelete.size(); j++)
						if (entity2Ptr == entitiesToDelete[j]) shouldAdd = false;
					if (shouldAdd) entitiesToDelete.push_back(entity2Ptr);

					redBulletExists = false;
					blueBulletExists = false;
					bounceCountR = 0;
					bounceCountB = 0;
					cont = false;
				}

				//if one is the red tank and another is a bullet
				if (cont && ((Get<TTN_Tag>(entity1Ptr).getName() == "red tank" && Get<TTN_Tag>(entity2Ptr).getName() == "bullet") ||
					(Get<TTN_Tag>(entity1Ptr).getName() == "bullet" && Get<TTN_Tag>(entity2Ptr).getName() == "red tank"))) {
					//figure out if it was the red or blue bullet to delete the correct one
					if (entity1Ptr == redBullet || entity2Ptr == redBullet) {
						bool shouldAdd = true;
						for (int j = 0; j < entitiesToDelete.size(); j++)
							if (redBullet == entitiesToDelete[j]) shouldAdd = false;
						if (shouldAdd) entitiesToDelete.push_back(redBullet);
						redBulletExists = false;
						bounceCountR = 0;
					}
					else if (entity1Ptr == blueBullet || entity2Ptr == blueBullet) {
						bool shouldAdd = true;
						for (int j = 0; j < entitiesToDelete.size(); j++)
							if (blueBullet == entitiesToDelete[j]) shouldAdd = false;
						if (shouldAdd) entitiesToDelete.push_back(blueBullet);
						blueBulletExists = false;
						bounceCountB = 0;
					}

					//play the expolsion effect
					Get<TTN_Transform>(redExpolsion).SetPos(Get<TTN_Transform>(redPlayer).GetPos());
					redExpolsionDuration = 1.0f;

					//delete and recreate the red player
					bool shouldAdd = true;
					for (int j = 0; j < entitiesToDelete.size(); j++)
						if (redPlayer == entitiesToDelete[j]) shouldAdd = false;
					if (shouldAdd) entitiesToDelete.push_back(redPlayer);
					deleteRed = true;
					//CreateRedPlayer();
					cont = false;
				}

				//if one is the blue tank and another is a bullet
				if (cont && ((Get<TTN_Tag>(entity1Ptr).getName() == "blue tank" && Get<TTN_Tag>(entity2Ptr).getName() == "bullet") ||
					(Get<TTN_Tag>(entity1Ptr).getName() == "bullet" && Get<TTN_Tag>(entity2Ptr).getName() == "blue tank"))) {
					//figure out if it was the red or blue bullet to delete the correct one
					if (entity1Ptr == redBullet || entity2Ptr == redBullet) {
						bool shouldAdd = true;
						for (int j = 0; j < entitiesToDelete.size(); j++)
							if (redBullet == entitiesToDelete[j]) shouldAdd = false;
						if (shouldAdd) entitiesToDelete.push_back(redBullet);
						redBulletExists = false;
						bounceCountR = 0;
					}
					else if (entity1Ptr == blueBullet || entity2Ptr == blueBullet) {
						bool shouldAdd = true;
						for (int j = 0; j < entitiesToDelete.size(); j++)
							if (blueBullet == entitiesToDelete[j]) shouldAdd = false;
						if (shouldAdd) entitiesToDelete.push_back(blueBullet);
						blueBulletExists = false;
						bounceCountB = 0;
					}

					//play the expolsion effect
					Get<TTN_Transform>(blueExpolsion).SetPos(Get<TTN_Transform>(bluePlayer).GetPos());
					blueExpolsionDuration = 1.0f;

					//delete and recreate the red player
					bool shouldAdd = true;
					for (int j = 0; j < entitiesToDelete.size(); j++)
						if (bluePlayer == entitiesToDelete[j]) shouldAdd = false;
					if (shouldAdd) entitiesToDelete.push_back(bluePlayer);
					deleteBlue = true;
					//CreateBluePlayer();
					cont = false;
				}
			}
		}
	}

	//when all the collisions have been processed, delete the entites that should be delete
	for (int i = 0; i < entitiesToDelete.size(); i++) {
		DeleteEntity(entitiesToDelete[i]);
	}

	if (deleteRed) {
		//recreate red
		CreateRedPlayer();
		//add a point to blue
		scoreBlue++;
		//and update blue's score display
		if (scoreBlue < 10) Get<TTN_Renderer>(blueScoreEntity).SetMesh(scoreMesh[scoreBlue]);
		//reset direction
		redPlayerDir = glm::vec2(-1.0f, 0.0f);
	}
	if (deleteBlue) {
		//recreate blue
		CreateBluePlayer();
		//add a point to red
		scoreRed++;
		//and update red's score display
		if (scoreRed < 10) Get<TTN_Renderer>(redScoreEntity).SetMesh(scoreMesh[scoreRed]);
		//reset direction
		bluePlayerDir = glm::vec2(1.0f, 0.0f);
	}

	//Please don't forget to call the base scene's update at the end of the child class' update
	TTN_Scene::Update(deltaTime);

	//match the lights position to the tanks cause parenting isn't working for them for some reason
	//Get<TTN_Transform>(lightRedPlayer).SetPos(Get<TTN_Transform>(redPlayer).GetPos());
	//Get<TTN_Transform>(lightBluePlayer).SetPos(Get<TTN_Transform>(bluePlayer).GetPos());
}

//checks for when a key is pressed down
void TanksScene::KeyDownChecks()
{
	//allow the players to move forwards and backwards
	if (TTN_Application::TTN_Input::GetKeyDown(TTN_KeyCode::W) && 
			!TTN_Application::TTN_Input::GetKey(TTN_KeyCode::A) && !TTN_Application::TTN_Input::GetKey(TTN_KeyCode::D)) {
		//add an impulse in the direction the red tank is facing
		Get<TTN_Physics>(redPlayer).SetLinearVelocity(glm::vec3(glm::normalize(redPlayerDir).x * playerSpeed, glm::normalize(redPlayerDir).y * playerSpeed, 0.0f));
	}
	//red player backwards
	if (TTN_Application::TTN_Input::GetKeyDown(TTN_KeyCode::S) &&
		!TTN_Application::TTN_Input::GetKey(TTN_KeyCode::A) && !TTN_Application::TTN_Input::GetKey(TTN_KeyCode::D)) {
		//add an impulse opposite the direction the red tank is facing
		Get<TTN_Physics>(redPlayer).SetLinearVelocity(-1.0f * glm::vec3(glm::normalize(redPlayerDir).x * playerSpeed, glm::normalize(redPlayerDir).y * playerSpeed, 0.0f));
	}
	//blue player forward
	if (TTN_Application::TTN_Input::GetKeyDown(TTN_KeyCode::UpArrow) &&
		!TTN_Application::TTN_Input::GetKey(TTN_KeyCode::LeftArrow) && !TTN_Application::TTN_Input::GetKey(TTN_KeyCode::RightArrow)) {
		//add an impulse in the direction the blue tank is facing
		Get<TTN_Physics>(bluePlayer).SetLinearVelocity(glm::vec3(glm::normalize(bluePlayerDir).x * playerSpeed, glm::normalize(bluePlayerDir).y * playerSpeed, 0.0f));
	}
	//blue player backwards
	if (TTN_Application::TTN_Input::GetKeyDown(TTN_KeyCode::DownArrow) &&
		!TTN_Application::TTN_Input::GetKey(TTN_KeyCode::LeftArrow) && !TTN_Application::TTN_Input::GetKey(TTN_KeyCode::RightArrow)) {
		//add an impulse opposite the direction the blue tank is facing
		Get<TTN_Physics>(bluePlayer).SetLinearVelocity(-1.0f * glm::vec3(glm::normalize(bluePlayerDir).x * playerSpeed, glm::normalize(bluePlayerDir).y * playerSpeed, 0.0f));
	}
}

//checks for when a key is pressed
void TanksScene::KeyChecks()
{
	//Allow the player to rotate the tanks
	//get deltaTime
	float dt = TTN_Application::GetDeltaTime();

	//get a reference to the red player's transform
	auto& redTrans = Get<TTN_Transform>(redPlayer);

	//allow the red player to rotate their tank
	if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::A) &&
		!TTN_Application::TTN_Input::GetKey(TTN_KeyCode::W) && !TTN_Application::TTN_Input::GetKey(TTN_KeyCode::S)) {
		redPlayerDir = glm::rotate(redPlayerDir, glm::radians(-50.0f * dt));
		redTrans.RotateFixed(glm::vec3(0.0f, 0.0f, -50.0f * dt));
	}
	if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::D) &&
		!TTN_Application::TTN_Input::GetKey(TTN_KeyCode::W) && !TTN_Application::TTN_Input::GetKey(TTN_KeyCode::S)) {
		redPlayerDir = glm::rotate(redPlayerDir, glm::radians(50.0f * dt));
		redTrans.RotateFixed(glm::vec3(0.0f, 0.0f, 50.0f * dt));
	}

	//get a refernece to the blue player's transform
	auto& blueTrans = Get<TTN_Transform>(bluePlayer);

	//allow the blue player to rotate their tank
	if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::LeftArrow) &&
		!TTN_Application::TTN_Input::GetKey(TTN_KeyCode::UpArrow) && !TTN_Application::TTN_Input::GetKey(TTN_KeyCode::DownArrow)) {
		bluePlayerDir = glm::rotate(bluePlayerDir, glm::radians(-50.0f * dt));
		blueTrans.RotateFixed(glm::vec3(0.0f, 0.0f, -50.0f * dt));
	}
	if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::RightArrow) &&
		!TTN_Application::TTN_Input::GetKey(TTN_KeyCode::UpArrow) && !TTN_Application::TTN_Input::GetKey(TTN_KeyCode::DownArrow)) {
		bluePlayerDir = glm::rotate(bluePlayerDir, glm::radians(50.0f * dt));
		blueTrans.RotateFixed(glm::vec3(0.0f, 0.0f, 50.0f * dt));
	}

	//allow the player to shoot
	if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::E) && !redBulletExists) {
		CreateRedBullet();
	}

	if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::M) && !blueBulletExists) {
		CreateBlueBullet();
	}
}

//checks for when a key is released
void TanksScene::KeyUpChecks()
{
	//make the players stop moving when keys are released
	//red player forward
	if (TTN_Application::TTN_Input::GetKeyUp(TTN_KeyCode::W)) {
		Get<TTN_Physics>(redPlayer).SetLinearVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
	}
	//red player backwards
	if (TTN_Application::TTN_Input::GetKeyUp(TTN_KeyCode::S)) {
		Get<TTN_Physics>(redPlayer).SetLinearVelocity(glm::vec3(0.0f, 0.0f, 0.0f));;
	}
	//blue player forward
	if (TTN_Application::TTN_Input::GetKeyUp(TTN_KeyCode::UpArrow)) {
		Get<TTN_Physics>(bluePlayer).SetLinearVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
	}
	//blue player backwards
	if (TTN_Application::TTN_Input::GetKeyUp(TTN_KeyCode::DownArrow)) {
		Get<TTN_Physics>(bluePlayer).SetLinearVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
	}
}

//checks for when a mouse button is first pressed
void TanksScene::MouseButtonDownChecks()
{
}

//checks for when a mouse button is pressed
void TanksScene::MouseButtonChecks()
{
}

//check for when a mouse button is released
void TanksScene::MouseButtonUpChecks()
{
}

//sets up the resources (shaders, meshes, textures, mats, etc.)
void TanksScene::InitResources()
{
	//create a shader program
	shaderProgram = TTN_Shader::Create();
	shaderProgram->LoadDefaultShader(TTN_DefaultShaders::VERT_NO_COLOR);
	shaderProgram->LoadDefaultShader(TTN_DefaultShaders::FRAG_BLINN_PHONG_ALBEDO_ONLY);
	shaderProgram->Link();

	//load in all our meshes
	tankMesh = TTN_ObjLoader::LoadFromFile("tank.obj");
	tankMesh->SetUpVao();
	cannonballMesh = TTN_ObjLoader::LoadFromFile("cannonball.obj");
	cannonballMesh->SetUpVao();
	planeMesh = TTN_ObjLoader::LoadFromFile("plane.obj");
	planeMesh->SetUpVao();
	cubeMesh = TTN_ObjLoader::LoadFromFile("wall.obj");
	cubeMesh->SetUpVao();
	expolsionMesh = TTN_ObjLoader::LoadFromFile("expolsion.obj");
	expolsionMesh->SetUpVao();
	for (int i = 0; i < 10; i++) {
		std::string fileName = std::to_string(i) + ".obj";
		scoreMesh[i] = TTN_ObjLoader::LoadFromFile(fileName);
		scoreMesh[i]->SetUpVao();
	}

	//load in all the textures
	redTankText = TTN_Texture2D::LoadFromFile("red_tank.png"); //red tank and cannonball
	blueTankText = TTN_Texture2D::LoadFromFile("blue_tank.png"); //blue tank and cannonball
	backgroundText = TTN_Texture2D::LoadFromFile("background.png"); //background
	wallText = TTN_Texture2D::LoadFromFile("wall.png"); //walls
	redScoreText = TTN_Texture2D::LoadFromFile("score_red.png");//red score
	blueScoreText = TTN_Texture2D::LoadFromFile("score_blue.png");//blue score
	expolsionText = TTN_Texture2D::LoadFromFile("expolsion.png");//expolsion

	//create materials and link them to textures
	//red tank and cannonball
	redTankMat = TTN_Material::Create();
	redTankMat->SetShininess(64.0f);
	redTankMat->SetAlbedo(redTankText);
	//blue tank and cannonball
	blueTankMat = TTN_Material::Create();
	blueTankMat->SetShininess(64.0f);
	blueTankMat->SetAlbedo(blueTankText);
	//background
	backgroundMat = TTN_Material::Create();
	backgroundMat->SetShininess(128.0f);
	backgroundMat->SetAlbedo(backgroundText);
	//walls
	wallMat = TTN_Material::Create();
	wallMat->SetShininess(128.0f);
	wallMat->SetAlbedo(wallText);
	//red's score
	redScoreMat = TTN_Material::Create();
	redScoreMat->SetShininess(128.0f);
	redScoreMat->SetAlbedo(redScoreText);
	//blue's score
	blueScoreMat = TTN_Material::Create();
	blueScoreMat->SetShininess(128.0f);
	blueScoreMat->SetAlbedo(blueScoreText);
	//expolsion effect
	expolsionMat = TTN_Material::Create();
	expolsionMat->SetShininess(2.0f);
	expolsionMat->SetAlbedo(expolsionText);
}

//sets up the entities that serve as actual game objects
void TanksScene::InitEntities()
{
	//camera entity
	{
		//create an entity in the scene
		camera = CreateEntity();
		//set this entity as the scene's camera
		SetCamEntity(camera);

		//add a camera component to it
		Attach<TTN_Camera>(camera);
		//add a transform component to it 
		Attach<TTN_Transform>(camera);

		//get a reference to that transform component and set it up
		auto& camTrans = Get<TTN_Transform>(camera);
		camTrans.SetPos(glm::vec3(0.0f, 0.0f, -5.0f));
		glm::vec3 direction = glm::normalize(-1.0f * camTrans.GetPos());
		camTrans.LookAlong(direction, glm::vec3(0.0f, 1.0f, 0.0f));
		camTrans.RotateFixed(glm::vec3(0.0f, 0.0f, 0.0f));

		//setup an orthographic projection for the camera
		//Get<TTN_Camera>(camera).CalcOrtho(-1920/200, 1920/200, -1080/200, 1080/200, 0.01, 1000);
		Get<TTN_Camera>(camera).CalcPerspective(98.0f, 1920.0f / 1080.0f, 0.01, 5.1f);
		//setup the view for the camera 
		Get<TTN_Camera>(camera).View();
	}

	
	//light
	{
		//create an entity in the scene for a light
		mainLight = CreateEntity();
		//add that light to the list of lights
		m_Lights.push_back(mainLight);

		//set up a trasnform for the light
		TTN_Transform lightTrans = TTN_Transform();
		lightTrans.SetPos(glm::vec3(0.0f, 0.0f, -5.0f));
		//attach that transform to the light entity
		AttachCopy<TTN_Transform>(mainLight, lightTrans);

		//set up a light component for the light
		TTN_Light lightLight = TTN_Light(glm::vec3(1.0f, 1.0f, 1.0f), 0.8f, 50.0f, 0.1f, 2.0f, 0.06f);
		//attach that light to the light entity
		AttachCopy<TTN_Light>(mainLight, lightLight);
	}

	//plane in the background
	{
		//create an entity for the background plane in the scene
		backgroundPlane = CreateEntity();

		//setup a mesh renderer for the plane
		TTN_Renderer bgRenderer = TTN_Renderer(planeMesh, shaderProgram);
		//attach the material to the renderer
		bgRenderer.SetMat(backgroundMat);
		//attach the renderer to the entity in the ecs
		AttachCopy<TTN_Renderer>(backgroundPlane, bgRenderer);

		//setup a transform for the plane
		TTN_Transform bgTrans = TTN_Transform();
		bgTrans.SetPos(glm::vec3(0.0f));
		bgTrans.SetScale(glm::vec3(15.0f, 0.0f, 8.0f));
		bgTrans.RotateFixed(glm::vec3(270.0f, 0.0f, 0.0f));
		//attach the transform to the entity in the ecs
		AttachCopy<TTN_Transform>(backgroundPlane, bgTrans);
	}

	CreateRedPlayer();

	//light that follows the red player
	{
		//create an entity in the scene for a light that follows the red player
		lightRedPlayer = CreateEntity();
		//add that light to the list of lights
		m_Lights.push_back(lightRedPlayer);

		//set up a trasnform for the light
		TTN_Transform lightTrans = TTN_Transform();
		lightTrans.SetPos(glm::vec3(5.5f, 0.0f, -1.2f));

		//attach that transform to the light entity
		AttachCopy<TTN_Transform>(lightRedPlayer, lightTrans);

		//set up a light component for the light
		TTN_Light lightLight = TTN_Light(glm::vec3(1.0f, 0.0f, 0.0f), 15.0f, 22.5f, 0.0f, 0.1f, 10.0f);
		//attach that light to the light entity
		AttachCopy<TTN_Light>(lightRedPlayer, lightLight);
	}

	CreateBluePlayer();
	
	//light that follows the blue player
	{
		//create an entity in the scene for a light that follows the blue player
		lightBluePlayer = CreateEntity();
		//add that light to the list of lights
		m_Lights.push_back(lightBluePlayer);

		//set up a trasnform for the light
		TTN_Transform lightTrans = TTN_Transform();
		lightTrans.SetPos(glm::vec3(-5.5f, 0.0f, -1.2f));
		//attach that transform to the light entity
		AttachCopy<TTN_Transform>(lightBluePlayer, lightTrans);

		//set up a light component for the light
		TTN_Light lightLight = TTN_Light(glm::vec3(0.0f, 0.0f, 1.0f), 15.0f, 22.5f, 0.0f, 0.1f, 10.0f);
		//attach that light to the light entity
		AttachCopy<TTN_Light>(lightBluePlayer, lightLight);
	}

	//entities for the walls
	int numOfWalls = 44;
	for (int i = 0; i < numOfWalls; i++) {
		//create an entity for the wall
		walls[i] = CreateEntity();

		//set up a meshrenderer for the wall
		Attach<TTN_Renderer>(walls[i]);
		auto& wallRenderer = Get<TTN_Renderer>(walls[i]);
		wallRenderer = TTN_Renderer(cubeMesh, shaderProgram);
		wallRenderer.SetMat(wallMat);
		
		//setup a transform for the wall
		Attach<TTN_Transform>(walls[i]);
		auto& wallTrans = Get<TTN_Transform>(walls[i]);
		//use ifs to go through each wall
		if (i < 7) {
			//top wall
			wallTrans.SetPos(glm::vec3(6.0f, 4.5f, -1.0f) - glm::vec3(2.0f, 0.0f, 0.0f) * (float)i);
			wallTrans.SetScale(glm::vec3(1.0f, 1.0f, 0.3f));

		}
		else if (i >= 7 && i < 14) {
			//bottom wall
			wallTrans.SetPos(glm::vec3(6.0f, -4.5f, -1.0f) - glm::vec3(2.0f, 0.0f, 0.0f) * (float)(i - 7));
			wallTrans.SetScale(glm::vec3(1.0f, 1.0f, 0.3f));
		}
		else if (i >= 14 && i < 19) {
			//left wall
			wallTrans.SetPos(glm::vec3(8.0f, 3.8f, -1.0f) - glm::vec3(0.0f, 2.0f, 0.0f) * (float)(i - 14));
			wallTrans.SetScale(glm::vec3(1.0f, 1.0f, 0.3f));
		}
		else if (i >= 19 && i < 24) {
			//right wall
			wallTrans.SetPos(glm::vec3(-8.0f, 3.8f, -1.0f) - glm::vec3(0.0f, 2.0f, 0.0f) * (float)(i - 19));
			wallTrans.SetScale(glm::vec3(1.0f, 1.0f, 0.3f));
		}
		else if (i >= 24 && i < 28) {
			//center left top
			wallTrans.SetPos(glm::vec3(3.0f, 1.5f, -1.0f) - glm::vec3(0.4f, 0.0f, 0.0f) * (float)(i - 24));
			wallTrans.SetScale(glm::vec3(0.2f, 0.2f, 0.2f));
		}
		else if (i >= 28 && i < 32) {
			//center left bottom
			wallTrans.SetPos(glm::vec3(3.0f, -1.5f, -1.0f) - glm::vec3(0.4f, 0.0f, 0.0f) * (float)(i - 28));
			wallTrans.SetScale(glm::vec3(0.2f, 0.2f, 0.2f));
		}
		else if (i >= 32 && i < 36) {
			//center right top
			wallTrans.SetPos(glm::vec3(-3.0f, 1.5f, -1.0f) + glm::vec3(0.4f, 0.0f, 0.0f) * (float)(i - 32));
			wallTrans.SetScale(glm::vec3(0.2f, 0.2f, 0.2f));
		}
		else if (i >= 36 && i < 40) {
			//center right bottom
			wallTrans.SetPos(glm::vec3(-3.0f, -1.5f, -1.0f) + glm::vec3(0.4f, 0.0f, 0.0f) * (float)(i - 36));
			wallTrans.SetScale(glm::vec3(0.2f, 0.2f, 0.2f));
		}
		else if (i == 40) {
			wallTrans.SetPos(glm::vec3(3.0f, 1.1f, -1.0f));
			wallTrans.SetScale(glm::vec3(0.2f, 0.2f, 0.2f));
		}
		else if (i == 41) {
			wallTrans.SetPos(glm::vec3(3.0f, -1.1f, -1.0f));
			wallTrans.SetScale(glm::vec3(0.2f, 0.2f, 0.2f));
		}
		else if (i == 42) {
			wallTrans.SetPos(glm::vec3(-3.0f, 1.1f, -1.0f));
			wallTrans.SetScale(glm::vec3(0.2f, 0.2f, 0.2f));
		}
		else if (i == 43) {
			wallTrans.SetPos(glm::vec3(-3.0f, -1.1f, -1.0f));
			wallTrans.SetScale(glm::vec3(0.2f, 0.2f, 0.2f));
		}

		//setup a tag for the wall
		Attach<TTN_Tag>(walls[i]);
		Get<TTN_Tag>(walls[i]) = TTN_Tag("wall");

		Attach<TTN_Physics>(walls[i]);
		auto& wallPhysBod = Get<TTN_Physics>(walls[i]);
		wallPhysBod = TTN_Physics(wallTrans.GetPos(), glm::vec3(0.0f), wallTrans.GetScale() + glm::vec3(0.8f), walls[i], false, 0.0f);
	}

	//red score
	{
		//create an entity for the red score
		redScoreEntity = CreateEntity();

		//set up a meshrenderer for the red score
		Attach<TTN_Renderer>(redScoreEntity);
		auto& redScoreRenderer = Get<TTN_Renderer>(redScoreEntity);
		redScoreRenderer = TTN_Renderer(scoreMesh[0], shaderProgram);
		redScoreRenderer.SetMat(redScoreMat);

		//set up a transform for the red score
		Attach<TTN_Transform>(redScoreEntity);
		auto& redScoreTrans = Get<TTN_Transform>(redScoreEntity);
		redScoreTrans.SetPos(glm::vec3(1.5f, 2.3f, -2.0f));
		redScoreTrans.SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
		redScoreTrans.RotateFixed(glm::vec3(270.0f, 0.0f, 180.0f));
	}

	//blue score
	{
		//create an entity for the blue score
		blueScoreEntity = CreateEntity();

		//set up a meshrenderer for the red score
		Attach<TTN_Renderer>(blueScoreEntity);
		auto& blueScoreRenderer = Get<TTN_Renderer>(blueScoreEntity);
		blueScoreRenderer = TTN_Renderer(scoreMesh[0], shaderProgram);
		blueScoreRenderer.SetMat(blueScoreMat);

		//set up a transform for the red score
		Attach<TTN_Transform>(blueScoreEntity);
		auto& blueScoreTrans = Get<TTN_Transform>(blueScoreEntity);
		blueScoreTrans.SetPos(glm::vec3(-1.5f, 2.3f, -2.0f));
		blueScoreTrans.SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
		blueScoreTrans.RotateFixed(glm::vec3(270.0f, 0.0f, 180.0f));
	}

	//red expolsion
	{
		redExpolsion = CreateEntity();

		//set up a meshrenderer for the red expolsion
		Attach<TTN_Renderer>(redExpolsion);
		auto& redExplodeRenderer = Get<TTN_Renderer>(redExpolsion);
		redExplodeRenderer = TTN_Renderer(expolsionMesh, shaderProgram);
		redExplodeRenderer.SetMat(expolsionMat);

		//set up a transform for the red expolsion
		Attach<TTN_Transform>(redExpolsion);
		auto& redExplodeTrans = Get<TTN_Transform>(redExpolsion);
		redExplodeTrans.SetPos(glm::vec3(100.f, 100.f, -1.0f));
		redExplodeTrans.SetScale(glm::vec3(0.05f));
		redExplodeTrans.RotateFixed(glm::vec3(270.0f, 0.0f, 0.0f));
	}

	//blue expolsion
	{
		blueExpolsion = CreateEntity();

		//set up a meshrenderer for the red expolsion
		Attach<TTN_Renderer>(blueExpolsion);
		auto& blueExplodeRenderer = Get<TTN_Renderer>(blueExpolsion);
		blueExplodeRenderer = TTN_Renderer(expolsionMesh, shaderProgram);
		blueExplodeRenderer.SetMat(expolsionMat);

		//set up a transform for the red expolsion
		Attach<TTN_Transform>(blueExpolsion);
		auto& blueExplodeTrans = Get<TTN_Transform>(blueExpolsion);
		blueExplodeTrans.SetPos(glm::vec3(100.f, 100.f, -1.0f));
		blueExplodeTrans.SetScale(glm::vec3(0.05f));
		blueExplodeTrans.RotateFixed(glm::vec3(270.0f, 0.0f, 0.0f));
	}

	//not working??
	//parent the light to the red player so it follows the red player
	Get<TTN_Transform>(lightRedPlayer).SetParent(&Get<TTN_Transform>(redPlayer), &redPlayer);
	//parent the light to the blue player so it follows the red player
	Get<TTN_Transform>(lightBluePlayer).SetParent(&Get<TTN_Transform>(bluePlayer), &bluePlayer);
}

//sets up other data the scene needs
void TanksScene::InitOtherData()
{
	//how much force should be applied to the player when they first start moving
	playerSpeed = 1.0f;

	//the directions the players are facing
	redPlayerDir = glm::vec2(-1.0f, 0.0f);
	bluePlayerDir = glm::vec2(1.0f, 0.0f);

	//the score for each player
	scoreRed = 0;
	scoreBlue = 0;

	//controls for the bullets of both tanks firing
	RedBufferUntilNextBounce = 0.0f; //control to prevent the cannonballs' bouncing to registr several times for a single bounce
	BlueBufferUntilNextBounce = 0.0f; 
	bounceCountR = 0; //tracks the number of times each ball has bounced
	bounceCountB = 0; 

	//screenshake control
	originalCamPos = Get<TTN_Transform>(camera).GetPos(); //base position of the camera
	shakeDuration = 0.0f; //change this to a positive value when you want the screen to shake
	shakeMagnitude = 0.2f; //control for how powerful the shake should be

	//expolsion control
	originalExpolsionPos = glm::vec3(100.0f, 100.0f, -1.0f); //where to hide the expolsions, out of the camera's sight
	redExpolsionDuration = 0.0f; //set this to a positve value when you want the expolsion to appear
	blueExpolsionDuration = 0.0f; //set this to a positive value when you want the expolsion to appear

	//set random seed
	srand(time(NULL));

	//set bullet existing
	redBulletExists = false;
	blueBulletExists = false;
}

void TanksScene::CreateRedBullet()
{
	//red bullet
	{
		redBullet = CreateEntity();

		//set up a meshrenderer for the red bullet
		Attach<TTN_Renderer>(redBullet);
		auto& redRenderer = Get<TTN_Renderer>(redBullet);
		redRenderer = TTN_Renderer(cannonballMesh, shaderProgram);
		redRenderer.SetMat(redTankMat);

		//set up a transform for the red bullet
		Attach<TTN_Transform>(redBullet);
		auto& redTransB = Get<TTN_Transform>(redBullet);
		redTransB.SetPos(Get<TTN_Transform>(redPlayer).GetPos());
		redTransB.SetScale(glm::vec3(0.1f / 3.0f));
		redTransB.RotateFixed(glm::vec3(270.0f, 0.0f, 90.0f));

		//Tag for the bullet
		Attach<TTN_Tag>(redBullet);
		Get<TTN_Tag>(redBullet) = TTN_Tag("bullet");

		//physics body for bullet
		Attach<TTN_Physics>(redBullet);
		auto& redPhysBodB = Get<TTN_Physics>(redBullet);
		redPhysBodB = TTN_Physics(redTransB.GetPos() + glm::vec3(glm::normalize(redPlayerDir).x, glm::normalize(redPlayerDir).y, 0.0f), glm::vec3(0.0f), glm::vec3(0.1f), redBullet);
		redPhysBodB.AddImpulse(glm::vec3(glm::normalize(redPlayerDir).x * playerSpeed, glm::normalize(redPlayerDir).y * playerSpeed, 0.0f));
	}

	redBulletExists = true;
}

void TanksScene::CreateBlueBullet()
{
	//blue bullet
	{
		blueBullet = CreateEntity();

		//set up a meshrenderer for the red bullet
		Attach<TTN_Renderer>(blueBullet);
		auto& blueRenderer = Get<TTN_Renderer>(blueBullet);
		blueRenderer = TTN_Renderer(cannonballMesh, shaderProgram);
		blueRenderer.SetMat(blueTankMat);

		//set up a transform for the red bullet
		Attach<TTN_Transform>(blueBullet);
		auto& blueTransB = Get<TTN_Transform>(blueBullet);
		blueTransB.SetPos(Get<TTN_Transform>(bluePlayer).GetPos());
		blueTransB.SetScale(glm::vec3(0.1f / 3.0f));
		blueTransB.RotateFixed(glm::vec3(270.0f, 0.0f, 90.0f));

		//Tag for the bullet
		Attach<TTN_Tag>(blueBullet);
		Get<TTN_Tag>(blueBullet) = TTN_Tag("bullet");

		//physics body for bullet
		Attach<TTN_Physics>(blueBullet);
		auto& bluePhysBodB = Get<TTN_Physics>(blueBullet);
		bluePhysBodB = TTN_Physics(blueTransB.GetPos() + glm::vec3(glm::normalize(bluePlayerDir).x, glm::normalize(bluePlayerDir).y, 0.0f), glm::vec3(0.0f), glm::vec3(0.1f), blueBullet);
		bluePhysBodB.AddImpulse(glm::vec3(glm::normalize(bluePlayerDir).x * playerSpeed, glm::normalize(bluePlayerDir).y * playerSpeed, 0.0f));
	}

	blueBulletExists = true;
}

void TanksScene::CreateRedPlayer()
{
	//red player
	{
		//create an entity for the red tank player
		redPlayer = CreateEntity();

		//set up a meshrenderer for the red player
		Attach<TTN_Renderer>(redPlayer);
		auto& redRenderer = Get<TTN_Renderer>(redPlayer);
		redRenderer = TTN_Renderer(tankMesh, shaderProgram);
		redRenderer.SetMat(redTankMat);

		//set up a transform for the red player
		Attach<TTN_Transform>(redPlayer);
		auto& redTrans = Get<TTN_Transform>(redPlayer);
		redTrans.SetPos(glm::vec3(4.5f, 0.0f, -1.0f));
		redTrans.SetScale(glm::vec3(0.1f));
		redTrans.RotateFixed(glm::vec3(270.0f, 0.0f, 90.0f));

		//Tag for the red player
		Attach<TTN_Tag>(redPlayer);
		Get<TTN_Tag>(redPlayer) = TTN_Tag("red tank");

		//setup a physics body for the red player
		//Attach<TTN_Physics>(redPlayer);
		TTN_Physics redPhysBod = TTN_Physics(redTrans.GetPos(), glm::vec3(0.0f), glm::vec3(0.40f, 0.30f, 0.30f), redPlayer);
		AttachCopy<TTN_Physics>(redPlayer, redPhysBod);
	}
}

void TanksScene::CreateBluePlayer()
{
	//blue player
	{
		//create an entity for the blue tank player
		bluePlayer = CreateEntity();

		//set up a meshrenderer for the blue player
		Attach<TTN_Renderer>(bluePlayer);
		auto& blueRenderer = Get<TTN_Renderer>(bluePlayer);
		blueRenderer = TTN_Renderer(tankMesh, shaderProgram);
		blueRenderer.SetMat(blueTankMat);

		//set up a transform for the blue player
		Attach<TTN_Transform>(bluePlayer);
		auto& blueTrans = Get<TTN_Transform>(bluePlayer);
		blueTrans.SetPos(glm::vec3(-4.5f, 0.0f, -1.0f));
		blueTrans.SetScale(glm::vec3(0.1f));
		blueTrans.RotateFixed(glm::vec3(-90.0f, 0.0f, -90.0f));

		//Tag for the blue player
		Attach<TTN_Tag>(bluePlayer);
		Get<TTN_Tag>(bluePlayer) = TTN_Tag("blue tank");

		//setup a physics body for the blue player
		TTN_Physics bluePhysBod = TTN_Physics(blueTrans.GetPos(), glm::vec3(0.0f), glm::vec3(0.40f, 0.30f, 0.30f), bluePlayer);
		AttachCopy<TTN_Physics>(bluePlayer, bluePhysBod);
	}
}