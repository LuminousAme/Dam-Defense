//Titan Engine, by Atlas X Games 
// Renderer.cpp - source file for the class that allows meshes, 2D sprites, etc. be rendered

//include the header
#include "Titan/Renderer.h"

namespace Titan {
	//constructor, creates a renderer object from a mesh
	TTN_Renderer::TTN_Renderer(TTN_Mesh::smptr mesh)
	{
		//set the mesh
		SetMesh(mesh);
		//sets the shader to a nullpointer
		m_Shader = nullptr;
		//sets the material to a nullpointer
		m_Mat = nullptr;
	}

	TTN_Renderer::TTN_Renderer(TTN_Mesh::smptr mesh, TTN_Shader::sshptr shader, TTN_Material::smatptr material, int Renderlayer)
	{
		//set the mesh
		SetMesh(mesh);
		//sets the shader
		m_Shader = shader;
		//sets the material
		m_Mat = material;
		//sets the render layer
		m_RenderLayer = Renderlayer;
	}

	//default constructor
	TTN_Renderer::TTN_Renderer()
	{
		m_mesh = nullptr;
		m_Shader = nullptr;
		m_Mat = nullptr;
		m_RenderLayer = 0;
	}

	//destructor, destroys the object
	TTN_Renderer::~TTN_Renderer()
	{
	}

	//sets the mesh, loading all it's VBOs into a VAO that OpenGL can use to draw
	void TTN_Renderer::SetMesh(TTN_Mesh::smptr mesh)
	{
		m_mesh = mesh;
	}

	//sets a shader
	void TTN_Renderer::SetShader(TTN_Shader::sshptr shader)
	{
		m_Shader = shader;
	}

	//sets a material
	void TTN_Renderer::SetMat(TTN_Material::smatptr mat)
	{
		m_Mat = mat;
	}

	//sets the renderlayer
	void TTN_Renderer::SetRenderLayer(int renderLayer)
	{
		m_RenderLayer = renderLayer;
	}

	//function that will send the uniforms with how to draw the object arounding to the camera to openGL
	void TTN_Renderer::Render(glm::mat4 model, glm::mat4 VP)
	{
		//make sure the vao is acutally set up before continuing
		if (m_mesh->GetVAOPointer() == nullptr)
			//if it isn't, then stop then return so the later code doesn't break the entire program
			return;

		//bind the shader this model uses
		m_Shader->Bind();
		//send the uniforms to openGL 
		m_Shader->SetUniformMatrix("MVP", VP * model);
		m_Shader->SetUniformMatrix("Model", model);
		m_Shader->SetUniformMatrix("NormalMat", glm::mat3(glm::transpose(glm::inverse(model))));
		//render the VAO
		m_mesh->GetVAOPointer()->Render();
		//unbind the shader
		m_Shader->UnBind();
	}
}