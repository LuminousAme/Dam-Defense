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
	//create a new scene
	TTN_Scene* test = new DemoScene;
	TTN_Scene* demo = new Review3Scene;

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