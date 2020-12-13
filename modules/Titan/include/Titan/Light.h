//Titan Engine, by Atlas X Games 
// Lights.h - header for the class that represents lights in a scene
#pragma once

//precompile header, this file uses GLM/glm.hpp
#include "ttn_pch.h"

namespace Titan {
	//class that contains all the data for a light in a scene
	class TTN_Light {
	public:
		//default constructor
		TTN_Light();
		//default desctructor
		~TTN_Light();

		//constructor with all the data 
		TTN_Light(glm::vec3 color, float ambientStrength, float specularStrength, float constantAttenuation, float linearAttenuation, float quadraticAttenuation);

		//copy, move, and assingment constrcutors for ENTT
		TTN_Light(const TTN_Light&) = default;
		TTN_Light(TTN_Light&&) = default;
		TTN_Light& operator=(TTN_Light&) = default;

		//setters
		//color
		void SetColor(glm::vec3 color);
		//strenghts
		void SetAmbientStrength(float ambientStrength);
		void SetSpecularStrength(float specularStrength);
		//Attenuation
		void SetConstantAttenuation(float constantAttenuation);
		void SetLinearAttenuation(float linearAttenuation);
		void SetQuadraticAttenuation(float quadraticAttenuation);

		//getters
		//color
		glm::vec3 GetColor() { return m_Color; }
		//strenghts
		float GetAmbientStrength() { return m_AmbientStr; }
		float GetSpecularStrength() { return m_SpecularStr; }
		//Attenuation 
		float GetConstantAttenuation() { return m_ConstAttenutation; }
		float GetLinearAttenuation() { return m_LinearAttenuation; }
		float GetQuadraticAttenuation() { return m_QuadraticAttenuation; }

	private:
		glm::vec3 m_Color;
		float m_AmbientStr;
		float m_SpecularStr;
		float m_ConstAttenutation;
		float m_LinearAttenuation;
		float m_QuadraticAttenuation;
	};
}
