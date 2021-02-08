#include "Titan/ttn_pch.h"
#include "Titan/PostEffect.h"

namespace Titan {
	//initliazes the post processing effect
	void TTN_PostEffect::Init(unsigned width, unsigned height)
	{
		//Set up framebuffers
		if (!m_shaders.size() > 0) {
			//creates a new framebuffer with a basic color and depth target
			int index = int(m_buffers.size());
			m_buffers.push_back(TTN_Framebuffer::Create());
			m_buffers[index]->AddColorTarget(GL_RGBA8);
			m_buffers[index]->AddDepthTarget();
			//initliaze the framebuffer
			m_buffers[index]->Init(width, height);
		}

		//set up the basic post effect shader
		m_shaders.push_back(TTN_Shader::Create());
		//load in a basic passthrough shader (maybe try to make another system later to manage the shader pointers better)
		m_shaders[m_shaders.size() - 1]->LoadShaderStageFromFile("shaders/Post/ttn_passthrough_vert.glsl", GL_VERTEX_SHADER);
		m_shaders[m_shaders.size() - 1]->LoadShaderStageFromFile("shaders/Post/ttn_passthrough_frag.glsl", GL_FRAGMENT_SHADER);
		m_shaders[m_shaders.size() - 1]->Link();
	}

	//applies the effect to the full screen quad
	void TTN_PostEffect::ApplyEffect(TTN_PostEffect::spostptr prevBuffer) {
		//binds the shader
		BindShader(m_shaders.size() - 1);
		//binds the color
		prevBuffer->BindColorAsTexture(0, 0, 0);
		//renders to the full screen quad
		m_buffers[0]->RenderToFSQ();
		//unbind everything
		prevBuffer->UnbindTexture(0);
		UnbindShader();
	}

	//draws the effect to the screen
	void TTN_PostEffect::DrawToScreen() {
		//binds the shader
		BindShader(m_shaders.size() - 1);
		//binds the color
		BindColorAsTexture(0, 0, 0);
		//draws the full screen quad to the screen
		m_buffers[0]->DrawFullScreenQuad();
		//and unbinds everything
		UnbindTexture(0);
		UnbindShader();
	}

	//resizes the framebuffers
	void TTN_PostEffect::Reshape(unsigned width, unsigned height) {
		//go through all the framebuffers and resize them
		for (unsigned int i = 0; i < m_buffers.size(); i++) {
			m_buffers[i]->Reshape(width, height);
		}
	}

	//clear all the framebuffers
	void TTN_PostEffect::Clear() {
		//go through all the framebuffers and clear them
		for (unsigned int i = 0; i < m_buffers.size(); i++) {
			m_buffers[i]->Clear();
		}
	}

	//unloads a post effect
	void TTN_PostEffect::Unload() {
		//delete all the smart pointers to the framebuffers and shaders
		m_buffers.clear();
		m_shaders.clear();
	}

	//binds the buffer at a given index
	void TTN_PostEffect::BindBuffer(int index) {
		m_buffers[index]->Bind();
	}

	//unbinds any framebuffers
	void TTN_PostEffect::UnbindBuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
	}

	//binds a color buffer as a texture to a given texture slot
	void TTN_PostEffect::BindColorAsTexture(int index, int colorBuffer, int textureSlot) {

		m_buffers[index]->BindColorAsTexture(colorBuffer, textureSlot);

	}

	//binds a depth buffer as a texture to a given texture slot
	void TTN_PostEffect::BindDepthAsTexture(int index, int textureSlot) {
		m_buffers[index]->BindDepthAsTexture(textureSlot);
	}

	//unbinds a textrue in a given texture slot
	void TTN_PostEffect::UnbindTexture(int textureSlot)
	{
		glActiveTexture(GL_TEXTURE0 + textureSlot);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
	}

	//binds the shader at a given shader index
	void TTN_PostEffect::BindShader(int index)
	{
		m_shaders[index]->Bind();
	}

	//unbinds any bound shader
	void TTN_PostEffect::UnbindShader()
	{
		glUseProgram(GL_NONE);
	}
}