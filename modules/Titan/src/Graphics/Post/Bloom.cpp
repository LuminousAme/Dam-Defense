//Titan Engine by Atlas X Games
//TTN_Bloom.cpp - Source file for the class for TTN_Bloom post processing effects

//include precompiled header
#include "Titan/ttn_pch.h"

//include the class
#include "Titan/Graphics/Post/Bloom.h"

namespace Titan {


	void TTN_Bloom::Init(unsigned width, unsigned height)
	{
		int index = (int)m_buffers.size();
		m_buffers.push_back( TTN_Framebuffer::Create());
		m_buffers[index]->AddColorTarget(GL_RGBA8);
		m_buffers[index]->AddDepthTarget();
		m_buffers[index]->Init(width, height);
		index++;
		m_buffers.push_back(TTN_Framebuffer::Create());
		m_buffers[index]->AddColorTarget(GL_RGBA8);
		m_buffers[index]->AddDepthTarget();
		m_buffers[index]->Init(width, height);
		index++;
		m_buffers.push_back(TTN_Framebuffer::Create());
		m_buffers[index]->AddColorTarget(GL_RGBA8);
		m_buffers[index]->AddDepthTarget();
		m_buffers[index]->Init(width, height);
		index++;

		//Loads the TTN_Shaders
		int indexS = int(m_shaders.size());
		m_shaders.push_back(TTN_Shader::Create());
		m_shaders[indexS]->LoadShaderStageFromFile("shaders/Post/ttn_passthrough_vert.glsl", GL_VERTEX_SHADER);
		m_shaders[indexS]->LoadShaderStageFromFile("shaders/Post/ttn_color_correction_frag.glsl", GL_FRAGMENT_SHADER);//gives an image
		m_shaders[indexS]->Link();
		indexS++;
		m_shaders.push_back(TTN_Shader::Create());
		m_shaders[indexS]->LoadShaderStageFromFile("shaders/Post/ttn_passthrough_vert.glsl", GL_VERTEX_SHADER);
		m_shaders[indexS]->LoadShaderStageFromFile("shaders/Post/TTN_bloom_brightpass_frag.glsl", GL_FRAGMENT_SHADER); //gets bright colors
		m_shaders[indexS]->Link();
		indexS++;
		m_shaders.push_back(TTN_Shader::Create());
		m_shaders[indexS]->LoadShaderStageFromFile("shaders/Post/ttn_passthrough_vert.glsl", GL_VERTEX_SHADER);
		m_shaders[indexS]->LoadShaderStageFromFile("shaders/Post/ttn_blur_gaussian_frag.glsl", GL_FRAGMENT_SHADER); //horizontal and vertical gaussian blur
		m_shaders[indexS]->Link();
		indexS++;
		m_shaders.push_back(TTN_Shader::Create());
		m_shaders[indexS]->LoadShaderStageFromFile("shaders/Post/ttn_passthrough_vert.glsl", GL_VERTEX_SHADER);
		m_shaders[indexS]->LoadShaderStageFromFile("shaders/Post/TTN_bloom_combine_frag.glsl", GL_FRAGMENT_SHADER);//combines all effects
		m_shaders[indexS]->Link();

		TTN_PostEffect::Init(width, height);
	}

	void TTN_Bloom::ApplyEffect(TTN_PostEffect::spostptr buffer)
	{
		//Draws previous buffer to first render target
		BindShader(0);

		buffer->BindColorAsTexture(0, 0, 0);
		m_buffers[0]->RenderToFSQ();
		buffer->UnbindTexture(0);

		UnbindShader();

		//Get bright colors on the first render target
		BindShader(1);
		m_shaders[1]->SetUniform("u_Threshold", m_threshold);

		BindColorAsTexture(0, 0, 0);
		m_buffers[1]->RenderToFSQ();
		UnbindTexture(0);

		UnbindShader();

		//blurs vertically and horizontally m_amount of times
		for (int i = 0; i < m_amount; i++)
		{
			//Horizontal pass
			BindShader(2);
			m_shaders[2]->SetUniform("u_Horizontal", (int)true);
			//_TTN_Shaders[2]->SetUniform("u_Blur", m_blur);

			BindColorAsTexture(1, 0, 0);

			m_buffers[2]->RenderToFSQ();

			UnbindTexture(0);

			//Vertical pass
			m_shaders[2]->SetUniform("u_Horizontal", (int)false);//set to vertical

			BindColorAsTexture(2, 0, 0);

			m_buffers[1]->RenderToFSQ();

			UnbindTexture(0);
			UnbindShader();
		}

		//combine the blur and scene to create TTN_Bloom effect
		BindShader(3);

		//_TTN_Shaders[3]->SetUniform("u_Intensity", m_intensity);//exposure
		//_TTN_Shaders[3]->SetUniform("u_Gamma", m_gamma);//gamma

		buffer->BindColorAsTexture(0, 0, 0);
		BindColorAsTexture(1, 0, 1);

		m_buffers[0]->RenderToFSQ();

		UnbindTexture(1);
		UnbindTexture(0);
		UnbindShader();
	}



}