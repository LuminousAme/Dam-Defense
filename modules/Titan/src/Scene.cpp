//Titan Engine, by Atlas X Games 
// Scene.cpp - source file for the class that handles ECS, render calls, etc. 
#include "Titan/Scene.h"

#include <GLM/gtc/matrix_transform.hpp>

namespace Titan {
	
	TTN_Scene::TTN_Scene() {
		shouldRender = true;
		m_Registry = new entt::registry();
	}

	TTN_Scene::~TTN_Scene() {

	}

	entt::entity TTN_Scene::CreateEntity()
	{
		//create the entity
		auto entity = m_Registry->create();

		//return the entity id 
		return entity;
	}

	void TTN_Scene::Update(float deltaTime)
	{
	}

	//renders all the messes in our game
	void TTN_Scene::Render()
	{
		//get the view and projection martix 
		glm::mat4 vp; 
		//update the camera for the scene
		auto camView = m_Registry->view<TTN_Transform, TTN_Camera>();
		for (auto entity : camView) {
			//set the camera's position to it's transform
			Get<TTN_Camera>(entity).SetPosition(Get<TTN_Transform>(entity).GetPos());
			//recalculate the view
			Get<TTN_Camera>(entity).View();
			//save the view and projection matrix
			vp = Get<TTN_Camera>(entity).GetVP();
		}
		//get the view and projection matrices for the scene
		glm::mat4 proj = glm::perspective(glm::radians(90.0f), 1.0f, 0.01f, 100.f);

		//go through every entity with a transform and a mesh renderer and render the mesh
		auto meshView = m_Registry->view<TTN_Transform, TTN_Renderer>();
		for (auto entity : meshView) {
			//render the mesh
			Get<TTN_Renderer>(entity).Render(Get<TTN_Transform>(entity).GetMatrix(), vp);
		}
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