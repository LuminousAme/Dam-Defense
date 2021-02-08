//Titan Engine, by Atlas X Games 
// VertexBuffer.h - header for the class that stores the buffer for vertices that we can render
#pragma once

//precompile header, this file uses memory
#include "Titan/ttn_pch.h"
//import the buffer base class
#include "Titan/Graphics/IBuffer.h"

namespace Titan {

	//class for the buffer that will store vertices for rendering
	class TTN_VertexBuffer : public TTN_IBuffer {
	public:
		//defines a special easier to use name for shared(smart) pointers to the class 
		typedef std::shared_ptr<TTN_VertexBuffer> svbptr;

		//creates and returns a shared(smart) pointer to the class 
		static inline svbptr Create(GLenum usage = GL_STATIC_DRAW) {
			return std::make_shared<TTN_VertexBuffer>(usage);
		}

	public:
		//constructor, creates a new vertex buffer with the given usage, data will be still need be loaded before it can be used though
		TTN_VertexBuffer(GLenum usage = GL_STATIC_DRAW) : TTN_IBuffer(GL_ARRAY_BUFFER, usage)
			{ }

		//unbinds the current vertex buffer
		static void UnBind() {
			TTN_IBuffer::UnBind(GL_ARRAY_BUFFER);
		}

	}; 

	//this class is a lot shorter than the index buffer as the exact type of data the vertices are stored in isn't that important 
	// so we can just use the LoadData from IBuffer and the overloads to ensure the types were compabatile with the index buffer 
	//took a lot of code, in reality, niether class is missing any required functionality 
}