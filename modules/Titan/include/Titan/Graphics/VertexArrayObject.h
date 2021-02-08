//Titan Engine, by Atlas X Games 
// VertexArrayObjects.h - header for the class that uses colections of VBOs and IBOs in order to determine how to draw geometry, 
//and other supporting enums and strcutures
#pragma once

//precompile header, this file uses glad/glad.h, cstdint, vector, and memory
#include "Titan/ttn_pch.h"

//include the VBO and IBO classes
#include "Titan/Graphics/VertexBuffer.h"	
#include "Titan/Graphics/IndexBuffer.h"

namespace Titan {
	//enum class to make it clear what the intended usage of any given attribute is to our users
	enum class AttribUsage {
		Unknown = 0,
		Position, 
		Color,
		Color1, //Extra
		Color2, //Extra
		Color3, //Extra
		Texture, 
		Texture2, //Extra
		Texture3, //Extra
		Normal,
		Tagent,
		BiNormal,
		User0, //Extra
		User1, //Extra
		User2, //Extra
		User3 //Extra
	};

	//structure for the parameters passed to glVertexAttribPointer commands
	struct BufferAttribute {
		//the input slot for the vertex shader that will recieve the data (ex location = 1 in GLSL)
		GLuint Slot;
		//the number of elements to be passed
		GLint Size;
		//the type of data to be passed
		GLenum Type;
		//Wheter or not the values should be normalized
		bool Normalized;
		//the total size of an element in this buffer
		GLsizei Stride;
		//the offset from the staret of an element to this attribute 
		size_t Offset;
		//the usage for this attirbute, not sent to OpenGL
		AttribUsage Usage;
		//divisor for instanced renders
		size_t attribDivisor;

		//constructor for the struct 
		BufferAttribute(uint32_t slot, uint32_t size, GLenum type, bool normalized, GLsizei stride, size_t offset, AttribUsage usage = AttribUsage::Unknown, size_t divisor = 0) :
			Slot(slot), Size(size), Type(type), Normalized(normalized), Stride(stride), Offset(offset), Usage(usage), attribDivisor(divisor)
			{ }
	};

	//Class for a VAO, wraps around the VAOs in OpenGL and basically represents the data for the basic shapes that make up the faces of meshes
	class TTN_VertexArrayObject final {
	public:
		//defines a special easier to use name for shared(smart) pointers to the class 
		typedef std::shared_ptr<TTN_VertexArrayObject> svaptr;

		//creates and returns a shared(smart) pointer to the class 
		static inline svaptr Create() {
			return std::make_shared<TTN_VertexArrayObject>();
		}

	public:
		//ensuring moving and copying is not allowed so we can control destructor calls through pointers
		TTN_VertexArrayObject(const TTN_VertexArrayObject& other) = delete;
		TTN_VertexArrayObject(TTN_VertexArrayObject& other) = delete;
		TTN_VertexArrayObject& operator=(const TTN_VertexArrayObject& other) = delete;
		TTN_VertexArrayObject& operator=(TTN_VertexArrayObject&& other) = delete;

	public:
		//default constructor, creates an empty VAO
		TTN_VertexArrayObject();
		//Destructor, deletes the VAO
		~TTN_VertexArrayObject();

		//Set the IBO for this VAO, note that the IBO is stored seperately and is not currently deleted when the VAO is 
		void SetIndexBuffer(const TTN_IndexBuffer::sibptr& ibo);
		//Adds a VBO to this VAO, with the attributes specified
		void AddVertexBuffer(const TTN_VertexBuffer::svbptr& vbo, const std::vector<BufferAttribute>& attributes);
		//Clears all the vertex buffers
		void ClearVertexBuffers();

		//Bind this VAO so that it is the source of data for draw operations
		void Bind() const;

		//Unbinds the VAO that is currently bound
		static void UnBind();

		//Gets the OpenGL handle this is wrapping around
		GLuint GetHandle() const { return 0; }

		//Renders the VAO
		void Render() const;
		void RenderInstanced(size_t numOfObjects, size_t numOfVerts = 0) const;

	private:
		//structure to store a VBO and it's attributes
		struct VertexBufferBinding
		{
			TTN_VertexBuffer::svbptr vbo;
			std::vector<BufferAttribute> Attributes;
		};

		//The index buffer bound to this VAO
		TTN_IndexBuffer::sibptr _ibo;
		//the vertex buffers bound to this VAO
		std::vector<VertexBufferBinding> _vbos;

		//the number of vertices
		GLsizei _vertexCount;

		//the openGL handle that the class is wrapping around
		GLuint _handle;
	};

}