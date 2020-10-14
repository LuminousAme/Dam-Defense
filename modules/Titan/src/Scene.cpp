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

	void TTN_Scene::Render()
	{
	}

}