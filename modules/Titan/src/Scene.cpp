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

		//setting up physics world
		collisionConfig = new btDefaultCollisionConfiguration(); //default collision config
		dispatcher = new btCollisionDispatcher(collisionConfig); //default collision dispatcher
		overlappingPairCache = new btDbvtBroadphase();//basic board phase
		solver = new btSequentialImpulseConstraintSolver;//default collision solver

		//create the physics world
		m_physicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfig);

		//set gravity to default none
		m_physicsWorld->setGravity(btVector3(0.0f, 0.0f, 0.0f));
	}

	TTN_Scene::TTN_Scene(glm::vec3 AmbientLightingColor, float AmbientLightingStrength)
		: m_AmbientColor(AmbientLightingColor), m_AmbientStrength(AmbientLightingStrength)
	{
		m_ShouldRender = true;
		m_Registry = new entt::registry();

		//setting up physics world
		collisionConfig = new btDefaultCollisionConfiguration(); //default collision config
		dispatcher = new btCollisionDispatcher(collisionConfig); //default collision dispatcher
		overlappingPairCache = new btDbvtBroadphase();//basic board phase
		solver = new btSequentialImpulseConstraintSolver;//default collision solver

		//create the physics world
		m_physicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfig);

		//set gravity to default none
		m_physicsWorld->setGravity(btVector3(0.0f, 0.0f, 0.0f));
	}

	TTN_Scene::~TTN_Scene() {
		Unload();
	}

	entt::entity TTN_Scene::CreateEntity()
	{
		//create the entity
		auto entity = m_Registry->create();

		//return the entity id 
		return entity;
	}

	void TTN_Scene::DeleteEntity(entt::entity entity)
	{
		m_Registry->destroy(entity);
	}

	//sets the underlying entt registry of the scene
	void TTN_Scene::SetScene(entt::registry* reg)
	{
		m_Registry = reg;
	}

	//unloads the scene, deleting the registry and physics world
	void TTN_Scene::Unload()
	{		
		//delete all the physics world stuff
		//delete the physics objects
		for (auto i = m_physicsWorld->getNumCollisionObjects() - 1; i >= 0; i--) {
			//get the object and it's rigid body
			btCollisionObject* PhyObject = m_physicsWorld->getCollisionObjectArray()[i];
			btRigidBody* PhysRigidBod = btRigidBody::upcast(PhyObject);
			//if it has a motion state, remove that
			if (PhysRigidBod != nullptr && PhysRigidBod->getMotionState() != nullptr) {
				delete PhysRigidBod->getMotionState();
			}
			//remove the object from the physics world
			m_physicsWorld->removeCollisionObject(PhyObject);
			//and delete it 
			delete PhyObject;
		}

		//supposed to delete collision shapes acoording to quickstart guide but uses local variables that they don't define so idk what I'm supposed to do lol

		//delete the physics world and it's attributes
		delete m_physicsWorld;
		delete solver;
		delete overlappingPairCache;
		delete dispatcher;
		delete collisionConfig;

		//delete registry
		if (m_Registry != nullptr) {
			delete m_Registry;
			m_Registry = nullptr;
		}
	}

	void TTN_Scene::Update(float deltaTime)
	{
<<<<<<< HEAD
			
=======
		//call the step simulation for bullet
		m_physicsWorld->stepSimulation(deltaTime);

>>>>>>> Ame
		//run through all of the physicsbody in the scene
		auto physicsBodyView = m_Registry->view<TTN_Physics>();
		for (auto entity : physicsBodyView) {
			//if the physics body isn't in the world, add it
			if (!Get<TTN_Physics>(entity).GetIsInWorld()) {
				m_physicsWorld->addRigidBody(Get<TTN_Physics>(entity).GetRigidBody());
				Get<TTN_Physics>(entity).SetIsInWorld(true);
			}
			//call the physics body's update
			Get<TTN_Physics>(entity).Update(deltaTime);
		}

		//run through all of the entities with both a physics body and a transform in the scene
		auto transAndPhysicsView = m_Registry->view<TTN_Transform, TTN_Physics>();
		for (auto entity : transAndPhysicsView) {
			if (!Get<TTN_Physics>(entity).GetIsStatic()) {
				//copy the position of the physics body into the position of the transform
				Get<TTN_Transform>(entity).SetPos(Get<TTN_Physics>(entity).GetTrans().GetPos());
			}
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
		glm::mat4 viewMat = glm::inverse(Get<TTN_Transform>(m_Cam).GetGlobal());
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
			renderer.Render(Get<TTN_Transform>(entity).GetGlobal(), vp);
		}

		/*
		//now that all the opaque objects have been rendered, let's render the physics boxes
		if (TTN_Physics::GetRenderingIsSetUp()) {
			auto physicsBodyView = m_Registry->view<TTN_Physics>(); 
			for (auto entity:physicsBodyView) {
				Get<TTN_Physics>(entity).Render(vp);
			}
		}*/
			
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

	//set the gravity for the physics world
	void TTN_Scene::SetGravity(glm::vec3 gravity)
	{
		btVector3 grav = btVector3(gravity.x, gravity.y, gravity.z);
		m_physicsWorld->setGravity(grav);
	}

	glm::vec3 TTN_Scene::GetGravity()
	{
		btVector3 grav = m_physicsWorld->getGravity();
		return glm::vec3((float)grav.getX(), (float)grav.getY(), (float)grav.getZ());
	}
}