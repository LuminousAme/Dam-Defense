//Titan Engine by Atlas X Games
//PostEffect.h - Header for the base class that all post processing effects polymorphically inherit from
#pragma once

//include the precompile header
#include "ttn_pch.h"
//include the graphics features needed
#include "Framebuffer.h"
#include "Shader.h"

namespace Titan {
	//base class for post processing effects
	class TTN_PostEffect
	{
	public:
		//defines a special easier to use name for shared(smart) pointers to the class 
		typedef std::shared_ptr<TTN_PostEffect> spostptr;

		//creates and returns a shared(smart) pointer to the class 
		static inline spostptr Create() {
			return std::make_shared<TTN_PostEffect>();
		}

	public:
		//destructor, just calls unload
		~TTN_PostEffect() { Unload(); }

		//init effect (override in each derived class)
		virtual void Init(unsigned width, unsigned height);

		//applies effect
		virtual void ApplyEffect(spostptr prevBuffer);

		//renders the effect to the screen
		virtual void DrawToScreen();

		//reshapes buffer
		virtual void Reshape(unsigned width, unsigned height);

		//clears the buffers
		void Clear();

		//unload buffers
		void Unload();

		//binds buffers
		void BindBuffer(int index);
		void UnbindBuffer();

		//bind textures
		void BindColorAsTexture(int index, int colorBuffer, int textureSlot);
		void BindDepthAsTexture(int index, int textureSlot);
		void UnbindTexture(int textureSlot);

		//bind shaders
		void BindShader(int index);
		void UnbindShader();

		//get if the effect should be applied 
		bool GetShouldApply() { return m_shouldRender; }
		//set if the effect should be applied
		void SetShouldApply(bool shouldApply) { m_shouldRender = shouldApply; }

	protected:
		//holds all our buffers for the effects
		std::vector <TTN_Framebuffer::sfboptr> m_buffers;

		//holds all our shaders for the effects
		std::vector <TTN_Shader::sshptr> m_shaders;

		//bool for if the effect should be rendered
		bool m_shouldRender = true;
	};
}