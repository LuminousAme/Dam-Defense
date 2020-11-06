#include "Titan/Application.h"
#include "Titan/Scene.h"
#include "Titan/ObjLoader.h"
#include "Titan/Renderer.h"
#include "Titan/Transform.h"
#include "Titan/Physics.h"
#include <iostream>
#include "glm/ext.hpp"

#include "DemoScene.h"

using namespace Titan;

int main() {
	Logger::Init();
	TTN_Application::Init("Dam Defense", 800, 800);
	TTN_Physics::SetUpPhysicsBoxRendering(); 

	//create a new scene
	TTN_Scene* demo = new DemoScene;

	demo->InitScene();

	//add the scene to the application
	TTN_Application::scenes.push_back(demo);
	//set the background to a blue
	TTN_Application::SetClearColor(glm::vec4(0.0f, 0.2f, 8.0f, 1.0f));

	while (!TTN_Application::GetIsClosing()) {
		//update the scenes and render the screen
		TTN_Application::Update(); 
	}
		
	return 0;
}