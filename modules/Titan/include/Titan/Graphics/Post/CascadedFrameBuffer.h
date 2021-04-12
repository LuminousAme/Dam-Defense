//Titan Engine by Atlas X Games
//CascadedFrameBuffer.h - Header for the class that represents framebuffer textures with a 3D texture array depth target
#pragma once

//include the precompile header
#include "Titan/ttn_pch.h"
//include other titan graphics features
#include "Titan/Graphics/Post/Framebuffer.h"

namespace Titan {
	//cascaded framebuffer class
	class TTN_CascadedFrameBuffer : public TTN_Framebuffer {
	public:
		//defines a special easier to use name for shared(smart) pointers to the class 
		typedef std::shared_ptr<TTN_CascadedFrameBuffer> scfboptr;

		//creates and returns a shared(smart) pointer to the class 
		static inline scfboptr Create() {
			return std::make_shared<TTN_CascadedFrameBuffer>();
		}

	public:
		//default constructor and destructors
		TTN_CascadedFrameBuffer();

		//init the framebuffer with a given size
		void Init(unsigned width, unsigned height);
	};
}