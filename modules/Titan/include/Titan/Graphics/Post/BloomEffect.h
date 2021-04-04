//Titan Engine by Atlas X Games
//BloomEffect.h - Header for the class for bloom post processing effects
#pragma once

//include the precompile header with a bunch of system stuff, as well as the base post effect class
#include "Titan/ttn_pch.h"
#include "Titan/Graphics/Post/PostEffect.h"

namespace Titan {
	//class for color correction post processing effects
	class TTN_BloomEffect : public TTN_PostEffect
	{
	public:
		//defines a special easier to use name for shared(smart) pointers to the class
		typedef std::shared_ptr<TTN_BloomEffect> sbloomptr;

		//creates and returns a shared(smart) pointer to the class
		static inline sbloomptr Create() {
			return std::make_shared<TTN_BloomEffect>();
		}

	public:
		//Init framebuffer
		void Init(unsigned width, unsigned height) override;

		//Applies effect to this buffer
		//passes the previous framebuffer with the texture to apply as a parameter
		void ApplyEffect(TTN_PostEffect::spostptr buffer) override;

		//reshapes the effect
		void Reshape(unsigned width, unsigned height);

		//Getters
		float GetThreshold() { return m_threshold; }
		int GetNumOfPasses() { return m_numOfPasses; }
		unsigned GetBlurDownScale() { return m_blurBufferDivisor; }
		float* GetWeights() { return m_weights; }
		float* GetSamples() { return m_samples; }
		float GetStrength() { return m_strength; }
		float GetRadius() { return m_radius; }
		//Setters
		void SetThreshold(float threshold) { m_threshold = threshold; }
		void SetNumOfPasses(int numOfPasses) { m_numOfPasses = numOfPasses; }
		void SetBlurDownScale(unsigned downscale);
		void SetWeights(float weights[5]) { for (int i = 0; i < 5; i++) m_weights[i] = weights[i]; }
		void SetRadius(float radius) { m_radius = radius; }
		void SetStrength(float strength) { m_strength = strength; }

	private:
		//the threshold for how much of the bright colours to extract
		float m_threshold = 0.2f;
		//the number of blur passes that should be performed
		int m_numOfPasses = 5;
		//the weights for the operation, the user can set these if they want to give them more fine control
		float m_weights[5] = { 0.22f, 0.19f, 0.12, 0.05, 0.03 };

		//smaples for radial blur
		float m_samples[10] = { -0.08f,-0.05f, -0.03f, -0.02f, -0.1f, 0.1f, 0.02f, 0.03f, 0.05f, 0.08f };
		//strength of radial blur
		float m_strength = 2.8f;

		//the scale by which the blur framebuffers should be reduced to save on memory
		unsigned m_blurBufferDivisor = 4.0;
		//the radius of the effect
		float m_radius = 1.0f;
	};
}