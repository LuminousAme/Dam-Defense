//Titan Engine, by Atlas X Games 
// Scene.cpp - source file for the class that handles ECS, render calls, etc. 
#include "Titan/Scene.h"

#include <GLM/gtc/matrix_transform.hpp>

namespace Titan {
	
	TTN_Scene::TTN_Scene() {
		m_ShouldRender = true;
		m_Registry = new entt::registry();
		m_AmbientColor = glm::vec3(1.0f);
		m_AmbientStrength = 1.0f;

		std::map<std::pair<TTN_Physics*, TTN_Physics*>, TTN_Collision::scolptr> m_CollisionMap 
							= std::map<std::pair<TTN_Physics*, TTN_Physics*>, TTN_Collision::scolptr>();
	}

	TTN_Scene::TTN_Scene(glm::vec3 AmbientLightingColor, float AmbientLightingStrength)
		: m_AmbientColor(AmbientLightingColor), m_AmbientStrength(AmbientLightingStrength)
	{
		m_ShouldRender = true;
		m_Registry = new entt::registry();

		std::map<std::pair<TTN_Physics*, TTN_Physics*>, TTN_Collision::scolptr> m_CollisionMap
			= std::map<std::pair<TTN_Physics*, TTN_Physics*>, TTN_Collision::scolptr>();
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
		//run through all of the physicsbody in the scene
		auto physicsBodyView = m_Registry->view<TTN_Physics>();
		for (auto entity : physicsBodyView) {
			//call the physics body's update
			Get<TTN_Physics>(entity).Update(deltaTime);
		}

		//run through all of the entities with both a physics body and a transform in the scene
		auto transAndPhysicsView = m_Registry->view<TTN_Transform, TTN_Physics>();
		for (auto entity : transAndPhysicsView) {
			if (!Get<TTN_Physics>(entity).GetIsStaticBody()) {
				//copy the position of the physics body into the position of the transform
				Get<TTN_Transform>(entity).SetPos(Get<TTN_Physics>(entity).GetPosition());
			}
		}

		//run through all the colisions objects in the scene and run their checks
		for (auto it = m_CollisionMap.begin(); it != m_CollisionMap.end(); it++)
		{
			//have it check it's collisions
			it->second->CheckCollision();
		}
	}

	//renders all the messes in our game
	void TTN_Scene::Render()
	{
		//get the view and projection martix 
		glm::mat4 vp; 
		//update the camera for the scene
		//set the camera's position to it's transform
		Get<TTN_Camera>(m_Cam).SetPosition(Get<TTN_Transform>(m_Cam).GetPos());
		//save the view and projection matrix
		vp = Get<TTN_Camera>(m_Cam).GetProj();
		glm::mat4 viewMat = glm::inverse(Get<TTN_Transform>(m_Cam).GetMatrix());
		vp *= viewMat;


		//go through every entity with a transform and a mesh renderer and render the mesh
		auto meshView = m_Registry->view<TTN_Transform, TTN_Renderer>();
		for (auto entity : meshView) {
			//get the mesh renderer
			auto& renderer = Get<TTN_Renderer>(entity);
			//get the shader pointer
			TTN_Shader::sshptr shader = renderer.GetShader();

			//bind the shader
			shader->Bind();

			//sets some uniforms
			//scene level ambient lighting
			shader->SetUniform("u_AmbientCol", m_AmbientColor);
			shader->SetUniform("u_AmbientStrength", m_AmbientStrength);
				
			//stuff from the light
			auto& light = Get<TTN_Light>(m_Light);
			auto& lightTrans = Get<TTN_Transform>(m_Light);
			shader->SetUniform("u_LightPos", lightTrans.GetPos());
			shader->SetUniform("u_LightCol", light.GetColor());
			shader->SetUniform("u_AmbientLightStrength", light.GetAmbientStrength());
			shader->SetUniform("u_SpecularLightStrength", light.GetSpecularStrength());
			shader->SetUniform("u_LightAttenuationConstant", light.GetConstantAttenuation());
			shader->SetUniform("u_LightAttenuationLinear", light.GetLinearAttenuation());
			shader->SetUniform("u_LightAttenuationQuadratic", light.GetQuadraticAttenuation());

			//stuff from the camera
			shader->SetUniform("u_CamPos", Get<TTN_Transform>(m_Cam).GetPos());

			//if the mesh has a material send data from that
			if (renderer.GetMat() != nullptr)
			{
				//give openGL the shinniess
				shader->SetUniform("u_Shininess", renderer.GetMat()->GetShininess());
				//if they're using a texture 
				if (shader->GetFragShaderDefaultStatus() == 4 || shader->GetFragShaderDefaultStatus() == 5)
				{
					//bind it so openGL can see it
					renderer.GetMat()->GetAlbedo()->Bind(0);
				}
				//if they're using a specular map 
				if (shader->GetFragShaderDefaultStatus() == 5) 
				{
					//bind it so openGL can see it
					renderer.GetMat()->GetSpecularMap()->Bind(1);
				}
			}
			//otherwise send a default shinnies value
			else {
				shader->SetUniform("u_Shininess", 128.0f);
			}
	
			//unbind the shader
			shader->UnBind();

			//and finsih by rendering the mesh
			renderer.Render(Get<TTN_Transform>(entity).GetMatrix(), vp);
		}

		//now that all the opaque objects have been rendered, let's render the physics boxes
		if (TTN_Physics::GetRenderingIsSetUp()) {
			auto physicsBodyView = m_Registry->view<TTN_Physics>(); 
			for (auto entity:physicsBodyView) {
				Get<TTN_Physics>(entity).Render(vp);
			}
		}
			
	}

	//sets wheter or not the scene should be rendered
	void TTN_Scene::SetShouldRender(bool _shouldRender)
	{
		m_ShouldRender = _shouldRender;
	}

	//sets the color of the scene's ambient lighting
	void TTN_Scene::SetSceneAmbientColor(glm::vec3 color)
	{
		m_AmbientColor = color;
	}

	//sets the strenght of the scene's ambient lighting
	void TTN_Scene::SetSceneAmbientLightStrength(float str)
	{
		m_AmbientStrength = str;
	}

	//returns wheter or not this scene should be rendered
	bool TTN_Scene::GetShouldRender()
	{
		return m_ShouldRender;
	}

	//returns the color of the scene's ambient lighting
	glm::vec3 TTN_Scene::GetSceneAmbientColor()
	{
		return m_AmbientColor;
	}

	//returns the strenght of the scene's ambient lighting
	float TTN_Scene::GetSceneAmbientLightStrength()
	{
		return m_AmbientStrength;
	}

	void TTN_Scene::SetUpCollisions(entt::entity entity)
	{
		//create a few of all the entities in the scene with physics bodies
		auto physicsBodyView = m_Registry->view<TTN_Physics>();

		//go through each entity in that view
		for (auto ent : physicsBodyView)
		{
			//if the current object in the view is the one first we're setting up with, skip it, don't need it colliding with itself
			if (ent == entity) {
				continue;
			}
			//otherwise it must be a different entity, so check if a collision should be setup
			else {
				//get references to both entities physics bodies 
				TTN_Physics* b1 = &Get<TTN_Physics>(entity);
				TTN_Physics* b2 = &Get<TTN_Physics>(ent);
				
				//if both bodies are static, skip it, no need to check collisions between 2 objects that don't move
				if (b1->GetIsStaticBody() && b2->GetIsStaticBody()) {
					continue;
				}
				//otherwise atleast one has to be dynamic, so set up a collison object for them
				else {
					//creat the collision pointer
					TTN_Collision::scolptr newCollision = TTN_Collision::Create();
					//set it's physics bodies
					newCollision->SetBody1(b1);
					newCollision->SetBody2(b2);
					//and add it to the map
					m_CollisionMap.emplace(std::pair<TTN_Physics*, TTN_Physics*>(b1, b2), newCollision);
				}
			}
		}
	}

	//removes all collisions with a given entity, to be called when an entity is being deleted
	void TTN_Scene::CleanUpCollisions(entt::entity entity)
	{
		//iterate through all the collisions
		//create a few of all the entities in the scene with physics bodies
		auto physicsBodyView = m_Registry->view<TTN_Physics>();

		//go through each entity in that view
		for (auto ent : physicsBodyView)
		{
			//if the current object in the view is the one first we're setting up with, skip it, it doesn't have a collision with itself anyways
			if (ent == entity) {
				continue;
			}
			//otherwise it must be a different entity, so check if there's a collision that needs to be deleted
			else {
				//check both posible key permutations
				auto it = m_CollisionMap.find(std::pair<TTN_Physics*, TTN_Physics*>(&Get<TTN_Physics>(entity), &Get<TTN_Physics>(ent)));
				auto it2 = m_CollisionMap.find(std::pair<TTN_Physics*, TTN_Physics*>(&Get<TTN_Physics>(ent), &Get<TTN_Physics>(entity)));
				//if either has a collision, delete it
				if (it != m_CollisionMap.end()) {
					m_CollisionMap.erase(it);
				}
				if (it2 != m_CollisionMap.end()) {
					m_CollisionMap.erase(it2);
				}
			}
		}
	}

	//finds a collision pointer based on the objects that could have collided, if it finds one it returns it, otherwise it returns a nullptr
	TTN_Collision::scolptr TTN_Scene::FindCollisionPointer(TTN_Physics* b1, TTN_Physics* b2)
	{
		//search the map
		if(m_CollisionMap.find(std::pair<TTN_Physics*, TTN_Physics*>(b1, b2)) != m_CollisionMap.end()) {
			//if it finds one, return it
			return m_CollisionMap.at(std::pair<TTN_Physics*, TTN_Physics*>(b1, b2));
		}
		//if can't be found then try searching the map with the pointers reverse (it is order senestive)
		else if (m_CollisionMap.find(std::pair<TTN_Physics*, TTN_Physics*>(b2, b1)) != m_CollisionMap.end()) {
			//if it finds one return it
				//if it finds one, return it
			return m_CollisionMap.at(std::pair<TTN_Physics*, TTN_Physics*>(b2, b1));
		}

		//otherwise return a nullptr
		return nullptr;
	}

}