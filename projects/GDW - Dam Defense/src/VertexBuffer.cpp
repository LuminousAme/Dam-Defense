#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(const GLfloat data[], GLuint element)
{
	id = element;
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glCreateBuffers(1, &element);
	//glGenBuffers(1, &element);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, element);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

}

void VertexBuffer::Create(GLuint e)
{
	glBindBuffer(GL_ARRAY_BUFFER, e);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(e); //pos

}

void VertexBuffer::Draw(GLint first, GLsizei count)
{
	glDrawArrays(GL_TRIANGLES, first, count);
}
