//Titan Engine by Atlas X Games
//Pixelation.h - Header for the class for pixelated post processing effects
#pragma once

//include the precompile header with a bunch of system
#include "Titan/ttn_pch.h"
#include "Titan/Graphics/Post/PostEffect.h"

namespace Titan {
	//class for color correction post processing effects
	class TTN_Pixelation : public TTN_PostEffect
	{
	public:
		//defines a special easier to use name for shared(smart) pointers to the class
		typedef std::shared_ptr<TTN_Pixelation> spixelptr;

		//creates and returns a shared(smart) pointer to the class
		static inline spixelptr Create() {
			return std::make_shared<TTN_Pixelation>();
		}

	public:
		//Init framebuffer
		void Init(unsigned width, unsigned height) override;

		//Applies effect to this buffer
		//passes the previous framebuffer with the texture to apply as a parameter
		void ApplyEffect(TTN_PostEffect::spostptr buffer) override;

		//Getters
		float GetPixels() const { return m_pixel; }
		//Setters
		void SetPixels(float intensity) { m_pixel = intensity; }
	private:
		//intensity of the effect
		float m_pixel = 1024.0f;
	};
}