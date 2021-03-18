//Titan Engine, by Atlas X Games

//precompile header, this file uses entt.hpp, and GLM/gtc/matrix_transform.hpp
#include "Titan/ttn_pch.h"
// Scene.cpp - source file for the class that handles ECS, render calls, etc.
#include "Titan/Utilities/Scene.h"

namespace Titan {
	//default constructor

	TTN_Scene::TTN_Scene(std::string name)
		: m_sceneName(name)
	{
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

		//init the basic effect
		glm::ivec2 windowSize = TTN_Backend::GetWindowSize();
		m_emptyEffect = TTN_PostEffect::Create();
		m_emptyEffect->Init(windowSize.x, windowSize.y);

	/*	gBuffer = TTN_GBuffer::Create();
		gBuffer->Init(windowSize.x, windowSize.y);

		illBuffer = TTN_IlluminationBuffer::Create();
		illBuffer->Init(windowSize.x, windowSize.y);*/

		//shadow buffer
		shadowWidth = 1024;
		shadowHeight = 1024;
		shadowBuffer = TTN_Framebuffer::Create();
		shadowBuffer->AddDepthTarget();
		shadowBuffer->Init(shadowWidth, shadowHeight);

		//directional light buffer
		sunBuffer.AllocateMemory(sizeof(TTN_DirectionalLight));
		sunBuffer.SendData(reinterpret_cast<void*>(&m_Sun), sizeof(TTN_DirectionalLight));
	}

	//construct with lightning data
	TTN_Scene::TTN_Scene(glm::vec3 AmbientLightingColor, float AmbientLightingStrength, std::string name)
		: m_AmbientColor(AmbientLightingColor), m_AmbientStrength(AmbientLightingStrength), m_sceneName(name)
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

		//init the basic effect
		glm::ivec2 windowSize = TTN_Backend::GetWindowSize();
		m_emptyEffect = TTN_PostEffect::Create();
		m_emptyEffect->Init(windowSize.x, windowSize.y);

		/*gBuffer = TTN_GBuffer::Create();
		gBuffer->Init(windowSize.x, windowSize.y);

		illBuffer = TTN_IlluminationBuffer::Create();
		illBuffer->Init(windowSize.x, windowSize.y);*/

		//shadow buffer
		shadowWidth = 1024;
		shadowHeight = 1024;
		shadowBuffer = TTN_Framebuffer::Create();
		shadowBuffer->AddDepthTarget();
		shadowBuffer->Init(shadowWidth, shadowHeight);

		//directional light buffer
		sunBuffer.AllocateMemory(sizeof(TTN_DirectionalLight));
		sunBuffer.SendData(reinterpret_cast<void*>(&m_Sun), sizeof(TTN_DirectionalLight));
	}

	//destructor
	TTN_Scene::~TTN_Scene() {
		//unload the scene before it's deleted
		Unload();
	}

	//function to create a new entity, returns it's entity number
	entt::entity TTN_Scene::CreateEntity(std::string name)
	{
		//create the entity
		auto entity = m_Registry->create();

		//attach a name compoment
		TTN_Name entityName = TTN_Name(name);
		AttachCopy(entity, entityName);

		//return the entity id
		return entity;
	}

	//function to create a new entity that deletes after a certain ammount of time, returns it's entity number
	entt::entity TTN_Scene::CreateEntity(float lifeTime, std::string name)
	{
		//create the entity
		entt::entity entity = CreateEntity(name);

		//attach a countdown component
		TTN_DeleteCountDown entityCountDown = TTN_DeleteCountDown(lifeTime);
		AttachCopy(entity, entityCountDown);

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

		//remove any parents and children
		if (m_Registry->has<TTN_Transform>(entity)) {
			Get<TTN_Transform>(entity).SetParent(nullptr, entt::null);
			Get<TTN_Transform>(entity).RemoveAllChildren();
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
			//reset it's children
			Get<TTN_Transform>(entity).ResetChildren();

			//if it should have a parent
			if (Get<TTN_Transform>(entity).GetParentEntity() != entt::null) {
				//then reatach that parent
				Get<TTN_Transform>(entity).SetParent(&Get<TTN_Transform>(Get<TTN_Transform>(entity).GetParentEntity()),
					Get<TTN_Transform>(entity).GetParentEntity());
			}
		}
	}

	//update the scene, running physics simulation, animations, and particle systems
	void TTN_Scene::Update(float deltaTime)
	{
		//bind the sun buffer
		//sunBuffer.Bind(0);

		//clear the shadow buffer
		shadowBuffer->Clear();
	/*	gBuffer->Clear();
		illBuffer->Clear();*/

		//only run the updates if the scene is not paused
		if (!m_Paused) {
			//call the step simulation for bullet
			m_physicsWorld->stepSimulation(deltaTime);

			//run through all of the physicsbody in the scene
			auto physicsBodyView = m_Registry->view<TTN_Physics>();
			for (auto entity : physicsBodyView) {
				//make sure the physics body are active on every frame
				Get<TTN_Physics>(entity).GetRigidBody()->setActivationState(true);

				//make sure those that shouldn't have gravity don't
				if (Get<TTN_Physics>(entity).GetHasGravity() == false) {
					Get<TTN_Physics>(entity).GetRigidBody()->setGravity(btVector3(0.0f, 0.0f, 0.0f));
				}

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

			//run through all of the entities with a 2D animatior and update it
			auto v_2DanimatorView = m_Registry->view<TTN_2DAnimator>();
			for (auto entity : v_2DanimatorView) {
				//update the active animation
				Get<TTN_2DAnimator>(entity).GetActiveAnimRef().Update(deltaTime);

				//if it also has a sprite renderer update the sprite it should be rendering
				if (Has<TTN_Renderer2D>(entity)) {
					Get<TTN_Renderer2D>(entity).SetSprite(Get<TTN_2DAnimator>(entity).GetActiveAnimRef().GetCurrentFrame());
				}
			}

			//run through all the of the entities with a particle system and run their updates
			auto psView = m_Registry->view<TTN_ParticeSystemComponent>();
			for (auto entity : psView) {
				//update the particle system
				Get<TTN_ParticeSystemComponent>(entity).GetParticleSystemPointer()->Update(deltaTime);
			}

			//list of entities to delete this frame
			std::vector<entt::entity> entitiesToDelete = std::vector<entt::entity>();
			//run through all the entities with a limited lifetime, run their updates and delete them if their lifetimes have ended
			auto deleteView = m_Registry->view<TTN_DeleteCountDown>();
			for (auto entity : deleteView) {
				//update the countdown
				Get<TTN_DeleteCountDown>(entity).Update(deltaTime);
				//check if it should delete
				if (Get<TTN_DeleteCountDown>(entity).GetLifeLeft() <= 0.0f) {
					//if it should, add the entity to a list of entities to delete
					entitiesToDelete.push_back(entity);
				}
			}
			//loop through and delete all the entities
			std::vector<entt::entity>::iterator it = entitiesToDelete.begin();
			while (it != entitiesToDelete.end()) {
				DeleteEntity(*it);
				it = entitiesToDelete.erase(it);
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

		//unbind the empty effect and run through all the post effect
		//m_emptyEffect->UnbindBuffer();
		/*gBuffer->Unbind();
		illBuffer->BindBuffer(0);*/

//		illBuffer->UnbindBuffer();

		shadowBuffer->BindDepthAsTexture(30);
	//	illBuffer->ApplyEffect(gBuffer);
		shadowBuffer->UnbindTexture(30);

		//if there are post processing effects that can be applied
		if (m_PostProcessingEffects.size() > 0) {
			//track the index of the last effect that was applied
			int index = -1;
			//and iterate through all the post processing effects
			for (int i = 0; i < m_PostProcessingEffects.size(); i++) {
				//if the effect should be applied
				if (m_PostProcessingEffects[i]->GetShouldApply()) {
					//apply the effect
					if (index == -1)
						m_PostProcessingEffects[i]->ApplyEffect(m_emptyEffect);
					else
						m_PostProcessingEffects[i]->ApplyEffect(m_PostProcessingEffects[index]);

					//and save the index as this was most recent effect applied
					index = i;
				}
			}
			//at the end, draw to the screen
			if (index == -1) {
				//if none should be applied, just draw the empty effect
				m_emptyEffect->DrawToScreen();
				//and save it as the last effect played
				TTN_Backend::SetLastEffect(m_emptyEffect);
			}
			else {
				//if they should be applied, draw from the last effect
				m_PostProcessingEffects[index]->DrawToScreen();
				//and save it
				TTN_Backend::SetLastEffect(m_PostProcessingEffects[index]);
			}
		}
		//if there are no post processing effects to apply, just save the empty effect
		else {
			m_emptyEffect->DrawToScreen();
			//and save it as the last effect played
			TTN_Backend::SetLastEffect(m_emptyEffect);
		}

		//gBuffer->DrawBuffersToScreen();
		////illBuffer->DrawToScreen();
		//illBuffer->DrawIllumBuffer();

		//unbind the sun buffer
	//	sunBuffer.Unbind(0);
	}

	//renders all the messes in our game
	void TTN_Scene::Render()
	{
		//clear all the post processing effects
		m_emptyEffect->Clear();
		for (int i = 0; i < m_PostProcessingEffects.size(); i++)
			m_PostProcessingEffects[i]->Clear();

		//gBuffer->Clear();
		//illBuffer->Clear();

		//get the view and projection martix
		glm::mat4 vp;
		//update the camera for the scene
		//set the camera's position to it's transform
		Get<TTN_Camera>(m_Cam).SetPosition(Get<TTN_Transform>(m_Cam).GetPos());
		//save the view and projection matrix
		vp = Get<TTN_Camera>(m_Cam).GetProj();
		glm::mat4 viewMat = glm::inverse(Get<TTN_Transform>(m_Cam).GetGlobal());
		vp *= viewMat;

		//set up light space matrix
		glm::mat4 lightProjectionMatrix = glm::ortho(-shadowOrthoXY, shadowOrthoXY, -shadowOrthoXY, shadowOrthoXY, -shadowOrthoZ, shadowOrthoZ);
		glm::mat4 lightViewMatrix = glm::lookAt(glm::vec3(m_Sun.m_lightDirection), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceViewProj = lightProjectionMatrix * lightViewMatrix;

	//	illBuffer->SetLightSpaceViewProj(lightSpaceViewProj);
		//glm::vec3 camPos = glm::inverse(viewMat) * glm::vec4(0, 0, 0, 1);
		//illBuffer->SetCamPos(camPos);

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
		//shadow depth pass
		//set the viewport
		glViewport(0, 0, shadowWidth, shadowHeight);
		//bind the framebuffer
		shadowBuffer->Bind();
		//bind the basic depth shader
		TTN_Shader::sshptr simpleShadowShader = TTN_Renderer::GetSimpleShadowShader();
		simpleShadowShader->Bind();

		glCullFace(GL_FRONT);

		//loop through all of the meshes
		m_RenderGroup->each([&](entt::entity entity, TTN_Transform& transform, TTN_Renderer& renderer) {
			// Render the mesh if it should be casting shadows
			if (renderer.GetCastShadows()) {
				simpleShadowShader->SetUniformMatrix("u_Model", transform.GetGlobal());
				simpleShadowShader->SetUniformMatrix("u_LightSpaceMatrix", lightSpaceViewProj);
				if (Has<TTN_MorphAnimator>(entity))
					simpleShadowShader->SetUniform("t", Get<TTN_MorphAnimator>(entity).getActiveAnimRef().getInterpolationParameter());
				else
					simpleShadowShader->SetUniform("t", 0.0f);
				renderer.Render(transform.GetGlobal(), vp, lightSpaceViewProj);
			}
		});

		glCullFace(GL_BACK);

		simpleShadowShader->UnBind();

		//unbind the shadow framebuffer
		shadowBuffer->Unbind();

		//TTN_Shader::sshptr gBufferShader = TTN_Renderer::GetgBufferShader();
		//gBufferShader->Bind();
		////loop through all of the meshes
		//m_RenderGroup->each([&](entt::entity entity, TTN_Transform& transform, TTN_Renderer& renderer) {
		//	int textureSlot = 0;
		//	// if renderer has material
		//	if (renderer.GetMat() != nullptr) {
		//		//gBufferShader->SetUniformMatrix("u_Model", transform.GetGlobal());
		//		//gBufferShader->SetUniformMatrix("u_LightSpaceMatrix", lightSpaceViewProj);

		//		gBufferShader->SetUniform("u_UseDiffuse", (int)renderer.GetMat()->GetUseAlbedo());
		//		renderer.GetMat()->GetAlbedo()->Bind(textureSlot);
		//		textureSlot++;

		//		renderer.GetMat()->GetSpecularMap()->Bind(textureSlot);
		//		textureSlot++;
		//		//gBufferShader->SetUniform("s_Diffuse", renderer.GetMat()->GetAlbedo());
		//		//gBufferShader->SetUniformMatrix("u_Specular", lightSpaceViewProj);
		//	}
		//});
		//gBufferShader->UnBind();

		//normal render pass
		glm::ivec2 windowSize = TTN_Backend::GetWindowSize();
		glViewport(0, 0, windowSize.x, windowSize.y);

		//before going through see if it needs to render another scene as the background first
		if (TTN_Backend::GetLastEffect() != nullptr) {
			//if it does, apply the buffer from that scene before drawing
			m_emptyEffect->ApplyEffect(TTN_Backend::GetLastEffect());
		}

		//bind the empty effect
		//gBuffer->Bind();
		//m_emptyEffect->BindBuffer(0); //this gets unbound in postRender

		TTN_Shader::sshptr currentShader = nullptr;
		TTN_Material::smatptr currentMatieral = nullptr;
		TTN_Mesh::smptr currentMesh = nullptr;
		bool morphAnimatedLastMesh = false;

		m_RenderGroup->each([&](entt::entity entity, TTN_Transform& transform, TTN_Renderer& renderer) {
			//bool to track if uniforms have been reset
			bool shaderChanged = false;
			//texture slot to dynamically send textures across different types of shaders
			int textureSlot = 0;

			//if the shader has changed
			if (currentShader != renderer.GetShader() && renderer.GetShader() != nullptr) {
				//unbind the old shader
				if (currentShader != nullptr) currentShader->UnBind();
				//mark that all the uniforms have been reset this frame
				shaderChanged = true;
				//update to the current shader
				currentShader = renderer.GetShader();
				//and bind it
				currentShader->Bind();

				//if the fragment shader is a default shader other than the skybox
				if (currentShader->GetFragShaderDefaultStatus() != (int)TTN_DefaultShaders::FRAG_SKYBOX
					&& currentShader->GetFragShaderDefaultStatus() != (int)TTN_DefaultShaders::NOT_DEFAULT) {
					//sets some uniforms
					//scene level ambient lighting
					currentShader->SetUniform("u_AmbientCol", m_AmbientColor);
					currentShader->SetUniform("u_AmbientStrength", m_AmbientStrength);

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
					currentShader->SetUniform("u_LightPos", lightPositions[0], 16);
					currentShader->SetUniform("u_LightCol", lightColor[0], 16);
					currentShader->SetUniform("u_AmbientLightStrength", lightAmbientStr[0], 16);
					currentShader->SetUniform("u_SpecularLightStrength", lightSpecStr[0], 16);
					currentShader->SetUniform("u_LightAttenuationConstant", lightAttenConst[0], 16);
					currentShader->SetUniform("u_LightAttenuationLinear", lightAttenLinear[0], 16);
					currentShader->SetUniform("u_LightAttenuationQuadratic", lightAttenQuadartic[0], 16);

					//and tell it how many lights there actually are
					currentShader->SetUniform("u_NumOfLights", (int)m_Lights.size());

					//stuff from the camera
					currentShader->SetUniform("u_CamPos", Get<TTN_Transform>(m_Cam).GetGlobalPos());
				}

				//if the vertex shader is a default shader other than the skybox
				if (currentShader->GetFragShaderDefaultStatus() != (int)TTN_DefaultShaders::FRAG_SKYBOX
					&& currentShader->GetFragShaderDefaultStatus() != (int)TTN_DefaultShaders::NOT_DEFAULT) {
					//send in the lightspace view projection matrix so it can recieve shadows correctly
					currentShader->SetUniformMatrix("u_LightSpaceMatrix", lightSpaceViewProj);
				}
			}

			//if the material, or shader has changed, and is not nullptr set some data from
			if (((shaderChanged || currentMatieral != renderer.GetMat()) && renderer.GetMat() != nullptr)) {
				//set this material to the current material
				currentMatieral = renderer.GetMat();
				//set the shinniness
				currentShader->SetUniform("u_Shininess", currentMatieral->GetShininess());
				//and material details about the lighting and shading
				currentShader->SetUniform("u_hasAmbientLighting", (int)(currentMatieral->GetHasAmbient()));
				currentShader->SetUniform("u_hasSpecularLighting", (int)(currentMatieral->GetHasSpecular()));
				//the ! is because it has to be reversed in the shader
				currentShader->SetUniform("u_hasOutline", (int)(!currentMatieral->GetHasOutline()));
				currentShader->SetUniform("u_OutlineSize", currentMatieral->GetOutlineSize());

				//wheter or not ramps for toon shading should be used
				currentShader->SetUniform("u_useDiffuseRamp", currentMatieral->GetUseDiffuseRamp());
				currentShader->SetUniform("u_useSpecularRamp", currentMatieral->GetUseSpecularRamp());

				//bind the ramps as textures
				currentMatieral->GetDiffuseRamp()->Bind(10);
				currentMatieral->GetSpecularRamp()->Bind(11);

				//bind the shadow map as a texture
				//shadowBuffer->BindDepthAsTexture(30);

				//set if the current material should use shadows or not
				currentShader->SetUniform("u_recievesShadows", (int)currentMatieral->GetRecievesShadows());

				//if this is a height map shader
				//if they're using a displacement map
				if (currentShader->GetVertexShaderDefaultStatus() == (int)TTN_DefaultShaders::VERT_COLOR_HEIGHTMAP
					|| currentShader->GetVertexShaderDefaultStatus() == (int)TTN_DefaultShaders::VERT_NO_COLOR_HEIGHTMAP)
				{
					//bind it to the slot
					renderer.GetMat()->GetHeightMap()->Bind(textureSlot);
					//update the texture slot for future textures to use
					textureSlot++;
					//and pass in the influence
					currentShader->SetUniform("u_influence", renderer.GetMat()->GetHeightInfluence());
				}

				//if it's a shader with albedo
				if (currentShader->GetFragShaderDefaultStatus() == (int)TTN_DefaultShaders::FRAG_BLINN_PHONG_ALBEDO_AND_SPECULAR ||
					currentShader->GetFragShaderDefaultStatus() == (int)TTN_DefaultShaders::FRAG_BLINN_PHONG_ALBEDO_ONLY) {
					//set wheter or not it should use it's albedo texture
					currentShader->SetUniform("u_UseDiffuse", (int)currentMatieral->GetUseAlbedo());
					//and bind that albedo
					currentMatieral->GetAlbedo()->Bind(textureSlot);
					textureSlot++;
				}

				//if it's a shader with a specular map
				if (currentShader->GetFragShaderDefaultStatus() == (int)TTN_DefaultShaders::FRAG_BLINN_PHONG_ALBEDO_AND_SPECULAR) {
					//bind that specular map
					currentMatieral->GetSpecularMap()->Bind(textureSlot);
					textureSlot++;
				}

				//if it's a skybox
				if (currentShader->GetFragShaderDefaultStatus() == (int)TTN_DefaultShaders::FRAG_SKYBOX) {
					//bind the skybox texture
					currentMatieral->GetSkybox()->Bind(textureSlot);
					textureSlot++;
					//set the rotation matrix uniform
					currentShader->SetUniformMatrix("u_EnvironmentRotation", glm::mat3(glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1, 0, 0))));
					//set the skybox matrix uniform
					currentShader->SetUniformMatrix("u_SkyboxMatrix", Get<TTN_Camera>(m_Cam).GetProj() * glm::mat4(glm::mat3(viewMat)));
				}
			}
			//otherwise just set a default shinnines
			else if (currentShader != nullptr) {
				currentShader->SetUniform("u_Shininess", 128.0f);
			}

			//if it is on a morph animated shader, set the interpolation parameter uniform
			if (currentShader->GetVertexShaderDefaultStatus() == (int)TTN_DefaultShaders::VERT_MORPH_ANIMATION_NO_COLOR ||
				currentShader->GetVertexShaderDefaultStatus() == (int)TTN_DefaultShaders::VERT_MORPH_ANIMATION_COLOR) {
				//try to get an animator component
				if (Has<TTN_MorphAnimator>(entity)) {
					currentShader->SetUniform("t", Get<TTN_MorphAnimator>(entity).getActiveAnimRef().getInterpolationParameter());
				}
				else
					currentShader->SetUniform("t", 0.0f);
			}

			//if the entity has an animator
			if (Has<TTN_MorphAnimator>(entity)) {
				currentMesh = renderer.GetMesh();
				//set up the vao on the mesh properly
				currentMesh->SetUpVao(Get<TTN_MorphAnimator>(entity).getActiveAnimRef().getCurrentMeshIndex(),
					Get<TTN_MorphAnimator>(entity).getActiveAnimRef().getNextMeshIndex());
				//set the last mesh as having been animated
				morphAnimatedLastMesh = true;
			}
			//if it doesn't
			else if (currentMesh != renderer.GetMesh() || !morphAnimatedLastMesh) {
				//save the mesh
				currentMesh = renderer.GetMesh();
				//set up the vao with both mesh indices on zero
				currentMesh->SetUpVao();
				//set the last mesh as having not been animated
				morphAnimatedLastMesh = false;
			}

			//and finish by rendering the mesh
			renderer.Render(transform.GetGlobal(), vp, lightSpaceViewProj);
		});

		//2D sprite rendering
		//make a vector to store all the entities to render
		std::vector<entt::entity> tempSpriteEntitiesToRender = std::vector<entt::entity>();
		//go through every entity with a 2d renderer and a transform, addding them to the list of entities to render
		auto render2DView = m_Registry->view<TTN_Transform, TTN_Renderer2D>();
		for (entt::entity entity : render2DView) {
			tempSpriteEntitiesToRender.push_back(entity);
		}

		//sort the entities by their z positions
		mergeSortEntitiesZ(tempSpriteEntitiesToRender, 0, tempSpriteEntitiesToRender.size() - 1);

		//bind their shader
		TTN_Renderer2D::BindShader();
		//and loop through, rendering them in reverse order
		for (int i = tempSpriteEntitiesToRender.size() - 1; i >= 0; i--)
			Get<TTN_Renderer2D>(tempSpriteEntitiesToRender[i]).Render(Get<TTN_Transform>(tempSpriteEntitiesToRender[i]).GetGlobal(), vp);
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

	//sets the directional light for the scene
	void TTN_Scene::SetSun(TTN_DirectionalLight newSun)
	{
		//save the sun data
		m_Sun = newSun;
		//and update the sun buffer
		sunBuffer.SendData(reinterpret_cast<void*>(&m_Sun), sizeof(TTN_DirectionalLight));
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
					const btVector3& location = point.getPositionWorldOnA();
					const btVector3& location2 = point.getPositionWorldOnB();
					glm::vec3 collisionLocation = (glm::vec3(location.getX(), location.getY(), location.getZ())
						+ glm::vec3(location2.getX(), location2.getY(), location2.getZ())) * 0.5f;

					//and make a collision object
					TTN_Collision::scolptr newCollision = TTN_Collision::Create();
					newCollision->SetBody1(static_cast<entt::entity>(reinterpret_cast<uint32_t>(b0->getUserPointer())));
					newCollision->SetBody2(static_cast<entt::entity>(reinterpret_cast<uint32_t>(b1->getUserPointer())));
					newCollision->SetCollisionPoint(collisionLocation);

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