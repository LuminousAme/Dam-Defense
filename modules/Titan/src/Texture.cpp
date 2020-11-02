//Titan Engine, by Atlas X Games 
// Texture.cpp - source file for the class that represents texture images

//include the header
#include "Titan/Texture.h"
//include other required features
#include <stb_image.h>
#include <filesystem>

namespace Titan {
	TTN_Texture2DData::TTN_Texture2DData(uint32_t width, uint32_t height, Texture_Pixel_Format format, Texture_Pixel_Data_Type type, void* sourceData, Texture_Internal_Format recommendedFormat) :
		_width(width), _height(height), _format(format), _type(type), _data(nullptr), _recommendedFormat(recommendedFormat)
	{
		LOG_ASSERT(width > 0 & height > 0, "Width and height must both be greater than zero! Got {}x{}", width, height);
		_dataSize = width * (size_t)height * GetTexelSize(_format, _type);
		_data = malloc(_dataSize);
		LOG_ASSERT(_data != nullptr, "Failed to allocate texture data!");
		if (sourceData != nullptr) {
			memcpy(_data, sourceData, _dataSize);
		}
	}

	TTN_Texture2DData::~TTN_Texture2DData()
	{
		free(_data);
	}

	TTN_Texture2DData::st2ddptr TTN_Texture2DData::LoadFromFile(const std::string& file, bool flipped, bool forceRgba)
	{
		// Variables that will store properties about our image
		int width, height, numChannels;
		const int targetChannels = forceRgba ? 4 : 0;

		// Use STBI to load the image
		stbi_set_flip_vertically_on_load(flipped);
		uint8_t* data = stbi_load(file.c_str(), &width, &height, &numChannels, targetChannels);

		// If we could not load any data, warn and return null
		if (data == nullptr) {
			LOG_WARN("STBI Failed to load image from \"{}\"", file);
			return nullptr;
		}

		// We should estimate a good format for our data

		// numChannels will store the number of channels in the image on disk, if we overrode that we should use the override value
		if (targetChannels != 0)
			numChannels = targetChannels;

		// We'll determine a recommended format for the image based on number of channels
		Texture_Internal_Format internal_format;
		Texture_Pixel_Format    image_format;
		switch (numChannels) {
		case 1:
			internal_format = Texture_Internal_Format::R8;
			image_format = Texture_Pixel_Format::Red;
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
			LOG_ASSERT(false, "Unsupported texture format for texture \"{}\" with {} channels", file, numChannels)
				break;
		}

		// This is one of those poorly documented things in OpenGL
		if ((numChannels * width) % 4 != 0) {
			LOG_WARN("The alignment of a horizontal line is not a multiple of 4, this will require a call to glPixelStorei(GL_PACK_ALIGNMENT)");
		}

		// Create the result and store our image data in it
		// Note that stbi will always give us an array of unsigned bytes (uint8_t)
		TTN_Texture2DData::st2ddptr result = std::make_shared<TTN_Texture2DData>(width, height, image_format, Texture_Pixel_Data_Type::UByte, data, internal_format);
		result->DebugName = std::filesystem::path(file).filename().string();

		// We now have a copy in our ptr, we can free STBI's copy of it
		stbi_image_free(data);

		return result;
	}

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
		TTN_Texture2DData::st2ddptr data = TTN_Texture2DData::LoadFromFile(fileName, flipped, forceRgba);
		LoadData(data);
	}

	void TTN_Texture::LoadData(const TTN_Texture2DData::st2ddptr& data)
	{
		if (m_data.format == Texture_Internal_Format::Interal_Format_Unknown) {
			m_data.format = data->GetRecommendedFormat();
		}

		if (m_data.width != data->GetWidth() ||
			m_data.height != data->GetHeight())
		{
			m_data.width = data->GetWidth();
			m_data.height = data->GetHeight();

			RecreateTexture();
		}

		// Align the data store to the size of a single component in
		// See https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glPixelStore.xhtml
		int componentSize = (GLint)GetTexelComponentSize(data->GetPixelType());
		glPixelStorei(GL_PACK_ALIGNMENT, componentSize);
		// Upload our data to our image
		glTextureSubImage2D(m_Handle, 0, 0, 0, m_data.width, m_data.height, data->GetFormat(),
			data->GetPixelType(), data->GetDataPtr());

		// We can get better error logs by attaching an object label!
		if (!data->DebugName.empty()) {
			glObjectLabel(GL_TEXTURE, m_Handle, data->DebugName.length(), data->DebugName.c_str());
		}
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

		if (m_data.width * m_data.height > 0 && m_data.format != Texture_Internal_Format::Interal_Format_Unknown)
		{
			glTextureStorage2D(m_Handle, 1, m_data.format, m_data.width, m_data.height);
			glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_S, (GLenum)m_data.horiWrapMode);
			glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_T, (GLenum)m_data.vertWrapMode);
			glTextureParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, (GLenum)m_data.minificationFilter);
			glTextureParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, (GLenum)m_data.magnificationFilter);
		}
	}
}