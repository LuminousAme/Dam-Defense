//Titan Engine by Atlas X Games
//ColorCorrect.h - Header for the class for color correction post processing effects
#pragma once

//include the precompile header with a bunch of system
#include "Titan/ttn_pch.h"
#include "Titan/Graphics/Post/PostEffect.h"
#include "Titan/Graphics/Post/LUT.h"

namespace Titan {
	//class for color correction post processing effects
	class TTN_ColorCorrect : public TTN_PostEffect
	{
	public:
		//defines a special easier to use name for shared(smart) pointers to the class 
		typedef std::shared_ptr<TTN_ColorCorrect> scolcorptr;

		//creates and returns a shared(smart) pointer to the class 
		static inline scolcorptr Create() {
			return std::make_shared<TTN_ColorCorrect>();
		}

	public:
		//Init framebuffer
		void Init(unsigned width, unsigned height) override;

		//Applies effect to this buffer
		//passes the previous framebuffer with the texture to apply as a parameter
		void ApplyEffect(TTN_PostEffect::spostptr buffer) override;

		//Getters
		float GetIntensity() const { return m_intensity; }
		TTN_LUT3D::sltptr GetCube() const { return m_cube; }
		//Setters
		void SetIntensity(float intensity) { m_intensity = intensity; }
		void SetCube(TTN_LUT3D::sltptr cube) { m_cube = cube; }
	private:
		//intensity of the effect
		float m_intensity = 1.0f;
		//cube look up table
		TTN_LUT3D::sltptr m_cube;
	};
}