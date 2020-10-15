//Titan Engine, by Atlas X Games 
// VertexArrayObjects.h - source file for the class that uses colections of VBOs and IBOs in order to determine how to draw geometry

//include the VAO header
#include "Titan/VertexArrayObject.h"
//include the other required files
#include "Titan/IndexBuffer.h"
#include "Titan/VertexBuffer.h"
#include "Logging.h"

namespace Titan {
	//default constructor, makes an empty VAO
	TTN_VertexArrayObject::TTN_VertexArrayObject() :
		_ibo(nullptr), _handle(0), _vertexCount(0)
	{
		glCreateVertexArrays(1, &_handle);
	}

	//destructor, destroys the VAO
	TTN_VertexArrayObject::~TTN_VertexArrayObject()
	{
		//if the VAO exists in OpenGL
		if (_handle != 0) {
			//delete it and reset the handle to 01
			glDeleteVertexArrays(1, &_handle);
			_handle = 0;
		}
	}

	//Set the IBO for this VAO (note the IBO is stored seperately and will not destroyed with the VAO)
	void TTN_VertexArrayObject::SetIndexBuffer(const TTN_IndexBuffer::sibptr& ibo)
	{
		//copy the pointer to the ibo
		_ibo = ibo;
		//bind this VAO 
		Bind();
		//check if the index buffer we passed in acutally exists, if it does, Bind it 
		if (_ibo != nullptr) _ibo->Bind();
		//if it doesn't then unbind whatever the currently bound ibo is 
		else TTN_IndexBuffer::UnBind();
		//unbind the VAO
		UnBind();
	}

	//Adds a VBO to this VAO
	void TTN_VertexArrayObject::AddVertexBuffer(const TTN_VertexBuffer::svbptr& vbo, const std::vector<BufferAttribute>& attributes)
	{
		//check if the ammount of vertex data is currently zero
		if (_vertexCount == 0)
			//if it is then set it equal to the element count in the vbo
			_vertexCount = vbo->GetElementCount();
		//if it's not, check if the vbo's element count is equal to the vertex count, if it isn't throw an error (only vbos of the same size 
		//can exist in this VAO implementation)
		else
			LOG_ASSERT(vbo->GetElementCount() == _vertexCount, "All VBOs bound to a VAO should be of equal size in this implemenation.");

		//create an object of the struct that can store a vbo and it's attributes
		VertexBufferBinding binding;
		//copy the vbo and attributes into it 
		binding.vbo = vbo;
		binding.Attributes = attributes;
		//add it to the vector of these structs stored in the VAO object
		_vbos.push_back(binding);

		//bind the VAO
		Bind();
		//bind the VBO being added
		vbo->Bind();
		//send the data on them to openGL 
		for (const BufferAttribute& attrib : attributes) {
			glEnableVertexArrayAttrib(_handle, attrib.Slot);
			glVertexAttribPointer(attrib.Slot, attrib.Size, attrib.Type, attrib.Normalized, attrib.Stride, (void*)attrib.Offset);
		}
		//unbind the VAO
		UnBind();
	}

	//Binds the VAO for use 
	void TTN_VertexArrayObject::Bind() const
	{
		glBindVertexArray(_handle);
	}

	//Unbinds the currently bound VAO
	void TTN_VertexArrayObject::UnBind()
	{
		glBindVertexArray(0);
	}

	//calls the openGL functions to acutally draw the triangles contained within the VAO
	void TTN_VertexArrayObject::Render() const
	{
		//bind the VAO so we can use it
		Bind();
		//check if the VAO has an IBO bound to it 
		if (_ibo != nullptr)
			//if it does, then use the ibo to draw the triangles
			glDrawElements(GL_TRIANGLES, _ibo->GetElementCount(), _ibo->GetElementType(), nullptr);
		else
			//otherwise it must only have vbos, so use those vbos to draw the triangles
			glDrawArrays(GL_TRIANGLES, 0, _vertexCount);
		//unbind the VAO
		UnBind();
	}
}