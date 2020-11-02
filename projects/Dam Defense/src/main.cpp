#include "Titan/Application.h"
#include "Titan/Scene.h"
#include "Titan/ObjLoader.h"
#include "Titan/Renderer.h"
#include "Titan/Transform.h"
#include "Titan/Physics.h"
#include <iostream>
#include "glm/ext.hpp"

using namespace Titan;

int main() {
	Logger::Init();
	TTN_Application::Init("Dam Defense", 800, 800);
	TTN_Physics::SetUpPhysicsBoxRendering();

	//create a shader program object
	TTN_Shader::sshptr shaderProgam = TTN_Shader::Create();
	//load the shaders into the shader program 
	shaderProgam->LoadDefaultShader(TTN_DefaultShaders::VERT_NO_COLOR);
	shaderProgam->LoadDefaultShader(TTN_DefaultShaders::FRAG_BLINN_PHONG_NO_TEXTURE);
	shaderProgam->Link();

	//create a shader program object for textured objects
	TTN_Shader::sshptr shaderProgamTextured = TTN_Shader::Create();
	//load the shaders into the shader program
	shaderProgamTextured->LoadDefaultShader(TTN_DefaultShaders::VERT_NO_COLOR);
	shaderProgamTextured->LoadDefaultShader(TTN_DefaultShaders::FRAG_BLINN_PHONG_ALBEDO_ONLY);
	shaderProgamTextured->Link();

	//create mesh pointers and set up their vaos
<<<<<<< HEAD
	TTN_Mesh* boatMesh = TTN_ObjLoader::LoadFromFile("tank.obj"); // boat
=======
	TTN_Mesh::smptr boatMesh = TTN_ObjLoader::LoadFromFile("boat.obj"); // boat
>>>>>>> Ame
	boatMesh->SetUpVao();
	TTN_Mesh::smptr treeMesh = TTN_ObjLoader::LoadFromFile("tree.obj"); // tree
	treeMesh->SetUpVao();
	TTN_Mesh::smptr swordMesh = TTN_ObjLoader::LoadFromFile("Sword.obj"); //sword, texture test
	swordMesh->SetUpVao();

	//texture for the sword 
	TTN_Texture::stptr swordText = TTN_Texture::Create();
	//load the texture from a file
	swordText->LoadFromFile("Sword_Texture.png");

	//material for the sword 
	TTN_Material::smatptr swordMat = TTN_Material::Create();
	//add the texture to material and set the shininess
	swordMat->SetAlbedo(swordText);
	swordMat->SetShininess(128.0f);
	//put the sword mat in the sword mesh
	swordMesh->SetMat(swordMat);

	//create a new scene
	TTN_Scene testScene = TTN_Scene();

	//entity for the camera in testScene
	entt::entity camera;
	{
		//create an entity in the scene for the camera
		camera = testScene.CreateEntity();
		testScene.SetCamEntity(camera);
		testScene.Attach<TTN_Transform>(camera);
		testScene.Attach<TTN_Camera>(camera);
		auto& camTrans = testScene.Get<TTN_Transform>(camera);
		camTrans.SetPos(glm::vec3(0.0f, 0.0f, 0.0f));
		camTrans.SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
		camTrans.LookAlong(glm::vec3(0.0, 0.0, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		testScene.Get<TTN_Camera>(camera).CalcOrtho(-800.0f/100.0f, 800.0f/100.0f, -800.0f/100.0f, 800.0f/100.0f, 0.001f, 100.0f);
		//testScene.Get<TTN_Camera>(camera).CalcPerspective(90.0f, 1.0f, 0.01f, 100.f);
		testScene.Get<TTN_Camera>(camera).View();
	}

	//entity for the light in testScene
	entt::entity light;
	{
		//create an entity in the scene for a light
		light = testScene.CreateEntity();
		testScene.SetLightEntity(light);

		//set up a trasnform for the light
		TTN_Transform lightTrans = TTN_Transform();
		lightTrans.SetPos(glm::vec3(0.0f, 3.0f, 0.0f));
		//attach that transform to the light entity
		testScene.AttachCopy<TTN_Transform>(light, lightTrans);

		//set up a light component for the light
		TTN_Light lightLight = TTN_Light(glm::vec3(1.0f), 0.3f, 1.0f, 0.3f, 0.3f, 0.3f);
		//attach that light to the light entity
		testScene.AttachCopy<TTN_Light>(light, lightLight);
	}


	//entity for the sword in testScene
	entt::entity sword;
	{
		sword = testScene.CreateEntity();

		//setup a mesh renderer for the sword
		TTN_Renderer swordRenderer = TTN_Renderer(swordMesh, shaderProgamTextured);
		//attach that renderer to the tree entity
		testScene.AttachCopy<TTN_Renderer>(sword, swordRenderer);
		 
		//setup a transform for the first tree
		TTN_Transform swordTrans = TTN_Transform();
		swordTrans.SetPos(glm::vec3(0.0f, 0.0f, 2.0f));
		swordTrans.SetScale(glm::vec3(1.f, 1.f, 1.f));
		swordTrans.RotateFixed(glm::vec3(270.0f, 0.f, 90.0f));
		//attach that transform to the tree entity
		testScene.AttachCopy<TTN_Transform>(sword, swordTrans);
	}

	//entity for the first tree in testScene
	entt::entity tree1;
	{
		//create an entity in the scene for the first tree
		tree1 = testScene.CreateEntity();
		
		//setup a mesh renderer for the first tree
		TTN_Renderer treeRenderer = TTN_Renderer(treeMesh, shaderProgam);
		//attach that renderer to the tree entity
		testScene.AttachCopy<TTN_Renderer>(tree1, treeRenderer);

		//setup a transform for the first tree
		TTN_Transform treeTrans = TTN_Transform();
		treeTrans.SetPos(glm::vec3(-3.0f, -3.0f, 5.f));
		treeTrans.SetScale(glm::vec3(1.f, 1.f, 1.f));
		//attach that transform to the tree entity
		testScene.AttachCopy<TTN_Transform>(tree1, treeTrans);

<<<<<<< HEAD
		TTN_Physics pbody = TTN_Physics( glm::vec3(treeTrans.GetPos().x - 0.50f, treeTrans.GetPos().y - 8.0f, treeTrans.GetPos().z - 8.0f),
			glm::vec3(treeTrans.GetPos().x + 0.50f, treeTrans.GetPos().y + 8.0f, treeTrans.GetPos().z + 8.0f) );
		
		//TTN_Physics pbody = TTN_Physics(glm::vec3(treeTrans.GetPos().x , treeTrans.GetPos().y , treeTrans.GetPos().z ));

=======
		TTN_Physics pbody = TTN_Physics(treeTrans.GetPos(), glm::vec3(0.0f), glm::vec3(1.f, 1.f, 1.f));
		 
>>>>>>> Ame
		testScene.AttachCopy<TTN_Physics>(tree1, pbody);
	}

	//entity for the second tree in testScene
	entt::entity tree2;
	{
		//create an entity in the scene for the second tree
		tree2 = testScene.CreateEntity();

		//attach a mesh renderer to the second tree 
		testScene.Attach<TTN_Renderer>(tree2);
		//grab a referencce to that mesh renderer and set it up
		auto& treeRenderer = testScene.Get<TTN_Renderer>(tree2);
		treeRenderer = TTN_Renderer(treeMesh, shaderProgam);

		//attach a transform to the second tree
		testScene.Attach<TTN_Transform>(tree2);
		//grab a reference to that transform and set it up
		auto& treeTrans = testScene.Get<TTN_Transform>(tree2);
		treeTrans.SetPos(glm::vec3(2.0f, -3.0f, 3.0f));
		treeTrans.SetScale(glm::vec3(1.f, 1.f, 1.f));
	}

	//entity for the boat in testScene
	entt::entity boat;
	{
		//create an entity in the scene for the boat
		boat = testScene.CreateEntity();

		//attach a mesh renderer to the boat
		testScene.Attach<TTN_Renderer>(boat);
		//grab a referencce to that mesh renderer and set it up
		auto& boatRenderer = testScene.Get<TTN_Renderer>(boat);
		boatRenderer = TTN_Renderer(boatMesh, shaderProgam);

		//attach a transform to the boat
		testScene.Attach<TTN_Transform>(boat);
		//grab a reference to that transform and set it up
		auto& boatTrans = testScene.Get<TTN_Transform>(boat);
		boatTrans.SetPos(glm::vec3(-3.0f, -4.5f, 5.0f));
		boatTrans.RotateFixed(glm::vec3(0.0f, 270.0f, 0.0f));
		boatTrans.SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

<<<<<<< HEAD
		//TTN_Physics pbody = TTN_Physics(glm::vec3(-1.0f, -4.0f, 4.f), glm::vec3(1.0f, -2.0f, 6.f));
		TTN_Physics pbody = TTN_Physics(glm::vec3(boatTrans.GetPos().x - 1.0f, boatTrans.GetPos().y - 6.0f, boatTrans.GetPos().z - 6.0f),
			glm::vec3(boatTrans.GetPos().x + 1.0f, boatTrans.GetPos().y + 6.0f, boatTrans.GetPos().z + 6.0f));

		//TTN_Physics pbody = TTN_Physics(glm::vec3(boatTrans.GetPos().x , boatTrans.GetPos().y , boatTrans.GetPos().z));

=======
		
		TTN_Physics pbody = TTN_Physics(boatTrans.GetPos(), glm::vec3(0.0f), glm::vec3(1.f, 1.f, 1.f));
>>>>>>> Ame
		testScene.AttachCopy<TTN_Physics>(boat, pbody);
	}

	//entity for 

	//add the scene to the application
	TTN_Application::scenes.push_back(testScene);
	//set the background to a blue
	TTN_Application::SetClearColor(glm::vec4(0.0f, 0.2f, 8.0f, 1.0f));
	
	float speed = 1.0f;

	while (!TTN_Application::GetIsClosing()) {
		//get the change in time for the frame
		float dt = TTN_Application::GetDeltaTime();

		auto& treeTrans = testScene.Get<TTN_Transform>(tree1);
		//move the boat 
		auto& boatTrans = testScene.Get<TTN_Transform>(boat);
<<<<<<< HEAD
		//boatTrans.SetPos(glm::vec3(boatTrans.GetPos().x + speed * dt, boatTrans.GetPos().y, boatTrans.GetPos().z ));
		boatTrans.RotateFixed(glm::vec3(0, 1.5f * speed, 0));
		//flip the speed if it gets to a certain point
	//	if (boatTrans.GetPos().x < -5.0f || boatTrans.GetPos().x > 2.0f)
		//	speed *= -1; 

		auto& pboat = testScene.Get<TTN_Physics>(boat);
		pboat.SetMin(glm::vec3(boatTrans.GetPos().x - 0.50f, boatTrans.GetPos().y - 34.0f, boatTrans.GetPos().z - 56.0f));
		pboat.SetMax(glm::vec3(boatTrans.GetPos().x + 0.50f, boatTrans.GetPos().y + 34.0f, boatTrans.GetPos().z + 56.0f));
		//pboat.SetPos(glm::vec3(boatTrans.GetPos().x, boatTrans.GetPos().y, boatTrans.GetPos().z));

		auto& ptree = testScene.Get<TTN_Physics>(tree1);
		ptree.SetMin(glm::vec3(treeTrans.GetPos().x - 1.50f, treeTrans.GetPos().y - 12.0f, treeTrans.GetPos().z - 1.0f));
		ptree.SetMax(glm::vec3(treeTrans.GetPos().x + 1.50f, treeTrans.GetPos().y + 12.0f, treeTrans.GetPos().z + 1.0f));
		//ptree.SetPos(glm::vec3(treeTrans.GetPos().x, treeTrans.GetPos().y, treeTrans.GetPos().z));

		//TTN_Physics::Intersects(pboat, ptree);
		if (TTN_Physics::Inter(pboat, ptree)) {
			
			/*std::cout << "Touching " << (pboat.min.x <= ptree.max.x && pboat.max.x >= ptree.min.x) <<
				(pboat.min.y <= ptree.max.y && pboat.max.y >= ptree.min.y) <<
				(pboat.min.z <= ptree.max.z && pboat.max.z >= ptree.min.z)  <<std::endl;

			speed = 0;*/

		}
		else
		{
			/*std::cout << "Not Touching " << (pboat.min.x <= ptree.max.x && pboat.max.x >= ptree.min.x) <<
				(pboat.min.y <= ptree.max.y && pboat.max.y >= ptree.min.y) <<
				(pboat.min.z <= ptree.max.z && pboat.max.z >= ptree.min.z) << std::endl;*/

			//std::cout << glm::to_string(pboat.GetMin()) << std::endl;
			//std::cout << glm::to_string(pboat.GetMax()) << std::endl;
		}

		
		//rotate the second tree 
		//rotY += rotSpeed * dt;
	/*	while (rotY > 360.0f)
			rotY -= 360.f;*/
=======
		boatTrans.SetPos(glm::vec3(boatTrans.GetPos().x, boatTrans.GetPos().y + speed * dt, boatTrans.GetPos().z ));
		//flip the speed if it gets to a certain point
		if (boatTrans.GetPos().y < -5.0f || boatTrans.GetPos().y > 2.0f)
			speed *= -1;
>>>>>>> Ame

		auto& tree2Trans = testScene.Get<TTN_Transform>(tree2);
		tree2Trans.RotateFixed(glm::vec3(0, 5.0f * dt, 0));

<<<<<<< HEAD
		//auto& camTrans = testScene.Get<TTN_Transform>(testScene.GetCamEntity());
=======
		auto& camTrans = testScene.Get<TTN_Transform>(testScene.GetCamEntity());
>>>>>>> Ame
		//camTrans.RotateFixed(glm::vec3(0, 5.0f * dt, 0));
		
		//note:: always call keydown first
		if (TTN_Application::TTN_Input::GetKeyDown(TTN_KeyCode::Space)) {
			printf("Space down\n");
		}

		if (TTN_Application::TTN_Input::GetKey(TTN_KeyCode::Space)) {
			printf("Space held\n");
		}

		if (TTN_Application::TTN_Input::GetKeyUp(TTN_KeyCode::Space)) {
			printf("Space released\n");
		}

		//printf("mousePos x: %f, %f\n", TTN_Application::TTN_Input::GetMousePosition().x, TTN_Application::TTN_Input::GetMousePosition().y);

		//note:: always call GetMouseButtonDown first
		if (TTN_Application::TTN_Input::GetMouseButtonDown(TTN_MouseButton::Left)) {
			printf("left click down\n");
		}

		if (TTN_Application::TTN_Input::GetMouseButton(TTN_MouseButton::Left)) {
			printf("left click held\n");
		}

		if (TTN_Application::TTN_Input::GetMouseButtonUp(TTN_MouseButton::Left)) {
			printf("left click released\n");
		}

<<<<<<< HEAD
		//printf("fps: %f\n", 1.0f/dt);
=======
		
		auto& pboat = testScene.Get<TTN_Physics>(boat);

		auto& ptree = testScene.Get<TTN_Physics>(tree1);

		
		pboat.SetPos(boatTrans.GetPos());
		ptree.SetPos(treeTrans.GetPos());
		if (TTN_Physics::Inter(pboat, ptree)) {
			
			std::cout << "Touching " << (pboat.GetMin().x <= ptree.GetMax().x && pboat.GetMax().x >= ptree.GetMin().x) <<
				(pboat.GetMin().y <= ptree.GetMax().y && pboat.GetMax().y >= ptree.GetMin().y) <<
				(pboat.GetMin().z <= ptree.GetMax().z && pboat.GetMax().z >= ptree.GetMin().z)  <<std::endl;

			//speed = 0;

		}

		else
		{
			std::cout << "Not Touching " << (pboat.GetMin().x <= ptree.GetMax().x && pboat.GetMax().x >= ptree.GetMin().x) <<
				(pboat.GetMin().y <= ptree.GetMax().y && pboat.GetMax().y >= ptree.GetMin().y) <<
				(pboat.GetMin().z <= ptree.GetMax().z && pboat.GetMax().z >= ptree.GetMin().z) << std::endl;

		}

>>>>>>> Ame
		//render the screen

		TTN_Application::Update(); 
	}
		
	return 0;
}