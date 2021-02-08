//Titan Engine, by Atlas X Games 
// IndexBuffer.h - header for the class that stores the buffer for indices that we can render
#pragma once

//import the buffer base class, which also includes the precompile header, which has the other features we need here
//including cstdint, stdexcept, and memory
#include "Titan/Graphics/IBuffer.h"

namespace Titan {

	//class for the buffer that will store indices for rendering
	class TTN_IndexBuffer : public TTN_IBuffer {
	public:
		//defines a special easier to use name for shared(smart) pointers to the class 
		typedef std::shared_ptr<TTN_IndexBuffer> sibptr;

		//creates and returns a shared(smart) pointer to the class 
		static inline sibptr Create(GLenum usage = GL_STATIC_DRAW) {
			return std::make_shared<TTN_IndexBuffer>(usage);
		}

	public:
		//constructor, creates a new index buffer with given usage
		//data will have to be imported before it can be used however
		TTN_IndexBuffer(GLenum usage = GL_STATIC_DRAW) : 
			TTN_IBuffer(GL_ELEMENT_ARRAY_BUFFER, usage), _elementType(GL_NONE) 
		{ }

		//foricbly override the loaddata from the base class to throw an error so the user can only properly use one of the overloads 
		//specfic to Index Buffers 
		inline void LoadData(const void* data, size_t elementSize, size_t elementCount) override {
			throw std::runtime_error("For index buffers you must use templated overload, or the LoadData that specifies the element type");
		}

		//loads data into the index buffer, specifying the type of indices with the elementType paramter 
		inline void LoadData(const void* data, size_t elementSize, size_t elementCount, GLenum elementType) {
			//load the data into the base class
			TTN_IBuffer::LoadData(data, elementSize, elementCount);
			//save the element type
			_elementType = elementType;
		}

		//loads data of a known type into the index buffer
		//here it only throws an error but the template definitions below will override this one when those types are used
		//as index buffers can only exist with those types
		template <typename T>
		void LoadData(const T* data, size_t count) {
			throw std::runtime_error("Index buffer types must be one of uint8_t, uint16_5, or uint32_t.");
		}

		//Gets the underlying index type stored by this buffer
		GLenum GetElementType() const { return _elementType; }

		//Unbinds the currently bound index buffer
		static void UnBind() { TTN_IBuffer::UnBind(GL_ELEMENT_ARRAY_BUFFER); }

	protected:
		//stores the type of element being saved in the index buffer 
		GLenum _elementType;
	};

	//Definitions for the templates that allow special behavior for the various known types of LoadData

	//for uint8, for unsigned bytes
	template<>
	inline void TTN_IndexBuffer::LoadData<uint8_t>(const uint8_t* data, size_t count) {
		//load the data into the base class
		TTN_IBuffer::LoadData<uint8_t>(data, count);
		//and save the type
		_elementType = GL_UNSIGNED_BYTE;
	}

	//for uint16, for unsigned shorts
	template<>
	inline void TTN_IndexBuffer::LoadData<uint16_t>(const uint16_t* data, size_t count) {
		//load the data into the base class
		TTN_IBuffer::LoadData<uint16_t>(data, count);
		//and save the type
		_elementType = GL_UNSIGNED_SHORT;
	}

	//for uint32, for unsigned ints
	template<>
	inline void TTN_IndexBuffer::LoadData<uint32_t>(const uint32_t* data, size_t count) {
		//load the data into the base class
		TTN_IBuffer::LoadData<uint32_t>(data, count);
		//and save the type
		_elementType = GL_UNSIGNED_INT;
	}
}