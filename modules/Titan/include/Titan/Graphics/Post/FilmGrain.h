//Titan Engine by Atlas X Games
//filmgrain.h - Header for the class for film grain post processing effects
#pragma once

//include the precompile header with a bunch of system
#include "Titan/ttn_pch.h"
#include "Titan/Graphics/Post/PostEffect.h"

namespace Titan {
	//class for color correction post processing effects
	class TTN_FilmGrain : public TTN_PostEffect
	{
	public:
		//defines a special easier to use name for shared(smart) pointers to the class
		typedef std::shared_ptr<TTN_FilmGrain> sfilmgptr;

		//creates and returns a shared(smart) pointer to the class
		static inline sfilmgptr Create() {
			return std::make_shared<TTN_FilmGrain>();
		}

	public:
		//Init framebuffer
		void Init(unsigned width, unsigned height) override;

		//Applies effect to this buffer
		//passes the previous framebuffer with the texture to apply as a parameter
		void ApplyEffect(TTN_PostEffect::spostptr buffer) override;

		//Getters
		float GetAmount() const { return m_amount; }
		//Setters
		void SetAmount(float intensity) { m_amount = intensity; }
	private:
		//intensity of the effect
		float m_amount = 0.10f;
	};
}