//Titan Engine, by Atlas X Games 
// Material.cpp - source file for the class that represents materials on 3D objects

//include the header
#include "Titan/Material.h"

namespace Titan {
	//default constructor
	TTN_Material::TTN_Material() 
		: m_Shininess(0)
	{
		//set the albedo to an all white texture by default
		m_Albedo = TTN_Texture2D::Create();
		m_Albedo->Clear(glm::vec4(1.0f));

		//set the specular to an all white texture by default
		m_SpecularMap = TTN_Texture2D::Create();
		m_SpecularMap->Clear(glm::vec4(1.0f));
		//set the cube map to an all white texture by default
		m_SkyboxTexture = TTN_TextureCubeMap::Create();
		m_SkyboxTexture->Clear(glm::vec4(1.0f));
	}

	//default desctructor
	TTN_Material::~TTN_Material()
	{ }

	//sets the albedo texture
	void TTN_Material::SetAlbedo(TTN_Texture2D::st2dptr albedo)
	{
		m_Albedo = albedo;
	}

	//sets the shininess
	void TTN_Material::SetShininess(float shininess)
	{
		m_Shininess = shininess;
	}

	//sets the specular map texture
	void TTN_Material::SetSpecularMap(TTN_Texture2D::st2dptr specular)
	{
		m_SpecularMap = specular;
	}

	//sets a cube map texture for a skybox
	void TTN_Material::SetSkybox(TTN_TextureCubeMap::stcmptr Skybox)
	{
		m_SkyboxTexture = Skybox;
	}
}