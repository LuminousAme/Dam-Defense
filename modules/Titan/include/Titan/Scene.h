//Titan Engine, by Atlas X Games 
// Scene.h - header for the class that handles ECS, render calls, etc. 
#pragma once

//include the entity component system header
#include "entt.hpp"
//include all the component class definitions we need
#include "Transform.h"
#include "Renderer.h"
#include "Camera.h"
//include all the graphics features we need
#include "Shader.h"
#include "Physics.h"

namespace Titan {

	//scene class, handles the ECS, render class, etc. 
	class TTN_Scene
	{
	public:
		//constructor 
		TTN_Scene();

		//copy, move, and assingment operators
		TTN_Scene(const TTN_Scene& oldScene) = default;
		TTN_Scene(TTN_Scene&&) = default;
		TTN_Scene& operator=(TTN_Scene&) = default;

		//destrutor 
		~TTN_Scene();

#pragma region ECS_functions_dec
		//creates a new entity 
		entt::entity CreateEntity();

		//attaches a compontent to an entity 
		template<typename T>
		void Attach(entt::entity entity);

		//attaches a component to an entity, and makes that component a copy of whatever object is passed in 
		template<typename T>
		void AttachCopy(entt::entity entity, T copy);

		//returns a reference to the component of the relevant entity 
		template<typename T>
		T& Get(entt::entity entity);

		//removes a component from an object 
		template<typename T>
		void Remove(entt::entity entity);

		//updates all the entities in the scene
		void Update(float deltaTime);
#pragma endregion ECS_functions_dec

#pragma region Graphics_functions_dec
		//renders all the entities with meshes and transforms in the scene 
		void Render(); 

		//sets wheter or not the scene should be rendered 
		void SetShouldRender(bool _shouldRender);

		//gets wheter or not the scene should be rendered 
		bool GetShouldRender();

#pragma endregion Graphics_functions_dec

		//sets the camera entity reference
		void SetCamEntity(const entt::entity& cam) { m_Cam = cam; }
		//gets the camera entity
		const entt::entity& GetCamEntity() { return m_Cam; }


	private:
		//context that contains all our entities, their ids, and components 
		entt::registry* m_Registry = nullptr;

		//boolean to store wheter or not this scene should currently be rendered
		bool shouldRender; 

		//variable to store the entity for the camera
		entt::entity m_Cam;
	};

#pragma region ECS_functions_def
	template<typename T>
	inline void TTN_Scene::Attach(entt::entity entity)
	{
		//assign the component to the entity
		m_Registry->emplace<T>(entity);
	}

	template<typename T>
	inline void TTN_Scene::AttachCopy(entt::entity entity, T copy)
	{
		//assign the component to the entity 
		m_Registry->emplace<T>(entity);

		//copy the passed in object into the component 
		m_Registry->get<T>(entity) = copy;
	}

	template<typename T>
	inline T& TTN_Scene::Get(entt::entity entity)
	{
		//return a reference to the component 
		return m_Registry->get<T>(entity);
	}

	template<typename T>
	inline void TTN_Scene::Remove(entt::entity entity)
	{
		//remove the component from the entity
		m_Registry->remove<T>(entity);
	}
#pragma endregion ECS_functions_def

}