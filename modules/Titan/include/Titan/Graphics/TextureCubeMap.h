//Titan Engine, by Atlas X Games 
// Texture2D.h - header for the class that represents cube map texture images
#pragma once

//precompile header, this file uses cstdint, and string
#include "Titan/ttn_pch.h"
//include other titan features
#include "Titan/Graphics/TextureEnums.h"
#include "Titan/Graphics/Texture2D.h"

namespace Titan {
	//enum for the face of each cube
	enum CubeMapFace {
		PosX = 0,
		NegX = 1,
		PosY = 2,
		NegY = 3,
		PosZ = 4,
		NegZ = 5
	};

	//class for the data of a cube map texture
	class TTN_TextureCubeMapData final {
	public:
		TTN_TextureCubeMapData(const TTN_TextureCubeMapData& other) = delete;
		TTN_TextureCubeMapData(TTN_TextureCubeMapData&& other) = delete;
		TTN_TextureCubeMapData& operator=(const TTN_TextureCubeMapData& other) = delete;
		TTN_TextureCubeMapData& operator=(TTN_TextureCubeMapData&& other) = delete;
		typedef std::shared_ptr<TTN_TextureCubeMapData> stcmdptr;

		std::string DebugName;

		/// Creates a new cube texture data object
		TTN_TextureCubeMapData(uint32_t size, Texture_Pixel_Format format, Texture_Pixel_Data_Type type, void* sourceData, 
			Texture_Internal_Format recommendedFormat = Texture_Internal_Format::Interal_Format_Unknown);
		~TTN_TextureCubeMapData();

		//loads a cubemap from a set of 6 images
		static TTN_TextureCubeMapData::stcmdptr CreateFromImages(const std::vector<TTN_Texture2DData::st2ddptr>& images);

		//loads a cubemap for a set of 6 images stored in different files, the files should follow the naming convention image_(dir, pos/neg)_(axis, x/y/z).png
		static TTN_TextureCubeMapData::stcmdptr LoadFromImages(const std::string& rootImagePath);

		//loads an indivual face
		void LoadFaceData(const TTN_Texture2DData::st2ddptr& data, CubeMapFace face);

		//gets the size of the texture (width/height of each indivual face)
		uint32_t GetSize() const { return _size; }
		//gets the pixel format 
		Texture_Pixel_Format GetPixelFormat() const { return _format; }
		//gets the pixel type
		Texture_Pixel_Data_Type GetPixelType() const { return _type; }
		//gets a recommneded format
		Texture_Internal_Format GetRecommendedInternalFormat() const { return _recommendedFormat; }
		//get total size of underlying data
		size_t  GetDataSize() const { return _dataSize; }
		//get the size of a single face worth's of data, in bytes
		size_t GetFaceDataSize() const { return _faceDataSize; }
		//gets a read only copy of the underlying data
		const void* GetDataPtr() const { return _data; }
		//gets a read only copy of the underlying data for one face
		const void* GetFaceDataPtr(CubeMapFace face) const { return static_cast<char*>(_data) + (_faceDataSize * (size_t)face); }

	private:
		uint32_t    _size;
		size_t      _dataSize;
		size_t      _faceDataSize;
		Texture_Pixel_Format _format;
		Texture_Pixel_Data_Type   _type;
		Texture_Internal_Format _recommendedFormat;
		void* _data;
	};

	//struct with data for a texture
	struct TTN_TextureCubeMapDesc {
		uint32_t       Size;
		Texture_Internal_Format Format;
		Texture_Min_Filter      MinificationFilter;
		Texture_Mag_Filter      MagnificationFilter;
		bool           GenerateMipMaps;

		TTN_TextureCubeMapDesc() :
			Size(0),
			Format(Texture_Internal_Format::Interal_Format_Unknown),
			MinificationFilter(Texture_Min_Filter::Min_Linear),
			MagnificationFilter(Texture_Mag_Filter::Mag_Linear),
			GenerateMipMaps(false)
		{ }
	};

	//class for the actual cubemap texture
	class TTN_TextureCubeMap final : public TTN_ITexture {
	public:
		//defines a special easier to use name for shared(smart) pointers to the class 
		typedef std::shared_ptr<TTN_TextureCubeMap> stcmptr;

		//creates and returns a shared(smart) pointer to the class 
		static inline stcmptr Create() {
			return std::make_shared<TTN_TextureCubeMap>();
		}

	public:
		//ensuring moving and copying is not allowed so we can control destructor calls through pointers
		TTN_TextureCubeMap(const TTN_TextureCubeMap& other) = delete;
		TTN_TextureCubeMap(TTN_TextureCubeMap& other) = delete;
		TTN_TextureCubeMap& operator=(const TTN_TextureCubeMap& other) = delete;
		TTN_TextureCubeMap& operator=(TTN_TextureCubeMap&& other) = delete;

	public:
		//default constructor
		TTN_TextureCubeMap();
		//constructor that takes a description
		TTN_TextureCubeMap(const TTN_TextureCubeMapDesc& description);
		//default destructor, ITexture handles destroying data in opengl so we can just use the autogeneterated one
		~TTN_TextureCubeMap() = default;

		//loads data into the texture
		void LoadData(const TTN_TextureCubeMapData::stcmdptr& data);

		//loads from a series of 6 images
		static stcmptr LoadFromImages(const std::string& filePath);

		//Getters
		uint32_t GetSize() const { return m_data.Size; }
		Texture_Internal_Format GetFormat() { return m_data.Format; }
		Texture_Min_Filter GetMinFilter() { return m_data.MinificationFilter; }
		Texture_Mag_Filter GetMagFilter() { return m_data.MagnificationFilter; }
		const TTN_TextureCubeMapDesc& GetDescription() const { return m_data; }

		//Setters
		void SetMinFilter(Texture_Min_Filter filter);
		void SetMagFilter(Texture_Mag_Filter filter);

	private:
		TTN_TextureCubeMapDesc m_data;

		void RecreateTexture();
	};
}