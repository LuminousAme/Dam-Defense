//Titan Engine by Atlas X Games
//ColorCorrect.cpp - Source file for the class for color correction post processing effects

//include precompiled header
#include "Titan/ttn_pch.h"

//include the class
#include "Titan/Graphics/Post/ColorCorrect.h"

namespace Titan {
	//initliazes the color correction effect
	void TTN_ColorCorrect::Init(unsigned width, unsigned height)
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
		m_shaders[index]->LoadShaderStageFromFile("shaders/Post/ttn_color_correction_frag.glsl", GL_FRAGMENT_SHADER);
		m_shaders[index]->Link();

		//init the original 
		TTN_PostEffect::Init(width, height);
	}

	//applies the effect to the full screen quad
	void TTN_ColorCorrect::ApplyEffect(TTN_PostEffect::spostptr buffer)
	{
		//binds the shader (size() - 2 because we're loading the pass through as the last shader, and thus this is the second last shader)
		BindShader(m_shaders.size() - 2);
		m_shaders[m_shaders.size() - 2]->SetUniform("u_Intensity", m_intensity);
		//binds the color 
		buffer->BindColorAsTexture(0, 0, 0);
		//binds the cube 
		m_cube->bind(30);
		//renders to the full screen quad
		m_buffers[0]->RenderToFSQ();
		//unbinds everything
		m_cube->unbind(30);
		buffer->UnbindTexture(0);
		UnbindShader();
	}
}