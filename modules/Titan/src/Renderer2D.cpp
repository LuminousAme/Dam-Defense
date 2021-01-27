//Titan Engine, by Atlas X Games 
// Renderer.cpp - source file for the class that allows meshes to be rendered

//precompile header
#include "Titan/ttn_pch.h"
//include the header
#include "Titan/Renderer2D.h"

namespace Titan {
	//default constructor
	TTN_Renderer2D::TTN_Renderer2D()
	{
		m_sprite = nullptr;
		m_color = glm::vec4(1.0f);
		m_RenderLayer = 0;
	}

	//constructor that takes data
	TTN_Renderer2D::TTN_Renderer2D(TTN_Texture2D::st2dptr sprite, glm::vec4 color, int renderOrderLayer)
		: m_sprite(sprite), m_color(color), m_RenderLayer(renderOrderLayer)
	{}

	//renders the sprite
	void TTN_Renderer2D::Render(glm::mat4 model, glm::mat4 VP)
	{
		//make sure there's acutally a sprite to bind and things are set up first 
		if (m_sprite != nullptr && s_shader != nullptr) {
			//bind the sprite shader
			s_shader->Bind();

			//send the uniforms to openGL 
			s_shader->SetUniformMatrix("MVP", VP * model);
			s_shader->SetUniform("u_Color", m_color);

			//bind the texture
			m_sprite->Bind(0);

			//render the VAO
			s_vao->Render();
		}
	}

	//sets up the shader and vao for rendering
	void TTN_Renderer2D::InitRenderer2D()
	{
		//prepare the vbos
		//create arrays
		glm::vec3 positions[6];
		glm::vec2 textCords[6];

		//fill the arrays with data
		positions[0] = glm::vec3(0.0f, 1.0f, 0.0f);
		positions[1] = glm::vec3(1.0f, 0.0f, 0.0f);
		positions[2] = glm::vec3(0.0f, 0.0f, 0.0f);
		positions[3] = glm::vec3(0.0f, 1.0f, 0.0f);
		positions[4] = glm::vec3(1.0f, 1.0f, 0.0f);
		positions[5] = glm::vec3(1.0f, 0.0f, 0.0f);

		textCords[0] = glm::vec2(0.0f, 1.0f);
		textCords[1] = glm::vec2(1.0f, 0.0f);
		textCords[2] = glm::vec2(0.0f, 0.0f);
		textCords[3] = glm::vec2(0.0f, 1.0f);
		textCords[4] = glm::vec2(1.0f, 1.0f);
		textCords[5] = glm::vec2(1.0f, 0.0f);

		//create vbo and load them with the arrays
		s_vertPos = TTN_VertexBuffer::Create();
		s_vertPos->LoadData(positions, 6);
		s_Uvs = TTN_VertexBuffer::Create();
		s_Uvs->LoadData(textCords, 6);

		//create the vao and load the vertex buffers into it
		s_vao = TTN_VertexArrayObject::Create();
		s_vao->AddVertexBuffer(s_vertPos, { BufferAttribute(0, 3, GL_FLOAT, false, sizeof(float) * 3, 0, AttribUsage::Position) });
		s_vao->AddVertexBuffer(s_Uvs, { BufferAttribute(1, 2, GL_FLOAT, false, sizeof(float) * 2, 0, AttribUsage::Texture) });

		//create and load the shader program
		s_shader = TTN_Shader::Create();
		s_shader->LoadShaderStageFromFile("shaders/ttn_sprite_vert.glsl", GL_VERTEX_SHADER);
		s_shader->LoadShaderStageFromFile("shaders/ttn_sprite_frag.glsl", GL_FRAGMENT_SHADER);
		s_shader->Link();
	}
}