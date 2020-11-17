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
		m_Albedo->SetClearColor(glm::vec4(1.0f));

		//set the specular to an all white texture by default
		m_SpecularMap = TTN_Texture2D::Create();
		m_SpecularMap->SetClearColor(glm::vec4(1.0f));
	}

	//default desctructor
	TTN_Material::~TTN_Material()
	{ }

	//sets the albedo texture
	void TTN_Material::SetAlbedo(TTN_Texture2D::stptr albedo)
	{
		m_Albedo = albedo;
	}

	//sets the shininess
	void TTN_Material::SetShininess(float shininess)
	{
		m_Shininess = shininess;
	}

	//sets the specular map texture
	void TTN_Material::SetSpecularMap(TTN_Texture2D::stptr specular)
	{
		m_SpecularMap = specular;
	}
}