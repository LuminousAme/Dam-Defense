//Titan Engine, by Atlas X Games 
// Scene.cpp - source file for the class that handles ECS, render calls, etc. 
#include "Titan/Scene.h"

namespace Titan{
	
	TTN_Scene::TTN_Scene() {
	}

	TTN_Scene::~TTN_Scene() {

	}

	unsigned int TTN_Scene::CreateEntity()
	{
		//create the entity
		auto entity = m_Registry.create();

		//return the entity id 
		return (unsigned int)entity;
	}

	void TTN_Scene::Update(float deltaTime)
	{
	}

	void TTN_Scene::Render()
	{
	}

	//sets wheter or not the scene should be rendered
	void TTN_Scene::SetShouldRender(bool _shouldRender)
	{
		shouldRender = _shouldRender;
	}

	//returns wheter or not this scene should be rendered
	bool TTN_Scene::GetShouldRender()
	{
		return shouldRender;
	}

}