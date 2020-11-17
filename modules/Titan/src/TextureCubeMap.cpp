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

	void TTN_TextureCubeMapData::LoadFaceData(const TTN_Texture2DData::st2ddptr data, CubeMapFace face)
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
}