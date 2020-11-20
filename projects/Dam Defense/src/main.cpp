#include "Titan/Application.h"
#include "Titan/Scene.h"
#include "Titan/ObjLoader.h"
#include "Titan/Renderer.h"
#include "Titan/Transform.h"
#include "Titan/Physics.h"
#include <iostream>
#include "glm/ext.hpp"

#include "DemoScene.h"
#include "Review3Scene.h"

using namespace Titan;

int main() {
	Logger::Init();
	TTN_Application::Init("Dam Defense", 1920, 1080);
	//TTN_Physics::SetUpPhysicsBoxRendering(); 
	//create a new scene
<<<<<<< HEAD
	TTN_Scene testScene = TTN_Scene();

	printf("a");

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
		testScene.Get<TTN_Camera>(camera).CalcPerspective(90.0f, 1.0f, 0.01f, 100.f);
		testScene.Get<TTN_Camera>(camera).View();
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
		treeTrans.SetPos(glm::vec3(-2.0f, -3.0f, 5.f));
		treeTrans.SetScale(glm::vec3(1.f, 1.f, 1.f));
		//attach that transform to the tree entity
		testScene.AttachCopy<TTN_Transform>(tree1, treeTrans);
	}
=======
	TTN_Scene* test = new DemoScene;
	TTN_Scene* demo = new Review3Scene;
>>>>>>> Jacky

	test->InitScene();
	demo->InitScene();

	test->SetShouldRender(false);

	//add the scene to the application
	TTN_Application::scenes.push_back(test);
	TTN_Application::scenes.push_back(demo);
	//set the background to a blue
	TTN_Application::SetClearColor(glm::vec4(0.0f, 0.88f, 0.99f, 1.0f));
	
	while (!TTN_Application::GetIsClosing()) {

		if (TTN_Application::TTN_Input::GetKeyDown(TTN_KeyCode::Space)) {
			printf("check\n");
			test->SetShouldRender(!test->GetShouldRender());
			demo->SetShouldRender(!demo->GetShouldRender());
		}

		TTN_Application::TTN_Input::GetKeyUp(TTN_KeyCode::Space); //just to clear for the keydown
		//update the scenes and render the screen
		TTN_Application::Update(); 
	}
		
	return 0;
}