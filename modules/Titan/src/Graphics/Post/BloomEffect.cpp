//Titan Engine by Atlas X Games
//BloomEffect.cpp - Source file for the class for bloom post processing effects

//include precompiled header
#include "Titan/ttn_pch.h"

//include the class
#include "Titan/Graphics/Post/BloomEffect.h"

namespace Titan {
	//initliazes the color correction effect
	void TTN_BloomEffect::Init(unsigned width, unsigned height)
	{
		//Set up framebuffers
		//creates a new framebuffer with a basic color and depth target
		int index = (int)m_buffers.size();
		m_buffers.push_back(TTN_Framebuffer::Create());
		m_buffers[index]->AddColorTarget(GL_RGBA8);
		m_buffers[index]->AddDepthTarget();
		//initliaze the framebuffer
		m_buffers[index]->Init(width, height);
		index++;

		//create a new framebuffer with a basic color for the horizontal blurs
		m_buffers.push_back(TTN_Framebuffer::Create());
		m_buffers[index]->AddColorTarget(GL_RGBA8);
		m_buffers[index]->AddDepthTarget();
		//init the framebuffer, dividing by the divisor to save memory
		m_buffers[index]->Init(width / m_blurBufferDivisor, height / m_blurBufferDivisor);
		index++;

		//create a new framebuffer with a basic color and depth target for the vertical blurs
		m_buffers.push_back(TTN_Framebuffer::Create());
		m_buffers[index]->AddColorTarget(GL_RGBA8);
		m_buffers[index]->AddDepthTarget();
		//init the framebuffer, dividing by the divisor to save memory
		m_buffers[index]->Init(width / m_blurBufferDivisor, height / m_blurBufferDivisor);
		index++;

		//set up the shaders
		index = (int)m_shaders.size();
		//extraction pass shader
		m_shaders.push_back(TTN_Shader::Create());
		//load in the shader
		m_shaders[index]->LoadShaderStageFromFile("shaders/Post/ttn_passthrough_vert.glsl", GL_VERTEX_SHADER);
		m_shaders[index]->LoadShaderStageFromFile("shaders/Post/ttn_bloom_extraction_frag.glsl", GL_FRAGMENT_SHADER);
		m_shaders[index]->Link();
		index++;

		//composotion shader
		m_shaders.push_back(TTN_Shader::Create());
		//load in the shader
		m_shaders[index]->LoadShaderStageFromFile("shaders/Post/ttn_passthrough_vert.glsl", GL_VERTEX_SHADER);
		m_shaders[index]->LoadShaderStageFromFile("shaders/Post/ttn_blend_screen_frag.glsl", GL_FRAGMENT_SHADER);
		m_shaders[index]->Link();
		index++;

		//horizontal blur shader
		m_shaders.push_back(TTN_Shader::Create());
		//load in the shader
		m_shaders[index]->LoadShaderStageFromFile("shaders/Post/ttn_passthrough_vert.glsl", GL_VERTEX_SHADER);
		m_shaders[index]->LoadShaderStageFromFile("shaders/Post/ttn_box_horizontal_frag.glsl", GL_FRAGMENT_SHADER);
		m_shaders[index]->Link();
		index++;

		//vertical blur shader
		m_shaders.push_back(TTN_Shader::Create());
		//load in the shader
		m_shaders[index]->LoadShaderStageFromFile("shaders/Post/ttn_passthrough_vert.glsl", GL_VERTEX_SHADER);
		m_shaders[index]->LoadShaderStageFromFile("shaders/Post/ttn_box_vertical_frag.glsl", GL_FRAGMENT_SHADER);
		m_shaders[index]->Link();

		//init the original
		TTN_PostEffect::Init(width, height);
	}

	//applies the effect to the full screen quad
	void TTN_BloomEffect::ApplyEffect(TTN_PostEffect::spostptr buffer)
	{
		//do the extraction pass taking the brightness from the previous effect

		//bind the shader
		BindShader(0);
		//bind the previous effect as the color
		buffer->BindColorAsTexture(0, 0, 0);
		//m_buffers[0]->BindColorAsTexture(0, 0);
		//set the threshold as a uniform
		m_shaders[0]->SetUniform("u_Threshold", m_threshold);
		//renders to the fullscreen quad in the second (hori blur) framebuffer
		m_buffers[1]->RenderToFSQ();
		//unbinds everything
		buffer->UnbindTexture(0);
		UnbindShader();

		//loop through each of the blur passes
		for (int i = 0; i < m_numOfPasses; i++) {
			//horiztonal pass

			//bind the shader
			BindShader(2);
			//bind the second (hori blur) framebuffer as the color
			m_buffers[1]->BindColorAsTexture(0, 0);
			//set the uniforms
			m_shaders[2]->SetUniform("u_Step", m_radius / m_buffers[0]->m_width);
			m_shaders[2]->SetUniform("u_Weights", m_weights[0], 5);
			//renders to the fullscreen quad in the third (vert blur) framebuffer
			m_buffers[2]->RenderToFSQ();
			//unbinds everything
			m_buffers[1]->UnbindTexture(0);
			UnbindShader();

			//vertical pass

			//bind the shader
			BindShader(3);
			//bind the third (vert blur) framebuffer as the color
			m_buffers[2]->BindColorAsTexture(0, 0);
			//sets the uniforms
			m_shaders[3]->SetUniform("u_Step", m_radius / m_buffers[0]->m_height);
			m_shaders[3]->SetUniform("u_Weights", m_weights[0], 5);
			//renders to the fullscreen quad in the second (hori blur) framebuffer
			m_buffers[1]->RenderToFSQ();
			//unbinds everything
			m_buffers[2]->UnbindTexture(0);
			UnbindShader();
		}

		//composition pass

		//bind the shader
		BindShader(1);
		//bind the previous effect's color as a texture
		buffer->BindColorAsTexture(0, 0, 0);
		//and the result of the blur as a second texture
		m_buffers[1]->BindColorAsTexture(0, 1);
		//renders to the fullscreen quad in the last, unused framebuffer
		m_buffers[0]->RenderToFSQ();
		//and unbind everything
		buffer->UnbindTexture(0);
		m_buffers[1]->UnbindTexture(1);
		UnbindShader();
	}

	//reshapes the post processing effect and all it's framebuffers
	void TTN_BloomEffect::Reshape(unsigned width, unsigned height)
	{
		//resize all of the framebuffers
		m_buffers[0]->Reshape(width, height); //extraction
		m_buffers[1]->Reshape(width / m_blurBufferDivisor, height / m_blurBufferDivisor); //hori blur
		m_buffers[2]->Reshape(width / m_blurBufferDivisor, height / m_blurBufferDivisor); //vert blur
	}

	//sets the scale the blur framebuffers are divided by to save memory
	void TTN_BloomEffect::SetBlurDownScale(unsigned downscale)
	{
		//set it
		m_blurBufferDivisor = downscale;

		//reshape the blur framebuffers
		m_buffers[1]->Reshape(m_buffers[0]->m_width / downscale, m_buffers[0]->m_height / downscale); //hori blur
		m_buffers[2]->Reshape(m_buffers[0]->m_width / downscale, m_buffers[0]->m_height / downscale); //vert blur
	}
}