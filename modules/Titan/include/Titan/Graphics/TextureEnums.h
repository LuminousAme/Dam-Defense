//Titan Engine, by Atlas X Games 
// Texture.h - header for the enums that help with texture images
#pragma once

//precompile header, this file uses glad/glad.h
#include "Titan/ttn_pch.h"

namespace Titan {
	//enums for texture details
	//enum for some common unsized internal formats
	enum Texture_Internal_Format {
		Interal_Format_Unknown = GL_NONE,
		Interal_Format_Depth = GL_DEPTH_COMPONENT,
		Interal_Format_DepthStencil = GL_DEPTH_STENCIL,
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
		Red = GL_RED,
		RG = GL_RG,
		RGB = GL_RGB,
		SRGB = GL_SRGB,
		BGR = GL_BGR,
		RGBA = GL_RGBA,
		BGRA = GL_BGRA,
		Pixel_Format_Depth = GL_DEPTH_COMPONENT,
		Pixel_Format_DepthStencil = GL_DEPTH_STENCIL
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
	enum Texture_Min_Filter {
		Min_Nearest = GL_NEAREST,
		Min_Linear = GL_LINEAR,
		NearestMipNearest = GL_NEAREST_MIPMAP_NEAREST,
		LinearMipNearest = GL_LINEAR_MIPMAP_NEAREST,
		NearestMipLinear = GL_NEAREST_MIPMAP_LINEAR, // Default
		LinearMipLinear = GL_LINEAR_MIPMAP_LINEAR
	};
	//enum for the mag filter setting for scaling
	enum Texture_Mag_Filter {
		Mag_Nearest = GL_NEAREST,
		Mag_Linear = GL_LINEAR  // Default
	};
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
		case Texture_Pixel_Format::Pixel_Format_Depth:
		case Texture_Pixel_Format::Pixel_Format_DepthStencil:
		case Texture_Pixel_Format::Red:
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
}