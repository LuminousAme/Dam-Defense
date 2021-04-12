#include "Titan/ttn_pch.h"
#include "Titan/Graphics/Post/Pixelation.h"

namespace Titan {
	//initliazes the pixelation effect
	void TTN_Pixelation::Init(unsigned width, unsigned height)
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
		m_shaders[index]->LoadShaderStageFromFile("shaders/Post/TTN_Pixelation_frag.glsl", GL_FRAGMENT_SHADER);
		m_shaders[index]->Link();

		//init the original
		TTN_PostEffect::Init(width, height);
	}

	void TTN_Pixelation::ApplyEffect(TTN_PostEffect::spostptr buffer)
	{
		BindShader(m_shaders.size() - 2);
		m_shaders[m_shaders.size() - 2]->SetUniform("u_Pixels", m_pixel);

		//binds the color
		buffer->BindColorAsTexture(0, 0, 0);

		//renders to the full screen quad
		m_buffers[0]->RenderToFSQ();
		//unbinds everything
		buffer->UnbindTexture(0);
		UnbindShader();
	}
}