#include "Titan/ttn_pch.h"
#include "Titan/PostEffect.h"

namespace Titan {

	void TTN_PostEffect::Init(unsigned width, unsigned height)
	{
		//set up framebuffers
		int index = int(_buffers.size());

		_buffers.push_back(new TTN_Framebuffer());
		_buffers[index]->AddColorTarget(GL_RGBA8);
		_buffers[index]->AddDepthTarget();
		_buffers[index]->Init(width, height);

		_shaders.push_back(TTN_Shader::Create());
		//placeholder shaders
		_shaders[_shaders.size() - 1]->LoadShaderStageFromFile("shaders/passthrough_vert.glsl", GL_VERTEX_SHADER);
		_shaders[_shaders.size() - 1]->LoadShaderStageFromFile("shaders/color_correction_frag.glsl", GL_FRAGMENT_SHADER);
		_shaders[_shaders.size() - 1]->Link();

	}

	void TTN_PostEffect::ApplyEffect(TTN_PostEffect* prevBuffer) {
		BindShader(_shaders.size() - 1);

		prevBuffer->BindColorAsTexture(0, 0, 0);

		_buffers[0]->RenderToFSQ();

		prevBuffer->UnbindTexture(0);

		UnbindShader();
	}

	void TTN_PostEffect::DrawToScreen() {

		BindShader(_shaders.size() - 1);

		BindColorAsTexture(0, 0, 0);
		_buffers[0]->DrawFullScreenQuad();
		UnbindTexture(0);

		UnbindShader();
	}

	void TTN_PostEffect::Reshape(unsigned width, unsigned height) {

		for (unsigned int i = 0; i < _buffers.size(); i++) {
			_buffers[i]->Reshape(width, height);
		}

	}

	void TTN_PostEffect::Clear() {
		for (unsigned int i = 0; i < _buffers.size(); i++) {
			_buffers[i]->Clear();
		}
	}

	void TTN_PostEffect::Unload() {

		for (unsigned int i = 0; i < _buffers.size(); i++) {
			if (_buffers[i] != nullptr) {
				_buffers[i]->Unload();
				delete _buffers[i];
				_buffers[i] = nullptr;
			}

		}
		_shaders.clear();

	}

	void TTN_PostEffect::BindBuffer(int index) {
		_buffers[index]->Bind();

	}

	void TTN_PostEffect::UnbindBuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
	}

	void TTN_PostEffect::BindColorAsTexture(int index, int colorBuffer, int textureSlot) {

		_buffers[index]->BindColorAsTexture(colorBuffer, textureSlot);

	}

	void TTN_PostEffect::BindDepthAsTexture(int index, int textureSlot) {
		_buffers[index]->BindDepthAsTexture(textureSlot);
	}

	void TTN_PostEffect::UnbindTexture(int textureSlot)
	{
		glActiveTexture(GL_TEXTURE0 + textureSlot);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
	}

	void TTN_PostEffect::BindShader(int index)
	{
		_shaders[index]->Bind();
	}

	void TTN_PostEffect::UnbindShader()
	{
		glUseProgram(GL_NONE);
	}


}
