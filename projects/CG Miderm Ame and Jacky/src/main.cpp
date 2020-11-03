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
#include "glm/ext.hpp"

using namespace Titan;

//main function runs the program
int main() { 
	//set up logging
	Logger::Init();
	//set up the window through Titan
	TTN_Application::Init("CG midterm, Ame Gilham and Jackie Zhou", 1920, 1080);
	//set up the phyiscs body to render on screen so we can set them up properly
	TTN_Physics::SetUpPhysicsBoxRendering();

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

	//Load in all our textures
	TTN_Texture::stptr redTankText = TTN_Texture::Create();
	redTankText->LoadFromFile("red_tank.png");
	TTN_Texture::stptr blueTankText = TTN_Texture::Create();
	blueTankText->LoadFromFile("blue_tank.png");
	TTN_Texture::stptr backgroundText = TTN_Texture::Create();
	backgroundText->LoadFromFile("background.png");

	//Create our materials
	TTN_Material::smatptr redTankMat = TTN_Material::Create();
	redTankMat->SetShininess(32768.0f);
	TTN_Material::smatptr blueTankMat = TTN_Material::Create();
	blueTankMat->SetShininess(32768.0f);
	TTN_Material::smatptr backgroundMat = TTN_Material::Create();
	backgroundMat->SetShininess(128.0f);
	
	//Link our textures and materials together so we can link them to the mesh renderer components later
	redTankMat->SetAlbedo(redTankText);
	blueTankMat->SetAlbedo(blueTankText);
	backgroundMat->SetAlbedo(backgroundText);

	//Create a new scene
	TTN_Scene tanksScene = TTN_Scene();

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

		//setup an orthographic projection for the camera
		tanksScene.Get<TTN_Camera>(camera).CalcOrtho(-16.0f/2, 16.0f/2, -9.0f/2, 9.0f/2, 0.001f, 1000.0f);
		//tanksScene.Get<TTN_Camera>(camera).CalcPerspective(90.0f, 1.0f, 0.01f, 100.f);
		//setup the view for the camera
		tanksScene.Get<TTN_Camera>(camera).View();
	}

	//entity for the light in the scene
	entt::entity light;
	{
		//create an entity in the scene for a light
		light = tanksScene.CreateEntity();
		//set that entity as the main light for the scene
		tanksScene.SetLightEntity(light);

		//set up a trasnform for the light
		TTN_Transform lightTrans = TTN_Transform();
		lightTrans.SetPos(glm::vec3(0.0f, 1.2f, -1.0f));
		//attach that transform to the light entity
		tanksScene.AttachCopy<TTN_Transform>(light, lightTrans);

		//set up a light component for the light
		TTN_Light lightLight = TTN_Light(glm::vec3(1.0f), 0.1f, 50.0f, 0.1f, 0.3f, 0.6f);
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
		TTN_Physics physicBod = TTN_Physics(redTrans.GetPos(), glm::vec3(0.0f), glm::vec3(0.35f, 0.30f, 0.30f));
		tanksScene.AttachCopy<TTN_Physics>(redPlayer, physicBod);
		tanksScene.SetUpCollisions(redPlayer);
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
		TTN_Physics physicBod = TTN_Physics(blueTrans.GetPos(), glm::vec3(0.0f), glm::vec3(0.35f, 0.30f, 0.30f));
		tanksScene.AttachCopy<TTN_Physics>(bluePlayer, physicBod);
		tanksScene.SetUpCollisions(bluePlayer);
	}

	entt::entity redBullet;
	{
		redBullet = tanksScene.CreateEntity();

		//set up a meshrenderer for the red bullet
		tanksScene.Attach<TTN_Renderer>(redBullet);
		auto& redRenderer = tanksScene.Get<TTN_Renderer>(redBullet);
		redRenderer = TTN_Renderer(tankMesh, shaderProgram);
		redRenderer.SetMat(redTankMat);

		//set up a transform for the red bullet
		tanksScene.Attach<TTN_Transform>(redBullet);
		auto& redTransB = tanksScene.Get<TTN_Transform>(redBullet);
		redTransB.SetPos(glm::vec3(100.f, 100.f, -1.0f));
		redTransB.SetScale(glm::vec3(0.1f));
		redTransB.RotateFixed(glm::vec3(270.0f, 0.0f, 90.0f));

		//physics body for bullet
		TTN_Physics physicBod = TTN_Physics(redTransB.GetPos(), glm::vec3(0.0f), glm::vec3(0.35f, 0.30f, 0.30f));
		tanksScene.AttachCopy<TTN_Physics>(redBullet, physicBod);
	}

	entt::entity blueBullet;
	{
		blueBullet = tanksScene.CreateEntity();

		//set up a meshrenderer for the red bullet
		tanksScene.Attach<TTN_Renderer>(blueBullet);
		auto& blueRenderer = tanksScene.Get<TTN_Renderer>(blueBullet);
		blueRenderer = TTN_Renderer(tankMesh, shaderProgram);
		blueRenderer.SetMat(blueTankMat);

		//set up a transform for the red bullet
		tanksScene.Attach<TTN_Transform>(blueBullet);
		auto& blueTransB = tanksScene.Get<TTN_Transform>(blueBullet);
		blueTransB.SetPos(glm::vec3(-100.f, 100.f, -1.0f));
		blueTransB.SetScale(glm::vec3(0.1f));
		blueTransB.RotateFixed(glm::vec3(270.0f, 0.0f, 90.0f));

		//physics body for bullet
		TTN_Physics physicBod = TTN_Physics(blueTransB.GetPos(), glm::vec3(0.0f), glm::vec3(0.35f, 0.30f, 0.30f));
		tanksScene.AttachCopy<TTN_Physics>(blueBullet, physicBod);
		//tanksScene.SetUpCollisions(physicBod);
	}

	//add the scene to the application
	TTN_Application::scenes.push_back(tanksScene);
	//set the background to a green
	TTN_Application::SetClearColor(glm::vec4(0.0f, 0.2f, 0.0f, 1.0f));
	
	//speed of the players 
	float playerSpeed = 55.0f;
	glm::vec2 redPlayerDir = glm::vec2(-1.0f, 0.0f);
	glm::vec2 bluePlayerDir = glm::vec2(1.0f, 0.0f);
	int bounceCountR = 0;
	int bounceCountB = 0;

	//main game loop
	while (!TTN_Application::GetIsClosing()) {
		//get the change in time for the frame
		float dt = TTN_Application::GetDeltaTime();

		/// RED TANK MOVEMENT ///
		//get a reference to the red player's transform and physics body
		auto& redTrans = tanksScene.Get<TTN_Transform>(redPlayer);
		auto& redPhysBod = tanksScene.Get<TTN_Physics>(redPlayer);

		//allow the red player to rotate their tank
		if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::A)) {
			redPlayerDir = glm::rotate(redPlayerDir, glm::radians(-50.0f * dt));
			redTrans.RotateFixed(glm::vec3(0.0f, 0.0f, -50.0f * dt));
			//std::cout << /*glm::to_string */(redTrans.GetRotation().z) << std::endl;
		}
		if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::D)) {
			redPlayerDir = glm::rotate(redPlayerDir, glm::radians(50.0f * dt));
			redTrans.RotateFixed(glm::vec3(0.0f, 0.0f, 50.0f * dt));
		}

		redPhysBod.SetVelocity(glm::vec3(0.0f));

		//allow the player to move forwards and backwards
		if (!TTN_Application::TTN_Input::GetKey(TTN_KeyCode::A) && !TTN_Application::TTN_Input::GetKey(TTN_KeyCode::D)) {
			//if the player isn't rotating check if they're trying to move forwards or backwards
			if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::W)) {
				//if the player is pressing w, move them forwards
				redPhysBod.SetVelocity(glm::vec3(glm::normalize(redPlayerDir).x * playerSpeed * dt, glm::normalize(redPlayerDir).y * playerSpeed * dt, 0.0f));
			}
			if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::S)) {
				//if they are pressing S move them backwards
				redPhysBod.SetVelocity(-1.0f * glm::vec3(glm::normalize(redPlayerDir).x* playerSpeed* dt, glm::normalize(redPlayerDir).y* playerSpeed* dt, 0.0f));
			}
		}

		//red and blue bullet physics body
		auto& redPhysBodBullet = tanksScene.Get<TTN_Physics>(redBullet);
		auto& bluePhysBodBullet = tanksScene.Get<TTN_Physics>(blueBullet);

		//keycode for red player to shoot a bullet
		if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::E) /*&& (redPhysBodBullet.GetPosition() == glm::vec3(100.f, 100.f, -1.0f))*/) {
			auto& redTransB = tanksScene.Get<TTN_Transform>(redBullet);
			auto& redTransP = tanksScene.Get<TTN_Transform>(redPlayer);

			/*redTransB.SetPos(glm::vec3(redTransP.GetPos().x - (glm::normalize(redTransP.GetPos().x)/4.0f),
				redTransP.GetPos().y + (glm::normalize(redTransP.GetPos().y)/1.6f), -1.0f));*/

				//redTransB.SetPos(glm::vec3(redTransP.GetPos().x - (glm::normalize(redTransP.GetPos().x) ), redTransP.GetPos().y , -1.0f));

			if (redTransP.GetRotation().z <= 42 && redTransP.GetRotation().z >= -49) {

				redTransB.SetPos(glm::vec3((redPlayerDir.x) + (redPhysBod.GetPosition().x + 0.1f),
					(redPlayerDir.y) + (redPhysBod.GetPosition().y - 0.15f), -1.0f));
				std::cout << /*glm::to_string */"HELPPPPP 1 " << std::endl;

			}

			else if (redTransP.GetRotation().z >= 139 || redTransP.GetRotation().z <= -139) {
				redTransB.SetPos(glm::vec3((redPlayerDir.x) + (redPhysBod.GetPosition().x - 0.15f),
					(redPlayerDir.y) + (redPhysBod.GetPosition().y + 0.04f), -1.0f));
				std::cout << /*glm::to_string */"HELPPPPP 2" << std::endl;

			}

			else if (redTransP.GetRotation().z <= -50 && redTransP.GetRotation().z >= -139){
				redTransB.SetPos(glm::vec3((redPlayerDir.x) + (redPhysBod.GetPosition().x + 0.04f),
					(redPlayerDir.y) + (redPhysBod.GetPosition().y + 0.15f), -1.0f));
				std::cout << /*glm::to_string */"HELPPPPP 3 " << std::endl;

			}

			else {
				redTransB.SetPos(glm::vec3((redPlayerDir.x) + (redPhysBod.GetPosition().x + 0.05f),
					(redPlayerDir.y) + (redPhysBod.GetPosition().y + 0.05f), -1.0f));
				std::cout << /*glm::to_string */"HELPPPPP 4" << std::endl;
			}



			redPhysBodBullet.SetPos(redTransB.GetPos());

			redPhysBodBullet.SetVelocity(glm::vec3(glm::normalize(redPlayerDir).x* playerSpeed* dt* 2.0f, 
				glm::normalize(redPlayerDir).y* playerSpeed* dt*2.0f, 0.0f));

			//if ()
			bounceCountR = 0;
		}


		//get a reference to the blue player's transform and physics body
		auto& blueTrans = tanksScene.Get<TTN_Transform>(bluePlayer);
		auto& bluePhysBod = tanksScene.Get<TTN_Physics>(bluePlayer);

		//blue player bullets
		if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::M) /*&& (bluePhysBodBullet.GetPosition() == glm::vec3(-100.f, 100.f, -1.0f))*/) {
			auto& blueTransB = tanksScene.Get<TTN_Transform>(blueBullet);
			auto& blueTransP = tanksScene.Get<TTN_Transform>(bluePlayer);

			if (blueTransP.GetRotation().z <= 42 && blueTransP.GetRotation().z >= -49) {

				blueTransB.SetPos(glm::vec3((bluePlayerDir.x) + (bluePhysBod.GetPosition().x + 0.1f),
					(bluePlayerDir.y) + (bluePhysBod.GetPosition().y - 0.15f), -1.0f));
				std::cout << /*glm::to_string */"HELPPPPP 1 " << std::endl;

			}

			else if (blueTransP.GetRotation().z >= 139 || blueTransP.GetRotation().z <= -139) {
				blueTransB.SetPos(glm::vec3((bluePlayerDir.x) + (bluePhysBod.GetPosition().x - 0.15f),
					(bluePlayerDir.y) + (bluePhysBod.GetPosition().y + 0.04f), -1.0f));
				std::cout << /*glm::to_string */"HELPPPPP 2" << std::endl;

			}

			else if (blueTransP.GetRotation().z <= -50 && blueTransP.GetRotation().z >= -139) {
				blueTransB.SetPos(glm::vec3((bluePlayerDir.x) + (bluePhysBod.GetPosition().x + 0.04f),
					(bluePlayerDir.y) + (bluePhysBod.GetPosition().y + 0.15f), -1.0f));
				std::cout << /*glm::to_string */"HELPPPPP 3 " << std::endl;

			}

			else {
				blueTransB.SetPos(glm::vec3((bluePlayerDir.x) + (bluePhysBod.GetPosition().x + 0.05f),
					(bluePlayerDir.y) + (bluePhysBod.GetPosition().y + 0.05f), -1.0f));
				std::cout << /*glm::to_string */"HELPPPPP 4" << std::endl;
			}

			bluePhysBodBullet.SetPos(blueTransB.GetPos());

			bluePhysBodBullet.SetVelocity(glm::vec3(glm::normalize(bluePlayerDir).x * playerSpeed * dt * 2.0f,
				glm::normalize(bluePlayerDir).y * playerSpeed * dt * 2.0f, 0.0f));

			//if ()
			bounceCountB = 0;
		}

		/// RED BULLET COLLISIONS ////
		//if it collides with player they will be reset to spawn
		if (TTN_Physics::Inter(redPhysBodBullet, redPhysBod)) {
			redPhysBodBullet.SetPos(glm::vec3(100.f, 100.f, -1.0f));
			redPhysBodBullet.SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));

			redPhysBod.SetPos(glm::vec3(4.5f, 0.0f, -1.0f));
		}

		if (TTN_Physics::Inter(redPhysBodBullet, bluePhysBod)) {
			redPhysBodBullet.SetPos(glm::vec3(100.f, 100.f, -1.0f));
			redPhysBodBullet.SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));

			bluePhysBod.SetPos(glm::vec3(-4.5f, 0.0f, -1.0f));
		}

		/// BLUE BULLET COLLISIONS ////
		//if it collides with player they will be reset to spawn
		if (TTN_Physics::Inter(bluePhysBodBullet, redPhysBod)) {
			bluePhysBodBullet.SetPos(glm::vec3(100.f, 100.f, -1.0f));
			bluePhysBodBullet.SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));

			redPhysBod.SetPos(glm::vec3(4.5f, 0.0f, -1.0f));
		}

		if (TTN_Physics::Inter(bluePhysBodBullet, bluePhysBod)) {
			bluePhysBodBullet.SetPos(glm::vec3(100.f, 100.f, -1.0f));
			bluePhysBodBullet.SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));

			bluePhysBod.SetPos(glm::vec3(-4.5f, 0.0f, -1.0f));
		}

		//collsion with other bullet
		if (TTN_Physics::Inter(redPhysBodBullet, bluePhysBodBullet)) {
			redPhysBodBullet.SetPos(glm::vec3(100.f, 100.f, -1.0f));
			redPhysBodBullet.SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
			bluePhysBodBullet.SetPos(glm::vec3(-100.f, 100.f, -1.0f));
			bluePhysBodBullet.SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
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
	

		/// WALL COLLISIONS /////
		//if the bullet collides with something it will bounce and increase the counter (this should be used for walls physics body)
		if (TTN_Physics::Inter(redPhysBodBullet, bluePhysBod)) {
			bounceCountR++;
			//std::cout << bounceCountR << std::endl;
		}
		if (TTN_Physics::Inter(bluePhysBodBullet, bluePhysBod)) {
			bounceCountB++;
			//std::cout << bounceCountR << std::endl;
		}

		//if the bullet bounces for 2 times then the third time it collides it will be 'destroyed' (teleported back to base location)
		if (bounceCountR == 2) {
			redPhysBodBullet.SetPos(glm::vec3(100.f, 100.f, -1.0f));
			redPhysBodBullet.SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
		}
		if (bounceCountB == 2) {
			bluePhysBodBullet.SetPos(glm::vec3(-100.f, 100.f, -1.0f));
			bluePhysBodBullet.SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
		}



		//call all the backend updates and render everything
		TTN_Application::Update();
	}

	
	return 0; 
} 