//Titan Engine by Atlas X Games
//CombinationBuffer.h - Header for the class for the post processing effect that combines a framebuffer into this framebuffer
#pragma once

//include the precompile header with a bunch of system stuff, as well as the base post effect class
#include "Titan/ttn_pch.h"
#include "Titan/Graphics/Post/PostEffect.h"

namespace Titan {
	//class for framebuffer merging post processing effects
	class TTN_CombineFrameBuffer : public TTN_PostEffect
	{
	public:
		//defines a special easier to use name for shared(smart) pointers to the class 
		typedef std::shared_ptr<TTN_CombineFrameBuffer> scombineptr;

		//creates and returns a shared(smart) pointer to the class 
		static inline scombineptr Create() {
			return std::make_shared<TTN_CombineFrameBuffer>();
		}

	public:
		//Init framebuffer
		void Init(unsigned width, unsigned height) override;

		//Applies effect to this buffer
		//passes the previous framebuffer with the texture to apply as a parameter
		void ApplyEffect(TTN_PostEffect::spostptr buffer) override;
	};
}