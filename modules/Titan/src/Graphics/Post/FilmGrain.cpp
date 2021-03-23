#include "Titan/ttn_pch.h"
#include "Titan/Graphics/Post/FilmGrain.h"

namespace Titan {
	//initliazes the pixelation effect
	void TTN_FilmGrain::Init(unsigned width, unsigned height)
	{
		//Set up framebuffers
		//creates a new framebuffer with a basic color and depth target
		int index = (int)m_buffers.size();
		m_buffers.push_back(TTN_Framebuffer::Create());
		m_buffers[index]->AddColorTarget(GL_RGBA8);
		m_buffers[index]->AddDepthTarget();
		//initliaze the framebuffer
		m_buffers[index]->Init(width, height);

		index = (int)m_shaders.size();
		//set up color correction shader
		m_shaders.push_back(TTN_Shader::Create());
		//load in the shader
		m_shaders[index]->LoadShaderStageFromFile("shaders/Post/ttn_passthrough_vert.glsl", GL_VERTEX_SHADER);
		m_shaders[index]->LoadShaderStageFromFile("shaders/Post/ttn_film_grain_frag.glsl", GL_FRAGMENT_SHADER);
		m_shaders[index]->Link();

		//init the original
		TTN_PostEffect::Init(width, height);
	}

	void TTN_FilmGrain::ApplyEffect(TTN_PostEffect::spostptr buffer)
	{
		BindShader(m_shaders.size() - 2);
		m_shaders[m_shaders.size() - 2]->SetUniform("u_Amount", m_amount);

		//binds the color
		buffer->BindColorAsTexture(0, 0, 0);

		//renders to the full screen quad
		m_buffers[0]->RenderToFSQ();
		//unbinds everything
		buffer->UnbindTexture(0);
		UnbindShader();
	}
}