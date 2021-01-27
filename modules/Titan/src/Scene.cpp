//Titan Engine, by Atlas X Games

//precompile header, this file uses entt.hpp, and GLM/gtc/matrix_transform.hpp
#include "Titan/ttn_pch.h"
// Scene.cpp - source file for the class that handles ECS, render calls, etc.
#include "Titan/Scene.h"

namespace Titan {
	//default constructor
 
	TTN_Scene::TTN_Scene() {
		//setup basic data and systems
		m_ShouldRender = true;
		m_Registry = new entt::registry();
		m_RenderGroup = std::make_unique<RenderGroupType>(m_Registry->group<TTN_Transform, TTN_Renderer>());
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

		m_Paused = false;
	}

	//construct with lightning data
	TTN_Scene::TTN_Scene(glm::vec3 AmbientLightingColor, float AmbientLightingStrength)
		: m_AmbientColor(AmbientLightingColor), m_AmbientStrength(AmbientLightingStrength)
	{
		///setup basic data and systems
		m_ShouldRender = true;
		m_Registry = new entt::registry();
		m_RenderGroup = std::make_unique<RenderGroupType>(m_Registry->group<TTN_Transform, TTN_Renderer>());

		//setting up physics world
		collisionConfig = new btDefaultCollisionConfiguration(); //default collision config
		dispatcher = new btCollisionDispatcher(collisionConfig); //default collision dispatcher
		overlappingPairCache = new btDbvtBroadphase();//basic board phase
		solver = new btSequentialImpulseConstraintSolver;//default collision solver

		//create the physics world
		m_physicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfig);

		//set gravity to default none
		m_physicsWorld->setGravity(btVector3(0.0f, 0.0f, 0.0f));

		m_Paused = false;
	}

	//destructor
	TTN_Scene::~TTN_Scene() {
		//unload the scene before it's deleted
		Unload();
	}

	//function to create a new entity, returns it's entity number
	entt::entity TTN_Scene::CreateEntity()
	{
		//create the entity
		auto entity = m_Registry->create();

		//reconstruct scenegraph as entt was shuffled
		ReconstructScenegraph();

		//return the entity id
		return entity;
	}

	//function to delete an entity
	void TTN_Scene::DeleteEntity(entt::entity entity)
	{
		//if the entity has a bullet physics body, delete it from bullet
		if (m_Registry->has<TTN_Physics>(entity)) {
			btRigidBody* body = Get<TTN_Physics>(entity).GetRigidBody();
			delete body->getMotionState();
			delete body->getCollisionShape();
			m_physicsWorld->removeRigidBody(body);
			delete body;
		}

		//delete the entity from the registry
		m_Registry->destroy(entity);

		//reconstruct the scenegraph as entt was shuffled
		ReconstructScenegraph();
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

	//reconstructs the scenegraph, should be done every time entt shuffles
	void TTN_Scene::ReconstructScenegraph()
	{
		//reconstruct any scenegraph relationships
		auto transView = m_Registry->view<TTN_Transform>();
		for (auto entity : transView) {
			//if it should have a parent
			if (Get<TTN_Transform>(entity).GetParentEntity() != nullptr) {
				//then reatach that parent
				Get<TTN_Transform>(entity).SetParent(&Get<TTN_Transform>(*Get<TTN_Transform>(entity).GetParentEntity()),
					Get<TTN_Transform>(entity).GetParentEntity());
			}
		}
	}

	//update the scene, running physics simulation, animations, and particle systems
	void TTN_Scene::Update(float deltaTime)
	{
		//only run the updates if the scene is not paused
		if (!m_Paused) {
			//call the step simulation for bullet
			m_physicsWorld->stepSimulation(deltaTime);

			//run through all of the physicsbody in the scene
			auto physicsBodyView = m_Registry->view<TTN_Physics>();
			for (auto entity : physicsBodyView) {
				//if the physics body isn't in the world, add it
				if (!Get<TTN_Physics>(entity).GetIsInWorld()) {
					Get<TTN_Physics>(entity).SetEntity(entity);
					m_physicsWorld->addRigidBody(Get<TTN_Physics>(entity).GetRigidBody());
					Get<TTN_Physics>(entity).SetIsInWorld(true);
				}

				//make sure the physics body are active on every frame
				Get<TTN_Physics>(entity).GetRigidBody()->setActivationState(true);

				//call the physics body's update
				Get<TTN_Physics>(entity).Update(deltaTime);
			}

			//construct the collisions for the frame
			ConstructCollisions();

			//run through all of the entities with both a physics body and a transform in the scene
			auto transAndPhysicsView = m_Registry->view<TTN_Transform, TTN_Physics>();
			for (auto entity : transAndPhysicsView) {
				if (!Get<TTN_Physics>(entity).GetIsStatic()) {
					//copy the position of the physics body into the position of the transform
					Get<TTN_Transform>(entity).SetPos(Get<TTN_Physics>(entity).GetTrans().GetPos());
				}
			}

			//run through all the of entities with an animator and renderer in the scene and run it's update
			auto manimatorRendererView = m_Registry->view<TTN_MorphAnimator>();
			for (auto entity : manimatorRendererView) {
				//update the active animation
				Get<TTN_MorphAnimator>(entity).getActiveAnimRef().Update(deltaTime);
			}

			//run through all the of the entities with a particle system and run their updates
			auto psView = m_Registry->view<TTN_ParticeSystemComponent>();
			for (auto entity : psView) {
				//update the particle system
				Get<TTN_ParticeSystemComponent>(entity).GetParticleSystemPointer()->Update(deltaTime);
			}
		}
	}

	//function that executes after the main render 
	void TTN_Scene::PostRender()
	{
		//set up the view matrix 
		glm::mat4 viewMat = glm::inverse(Get<TTN_Transform>(m_Cam).GetGlobal());

		//create a view of all the entities with a particle system and a transform
		auto psTransView = m_Registry->view<TTN_ParticeSystemComponent, TTN_Transform>();
		for (auto entity : psTransView) {
			//render the particle system
			Get<TTN_ParticeSystemComponent>(entity).GetParticleSystemPointer()->Render(Get<TTN_Transform>(entity).GetGlobalPos(),
				viewMat, Get<TTN_Camera>(m_Cam).GetProj());
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

		//sort our render group
		m_RenderGroup->sort<TTN_Renderer>([](const TTN_Renderer& l, const TTN_Renderer& r) {
			//sort by render layer first, higher render layers get drawn later
			if (l.GetRenderLayer() < r.GetRenderLayer()) return true;
			if (l.GetRenderLayer() > r.GetRenderLayer()) return false;

			//sort by shader pointer to minimize state changes on active shader
			if (l.GetShader() < r.GetShader()) return true;
			if (l.GetShader() > r.GetShader()) return false;

			//sort by material pointer to  minimize state changes on textures and stuff
			if (l.GetMat() < r.GetMat()) return true;
			if (l.GetMat() > r.GetMat()) return false;
		});

		ReconstructScenegraph();

		//go through every entity with a transform and a mesh renderer and render the mesh
		m_RenderGroup->each([&](entt::entity entity, TTN_Transform& transform, TTN_Renderer& renderer) {
			//get the shader pointer
			TTN_Shader::sshptr shader = renderer.GetShader();

			//bind the shader
			shader->Bind();

			//sets some uniforms
			if (shader->GetFragShaderDefaultStatus() != (int)TTN_DefaultShaders::NOT_DEFAULT) {
				//scene level ambient lighting
				shader->SetUniform("u_AmbientCol", m_AmbientColor);
				shader->SetUniform("u_AmbientStrength", m_AmbientStrength);

				//stuff from the light
				glm::vec3 lightPositions[16];
				glm::vec3 lightColor[16];
				float lightAmbientStr[16];
				float lightSpecStr[16];
				float lightAttenConst[16];
				float lightAttenLinear[16];
				float lightAttenQuadartic[16];

				for (int i = 0; i < 16 && i < m_Lights.size(); i++) {
					auto& light = Get<TTN_Light>(m_Lights[i]);
					auto& lightTrans = Get<TTN_Transform>(m_Lights[i]);
					lightPositions[i] = lightTrans.GetPos();
					lightColor[i] = light.GetColor();
					lightAmbientStr[i] = light.GetAmbientStrength();
					lightSpecStr[i] = light.GetSpecularStrength();
					lightAttenConst[i] = light.GetConstantAttenuation();
					lightAttenLinear[i] = light.GetConstantAttenuation();
					lightAttenQuadartic[i] = light.GetQuadraticAttenuation();
				}

				//send all the data about the lights to glsl
				shader->SetUniform("u_LightPos", lightPositions[0], 16);
				shader->SetUniform("u_LightCol", lightColor[0], 16);
				shader->SetUniform("u_AmbientLightStrength", lightAmbientStr[0], 16);
				shader->SetUniform("u_SpecularLightStrength", lightSpecStr[0], 16);
				shader->SetUniform("u_LightAttenuationConstant", lightAttenConst[0], 16);
				shader->SetUniform("u_LightAttenuationLinear", lightAttenLinear[0], 16);
				shader->SetUniform("u_LightAttenuationQuadratic", lightAttenQuadartic[0], 16);

				//and tell it how many lights there actually are
				shader->SetUniform("u_NumOfLights", (int)m_Lights.size());

				//stuff from the camera
				shader->SetUniform("u_CamPos", Get<TTN_Transform>(m_Cam).GetPos());
			}

			//if it's not the skybox shader, set some uniforms for lighting
			if (shader->GetFragShaderDefaultStatus() != (int)TTN_DefaultShaders::FRAG_SKYBOX
				&& shader->GetFragShaderDefaultStatus() != (int)TTN_DefaultShaders::NOT_DEFAULT) {
				//sets some uniforms
				//scene level ambient lighting
				shader->SetUniform("u_AmbientCol", m_AmbientColor);
				shader->SetUniform("u_AmbientStrength", m_AmbientStrength);

				//stuff from the light
				glm::vec3 lightPositions[16];
				glm::vec3 lightColor[16];
				float lightAmbientStr[16];
				float lightSpecStr[16];
				float lightAttenConst[16];
				float lightAttenLinear[16];
				float lightAttenQuadartic[16];

				for (int i = 0; i < 16 && i < m_Lights.size(); i++) {
					auto& light = Get<TTN_Light>(m_Lights[i]);
					auto& lightTrans = Get<TTN_Transform>(m_Lights[i]);
					lightPositions[i] = lightTrans.GetGlobalPos();
					lightColor[i] = light.GetColor();
					lightAmbientStr[i] = light.GetAmbientStrength();
					lightSpecStr[i] = light.GetSpecularStrength();
					lightAttenConst[i] = light.GetConstantAttenuation();
					lightAttenLinear[i] = light.GetConstantAttenuation();
					lightAttenQuadartic[i] = light.GetQuadraticAttenuation();
				}

				//send all the data about the lights to glsl
				shader->SetUniform("u_LightPos", lightPositions[0], 16);
				shader->SetUniform("u_LightCol", lightColor[0], 16);
				shader->SetUniform("u_AmbientLightStrength", lightAmbientStr[0], 16);
				shader->SetUniform("u_SpecularLightStrength", lightSpecStr[0], 16);
				shader->SetUniform("u_LightAttenuationConstant", lightAttenConst[0], 16);
				shader->SetUniform("u_LightAttenuationLinear", lightAttenLinear[0], 16);
				shader->SetUniform("u_LightAttenuationQuadratic", lightAttenQuadartic[0], 16);

				//and tell it how many lights there actually are
				shader->SetUniform("u_NumOfLights", (int)m_Lights.size());

				//stuff from the camera
				shader->SetUniform("u_CamPos", Get<TTN_Transform>(m_Cam).GetPos());
			}

			//if the mesh has a material send data from that
			if (renderer.GetMat() != nullptr)
			{

				//give openGL the shinniess
				if (shader->GetFragShaderDefaultStatus() != 8) shader->SetUniform("u_Shininess", renderer.GetMat()->GetShininess());
				//if they're using a texture
				if (shader->GetFragShaderDefaultStatus() == 4 || shader->GetFragShaderDefaultStatus() == 5)

					//give openGL the shinniess if it's not a skybox being renderered
					if (shader->GetFragShaderDefaultStatus() != (int)TTN_DefaultShaders::FRAG_SKYBOX
						&& shader->GetFragShaderDefaultStatus() != (int)TTN_DefaultShaders::NOT_DEFAULT)
						shader->SetUniform("u_Shininess", renderer.GetMat()->GetShininess());

				//texture slot to dynamically send textures across different types of shaders
				int textureSlot = 0;

				//if they're using a displacement map 
				if (shader->GetVertexShaderDefaultStatus() == (int)TTN_DefaultShaders::VERT_COLOR_HEIGHTMAP
					|| shader->GetVertexShaderDefaultStatus() == (int)TTN_DefaultShaders::VERT_NO_COLOR_HEIGHTMAP)
				{
					//bind it to the slot 
					renderer.GetMat()->GetHeightMap()->Bind(textureSlot);
					//update the texture slot for future textures to use
					textureSlot++;
					//and pass in the influence
					shader->SetUniform("u_influence", renderer.GetMat()->GetHeightInfluence());
				}

				//if they're using an animator 
				if (shader->GetVertexShaderDefaultStatus() == (int)TTN_DefaultShaders::VERT_MORPH_ANIMATION_NO_COLOR
					|| shader->GetVertexShaderDefaultStatus() == (int)TTN_DefaultShaders::VERT_MORPH_ANIMATION_COLOR) {
					//try to get an animator component 
					if (Has<TTN_MorphAnimator>(entity)) {
						shader->SetUniform("t", Get<TTN_MorphAnimator>(entity).getActiveAnimRef().getInterpolationParameter());
					}
					else
						shader->SetUniform("t", 0.0f);
				}

				//if they're using an albedo texture 
				if (shader->GetFragShaderDefaultStatus() == (int)TTN_DefaultShaders::FRAG_BLINN_PHONG_ALBEDO_ONLY
					|| shader->GetFragShaderDefaultStatus() == (int)TTN_DefaultShaders::FRAG_BLINN_PHONG_ALBEDO_AND_SPECULAR)

				{
					//bind it so openGL can see it
					renderer.GetMat()->GetAlbedo()->Bind(textureSlot);
					//update the texture slot for future textures to use
					textureSlot++;
				}

				//if they're using a specular map
				if (shader->GetFragShaderDefaultStatus() == 5)


					//if they're using a specular map 
					if (shader->GetFragShaderDefaultStatus() == (int)TTN_DefaultShaders::FRAG_BLINN_PHONG_ALBEDO_AND_SPECULAR)

					{
						//bind it so openGL can see it
						renderer.GetMat()->GetSpecularMap()->Bind(textureSlot);
						//update the texture slot for future textures to use
						textureSlot++;
					}

				//if they're using a skybox
				if (shader->GetFragShaderDefaultStatus() == (int)TTN_DefaultShaders::FRAG_SKYBOX)
				{
					//bind the skybox texture
					renderer.GetMat()->GetSkybox()->Bind(textureSlot);
					//set the rotation uniform
					shader->SetUniformMatrix("u_EnvironmentRotation", glm::mat3(glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1, 0, 0))));
					//set the skybox matrix uniform
					shader->SetUniformMatrix("u_SkyboxMatrix", Get<TTN_Camera>(m_Cam).GetProj() * glm::mat4(glm::mat3(viewMat)));
				}

			}
			//otherwise send a default shinnies value
			else if (shader->GetFragShaderDefaultStatus() != (int)TTN_DefaultShaders::NOT_DEFAULT) {
				shader->SetUniform("u_Shininess", 128.0f);
			}

			//if the entity has an animator
			if (Has<TTN_MorphAnimator>(entity)) {
				entt::entity tempA = entity;
				//int tempNum = Get<TTN_MorphAnimator>(entity).getActiveAnimRef().getCurrentMeshIndex();
				//int tempNum2 = Get<TTN_MorphAnimator>(entity).getActiveAnimRef().getNextMeshIndex();
				TTN_MorphAnimation& tempAnim = Get<TTN_MorphAnimator>(entity).getActiveAnimRef();

				//set up the vao on the mesh properly
				renderer.GetMesh()->SetUpVao(Get<TTN_MorphAnimator>(entity).getActiveAnimRef().getCurrentMeshIndex(),
					Get<TTN_MorphAnimator>(entity).getActiveAnimRef().getNextMeshIndex());
			}
			//if it doesn't
			else {
				//set up the vao with both mesh indices on zero
				renderer.GetMesh()->SetUpVao();
			}

			//and finish by rendering the mesh
			renderer.Render(transform.GetGlobal(), vp);
		});

		//go through every entity with a transform and a sprite renderer and render the sprite 
		auto render2DView = m_Registry->view<TTN_Transform, TTN_Renderer2D>();
		for (auto entity : render2DView) {
			Get<TTN_Renderer2D>(entity).Render(Get<TTN_Transform>(entity).GetGlobal(), vp);
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

	//set the gravity for the physics world
	void TTN_Scene::SetGravity(glm::vec3 gravity)
	{
		btVector3 grav = btVector3(gravity.x, gravity.y, gravity.z);
		m_physicsWorld->setGravity(grav);
	}

	//gets the vector representing the gravity
	glm::vec3 TTN_Scene::GetGravity()
	{
		btVector3 grav = m_physicsWorld->getGravity();
		return glm::vec3((float)grav.getX(), (float)grav.getY(), (float)grav.getZ());
	}

	//makes all the collision objects by going through all the overalapping manifolds in bullet
	//based on code from https://andysomogyi.github.io/mechanica/bullet.html specfically the first block in the bullet callbacks and triggers section
	void TTN_Scene::ConstructCollisions()
	{
		//clear all the collisions from the previous frame
		collisions.clear();

		int numManifolds = m_physicsWorld->getDispatcher()->getNumManifolds();
		//iterate through all the manifolds
		for (int i = 0; i < numManifolds; i++) {
			//get the contact manifolds and both objects
			btPersistentManifold* contactManifold = m_physicsWorld->getDispatcher()->getManifoldByIndexInternal(i);

			const btCollisionObject* obj0 = contactManifold->getBody0();
			const btCollisionObject* obj1 = contactManifold->getBody1();

			//iterate through all the contact points
			int numOfContacts = contactManifold->getNumContacts();
			for (int j = 0; j < numOfContacts; j++)
			{
				//get the contact point
				btManifoldPoint& point = contactManifold->getContactPoint(j);
				//if it's within the contact point distance
				if (point.getDistance() < 0.f) {
					//get the rigid bodies
					const btRigidBody* b0 = btRigidBody::upcast(obj0);
					const btRigidBody* b1 = btRigidBody::upcast(obj1);

					//and make a collision object
					TTN_Collision::scolptr newCollision = TTN_Collision::Create();
					newCollision->SetBody1(static_cast<entt::entity>(reinterpret_cast<uint32_t>(b0->getUserPointer())));
					newCollision->SetBody2(static_cast<entt::entity>(reinterpret_cast<uint32_t>(b1->getUserPointer())));

					//compare it to all the previous collisions
					bool shouldAdd = true;
					for (int k = 0; k < collisions.size(); k++) {
						if (TTN_Collision::same(newCollision, collisions[k])) {
							shouldAdd = false;
							break;
						}
					}
					//if it's a new collision then add to the list of collisions
					if (shouldAdd) collisions.push_back(newCollision);
				}
			}
		}
	}
}