//Titan Engine, by Atlas X Games 
// Material.h - header for the class that represents materials on 3D objects
#pragma once

//include texture class
#include "Texture2D.h"
#include "TextureCubeMap.h"

namespace Titan {
	//class for materials on 3D objects
	class TTN_Material {
	public:
		//defines a special easier to use name for shared(smart) pointers to the class 
		typedef std::shared_ptr<TTN_Material> smatptr;

		//creates and returns a shared(smart) pointer to the class 
		static inline smatptr Create() {
			return std::make_shared<TTN_Material>();
		}

	public:
		//ensuring moving and copying is not allowed so we can control destructor calls through pointers
		TTN_Material(const TTN_Material& other) = delete;
		TTN_Material(TTN_Material& other) = delete;
		TTN_Material& operator=(const TTN_Material& other) = delete;
		TTN_Material& operator=(TTN_Material&& other) = delete;

	public:
		//default constructor
		TTN_Material();
		//default destructor
		~TTN_Material();

		//setters
		void SetAlbedo(TTN_Texture2D::st2dptr albedo);
		void SetShininess(float shininess);
		void SetSpecularMap(TTN_Texture2D::st2dptr specular);
		void SetSkybox(TTN_TextureCubeMap::stcmptr Skybox);

		//getters
		TTN_Texture2D::st2dptr GetAlbedo() { return m_Albedo; }
		float GetShininess() { return m_Shininess; }
		TTN_Texture2D::st2dptr GetSpecularMap() { return m_SpecularMap; }
		TTN_TextureCubeMap::stcmptr GetSkybox() { return m_SkyboxTexture; }

	private:
		TTN_Texture2D::st2dptr m_Albedo;
		TTN_Texture2D::st2dptr m_SpecularMap;
		float m_Shininess;
		TTN_TextureCubeMap::stcmptr m_SkyboxTexture;
	};
}