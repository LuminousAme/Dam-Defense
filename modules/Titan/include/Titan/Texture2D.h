//Titan Engine, by Atlas X Games 
// Texture2D.h - header for the class that represents 2d texture images
#pragma once

//import required features
#include <cstdint>
#include <memory>
#include <GLM/glm.hpp>
#include <string>

#include "ITexture.h"
#include "TextureEnums.h"


namespace Titan {
	//class for the data of a 2D texture
	class TTN_Texture2DData final
	{
	public:
		TTN_Texture2DData(const TTN_Texture2DData& other) = delete;
		TTN_Texture2DData(TTN_Texture2DData&& other) = delete;
		TTN_Texture2DData& operator=(const TTN_Texture2DData& other) = delete;
		TTN_Texture2DData& operator=(TTN_Texture2DData&& other) = delete;
		typedef std::shared_ptr<TTN_Texture2DData> st2ddptr;

		std::string DebugName;

		/// Creates a new 2D texture data object
		TTN_Texture2DData(uint32_t width, uint32_t height, Texture_Pixel_Format format, Texture_Pixel_Data_Type type, void* sourceData, Texture_Internal_Format recommendedFormat = Texture_Internal_Format::Interal_Format_Unknown);
		~TTN_Texture2DData();


		/// Loads image data from an external file
		static TTN_Texture2DData::st2ddptr LoadFromFile(const std::string& file, bool flipped = true, bool forceRgba = false);

		
		/// Gets the width of the texture data, in pixels
		uint32_t GetWidth() const { return _width; }

		/// Gets the height of the texture data, in pixels
		uint32_t GetHeight() const { return _height; }

		/// Gets the Pixel Format (RG, RGB, RGBA, etc) of the texture data
		Texture_Pixel_Format GetFormat() const { return _format; }

		/// Gets the underlying data type of a single component
		Texture_Pixel_Data_Type  GetPixelType() const { return _type; }

		/// Gets a recommended internal format to use when creating a texture using this data, may be
		/// InternalFormat::Unknown
		Texture_Internal_Format  GetRecommendedFormat() const { return _recommendedFormat; }

		/// Get the total size of the underlying data (size of individual pixel * width * height)
		size_t  GetDataSize() const { return _dataSize; }

		
		/// Gets a readonly copy of the underlying data in this image for upload
		const void* GetDataPtr() const { return _data; }

	private:
		uint32_t    _width, _height;
		size_t      _dataSize;
		Texture_Pixel_Format _format;
		Texture_Pixel_Data_Type   _type;
		Texture_Internal_Format _recommendedFormat;
		void* _data;
	};

	//struct with data for a texture
	struct TTN_Texture2DDesc {
		//constructor, sets default values for everything
		TTN_Texture2DDesc() : width(0), height(0),
			format(Texture_Internal_Format::Interal_Format_Unknown),
			horiWrapMode(Texture_Wrap_Mode::Repeat),
			vertWrapMode(Texture_Wrap_Mode::Repeat),
			minificationFilter(Texture_Min_Filter::NearestMipLinear),
			magnificationFilter(Texture_Mag_Filter::Mag_Linear),
			data(nullptr), 
			MaxAnisotropic(-1.0f),
			GenerateMipMaps(true) {};

		uint32_t width, height;
		Texture_Internal_Format format;
		Texture_Wrap_Mode horiWrapMode;
		Texture_Wrap_Mode vertWrapMode;
		Texture_Min_Filter minificationFilter;
		Texture_Mag_Filter magnificationFilter;
		uint8_t* data;
		float MaxAnisotropic;
		bool GenerateMipMaps;
	};

	//class for the 2D texture
	class TTN_Texture2D final : public TTN_ITexture {
	public:
		//defines a special easier to use name for shared(smart) pointers to the class 
		typedef std::shared_ptr<TTN_Texture2D> st2dptr;

		//creates and returns a shared(smart) pointer to the class 
		static inline st2dptr Create() {
			return std::make_shared<TTN_Texture2D>();
		}

	public:
		//ensuring moving and copying is not allowed so we can control destructor calls through pointers
		TTN_Texture2D(const TTN_Texture2D& other) = delete;
		TTN_Texture2D(TTN_Texture2D& other) = delete;
		TTN_Texture2D& operator=(const TTN_Texture2D& other) = delete;
		TTN_Texture2D& operator=(TTN_Texture2D&& other) = delete;

	public:
		//default constructor
		TTN_Texture2D();
		//constructor that takes in a descpiriton for the texture
		TTN_Texture2D(const TTN_Texture2DDesc& description);
		//default destrcutor
		~TTN_Texture2D() = default;

		//loads a texture from a file
		static st2dptr LoadFromFile(const std::string& fileName, bool flipped = true, bool forceRgba = false);
		//loads a texture from a texture data object
		void LoadData(const TTN_Texture2DData::st2ddptr& data);

		//Getters for details about the texture
		//width
		uint32_t GetWidth() const { return m_data.width; }
		//height
		uint32_t GetHeight() const { return m_data.height; }
		//internal format
		uint32_t GetInternalFormat() const { return m_data.format; }
		//minification filter
		Texture_Min_Filter GetMinFilter() const { return m_data.minificationFilter; }
		//magnification filter
		Texture_Mag_Filter GetMagFilter() const { return m_data.magnificationFilter; }
		//horizontal wrap mode
		Texture_Wrap_Mode GetHoriWrapMode() const { return m_data.horiWrapMode; }
		//vertical wrap mode
		Texture_Wrap_Mode GetVertWrapMode() const { return m_data.vertWrapMode; }
		//underlying data
		const TTN_Texture2DDesc& GetDescription() const { return m_data; }


		//setters for the filters and wrap mode
		//minification filter
		void SetMinFilter(Texture_Min_Filter filter);
		//magnification filter
		void SetMagFilter(Texture_Mag_Filter filter);
		//horizontal wrap mode
		void SetHoriWrapMode(Texture_Wrap_Mode mode);
		//vertical wrap mode
		void SetVertWrapMode(Texture_Wrap_Mode mode);
		//Anisotropic filtering
		void SetAnisotropicFiltering(float level = -1.0f);

	private:
		TTN_Texture2DDesc m_data;

		void RecreateTexture();
	};
}