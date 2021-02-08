//Titan Engine, by Atlas X Games 
// VertexArrayObjects.cpp - source file for the class that uses colections of VBOs and IBOs in order to determine how to draw geometry

//precompile header, this file uses Logging.h
#include "Titan/ttn_pch.h"
//include the VAO header
#include "Titan/Graphics/VertexArrayObject.h"

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
		const uint32_t stride = attributes[0].Stride;
		const uint32_t instanceDivisor = attributes[0].attribDivisor;

		for (int ix = 1; ix < attributes.size(); ix++) {
			LOG_ASSERT(stride == attributes[ix].Stride, "Stride mismatch");
			LOG_ASSERT(instanceDivisor == attributes[ix].attribDivisor, "InstanceDivisor mismatch");
		}
		if (instanceDivisor == 0) {
			//check if the ammount of vertex data is currently zero
			if (_vertexCount == 0)
				//if it is then set it equal to the element count in the vbo
				_vertexCount = vbo->GetElementCount();
			//if it's not, check if the vbo's element count is equal to the vertex count, if it isn't throw an error (only vbos of the same size 
			//can exist in this VAO implementation)
			else
				LOG_ASSERT(vbo->GetElementCount() == _vertexCount, "All VBOs bound to a VAO should be of equal size in this implemenation.");
		}

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
			if (attrib.attribDivisor != 0) glVertexAttribDivisor(attrib.Slot, attrib.attribDivisor);
		}
		//unbind the vbo
		vbo->UnBind();
		//unbind the VAO
		UnBind();
	}

	//clears all the vbos from this vao
	void TTN_VertexArrayObject::ClearVertexBuffers()
	{
		//bind the vao
		Bind();

		//iterate through all the vertex buffers
		for (size_t i = 0; i < _vbos.size(); i++) {
			//iterate through the attributes and remove them from opengl
			for (size_t j = 0; j < _vbos[i].Attributes.size(); j++) {
				if(_vbos[i].Attributes[j].attribDivisor != 0) glVertexAttribDivisor(_vbos[i].Attributes[j].Slot, 0);
				glDisableVertexArrayAttrib(_handle, _vbos[i].Attributes[j].Slot);
			}
		}
		//get rid of the base vectors and the stored vbo pointers too
		_vbos.clear();

		//unbind the vao
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

	//calls the openGL functions to acutally draw the triangles contained within the VAO, but does so with instancing
	void TTN_VertexArrayObject::RenderInstanced(size_t numOfObjects, size_t numOfVerts) const
	{
		//bind the VAO so we can use it
		Bind();
		//check if the VAO has an IBO bound to it 
		if (_ibo != nullptr)
			//if it does, then use the ibo to draw the triangles
			glDrawElementsInstanced(GL_TRIANGLES, _ibo->GetElementCount(), _ibo->GetElementType(), nullptr, numOfObjects);
		else
			//otherwise it must only have vbos, so use those vbos to draw the triangles
			if(numOfVerts == 0) glDrawArraysInstanced(GL_TRIANGLES, 0, _vertexCount, numOfObjects);
			else glDrawArraysInstanced(GL_TRIANGLES, 0, numOfVerts, numOfObjects);
		//unbind the VAO
		UnBind();
	}
}