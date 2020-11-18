//Titan Engine, by Atlas X Games 
// Texture2D.cpp - source file for the class that represents 2d texture images

//include the header
#include "Titan/TextureCubeMap.h"
//include other required features
#include <stb_image.h>
#include <filesystem>

namespace Titan {
	TTN_TextureCubeMapData::TTN_TextureCubeMapData(uint32_t size, Texture_Pixel_Format format, Texture_Pixel_Data_Type type, void* sourceData, Texture_Internal_Format recommendedFormat)
	{
		LOG_ASSERT(size > 0, "Size must be greater than zero! Got {}", size)
			_faceDataSize = (size_t)_size * _size * GetTexelSize(_format, _type);
		_dataSize = _faceDataSize * 6;
		_data = malloc(_dataSize);
		LOG_ASSERT(_data != nullptr, "Failed to allocate texture data!")
			if (sourceData != nullptr) {
				memcpy(_data, sourceData, _dataSize);
			}
	}

	TTN_TextureCubeMapData::~TTN_TextureCubeMapData()
	{
		free(_data);
	}

	TTN_TextureCubeMapData::stcmdptr TTN_TextureCubeMapData::CreateFromImages(const std::vector<TTN_Texture2DData::st2ddptr>& images)
	{
		LOG_ASSERT(images.size() == 6, "Must pass in exactly 6 images!");
		// We'll grab our settings from the first image and assume that they're the same everywhere
		uint32_t size = images[0]->GetWidth();
		Texture_Pixel_Format format = images[0]->GetFormat();
		Texture_Pixel_Data_Type type = images[0]->GetPixelType();
		Texture_Internal_Format internal_format = images[0]->GetRecommendedFormat();
		TTN_TextureCubeMapData::stcmdptr result = std::make_shared<TTN_TextureCubeMapData>(size, format, type, nullptr,
			internal_format);
		for (int ix = 0; ix < 6; ix++) {
			result->LoadFaceData(images[ix], (CubeMapFace)ix);
		}

		return result;
	}

	TTN_TextureCubeMapData::stcmdptr TTN_TextureCubeMapData::LoadFromImages(const std::string& rootImagePath)
	{
		namespace fs = std::filesystem;
		fs::path imagePath = fs::path(rootImagePath);
		fs::path directory = imagePath.parent_path();
		fs::path rootFile = directory / imagePath.stem();
		fs::path extension = imagePath.extension();

		const std::string PATHS[6] = {
			"_pos_x",
			"_neg_x",
			"_pos_y",
			"_neg_y",
			"_pos_z",
			"_neg_z"
		};

		std::vector<TTN_Texture2DData::st2ddptr> data;
		data.resize(6);

		for (int ix = 0; ix < 6; ix++) {
			fs::path imagePath = rootFile;
			imagePath += PATHS[ix];
			imagePath += extension;
			if (fs::exists(imagePath)) {
				data[ix] = TTN_Texture2DData::LoadFromFile(imagePath.string());
			}
			else {
				LOG_WARN("Image \"{}\" could not be found!", imagePath.string());
			}
		}

		return CreateFromImages(data);
	}

	void TTN_TextureCubeMapData::LoadFaceData(const TTN_Texture2DData::st2ddptr& data, CubeMapFace face)
	{
		if (data != nullptr) {
			LOG_ASSERT(data->GetWidth() == data->GetHeight() && data->GetWidth() == _size, "Data is not square or does not match size of cubemap! {}x{} vs {}", data->GetWidth(), data->GetHeight(), _size);
			LOG_ASSERT(data->GetFormat() == _format, "Data format does not match! {} vs {}", data->GetFormat(), _format);
			LOG_ASSERT(data->GetPixelType() == _type, "Data pixel type does not match! {} vs {}", data->GetPixelType(), _type);

			size_t offset = (size_t)face * _faceDataSize;
			memcpy(static_cast<char*>(_data) + offset, data->GetDataPtr(), _faceDataSize);
		}
		else {
			LOG_WARN("Data for face {} was null, ignoring", face);
		}
	}

	//default constructor
	TTN_TextureCubeMap::TTN_TextureCubeMap()
		: TTN_ITexture()
	{
		m_data = TTN_TextureCubeMapDesc();
	}

	//constructor that takes in a description
	TTN_TextureCubeMap::TTN_TextureCubeMap(const TTN_TextureCubeMapDesc& description)
		: TTN_ITexture(), m_data(description)
	{
		RecreateTexture();
	}

	//loads from data
	void TTN_TextureCubeMap::LoadData(const TTN_TextureCubeMapData::stcmdptr& data)
	{
		if (m_data.Size != data->GetSize()) {
			m_data.Size = data->GetSize();
			if (m_data.Format == Texture_Internal_Format::Interal_Format_Unknown) {
				m_data.Format = data->GetRecommendedInternalFormat();
			}
			RecreateTexture();
		}

		// We can get better error logs by attaching an object label!
		if (!data->DebugName.empty()) {
			glObjectLabel(GL_TEXTURE, _handle, data->DebugName.length(), data->DebugName.c_str());
		}

		// Align the data store to the size of a single component in
		// See https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glPixelStore.xhtml
		int componentSize = (GLint)GetTexelComponentSize(data->GetPixelType());
		glPixelStorei(GL_PACK_ALIGNMENT, componentSize);
		// Upload our data to our image
		glTextureSubImage3D(_handle, 0, 0, 0, 0, m_data.Size, m_data.Size, 6, data->GetPixelFormat(), data->GetPixelType(), data->GetDataPtr());

		if (m_data.GenerateMipMaps) {
			glGenerateTextureMipmap(_handle);
		}
	}

	//loads from 6 images
	TTN_TextureCubeMap::stcmptr TTN_TextureCubeMap::LoadFromImages(const std::string& filePath)
	{
		TTN_TextureCubeMapData::stcmdptr data = TTN_TextureCubeMapData::LoadFromImages(filePath);
		TTN_TextureCubeMap::stcmptr result = TTN_TextureCubeMap::Create();
		result->LoadData(data);
		return result;
	}

	//sets the minification filter
	void TTN_TextureCubeMap::SetMinFilter(Texture_Min_Filter filter)
	{
		m_data.MinificationFilter = filter;
		if (_handle != 0) {
			glTextureParameteri(_handle, GL_TEXTURE_MIN_FILTER, (GLenum)m_data.MinificationFilter);
		}
	}

	//sets the magnification filter
	void TTN_TextureCubeMap::SetMagFilter(Texture_Mag_Filter filter)
	{
		m_data.MagnificationFilter = filter;
		if (_handle != 0) {
			glTextureParameteri(_handle, GL_TEXTURE_MAG_FILTER, (GLenum)m_data.MagnificationFilter);
		}
	}

	//creates the texture in opengl
	void TTN_TextureCubeMap::RecreateTexture()
	{
		if (_handle != 0) {
			glDeleteTextures(1, &_handle);
			_handle = 0;
		}

		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &_handle);
		if (m_data.Size > 0 && m_data.Format != Texture_Internal_Format::Interal_Format_Unknown)
		{
			glTextureStorage2D(_handle, 1, m_data.Format, m_data.Size, m_data.Size);
			glTextureParameteri(_handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTextureParameteri(_handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTextureParameteri(_handle, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTextureParameteri(_handle, GL_TEXTURE_MIN_FILTER, (GLenum)m_data.MinificationFilter);
			glTextureParameteri(_handle, GL_TEXTURE_MAG_FILTER, (GLenum)m_data.MagnificationFilter);
		}
	}
}