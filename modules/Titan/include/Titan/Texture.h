//Titan Engine, by Atlas X Games 
// Texture.h - header for the class that represents texture images
#pragma once

//import required features
#include <cstdint>
#include <memory>
#include "glad/glad.h"
#include <GLM/glm.hpp>
#include <string>

namespace Titan {
	//enums for texture details
	//enum for some common unsized internal formats
	enum Texture_Internal_Format {
		Unknown = GL_NONE,
		Depth = GL_DEPTH_COMPONENT,
		DepthStencil = GL_DEPTH_STENCIL,
		R8 = GL_R8,
		R16 = GL_R16,
		RG8 = GL_RG8,
		RGB8 = GL_RGB8,
		RGB10 = GL_RGB10,
		RGB16 = GL_RGB16,
		RGBA8 = GL_RGBA8,
		RGBA16 = GL_RGBA16
	};
	//enum for some common pixel format data
	enum Texture_Pixel_Format {
		R = GL_RED,
		RG = GL_RG,
		RGB = GL_RGB,
		SRGB = GL_SRGB,
		BGR = GL_BGR,
		RGBA = GL_RGBA,
		BGRA = GL_BGRA,
		Depth = GL_DEPTH_COMPONENT,
		DepthStencil = GL_DEPTH_STENCIL
	};
	//enum for the type of each component of a pixel's data (e.g. the type of the b colour value)
	enum Texture_Pixel_Data_Type {
		UByte = GL_UNSIGNED_BYTE,
		Byte = GL_BYTE,
		UShort = GL_UNSIGNED_SHORT,
		Short = GL_SHORT,
		UInt = GL_UNSIGNED_INT,
		Int = GL_INT,
		Float = GL_FLOAT
	};
	//enum for the wrap mode for how it deals with the textures not fitting an object perfectly
	enum Texture_Wrap_Mode {
		ClampToEdge = GL_CLAMP_TO_EDGE,
		ClampToBorder = GL_CLAMP_TO_BORDER,
		MirroredRepeat = GL_MIRRORED_REPEAT,
		Repeat = GL_REPEAT, // Default
		MirrorClampToEdge = GL_MIRROR_CLAMP_TO_EDGE
	};
	//enum for the min filter setting for scaling
	enum Texture_Min_Filter{
		Nearest = GL_NEAREST,
		Linear = GL_LINEAR,
		NearestMipNearest = GL_NEAREST_MIPMAP_NEAREST,
		LinearMipNearest = GL_LINEAR_MIPMAP_NEAREST,
		NearestMipLinear = GL_NEAREST_MIPMAP_LINEAR, // Default
		LinearMipLinear = GL_LINEAR_MIPMAP_LINEAR
	};
	//enum for the mag filter setting for scaling
	enum Texture_Mag_Filter {
		Nearest = GL_NEAREST,
		Linear = GL_LINEAR  // Default
	};

	//class for the texture and all it's data
	class TTN_Texture {
	public:
		//defines a special easier to use name for shared(smart) pointers to the class 
		typedef std::shared_ptr<TTN_Texture> stptr;

		//creates and returns a shared(smart) pointer to the class 
		static inline stptr Create() {
			return std::make_shared<TTN_Texture>();
		}

	public:
		//ensuring moving and copying is not allowed so we can control destructor calls through pointers
		TTN_Texture(const TTN_Texture& other) = delete;
		TTN_Texture(TTN_Texture& other) = delete;
		TTN_Texture& operator=(const TTN_Texture& other) = delete;
		TTN_Texture& operator=(TTN_Texture&& other) = delete;

	private:
		//struct with data for a texture
		struct TTN_Texture_Data {
			//constructor, sets default values for everything
			TTN_Texture_Data() : width(0), height(0),
				format(Texture_Internal_Format::Unknown),
				horiWrapMode(Texture_Wrap_Mode::Repeat),
				vertWrapMode(Texture_Wrap_Mode::Repeat),
				minificationFilter(Texture_Min_Filter::NearestMipLinear),
				magnificationFilter(Texture_Mag_Filter::Linear), 
				data(nullptr)	{};

			uint32_t width, height;
			Texture_Internal_Format format;
			Texture_Wrap_Mode horiWrapMode;
			Texture_Wrap_Mode vertWrapMode;
			Texture_Min_Filter minificationFilter;
			Texture_Mag_Filter magnificationFilter;
			uint8_t* data; 
		};

	public:
		//default constructor
		TTN_Texture();
		//default destrcutor
		~TTN_Texture();

		//loads a texture from a file
		void LoadFromFile(const std::string& fileName, bool flipped = true, bool forceRgba = false);

		//set the clear colour of the texture
		void SetClearColor(const glm::vec4 color);

		//Binds the texture to a given texture slot
		void Bind(int slot);
		//Unbinds whatever texture is in a given slot
		static void UnBind(int slot);

		//Gets the underlying OpenGL handle for the texture
		GLuint GetHandle() const { return m_Handle; };

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

		//setters for the filters and wrap mode
		//minification filter
		void SetMinFilter(Texture_Min_Filter filter);
		//magnification filter
		void SetMagFilter(Texture_Mag_Filter filter);
		//horizontal wrap mode
		void SetHoriWrapMode(Texture_Wrap_Mode mode);
		//vertical wrap mode
		void SetVertWrapMode(Texture_Wrap_Mode mode);

	private:
		GLuint m_Handle;
		TTN_Texture_Data m_data;

		static int MAX_TEXTURE_SIZE;

		void RecreateTexture();


		//Gets the size of a single pixel component in the given format, in bytes.
		constexpr size_t GetTexelComponentSize(Texture_Pixel_Data_Type type)
		{
			switch (type) {
			case Texture_Pixel_Data_Type::UByte:
			case Texture_Pixel_Data_Type::Byte:
				return 1;
			case Texture_Pixel_Data_Type::UShort:
			case Texture_Pixel_Data_Type::Short:
				return 2;
			case Texture_Pixel_Data_Type::Int:
			case Texture_Pixel_Data_Type::UInt:
				return 4;
			default:
				printf("Unknown pixel component type\n");
				return 0;
			}
		}

		//Gets the number of components in a given pixel format
		constexpr GLint GetTexelComponentCount(Texture_Pixel_Format format)
		{
			switch (format)
			{
			case Texture_Pixel_Format::Depth:
			case Texture_Pixel_Format::DepthStencil:
			case Texture_Pixel_Format::R:
				return 1;
			case Texture_Pixel_Format::RG:
				return 2;
			case Texture_Pixel_Format::RGB:
			case Texture_Pixel_Format::BGR:
				return 3;
			case Texture_Pixel_Format::RGBA:
			case Texture_Pixel_Format::BGRA:
				return 4;
			default:
				printf("Unknown pixel component format\n");
				return 0;
			}
		}

		//Gets the number of bytes needed to represent a single texel of the given format and type
		constexpr size_t GetTexelSize(Texture_Pixel_Format format, Texture_Pixel_Data_Type type) {
			return GetTexelComponentSize(type) * GetTexelComponentCount(format);
		}
	};
}