//Titan Engine, by Atlas X Games 
// IBuffer.h - header for the abstract base class for all the OpenGL buffer types
#pragma once

//precompile header, this file uses glad/glad.h
#include "Titan/ttn_pch.h"

namespace Titan {

	//IBuffer class, abstract base class for the different openGL buffer types
	class TTN_IBuffer {
	public:
		//ensuring moving and copying is not allowed so we can manually control destructor calls
		TTN_IBuffer(const TTN_IBuffer& other) = delete;
		TTN_IBuffer(TTN_IBuffer& other) = delete;
		TTN_IBuffer& operator=(const TTN_IBuffer& other) = delete;
		TTN_IBuffer& operator=(TTN_IBuffer&& other) = delete;

	public:
		//virtual destructor, to allow child buffers to overload it with their own destrutors
		virtual ~TTN_IBuffer();

		//loads the data into the buffer, using the data, the size of an element of the data in bytes, and the number of elements
		virtual void LoadData(const void* data, size_t elementSize, size_t elementCount);

		//loads an array of data into this buffer (using glNamedBufferData) using the type of data, a point to the first element in the array
		//and the number of elements
		template <typename T>
		void LoadData(const T* data, size_t count) {
			TTN_IBuffer::LoadData((const void*)(data), sizeof(T), count);
		}

		//GETTERS
		//get the number of elements in this buffer
		GLsizei GetElementCount() const { return static_cast<GLsizei>(_elementCount); }
		//gets the size in bytes of a single element 
		size_t GetElementSize() const { return _elementSize; }
		//gets the total size in bytes that the buffer takes up 
		size_t GetTotalSize() const { return _elementCount * _elementSize; }
		//gets the type of the buffer 
		GLenum GetType() const { return _type; }
		//gets the usage hint for the buffer (STATIC_DRAW, DYNMAIC_DRAW, etc.)
		GLenum GetUsage() const { return _usage; }
		//gets the openGL handle that the class is wrapping around 
		GLuint GetHandle() const { return _handle; }

		//binds the buffer for use (to the slot returned by GetType()) 
		virtual void Bind();
		//unbinds the buffer bound to the slot given by type
		static void UnBind(GLenum type);

	protected:
		//constructor to create a new buffer with the given type and usage
		//protected so only child classes can use it, as this class should not be used on it's own
		TTN_IBuffer(GLenum type, GLenum usage);

		size_t _elementSize; // The size or stride of our elements
		size_t _elementCount; // The number of elements in the buffer
		GLuint _handle; // The OpenGL handle for the underlying buffer
		GLenum _usage; // The buffer usage mode (GL_STATIC_DRAW, GL_DYNAMIC_DRAW)
		GLenum _type; // The buffer type (ex GL_ARRAY_BUFFER, GL_ARRAY_ELEMENT_BUFFER)
	};
}