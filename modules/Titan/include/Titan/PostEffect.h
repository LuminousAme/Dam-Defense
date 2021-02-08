#pragma once
#include "ttn_pch.h"
#include "Framebuffer.h"
#include "Shader.h"


namespace Titan {
	class TTN_PostEffect
	{
	public:

		//init effect (override in each derived class)
		virtual void Init(unsigned width, unsigned height);

		//applies effect
		virtual void ApplyEffect(TTN_PostEffect* prevBuffer);
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


	protected:
		//holds all our buffers for the effects
		std::vector <TTN_Framebuffer*> _buffers;

		//holds all our shaders for the effects
		std::vector <TTN_Shader::sshptr> _shaders;



	};

}
