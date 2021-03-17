//Titan Engine, by Atlas X Games
// Renderer.cpp - source file for the class that allows meshes to be rendered

//precompile header
#include "Titan/ttn_pch.h"
//include the header
#include "Titan/Graphics/Renderer.h"

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
		//set the renderlayer to zero
		m_RenderLayer = 0;
		//set shadow cast
		m_castShadows = true;
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
		//set shadow cast
		m_castShadows = true;
	}

	//default constructor
	TTN_Renderer::TTN_Renderer()
	{
		m_mesh = nullptr;
		m_Shader = nullptr;
		m_Mat = nullptr;
		m_RenderLayer = 0;
		//set shadow cast
		m_castShadows = true;
	}

	//initliazes the shader that renders the depth pass for shadow mapping
	void TTN_Renderer::InitShadowRendering()
	{
		s_simpleShadowShader = TTN_Shader::Create();
		s_simpleShadowShader->LoadShaderStageFromFile("shaders/ttn_simple_depth_vert.glsl", GL_VERTEX_SHADER);
		s_simpleShadowShader->LoadShaderStageFromFile("shaders/ttn_simple_depth_geo.glsl", GL_GEOMETRY_SHADER);
		s_simpleShadowShader->LoadShaderStageFromFile("shaders/ttn_simple_depth_frag.glsl", GL_FRAGMENT_SHADER);
		s_simpleShadowShader->Link();

		s_shaderRenderingIsInit = true;
	}

	void TTN_Renderer::InitgBufferRendering()
	{
		//s_gBufferPassShader = TTN_Shader::Create();
		//s_gBufferPassShader->LoadShaderStageFromFile("shaders/ttn_vert_color.glsl", GL_VERTEX_SHADER);
		//s_gBufferPassShader->LoadShaderStageFromFile("shaders/ttn_gBuffer_pass_frag.glsl", GL_FRAGMENT_SHADER);
		//s_gBufferPassShader->Link();
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
	void TTN_Renderer::Render(glm::mat4 model, glm::mat4 VP, glm::mat4 lightSpaceMat[], glm::mat4 view)
	{
		//make sure the vao is acutally set up before continuing
		if (m_mesh->GetVAOPointer() == nullptr)
			//if it isn't, then stop then return so the later code doesn't break the entire program
			return;

		//send the uniforms to openGL
		if (m_Shader->GetVertexShaderDefaultStatus() != (int)TTN_DefaultShaders::VERT_SKYBOX &&
			m_Shader->GetVertexShaderDefaultStatus() != (int)TTN_DefaultShaders::NOT_DEFAULT) {
			m_Shader->SetUniformMatrix("MVP", VP * model);
			m_Shader->SetUniformMatrix("Model", model);
			m_Shader->SetUniformMatrix("NormalMat", glm::mat3(glm::transpose(glm::inverse(model))));
			m_Shader->SetUniformMatrix("u_LightSpaceMatrix", lightSpaceMat[0], sizeof(lightSpaceMat)/sizeof(lightSpaceMat[0]));
			m_Shader->SetUniformMatrix("View", view);
		}

		//render the VAO
		m_mesh->GetVAOPointer()->Render();
	}
}