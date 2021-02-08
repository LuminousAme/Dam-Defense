//Titan Engine, by Atlas X Games 
// ITexture.cpp - source file for the base class that different types of textures inherit from

//precompile header, this file uses Logging.h
#include "Titan/ttn_pch.h"
//include the header
#include "Titan/Graphics/ITexture.h"

namespace Titan {
	//init the static variable representing the limits of the current gpu
	TTN_ITexture::TTN_Texture_Limits TTN_ITexture::_limits = TTN_ITexture::TTN_Texture_Limits();
	bool TTN_ITexture::_isStaticInit = false;

	//constructor, sets a default handle, and if the limits haven't been read in from openGL yet, read them in
	TTN_ITexture::TTN_ITexture()
		: _handle(0) {
		if (!_isStaticInit) {
			// Example of reading limits from the OpenGL renderer
			glGetIntegerv(GL_MAX_TEXTURE_SIZE, &_limits.MAX_TEXTURE_SIZE);
			glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &_limits.MAX_TEXTURE_UNITS);
			glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &_limits.MAX_3D_TEXTURE_SIZE);
			glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &_limits.MAX_TEXTURE_IMAGE_UNITS);
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &_limits.MAX_ANISOTROPY);

			glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

			LOG_INFO("==== Texture Limits =====");
			LOG_INFO("\tSize:       {}", _limits.MAX_TEXTURE_SIZE);
			LOG_INFO("\tUnits:      {}", _limits.MAX_TEXTURE_UNITS);
			LOG_INFO("\t3D Size:    {}", _limits.MAX_3D_TEXTURE_SIZE);
			LOG_INFO("\tUnits (FS): {}", _limits.MAX_TEXTURE_IMAGE_UNITS);
			LOG_INFO("\tMax Aniso.: {}", _limits.MAX_ANISOTROPY);

			_isStaticInit = true;
		}
	}

	//destructor, deletes the texture
	TTN_ITexture::~TTN_ITexture() {
		if (glIsTexture(_handle)) {
			glDeleteTextures(1, &_handle);
		}
	}

	//Binds the texture to the given slot
	void TTN_ITexture::Bind(int slot) const {
		if (_handle != 0) {
			glBindTextureUnit(slot, _handle);
		}
	}

	//Unbinds whatever texture is in the given slot
	void TTN_ITexture::Unbind(int slot) {
		glBindTextureUnit(slot, 0);
	}

	//clears the texture to a colour
	void TTN_ITexture::Clear(glm::vec4 color) {
		if (_handle != 0) {
			glClearTexImage(_handle, 0, GL_RGBA, GL_FLOAT, &color[0]);
		}
	}
}