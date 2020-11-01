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
	}

	TTN_Scene::TTN_Scene(glm::vec3 AmbientLightingColor, float AmbientLightingStrength)
		: m_AmbientColor(AmbientLightingColor), m_AmbientStrength(AmbientLightingStrength)
	{
		m_ShouldRender = true;
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
			if (renderer.GetMesh()->GetMatPointer() != nullptr)
			{
				//give openGL the shinniess
				shader->SetUniform("u_Shininess", renderer.GetMesh()->GetMatPointer()->GetShininess());
				//if they're using a texture 
				if (shader->GetFragShaderDefaultStatus() == 4 || shader->GetFragShaderDefaultStatus() == 5)
				{
					//bind it so openGL can see it
					renderer.GetMesh()->GetMatPointer()->GetAlbedo()->Bind(0);
				}
				//if they're using a specular map 
				if (shader->GetFragShaderDefaultStatus() == 5) 
				{
					//bind it so openGL can see it
					renderer.GetMesh()->GetMatPointer()->GetSpecularMap()->Bind(1);
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

}