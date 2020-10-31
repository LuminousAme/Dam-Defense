//Titan Engine, by Atlas X Games 
// Texture.cpp - source file for the class that represents texture images

//include the header
#include "Titan/Texture.h"
//include other required features
#include <stb_image.h>

namespace Titan {
	//default constructor
	TTN_Texture::TTN_Texture()
		: m_Handle(0)
	{
		m_data = TTN_Texture_Data();
	}

	//default desctructor
	TTN_Texture::~TTN_Texture()
	{
		//delete the texture from openGL's state machine
		if (glIsTexture(m_Handle)) {
			glDeleteTextures(1, &m_Handle);
		}
	}

	//loads a texture in from a file
	void TTN_Texture::LoadFromFile(const std::string& fileName, bool flipped, bool forceRgba)
	{
		//variables for properties of the image
		int width, height, numChannels;
		const int targetChannels = forceRgba ? 4 : 0;

		//use stbi to load the image
		stbi_set_flip_vertically_on_load(flipped);
		uint8_t* data = stbi_load(fileName.c_str(), &width, &height, &numChannels, targetChannels);

		// If we could not load any data, return
		if (data == nullptr) {
			printf("Error STBI could not load image from file: %s\n", fileName.c_str());
			return;
		}

		// numChannels will store the number of channels in the image on disk, if we overrode that we should use the override value
		if (targetChannels != 0)
			numChannels = targetChannels;

		//determine recommended format based on number of channels
		Texture_Internal_Format internal_format;
		Texture_Pixel_Format    image_format;
		switch (numChannels) {
		case 1:
			internal_format = Texture_Internal_Format::R8;
			image_format = Texture_Pixel_Format::R;
			break;
		case 2:
			internal_format = Texture_Internal_Format::RG8;
			image_format = Texture_Pixel_Format::RG;
			break;
		case 3:
			internal_format = Texture_Internal_Format::RGB8;
			image_format = Texture_Pixel_Format::RGB;
			break;
		case 4:
			internal_format = Texture_Internal_Format::RGBA8;
			image_format = Texture_Pixel_Format::RGBA;
			break;
		default:
			printf("Unsupported texture format for texture %s with %d channels\n", fileName.c_str(), numChannels);
			break;
		}

		//poorly documented opengl thing
		if ((numChannels * width) % 4 != 0) {
			printf("Warning: the alignment of a horizontal line is not a multiple of 4, this will require a call to glPixelStorei(GL_PACK_ALIGNMENT)\n");
		}

		//copy the data over into the m_data variable
		m_data.width = width;
		m_data.height = height;
		m_data.format = internal_format;
		m_data.data = data;

		//we can free up stbi's copy of the data now as we're storing it in m_data.data 
		stbi_image_free(data);

		//recreate the texture data in openGL
		RecreateTexture();

		//Align the data to store the size of a compontent
		int componentSize = (GLint)GetTexelComponentSize(Texture_Pixel_Data_Type::UByte);
		glPixelStorei(GL_PACK_ALIGNMENT, componentSize);
		//update data to our openGL image object
		glTextureSubImage2D(m_Handle, 0, 0, 0, m_data.width, m_data.height, m_data.format, image_format, m_data.data);

		//attach the filename as an object label
		if (!fileName.empty())
			glObjectLabel(GL_TEXTURE, m_Handle, fileName.length(), fileName.c_str());
	}

	//sets the color of an entire texture
	void TTN_Texture::SetClearColor(const glm::vec4 color)
	{
		if (m_Handle != 0) {
			glClearTexImage(m_Handle, 0, GL_RGBA, GL_FLOAT, &color[0]);
		}
	}

	//binds the texture to the given openGL slot
	void TTN_Texture::Bind(int slot)
	{
		if (m_Handle != 0) {
			glActiveTexture(GL_TEXTURE0 + slot);
			glBindTexture(GL_TEXTURE_2D, m_Handle);
		}
	}

	//unbinds whatever textures is at the given openGL slot
	void TTN_Texture::UnBind(int slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	//sets the minification filter
	void TTN_Texture::SetMinFilter(Texture_Min_Filter filter)
	{
		m_data.minificationFilter = filter;
		if (m_Handle != 0)
			glTextureParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, (GLenum)m_data.minificationFilter);
	}


	////sets the magnification filter
	void TTN_Texture::SetMagFilter(Texture_Mag_Filter filter)
	{
		m_data.magnificationFilter = filter;
		if (m_Handle != 0)
			glTextureParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, (GLenum)m_data.magnificationFilter);
	}

	//sets the horizontal wrap mode
	void TTN_Texture::SetHoriWrapMode(Texture_Wrap_Mode mode)
	{
		m_data.horiWrapMode = mode;
		if (m_Handle != 0)
			glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_S, (GLenum)m_data.horiWrapMode);
	}

	void TTN_Texture::SetVertWrapMode(Texture_Wrap_Mode mode)
	{
		m_data.vertWrapMode = mode;
		if (m_Handle != 0)
		glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_T, (GLenum)m_data.vertWrapMode);
	}

	//recreates the texture in openGL
	void TTN_Texture::RecreateTexture()
	{
		if (m_Handle != 0) {
			glDeleteTextures(1, &m_Handle);
			m_Handle = 0;
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &m_Handle);

		if (m_data.width * m_data.height > 0 && m_data.format != Texture_Internal_Format::Unknown)
		{
			glTextureStorage2D(m_Handle, 1, m_data.format, m_data.width, m_data.height);
			glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_S, (GLenum)m_data.horiWrapMode);
			glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_T, (GLenum)m_data.vertWrapMode);
			glTextureParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, (GLenum)m_data.minificationFilter);
			glTextureParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, (GLenum)m_data.magnificationFilter);
		}
	}
}