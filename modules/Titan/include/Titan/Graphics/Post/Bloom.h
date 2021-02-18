//Titan Engine by Atlas X Games
//Bloom.h - Header for the class for bloom post processing effects
#pragma once

//include the precompile header with a bunch of system
#include "Titan/ttn_pch.h"
#include "Titan/Graphics/Post/PostEffect.h"

namespace Titan {

	class TTN_Bloom : public TTN_PostEffect
	{
	public:

	public:
		//Init framebuffer
		void Init(unsigned width, unsigned height) override;

		//Applies effect to this buffer
		//passes the previous framebuffer with the texture to apply as a parameter
		void ApplyEffect(TTN_PostEffect::spostptr buffer) override;

		//Getters
		float GetThreshold() const { return m_threshold; }
		int GetAmount() const { return m_amount; }
		
		//Setters
		void SetThreshold(float threshold) { m_threshold = threshold; }
		void SetAmount(int amount) { m_amount = amount; }

		
	private:
		//threshold for brightness
		float m_threshold = 0.01f;
		//amount of times to blur
		int m_amount = 5; 
	};





}



