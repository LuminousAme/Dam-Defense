#pragma once
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

class VertexBuffer
{

public:

	VertexBuffer(const GLfloat data[], GLuint element);
	~VertexBuffer(){
	}

	void Create(GLuint elements);
	void Draw(GLint first, GLsizei count);


protected:
	GLuint id;

	VertexBuffer(GLenum type, GLenum usage);

	size_t _elementSize; // The size or stride of our elements
	size_t _elementCount; // The number of elements in the buffer
	GLuint _id; // The OpenGL handle for the underlying buffer
	GLenum _usage; // The buffer usage mode (GL_STATIC_DRAW, GL_DYNAMIC_DRAW)
	GLenum _type; // The buffer type (ex GL_ARRAY_BUFFER, GL_ARRAY_ELEMENT_BUFFER)


};

