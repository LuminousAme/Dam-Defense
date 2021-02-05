//Titan Engine, by Atlas X Games 
// ITexture.h - header for the base class that different types of textures inherit from
#pragma once

//precompile header, this file uses cstdint, memory, glad/glad.h, and GLM/glm.hpp
#include "ttn_pch.h"

namespace Titan {
	//base class for different texture types
	class TTN_ITexture {
	public:
		//define names for shared and unique pointers of this class
		typedef std::shared_ptr<TTN_ITexture> sitptr;
		typedef std::unique_ptr<TTN_ITexture> uitptr;

		//struct representing limits avaible to OpenGl textures based on the current renderer
		struct TTN_Texture_Limits {
			int   MAX_TEXTURE_SIZE;
			int   MAX_TEXTURE_UNITS;
			int   MAX_3D_TEXTURE_SIZE;
			int   MAX_TEXTURE_IMAGE_UNITS;
			float MAX_ANISOTROPY;
		};

		// Gets the texture limits on the current GPU
		static const TTN_Texture_Limits& GetLimits() { return _limits; }

		// Unbinds a texture from the given slot
		static void Unbind(int slot);

		// Gets the underlying OpenGL handle for this texture
		GLuint& GetHandle() { return _handle; }

		//clears this texutre to a given colour
		void Clear(const glm::vec4 color = glm::vec4(1.0f));

		// Binds this texture to the given texture slot
		void Bind(int slot) const;

	protected:
		TTN_ITexture();
		virtual ~TTN_ITexture();

		GLuint _handle;

		static TTN_Texture_Limits _limits;
		static bool _isStaticInit;
	};
}