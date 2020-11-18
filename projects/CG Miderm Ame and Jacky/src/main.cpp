//INFR 1350 Pratical Midterm - Battle Tanks by Ame Gilham (100741352) and Jackie Zhou (100750922)
//The project makes use of the Titan Engine, the engine we developed together through this class and GDW (team: Atlas X Games)
//all classes with the prefix TTN_ are classes from the Titan engine

//include titan headers
#include "Titan/Application.h"
#include "Titan/ObjLoader.h"
#include "Titan/Scene.h"
#include "Player.h"
#include "iostream"
//include other features
#include <GLM/gtx/rotate_vector.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include "glm/ext.hpp"

using namespace Titan;

//main function runs the program
int main() { 
	//set up logging
	Logger::Init();
	//set up the window through Titan
	TTN_Application::Init("CG midterm, Ame Gilham and Jackie Zhou", 1920, 1080);

	//random number generation seed
	srand(time(NULL));

	//create a shader program for textured objects
	TTN_Shader::sshptr shaderProgram = TTN_Shader::Create();
	//load in some of titan's default shaders
	shaderProgram->LoadDefaultShader(TTN_DefaultShaders::VERT_NO_COLOR);
	shaderProgram->LoadDefaultShader(TTN_DefaultShaders::FRAG_BLINN_PHONG_ALBEDO_ONLY);
	shaderProgram->Link();

	//Load in all our meshes
	TTN_Mesh::smptr tankMesh = TTN_ObjLoader::LoadFromFile("tank.obj"); // tank
	tankMesh->SetUpVao();
	TTN_Mesh::smptr cannonballMesh = TTN_ObjLoader::LoadFromFile("cannonball.obj"); // cannonball
	cannonballMesh->SetUpVao();
	TTN_Mesh::smptr planeMesh = TTN_ObjLoader::LoadFromFile("plane.obj"); //plane mesh
	planeMesh->SetUpVao();
	TTN_Mesh::smptr cubeMesh = TTN_ObjLoader::LoadFromFile("wall.obj"); // cube mesh for walls
	cubeMesh->SetUpVao();
	TTN_Mesh::smptr expolsionMesh = TTN_ObjLoader::LoadFromFile("expolsion.obj"); //mesh for the 2D expolsion effect
	expolsionMesh->SetUpVao();

	//score meshes
	TTN_Mesh::smptr scoreMesh[10];
	for (int i = 0; i < 10; i++) {
		std::string fileName = std::to_string(i) + ".obj";
		scoreMesh[i] = TTN_ObjLoader::LoadFromFile(fileName);
		scoreMesh[i]->SetUpVao();
	}

	//Load in all our textures
	TTN_Texture::stptr redTankText = TTN_Texture::Create();
	redTankText->LoadFromFile("red_tank.png"); //red tank and cannonball
	TTN_Texture::stptr blueTankText = TTN_Texture::Create();
	blueTankText->LoadFromFile("blue_tank.png"); //blue tank and cannonball
	TTN_Texture::stptr backgroundText = TTN_Texture::Create();
	backgroundText->LoadFromFile("background.png"); //background
	TTN_Texture::stptr wallText = TTN_Texture::Create();
	wallText->LoadFromFile("wall.png"); //wall 
	TTN_Texture::stptr redScoreText = TTN_Texture::Create();
	redScoreText->LoadFromFile("score_red.png"); //texture for the score meshes for the red player
	TTN_Texture::stptr blueScoreText = TTN_Texture::Create();
	blueScoreText->LoadFromFile("score_blue.png"); //texture for the score meshes for the blue player
	TTN_Texture::stptr expolsionText = TTN_Texture::Create();
	expolsionText->LoadFromFile("expolsion.png"); //texture for the expolsions when a tank is hit


	//Create our materials
	TTN_Material::smatptr redTankMat = TTN_Material::Create(); //red tank and cannonball
	redTankMat->SetShininess(64.0f);
	TTN_Material::smatptr blueTankMat = TTN_Material::Create(); //blue tank and cannonball
	blueTankMat->SetShininess(64.0f);
	TTN_Material::smatptr backgroundMat = TTN_Material::Create(); //background
	backgroundMat->SetShininess(128.0f);
	TTN_Material::smatptr wallMat = TTN_Material::Create(); //walls
	wallMat->SetShininess(128.0f);
	TTN_Material::smatptr redScoreMat = TTN_Material::Create(); //red's score
	redScoreMat->SetShininess(128.0f);
	TTN_Material::smatptr blueScoreMat = TTN_Material::Create(); //blue's score
	blueScoreMat->SetShininess(128.0f);
	TTN_Material::smatptr expolsionMat = TTN_Material::Create(); //expolsion effect
	expolsionMat->SetShininess(2.0f);

	
	//Link our textures and materials together so we can link them to the mesh renderer components later
	redTankMat->SetAlbedo(redTankText);
	blueTankMat->SetAlbedo(blueTankText);
	backgroundMat->SetAlbedo(backgroundText);
	wallMat->SetAlbedo(wallText);
	redScoreMat->SetAlbedo(redScoreText);
	blueScoreMat->SetAlbedo(blueScoreText);
	expolsionMat->SetAlbedo(expolsionText);

	//Create a new scene
	TTN_Scene tanksScene = TTN_Scene(glm::vec3(1.0f, 1.0f, 1.0f), 0.5f);

	//entity for the camera in tanksScene
	entt::entity camera;
	{
		//create an entity in the scene
		camera = tanksScene.CreateEntity();
		//set this entity as the scene's camera
		tanksScene.SetCamEntity(camera);

		//add a camera component to it
		tanksScene.Attach<TTN_Camera>(camera);
		//add a transform component to it 
		tanksScene.Attach<TTN_Transform>(camera);

		//get a reference to that transform component and set it up
		auto& camTrans = tanksScene.Get<TTN_Transform>(camera);
		camTrans.SetPos(glm::vec3(0.0f, 0.0f, -5.0f));
		camTrans.LookAlong(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		camTrans.RotateFixed(glm::vec3(0.0f, 0.0f, 180.0f));

		//setup an orthographic projection for the camera
		//tanksScene.Get<TTN_Camera>(camera).CalcOrtho(-1920/200, 1920/200, -1080/200, 1080/200, 0.01, 1000);
		tanksScene.Get<TTN_Camera>(camera).CalcPerspective(80.0f, 1920.0f/1080.0f, 0.01, 1000.0f);
		//setup the view for the camera 
		tanksScene.Get<TTN_Camera>(camera).View();
	}

	//entity for the light in the scene
	entt::entity light;
	{
		//create an entity in the scene for a light
		light = tanksScene.CreateEntity();
		//set that entity as the main light for the scene
		tanksScene.m_Lights.push_back(light);

		//set up a trasnform for the light
		TTN_Transform lightTrans = TTN_Transform();
		lightTrans.SetPos(glm::vec3(0.0f, 2.0f, -2.0f));
		//attach that transform to the light entity
		tanksScene.AttachCopy<TTN_Transform>(light, lightTrans);

		//set up a light component for the light
		TTN_Light lightLight = TTN_Light(glm::vec3(1.0f), 0.5f, 500.0f, 0.1f, 0.03f, 0.06f);
		//attach that light to the light entity
		tanksScene.AttachCopy<TTN_Light>(light, lightLight);
	}

	//entity for the plane in the background
	entt::entity backgroundPlane;
	{ 
		//create an entity for the background plane in the scene
		backgroundPlane = tanksScene.CreateEntity();

		//setup a mesh renderer for the plane
		TTN_Renderer bgRenderer = TTN_Renderer(planeMesh, shaderProgram);
		//attach the material to the renderer
		bgRenderer.SetMat(backgroundMat);
		//attach the renderer to the entity in the ecs
		tanksScene.AttachCopy<TTN_Renderer>(backgroundPlane, bgRenderer);

		//setup a transform for the plane
		TTN_Transform bgTrans = TTN_Transform();
		bgTrans.SetPos(glm::vec3(0.0f));
		bgTrans.SetScale(glm::vec3(15.0f, 0.0f, 8.0f));
		bgTrans.RotateFixed(glm::vec3(270.0f, 0.0f, 0.0f));
		//attach the transform to the entity in the ecs
		tanksScene.AttachCopy<TTN_Transform>(backgroundPlane, bgTrans);
	}

	//entity for the red player
	entt::entity redPlayer; 
	{
		//create an entity for the red tank player
		redPlayer = tanksScene.CreateEntity();

		//set up a meshrenderer for the red player
		tanksScene.Attach<TTN_Renderer>(redPlayer);
		auto& redRenderer = tanksScene.Get<TTN_Renderer>(redPlayer);
		redRenderer = TTN_Renderer(tankMesh, shaderProgram);
		redRenderer.SetMat(redTankMat);

		//set up a transform for the red player
		tanksScene.Attach<TTN_Transform>(redPlayer);
		auto& redTrans = tanksScene.Get<TTN_Transform>(redPlayer);
		redTrans.SetPos(glm::vec3(4.5f, 0.0f, -1.0f));
		redTrans.SetScale(glm::vec3(0.1f));
		redTrans.RotateFixed(glm::vec3(270.0f, 0.0f, 90.0f));

		//set up a player data component for the red player
		tanksScene.Attach<PlayerComponent>(redPlayer);
		auto& redplayComp = tanksScene.Get<PlayerComponent>(redPlayer);

		//make the direction face the same as the tank
		glm::vec4 newDirection = glm::vec4(redplayComp.GetDirection().x, redplayComp.GetDirection().y, redplayComp.GetDirection().z, 1.0f);
		newDirection = redTrans.GetMatrix() * newDirection;
		//and save that direction
		redplayComp.SetDirection(glm::vec3(newDirection.x, newDirection.y, newDirection.z));

		//setup a physics body for the red player
		//tanksScene.Attach<TTN_Physics>(redPlayer);
		//TTN_Physics& physicBod = tanksScene.Get<TTN_Physics>(redPlayer);
		//physicBod = TTN_Physics(redTrans.GetPos(), glm::vec3(0.0f), glm::vec3(0.40f, 0.30f, 0.30f));
	}

	//entity for the blue player
	entt::entity bluePlayer;
	{
		//create an entity for the blue tank player
		bluePlayer = tanksScene.CreateEntity();

		//set up a meshrenderer for the blue player
		tanksScene.Attach<TTN_Renderer>(bluePlayer);
		auto& blueRenderer = tanksScene.Get<TTN_Renderer>(bluePlayer);
		blueRenderer = TTN_Renderer(tankMesh, shaderProgram);
		blueRenderer.SetMat(blueTankMat);

		//set up a transform for the blue player
		tanksScene.Attach<TTN_Transform>(bluePlayer);
		auto& blueTrans = tanksScene.Get<TTN_Transform>(bluePlayer);
		blueTrans.SetPos(glm::vec3(-4.5f, 0.0f, -1.0f));
		blueTrans.SetScale(glm::vec3(0.1f));
		blueTrans.RotateFixed(glm::vec3(270.0f, 0.0f, 270.0f));

		//set up a player data component for the blue player
		tanksScene.Attach<PlayerComponent>(bluePlayer);
		auto& blueplayComp = tanksScene.Get<PlayerComponent>(bluePlayer);
		//make the direction face the same as the tank
		glm::vec4 newDirection = glm::vec4(blueplayComp.GetDirection().x, blueplayComp.GetDirection().y, blueplayComp.GetDirection().z, 1.0f);
		newDirection = blueTrans.GetMatrix() * newDirection;
		//and save that direction
		blueplayComp.SetDirection(glm::vec3(newDirection.x, newDirection.y, newDirection.z));

		//setup a physics body for the blue player
		//tanksScene.Attach<TTN_Physics>(bluePlayer);
		//TTN_Physics& physicBod = tanksScene.Get<TTN_Physics>(bluePlayer);
		//physicBod = TTN_Physics(blueTrans.GetPos(), glm::vec3(0.0f), glm::vec3(0.40f, 0.30f, 0.30f));
	}

	//entities for the walls
	int numOfWalls = 44;
	entt::entity walls[44];
	for (int i = 0; i < numOfWalls; i++) {
		//create an entity for the wall
		walls[i] = tanksScene.CreateEntity();

		//set up a meshrenderer for the wall
		tanksScene.Attach<TTN_Renderer>(walls[i]);
		auto& wallRenderer = tanksScene.Get<TTN_Renderer>(walls[i]);
		wallRenderer = TTN_Renderer(cubeMesh, shaderProgram);
		wallRenderer.SetMat(wallMat);

		//setup a transform for the wall
		tanksScene.Attach<TTN_Transform>(walls[i]);
		auto& wallTrans = tanksScene.Get<TTN_Transform>(walls[i]);
		//use ifs to go through each wall
		if (i < 7) {
			//top wall
			wallTrans.SetPos(glm::vec3(6.0f, 4.5f, -1.0f) - glm::vec3(2.0f, 0.0f, 0.0f) * (float)i);
			wallTrans.SetScale(glm::vec3(1.0f, 1.0f, 0.3f));

		}
		else if (i >= 7 && i < 14) {
			//bottom wall
			wallTrans.SetPos(glm::vec3(6.0f, -4.5f, -1.0f) - glm::vec3(2.0f, 0.0f, 0.0f) * (float)(i-7));
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

		//setup a physics body for the wall
		tanksScene.Attach<TTN_Physics>(walls[i]);
		//TTN_Physics &physicBod = tanksScene.Get<TTN_Physics>(walls[i]);
		//physicBod = TTN_Physics(wallTrans.GetPos(), glm::vec3(0.0f), wallTrans.GetScale() + glm::vec3(0.0f, 0.0f ,0.001f));
		//physicBod.SetIsStaticBody(true); //make it static as the walls shouldn't move
	}

	//score display for the red player
	entt::entity redScoreEntity;
	{
		//create an entity for the red score
		redScoreEntity = tanksScene.CreateEntity();

		//set up a meshrenderer for the red score
		tanksScene.Attach<TTN_Renderer>(redScoreEntity);
		auto& redScoreRenderer = tanksScene.Get<TTN_Renderer>(redScoreEntity);
		redScoreRenderer = TTN_Renderer(scoreMesh[0], shaderProgram);
		redScoreRenderer.SetMat(redScoreMat);

		//set up a transform for the red score
		tanksScene.Attach<TTN_Transform>(redScoreEntity);
		auto& redScoreTrans = tanksScene.Get<TTN_Transform>(redScoreEntity);
		redScoreTrans.SetPos(glm::vec3(1.5f, 2.3f, -2.0f));
		redScoreTrans.SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
		redScoreTrans.RotateFixed(glm::vec3(270.0f, 0.0f, 180.0f));
	}

	//score display for the blue player
	entt::entity blueScoreEntity;
	{
		//create an entity for the blue score
		blueScoreEntity = tanksScene.CreateEntity();

		//set up a meshrenderer for the red score
		tanksScene.Attach<TTN_Renderer>(blueScoreEntity);
		auto& blueScoreRenderer = tanksScene.Get<TTN_Renderer>(blueScoreEntity);
		blueScoreRenderer = TTN_Renderer(scoreMesh[0], shaderProgram);
		blueScoreRenderer.SetMat(blueScoreMat);

		//set up a transform for the red score
		tanksScene.Attach<TTN_Transform>(blueScoreEntity);
		auto& blueScoreTrans = tanksScene.Get<TTN_Transform>(blueScoreEntity);
		blueScoreTrans.SetPos(glm::vec3(-1.5f, 2.3f, -2.0f));
		blueScoreTrans.SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
		blueScoreTrans.RotateFixed(glm::vec3(270.0f, 0.0f, 180.0f));
	}

	//entity for the red tank's projectiles
	entt::entity redBullet;
	{
		redBullet = tanksScene.CreateEntity();

		//set up a meshrenderer for the red bullet
		tanksScene.Attach<TTN_Renderer>(redBullet);
		auto& redRenderer = tanksScene.Get<TTN_Renderer>(redBullet);
		redRenderer = TTN_Renderer(cannonballMesh, shaderProgram);
		redRenderer.SetMat(redTankMat);

		//set up a transform for the red bullet
		tanksScene.Attach<TTN_Transform>(redBullet);
		auto& redTransB = tanksScene.Get<TTN_Transform>(redBullet);
		redTransB.SetPos(glm::vec3(100.f, 100.f, -1.0f));
		redTransB.SetScale(glm::vec3(0.1f / 3.0f));
		redTransB.RotateFixed(glm::vec3(270.0f, 0.0f, 90.0f));

		//physics body for bullet
		//TTN_Physics physicBod = TTN_Physics(redTransB.GetPos(), glm::vec3(0.0f), glm::vec3(0.10f, 0.10f, 0.10f));
		//tanksScene.AttachCopy<TTN_Physics>(redBullet, physicBod);
	}

	//entity for the blue tank's projectiles
	entt::entity blueBullet;
	{
		blueBullet = tanksScene.CreateEntity();

		//set up a meshrenderer for the red bullet
		tanksScene.Attach<TTN_Renderer>(blueBullet);
		auto& blueRenderer = tanksScene.Get<TTN_Renderer>(blueBullet);
		blueRenderer = TTN_Renderer(cannonballMesh, shaderProgram);
		blueRenderer.SetMat(blueTankMat);

		//set up a transform for the red bullet
		tanksScene.Attach<TTN_Transform>(blueBullet);
		auto& blueTransB = tanksScene.Get<TTN_Transform>(blueBullet);
		blueTransB.SetPos(glm::vec3(-100.f, 100.f, -1.0f));
		blueTransB.SetScale(glm::vec3(0.1f / 3.0f));
		blueTransB.RotateFixed(glm::vec3(270.0f, 0.0f, 90.0f));

		//physics body for bullet
		//TTN_Physics physicBod = TTN_Physics(blueTransB.GetPos(), glm::vec3(0.0f), glm::vec3(0.10f, 0.10f, 0.10f));
		//tanksScene.AttachCopy<TTN_Physics>(blueBullet, physicBod);
	}

	//entity for the red tank expolding
	entt::entity redExpolsion;
	{
		redExpolsion = tanksScene.CreateEntity();

		//set up a meshrenderer for the red expolsion
		tanksScene.Attach<TTN_Renderer>(redExpolsion);
		auto& redExplodeRenderer = tanksScene.Get<TTN_Renderer>(redExpolsion);
		redExplodeRenderer = TTN_Renderer(expolsionMesh, shaderProgram);
		redExplodeRenderer.SetMat(expolsionMat);

		//set up a transform for the red expolsion
		tanksScene.Attach<TTN_Transform>(redExpolsion);
		auto& redExplodeTrans = tanksScene.Get<TTN_Transform>(redExpolsion);
		redExplodeTrans.SetPos(glm::vec3(100.f, 100.f, -1.0f));
		redExplodeTrans.SetScale(glm::vec3(0.1f));
		redExplodeTrans.RotateFixed(glm::vec3(270.0f, 0.0f, 0.0f));
	}

	//entity for the blue tank expolding
	entt::entity blueExpolsion;
	{
		blueExpolsion = tanksScene.CreateEntity();

		//set up a meshrenderer for the red expolsion
		tanksScene.Attach<TTN_Renderer>(blueExpolsion);
		auto& blueExplodeRenderer = tanksScene.Get<TTN_Renderer>(blueExpolsion);
		blueExplodeRenderer = TTN_Renderer(expolsionMesh, shaderProgram);
		blueExplodeRenderer.SetMat(expolsionMat);

		//set up a transform for the red expolsion
		tanksScene.Attach<TTN_Transform>(blueExpolsion);
		auto& blueExplodeTrans = tanksScene.Get<TTN_Transform>(blueExpolsion);
		blueExplodeTrans.SetPos(glm::vec3(100.f, 100.f, -1.0f));
		blueExplodeTrans.SetScale(glm::vec3(0.1f));
		blueExplodeTrans.RotateFixed(glm::vec3(270.0f, 0.0f, 0.0f));
	}

	//add the scene to the application
	TTN_Application::scenes.push_back(&tanksScene);
	//set the background to a green
	TTN_Application::SetClearColor(glm::vec4(0.0f, 0.2f, 0.0f, 1.0f));
	
	//speed of the players 
	float playerSpeed = 50.0f;

	//the directions the players are facing
	glm::vec2 redPlayerDir = glm::vec2(-1.0f, 0.0f);
	glm::vec2 bluePlayerDir = glm::vec2(1.0f, 0.0f);

	//the score of each player
	int scoreRed = 0;
	int scoreBlue = 0;

	//controls for the canoonballs of both tanks firing
	float RedBufferUntilNextBounce = 0.0f; //control to prevent the cannonballs' bouncing to registry several times for a single bounce
	float BlueBufferUntilNextBounce = 0.0f;
	int bounceCountR = 0;//tracks the number of times each cannonball has bounced
	int bounceCountB = 0;

	//screenshake control
	//original camera pos
	glm::vec3 originalCamPos = tanksScene.Get<TTN_Transform>(camera).GetPos();
	float shakeDuration = 0.0f;
	float shakeMagnitude = 0.02f;

	//expolsion control
	glm::vec3 originalExpolsionPos = glm::vec3(100.f, 100.f, -1.0f);
	float redExpolsionDuration = 0.0f;
	float blueExpolsionDuration = 0.0f;

	//main game loop
	while (!TTN_Application::GetIsClosing()) {
		//get the change in time for the frame
		float dt = TTN_Application::GetDeltaTime();

		//Cannonball update
		RedBufferUntilNextBounce -= dt;
		BlueBufferUntilNextBounce -= dt;

		/// Expolsion effect ///
		//red
		if (redExpolsionDuration > 0.0f) {
			auto& redExplodeTrans = tanksScene.Get<TTN_Transform>(redExpolsion);
			redExplodeTrans.SetScale(redExplodeTrans.GetScale() + glm::vec3(dt));
			redExplodeTrans.RotateFixed(glm::vec3(0.0f, 0.0f, 90.0f * dt * 4.0f));

			redExpolsionDuration -= dt;
		}
		else if (tanksScene.Get<TTN_Transform>(redExpolsion).GetPos() != originalExpolsionPos) {
			tanksScene.Get<TTN_Transform>(redExpolsion).SetPos(originalExpolsionPos);
			tanksScene.Get<TTN_Transform>(redExpolsion).SetScale(glm::vec3(0.1f));
		}
		//blue
		if (blueExpolsionDuration > 0.0f) {
			auto& blueExplodeTrans = tanksScene.Get<TTN_Transform>(blueExpolsion);
			blueExplodeTrans.SetScale(blueExplodeTrans.GetScale() + glm::vec3(dt));
			blueExplodeTrans.RotateFixed(glm::vec3(0.0f, 0.0f, 90.0f * dt * 4.0f));

			blueExpolsionDuration -= dt;
		}
		else if (tanksScene.Get<TTN_Transform>(blueExpolsion).GetPos() != originalExpolsionPos) {
			tanksScene.Get<TTN_Transform>(blueExpolsion).SetPos(originalExpolsionPos);
			tanksScene.Get<TTN_Transform>(blueExpolsion).SetScale(glm::vec3(0.1f));
		}

		/// Camera Shake ///

		//if the camera should be shaking
		if (shakeDuration > 0.0f) {
			//generate the random numbers
			float randomX = (2.0f * (float)(rand() / (float)(RAND_MAX))) - 1;
			float randomY = (2.0f * (float)(rand() / (float)(RAND_MAX))) - 1;
			float randomZ = (2.0f * (float)(rand() / (float)(RAND_MAX))) - 1;
			//add them to the camera's position
			tanksScene.Get<TTN_Transform>(camera).SetPos(originalCamPos + glm::vec3(randomX, randomY, randomZ) * shakeMagnitude);

			//reduce the remaining time on the screenshake
			shakeDuration -= dt;
		}
		/*
		/// RED TANK MOVEMENT /// 

		//get a reference to the red player's transform and physics body
		auto& redTrans = tanksScene.Get<TTN_Transform>(redPlayer);
		auto& redPhysBod = tanksScene.Get<TTN_Physics>(redPlayer);

		//allow the red player to rotate their tank
		if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::A)) {
			redPlayerDir = glm::rotate(redPlayerDir, glm::radians(-50.0f * dt));
			redTrans.RotateFixed(glm::vec3(0.0f, 0.0f, -50.0f * dt));
			//std::cout << /*glm::to_string (redTrans.GetRotation().z) << std::endl;
		}
		if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::D)) {
			redPlayerDir = glm::rotate(redPlayerDir, glm::radians(50.0f * dt));
			redTrans.RotateFixed(glm::vec3(0.0f, 0.0f, 50.0f * dt));
		}
		
		//redPhysBod.SetVelocity(glm::vec3(0.0f));

		//allow the player to move forwards and backwards
		if (!TTN_Application::TTN_Input::GetKey(TTN_KeyCode::A) && !TTN_Application::TTN_Input::GetKey(TTN_KeyCode::D)) {
			//if the player isn't rotating check if they're trying to move forwards or backwards
			if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::W)) {
				//if the player is pressing w, move them forwards
				//redPhysBod.SetVelocity(glm::vec3(glm::normalize(redPlayerDir).x * playerSpeed * dt, glm::normalize(redPlayerDir).y * playerSpeed * dt, 0.0f));
			}
			if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::S)) {
				//if they are pressing S move them backwards
				//redPhysBod.SetVelocity(-1.0f * glm::vec3(glm::normalize(redPlayerDir).x* playerSpeed* dt, glm::normalize(redPlayerDir).y* playerSpeed* dt, 0.0f));
			}
		}

		//red and blue bullet physics body
		auto& redPhysBodBullet = tanksScene.Get<TTN_Physics>(redBullet);
		auto& bluePhysBodBullet = tanksScene.Get<TTN_Physics>(blueBullet);

		//keycode for red player to shoot a bullet
		if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::E) && (redPhysBodBullet.GetPosition() == glm::vec3(100.f, 100.f, -1.0f))) {
			shakeDuration = 0.25f;

			auto& redTransB = tanksScene.Get<TTN_Transform>(redBullet);
			auto& redTransP = tanksScene.Get<TTN_Transform>(redPlayer);

			if (redTransP.GetRotation().z <= 42 && redTransP.GetRotation().z >= -49) {

				redTransB.SetPos(glm::vec3((redPlayerDir.x) + (redPhysBod.GetPosition().x + 0.1f),
					(redPlayerDir.y) + (redPhysBod.GetPosition().y - 0.15f), -1.0f));
			}

			else if (redTransP.GetRotation().z >= 139 || redTransP.GetRotation().z <= -139) {
				redTransB.SetPos(glm::vec3((redPlayerDir.x) + (redPhysBod.GetPosition().x - 0.15f),
					(redPlayerDir.y) + (redPhysBod.GetPosition().y + 0.04f), -1.0f));
			}

			else if (redTransP.GetRotation().z <= -50 && redTransP.GetRotation().z >= -139){
				redTransB.SetPos(glm::vec3((redPlayerDir.x) + (redPhysBod.GetPosition().x + 0.04f),
					(redPlayerDir.y) + (redPhysBod.GetPosition().y + 0.15f), -1.0f));
			}

			else {
				redTransB.SetPos(glm::vec3((redPlayerDir.x) + (redPhysBod.GetPosition().x + 0.05f),
					(redPlayerDir.y) + (redPhysBod.GetPosition().y + 0.05f), -1.0f));
			}

			redPhysBodBullet.SetPos(redTransB.GetPos());

			redPhysBodBullet.SetVelocity(glm::vec3(glm::normalize(redPlayerDir).x* playerSpeed* dt* 2.0f, 
				glm::normalize(redPlayerDir).y* playerSpeed* dt*2.0f, 0.0f));

			bounceCountR = 0;
		}


		//get a reference to the blue player's transform and physics body
		auto& blueTrans = tanksScene.Get<TTN_Transform>(bluePlayer);
		auto& bluePhysBod = tanksScene.Get<TTN_Physics>(bluePlayer);

		//blue player bullets
		if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::M) && (bluePhysBodBullet.GetPosition() == glm::vec3(-100.f, 100.f, -1.0f))) {
			shakeDuration = 0.25f;

			auto& blueTransB = tanksScene.Get<TTN_Transform>(blueBullet);
			auto& blueTransP = tanksScene.Get<TTN_Transform>(bluePlayer);

			if (blueTransP.GetRotation().z <= 42 && blueTransP.GetRotation().z >= -49) {

				blueTransB.SetPos(glm::vec3((bluePlayerDir.x) + (bluePhysBod.GetPosition().x + 0.1f),
					(bluePlayerDir.y) + (bluePhysBod.GetPosition().y - 0.15f), -1.0f));
			}

			else if (blueTransP.GetRotation().z >= 139 || blueTransP.GetRotation().z <= -139) {
				blueTransB.SetPos(glm::vec3((bluePlayerDir.x) + (bluePhysBod.GetPosition().x - 0.15f),
					(bluePlayerDir.y) + (bluePhysBod.GetPosition().y + 0.04f), -1.0f));

			}

			else if (blueTransP.GetRotation().z <= -50 && blueTransP.GetRotation().z >= -139) {
				blueTransB.SetPos(glm::vec3((bluePlayerDir.x) + (bluePhysBod.GetPosition().x + 0.04f),
					(bluePlayerDir.y) + (bluePhysBod.GetPosition().y + 0.15f), -1.0f));
			}

			else {
				blueTransB.SetPos(glm::vec3((bluePlayerDir.x) + (bluePhysBod.GetPosition().x + 0.05f),
					(bluePlayerDir.y) + (bluePhysBod.GetPosition().y + 0.05f), -1.0f));
			}

			bluePhysBodBullet.SetPos(blueTransB.GetPos());

			bluePhysBodBullet.SetVelocity(glm::vec3(glm::normalize(bluePlayerDir).x * playerSpeed * dt * 2.0f,
				glm::normalize(bluePlayerDir).y * playerSpeed * dt * 2.0f, 0.0f));

			bounceCountB = 0;
		}

		/// BLUE TANK MOVMEMENT /// 
		//allow the blue player to rotate their tank
		if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::LeftArrow)) {
			bluePlayerDir = glm::rotate(bluePlayerDir, glm::radians(-50.0f * dt));
			blueTrans.RotateFixed(glm::vec3(0.0f, 0.0f, -50.0f * dt));
		}
		if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::RightArrow)) {
			bluePlayerDir = glm::rotate(bluePlayerDir, glm::radians(50.0f * dt));
			blueTrans.RotateFixed(glm::vec3(0.0f, 0.0f, 50.0f * dt));
		}

		bluePhysBod.SetVelocity(glm::vec3(0.0f));

		//allow the player to move forwards and backwards
		if (!TTN_Application::TTN_Input::GetKey(TTN_KeyCode::LeftArrow) && !TTN_Application::TTN_Input::GetKey(TTN_KeyCode::RightArrow)) {
			//if the player isn't rotating check if they're trying to move forwards or backwards
			if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::UpArrow)) {
				//if the player is pressing the up arrow, move them forwards
				bluePhysBod.SetVelocity(glm::vec3(glm::normalize(bluePlayerDir).x * playerSpeed * dt, glm::normalize(bluePlayerDir).y * playerSpeed * dt, 0.0f));
			}
			if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::DownArrow)) {
				//if they are pressing the down arrow move them backwards
				bluePhysBod.SetVelocity(-1.0f * glm::vec3(glm::normalize(bluePlayerDir).x * playerSpeed * dt, glm::normalize(bluePlayerDir).y * playerSpeed * dt, 0.0f));
			}
		}

		/// Collision checks ///

		//tank - tank collision check
		TTN_Physics::Inter(redPhysBod, bluePhysBod);

		//collision with walls and obstacles
		for (int i = 0; i < numOfWalls; i++) {
			//get the physics body of the wall, and check against collisions with it
			auto& wallPhysBod = tanksScene.Get<TTN_Physics>(walls[i]);
			//tanks
			TTN_Physics::Inter(redPhysBod, wallPhysBod);
			TTN_Physics::Inter(bluePhysBod, wallPhysBod);

			//bullets
			if (TTN_Physics::Inter(redPhysBodBullet, wallPhysBod) && RedBufferUntilNextBounce <= 0.0f) {
				//add to the number of times this bullet has bounced
				bounceCountR++;
				//don't allow it's bounce counter to increase again until a twenth of a second has past
				RedBufferUntilNextBounce = 0.05f;
			}
			if (TTN_Physics::Inter(bluePhysBodBullet, wallPhysBod) && BlueBufferUntilNextBounce <= 0.0f) {
				//add to the number of times this bullet has bounced
				bounceCountB++;
				//don't allow it's bounce counter to increase again until a twenth of a second has past
				BlueBufferUntilNextBounce = 0.05f;
			}
		}

		/// RED BULLET COLLISIONS ////
		//if it collides with player they will be reset to spawn
		if (TTN_Physics::Inter(redPhysBodBullet, redPhysBod)) {
			//collision with red
			//hide the bullet
			redPhysBodBullet.SetPos(glm::vec3(100.f, 100.f, -1.0f));
			redPhysBodBullet.SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
			bounceCountR = 0;

			//play the expolsion effect
			tanksScene.Get<TTN_Transform>(redExpolsion).SetPos(redTrans.GetPos());
			redExpolsionDuration = 1.0f;

			//reset the position of the red tank
			redPhysBod.SetPos(glm::vec3(4.5f, 0.0f, -1.0f));
			//add a point to blue
			scoreBlue++;
			//and update blue's score display
			if(scoreBlue < 10) tanksScene.Get<TTN_Renderer>(blueScoreEntity).SetMesh(scoreMesh[scoreBlue]);

		}

		if (TTN_Physics::Inter(redPhysBodBullet, bluePhysBod)) {
			//collision with blue
			//hide the bullet
			redPhysBodBullet.SetPos(glm::vec3(100.f, 100.f, -1.0f));
			redPhysBodBullet.SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
			bounceCountR = 0;

			//play the expolsion effect
			tanksScene.Get<TTN_Transform>(blueExpolsion).SetPos(blueTrans.GetPos());
			blueExpolsionDuration = 1.0f;

			//reset the position of the blue tank
			bluePhysBod.SetPos(glm::vec3(-4.5f, 0.0f, -1.0f));
			//add a point to red
			scoreRed++;
			//and update red's score display
			if (scoreRed < 10) tanksScene.Get<TTN_Renderer>(redScoreEntity).SetMesh(scoreMesh[scoreRed]);
		}

		/// BLUE BULLET COLLISIONS ////
		//if it collides with player they will be reset to spawn
		if (TTN_Physics::Inter(bluePhysBodBullet, redPhysBod)) {
			//collision with red
			//hide the bullet
			bluePhysBodBullet.SetPos(glm::vec3(100.f, 100.f, -1.0f));
			bluePhysBodBullet.SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
			bounceCountB = 0;

			//play the expolsion effect
			tanksScene.Get<TTN_Transform>(redExpolsion).SetPos(redTrans.GetPos());
			redExpolsionDuration = 1.0f;

			//reset the position of the red tank
			redPhysBod.SetPos(glm::vec3(4.5f, 0.0f, -1.0f));
			//add a point to blue
			scoreBlue++;
			//and update blue's score display
			if (scoreBlue < 10) tanksScene.Get<TTN_Renderer>(blueScoreEntity).SetMesh(scoreMesh[scoreBlue]);
		}

		if (TTN_Physics::Inter(bluePhysBodBullet, bluePhysBod)) {
			//colision with blue
			//hide the bullet
			bluePhysBodBullet.SetPos(glm::vec3(100.f, 100.f, -1.0f));
			bluePhysBodBullet.SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
			bounceCountB = 0;

			//play the expolsion effect
			tanksScene.Get<TTN_Transform>(blueExpolsion).SetPos(blueTrans.GetPos());
			blueExpolsionDuration = 1.0f;

			//reset the position of the blue tank
			bluePhysBod.SetPos(glm::vec3(-4.5f, 0.0f, -1.0f));
			//add a point to red
			scoreRed++;
			//and update red's score display
			if (scoreRed < 10) tanksScene.Get<TTN_Renderer>(redScoreEntity).SetMesh(scoreMesh[scoreRed]);
		}

		//collsion with other bullet
		if (TTN_Physics::Inter(redPhysBodBullet, bluePhysBodBullet)) {
			//hide both bullets
			redPhysBodBullet.SetPos(glm::vec3(100.f, 100.f, -1.0f));
			redPhysBodBullet.SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
			bluePhysBodBullet.SetPos(glm::vec3(-100.f, 100.f, -1.0f));
			bluePhysBodBullet.SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
		}
		
		//if the bullet bounces for 2 times then the third time it collides it will be 'destroyed' (teleported back to base location)
		if (bounceCountR == 3) {
			redPhysBodBullet.SetPos(glm::vec3(100.f, 100.f, -1.0f));
			redPhysBodBullet.SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
		}
		if (bounceCountB == 3) {
			bluePhysBodBullet.SetPos(glm::vec3(-100.f, 100.f, -1.0f));
			bluePhysBodBullet.SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
		}



		/// Check if game is over, and reset if so /// 
		if (scoreRed > 9 || scoreBlue > 9) {
			//reset both scores
			scoreRed = 0;
			scoreBlue = 0;
			tanksScene.Get<TTN_Renderer>(redScoreEntity).SetMesh(scoreMesh[0]);
			tanksScene.Get<TTN_Renderer>(blueScoreEntity).SetMesh(scoreMesh[0]);

			//reset red's position
			redPhysBod.SetPos(glm::vec3(4.5f, 0.0f, -1.0f));
			//reset blue's position 
			bluePhysBod.SetPos(glm::vec3(-4.5f, 0.0f, -1.0f));

			//reset the positions and data about the cannonballs
			redPhysBodBullet.SetPos(glm::vec3(100.f, 100.f, -1.0f));
			redPhysBodBullet.SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
			bounceCountR = 0;
			//blue
			bluePhysBodBullet.SetPos(glm::vec3(100.f, 100.f, -1.0f));
			bluePhysBodBullet.SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
			bounceCountB = 0;
		}
			*/
		//call all the backend updates and render everything
		TTN_Application::Update();
	}

	
	return 0; 
} 