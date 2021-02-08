//Titan Engine, by Atlas X Games 
// Texture2D.cpp - source file for the class that represents 2d texture images

//precompile header, this file uses stb_image.h, filesystem, and memory
#include "Titan/ttn_pch.h"
//include the header
#include "Titan/Graphics/Texture2D.h"

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
	TTN_Texture2D::TTN_Texture2D()
		: TTN_ITexture()
	{
	}

	//constructor that takes in a descpiriton for the texture
	TTN_Texture2D::TTN_Texture2D(const TTN_Texture2DDesc& description)
		: TTN_ITexture(), m_data(description)
	{
		RecreateTexture();
	}

	//loads a texture in from a file
	TTN_Texture2D::st2dptr TTN_Texture2D::LoadFromFile(const std::string& fileName, bool flipped, bool forceRgba)
	{
		TTN_Texture2DData::st2ddptr data = TTN_Texture2DData::LoadFromFile(fileName, flipped, forceRgba);
		LOG_ASSERT(data != nullptr, "Failed to load image from file!");
		TTN_Texture2D::st2dptr result = TTN_Texture2D::Create();
		result->LoadData(data);
		return result;
	}

	void TTN_Texture2D::LoadData(const TTN_Texture2DData::st2ddptr& data)
	{
		if (m_data.width != data->GetWidth() ||
			m_data.height != data->GetHeight())
		{
			m_data.width = data->GetWidth();
			m_data.height = data->GetHeight();

			if (m_data.format == Texture_Internal_Format::Interal_Format_Unknown) {
				m_data.format = data->GetRecommendedFormat();
			}

			RecreateTexture();
		}

		// Align the data store to the size of a single component in
		// See https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glPixelStore.xhtml
		int componentSize = (GLint)GetTexelComponentSize(data->GetPixelType());
		glPixelStorei(GL_PACK_ALIGNMENT, componentSize);
		// Upload our data to our image
		glTextureSubImage2D(_handle, 0, 0, 0, m_data.width, m_data.height, data->GetFormat(),
			data->GetPixelType(), data->GetDataPtr());

		// We can get better error logs by attaching an object label!
		if (!data->DebugName.empty()) {
			glObjectLabel(GL_TEXTURE, _handle, data->DebugName.length(), data->DebugName.c_str());
		}

		//mipmaping
		if (m_data.GenerateMipMaps) {
			glGenerateTextureMipmap(_handle);
		}
	}

	//sets the minification filter
	void TTN_Texture2D::SetMinFilter(Texture_Min_Filter filter)
	{
		m_data.minificationFilter = filter;
		if (_handle != 0)
			glTextureParameteri(_handle, GL_TEXTURE_MIN_FILTER, (GLenum)m_data.minificationFilter);
	}

	////sets the magnification filter
	void TTN_Texture2D::SetMagFilter(Texture_Mag_Filter filter)
	{
		m_data.magnificationFilter = filter;
		if (_handle != 0)
			glTextureParameteri(_handle, GL_TEXTURE_MAG_FILTER, (GLenum)m_data.magnificationFilter);
	}

	//sets the horizontal wrap mode
	void TTN_Texture2D::SetHoriWrapMode(Texture_Wrap_Mode mode)
	{
		m_data.horiWrapMode = mode;
		if (_handle != 0)
			glTextureParameteri(_handle, GL_TEXTURE_WRAP_S, (GLenum)m_data.horiWrapMode);
	}

	//sets the verticla wrap mode
	void TTN_Texture2D::SetVertWrapMode(Texture_Wrap_Mode mode)
	{
		m_data.vertWrapMode = mode;
		if (_handle != 0)
		glTextureParameteri(_handle, GL_TEXTURE_WRAP_T, (GLenum)m_data.vertWrapMode);
	}

	//sets the Anisotropic filtering
	void TTN_Texture2D::SetAnisotropicFiltering(float level)
	{
		if (level < 0.0f) {
			level = TTN_Texture2D::GetLimits().MAX_ANISOTROPY;
		}
		m_data.MaxAnisotropic = level;
		if (_handle != 0) {
			glTextureParameterf(_handle, GL_TEXTURE_MAX_ANISOTROPY, m_data.MaxAnisotropic);
		}
	}

	//recreates the texture in openGL
	void TTN_Texture2D::RecreateTexture()
	{
		if (_handle != 0) {
			glDeleteTextures(1, &_handle);
			_handle = 0;
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &_handle);

		if (m_data.MaxAnisotropic < 0.0f) {
			m_data.MaxAnisotropic = TTN_ITexture::GetLimits().MAX_ANISOTROPY;
		}

		if (m_data.width * m_data.height > 0 && m_data.format != Texture_Internal_Format::Interal_Format_Unknown)
		{
			glTextureStorage2D(_handle, 1, m_data.format, m_data.width, m_data.height);
			glTextureParameteri(_handle, GL_TEXTURE_WRAP_S, (GLenum)m_data.horiWrapMode);
			glTextureParameteri(_handle, GL_TEXTURE_WRAP_T, (GLenum)m_data.vertWrapMode);
			glTextureParameteri(_handle, GL_TEXTURE_MIN_FILTER, (GLenum)m_data.minificationFilter);
			glTextureParameteri(_handle, GL_TEXTURE_MAG_FILTER, (GLenum)m_data.magnificationFilter);
		}
	}
}