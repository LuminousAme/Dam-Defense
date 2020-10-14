//Titan Engine, by Atlas X Games 
// Scene.h - header for the class that handles ECS, render calls, etc. 
#pragma once

#include "entt.hpp"

namespace Titan {

	//scene class, handles the ECS, render class, etc. 
	class TTN_Scene
	{
	public:
		//constructor 
		TTN_Scene();

		//destrutor 
		~TTN_Scene();

#pragma region ECS_functions_dec
		//creates a new entity 
		unsigned int CreateEntity();

		//attaches a compontent to an entity 
		template<typename T>
		void Attach(unsigned int entity);

		//attaches a component to an entity, and makes that component a copy of whatever object is passed in 
		template<typename T>
		void AttachCopy(unsigned int entity, T copy);

		//returns a reference to the component of the relevant entity 
		template<typename T>
		T& Get(unsigned int entity);

		//removes a component from an object 
		template<typename T>
		void Remove(unsigned int entity);
#pragma endregion ECS_functions_dec

#pragma region Graphics_functions_dec
		//renders all the entities with meshes and transforms in the scene 
		void Render(); 

#pragma endregion Graphics_functions_dec

	private:
		//context that contains all our entities, their ids, and components 
		entt::registry m_Registry;
	};

#pragma region ECS_functions_def
	template<typename T>
	inline void TTN_Scene::Attach(unsigned int entity)
	{
		//assign the component to the entity
		m_Registry.assign<T>(entity);
	}

	template<typename T>
	inline void TTN_Scene::AttachCopy(unsigned int entity, T copy)
	{
		//assign the component to the entity 
		m_Registry.assign<T>(entity);

		//copy the passed in object into the component 
		m_Registry.get<T>(entity) = copy;
	}

	template<typename T>
	inline T& TTN_Scene::Get(unsigned int entity)
	{
		//return a reference to the component 
		return m_Registry.get<T>(entity);
	}

	template<typename T>
	inline void TTN_Scene::Remove(unsigned int entity)
	{
		//remove the component from the entity
		m_Registry.remove<T>(entity);
	}
#pragma endregion ECS_functions_def

}