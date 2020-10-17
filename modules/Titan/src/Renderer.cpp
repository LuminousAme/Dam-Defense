//Titan Engine, by Atlas X Games 
// Renderer.cpp - source file for the class that allows meshes, 2D sprites, etc. be rendered

//include the header
#include "Titan/Renderer.h"

namespace Titan {
	//constructor, creates a renderer object from a mesh
	TTN_Renderer::TTN_Renderer(TTN_Mesh mesh)
	{
		//set the mesh
		SetMesh(mesh);
		//sets the shader to a nullpointer
		m_Shader = nullptr;
	}

	TTN_Renderer::TTN_Renderer(TTN_Mesh mesh, TTN_Shader::sshptr shader)
	{
		//set the mesh
		SetMesh(mesh);
		//sets the shader
		m_Shader = shader;
	}

	//default constructor
	TTN_Renderer::TTN_Renderer()
	{
		m_Shader = nullptr;
		m_Vao = nullptr;
	}

	//destructor, destroys the object
	TTN_Renderer::~TTN_Renderer()
	{
	}

	//sets the mesh, loading all it's VBOs into a VAO that OpenGL can use to draw
	void TTN_Renderer::SetMesh(TTN_Mesh& mesh)
	{
		m_Vao = TTN_VertexArrayObject::Create();

		//positions
		m_Vao->AddVertexBuffer(mesh.GetVBOPointer(0), {BufferAttribute(0, 3, GL_FLOAT, false, sizeof(float)*3, 0)});
		//normals
		m_Vao->AddVertexBuffer(mesh.GetVBOPointer(1), { BufferAttribute(1, 3, GL_FLOAT, false, sizeof(float) * 3, 0) });
		//uvs
		m_Vao->AddVertexBuffer(mesh.GetVBOPointer(2), { BufferAttribute(2, 2, GL_FLOAT, false, sizeof(float) * 3, 0) });
	}

	//sets a shader
	void TTN_Renderer::SetShader(TTN_Shader::sshptr shader)
	{
		m_Shader = shader;
	}

	//function that will send the uniforms with how to draw the object arounding to the camera to openGL
	void TTN_Renderer::Render(glm::mat4 model, glm::mat4 proj, glm::mat4 view)
	{
		//bind the shader this model uses
		m_Shader->Bind();
		//send the uniforms to openGL 
		m_Shader->SetUniformMatrix("MVP", proj * view * model);
		//render the VAO
		m_Vao->Render();
		//unbind the shader
		m_Shader->UnBind();
	}
}