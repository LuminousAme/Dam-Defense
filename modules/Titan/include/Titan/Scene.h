//Titan Engine, by Atlas X Games 
// Scene.h - header for the class that handles ECS, render calls, etc. 
#pragma once

//include the entity component system header
#include "entt.hpp"
//include all the component class definitions we need
#include "Transform.h"
#include "Renderer.h"
#include "Camera.h"
#include "Light.h"
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

		TTN_Scene(glm::vec3 AmbientLightingColor, float AmbientLightingStrength);

		//copy, move, and assingment operators
		TTN_Scene(const TTN_Scene& oldScene) = default;
		TTN_Scene(TTN_Scene&&) = default;
		TTN_Scene& operator=(TTN_Scene&) = default;

		//destrutor 
		~TTN_Scene();

#pragma region ECS_functions_dec
		//creates a new entity 
		entt::entity CreateEntity();

		//deletes an entity
		void DeleteEntity(entt::entity entity);

		//attaches a compontent to an entity 
		template<typename T>
		void Attach(entt::entity entity);

		//attaches a component to an entity, and makes that component a copy of whatever object is passed in 
		template<typename T>
		void AttachCopy(entt::entity entity, const T& copy);

		//returns a reference to the component of the relevant entity 
		template<typename T>
		T& Get(entt::entity entity);

		//removes a component from an object 
		template<typename T>
		void Remove(entt::entity entity);

		//sets the registry
		void SetScene(entt::registry* reg);
		//gets the registry
		entt::registry* GetScene() { return m_Registry; }

		//deletes and clears the scene
		void Unload();
		
#pragma endregion ECS_functions_dec

		//Derivable functions 

		//updates all the entities in the scene
		virtual void Update(float deltaTime);

		//input checks
		//keyboard
		virtual void KeyDownChecks() {}
		virtual void KeyChecks() {}
		virtual void KeyUpChecks() {}
		//mouse
		virtual void MouseButtonDownChecks() {}
		virtual void MouseButtonChecks() {}
		virtual void MouseButtonUpChecks() {}

		//init
		virtual void InitScene() {} 

#pragma region Graphics_functions_dec
		//renders all the entities with meshes and transforms in the scene 
		void Render(); 

		//sets wheter or not the scene should be rendered 
		void SetShouldRender(bool _shouldRender);
		//sets the ambient color of the lighting in the scene
		void SetSceneAmbientColor(glm::vec3 color);
		//sets the strenght of the abmient lighting in the scene
		void SetSceneAmbientLightStrength(float str);

		//gets wheter or not the scene should be rendered 
		bool GetShouldRender();
		//gets the ambient color of the lighting in the scene
		glm::vec3 GetSceneAmbientColor();
		//gets the strength of the ambient lighting in the scene
		float GetSceneAmbientLightStrength();

#pragma endregion Graphics_functions_dec

		//sets the camera entity reference
		void SetCamEntity(const entt::entity& cam) { m_Cam = cam; }
		//gets the camera entity
		const entt::entity& GetCamEntity() { return m_Cam; }

		//sets the light entity reference
		void SetLightEntity(const entt::entity& light) { m_Light = light; }
		//gets the light entity
		const entt::entity& GetLightEntity() { return m_Light; }

		//bullet physics stuff
		//set the gravity
		void SetGravity(glm::vec3 gravity);
		//gets the gravity
		glm::vec3 GetGravity();


		//sets up collisions
		void SetUpCollisions(entt::entity entity);
		//removes all collisions with a given entity, to be called when an entity is being deleted
		void CleanUpCollisions(entt::entity entity);
		//finds a collision based on it's underlying physics bodies
		TTN_Collision::scolptr FindCollisionPointer(TTN_Physics* b1, TTN_Physics* b2);


	private:
		//context that contains all our entities, their ids, and components 
		entt::registry* m_Registry = nullptr;

		//boolean to store wheter or not this scene should currently be rendered
		bool m_ShouldRender; 

		//variable to store the entity for the camera
		entt::entity m_Cam;

		//variable to store the entity of the light
		entt::entity m_Light;

		//the colour of the scene's ambient lighting
		glm::vec3 m_AmbientColor;
		//the strenght of that ambient color
		float m_AmbientStrength;

		//physics world properties
		btDefaultCollisionConfiguration* collisionConfig;
		btCollisionDispatcher* dispatcher;
		btBroadphaseInterface* overlappingPairCache;
		btSequentialImpulseConstraintSolver* solver;
		//physics world
		btDiscreteDynamicsWorld* m_physicsWorld;

		//map of all the collisions
		std::map<std::pair<TTN_Physics*, TTN_Physics*>, TTN_Collision::scolptr> m_CollisionMap;
	};

#pragma region ECS_functions_def
	template<typename T>
	inline void TTN_Scene::Attach(entt::entity entity)
	{
		//assign the component to the entity
		m_Registry->emplace<T>(entity);
	}

	template<typename T>
	inline void TTN_Scene::AttachCopy(entt::entity entity, const T& copy)
	{
		//assign the component to the entity 
		m_Registry->emplace_or_replace<T>(entity, copy);
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