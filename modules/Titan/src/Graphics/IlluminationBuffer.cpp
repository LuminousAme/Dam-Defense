//include titan's precompile header
#include "Titan/ttn_pch.h"
#include "Titan/Graphics/Post/Framebuffer.h"
#include "Titan/Graphics/IlluminationBuffer.h"

namespace Titan {
	void TTN_IlluminationBuffer::Init(unsigned width, unsigned height)
	{
		//composite buffer
		int index = int(m_buffers.size());
		m_buffers.push_back(TTN_Framebuffer::Create());
		m_buffers[index]->AddColorTarget(GL_RGBA8);
		m_buffers[index]->AddDepthTarget();
		m_buffers[index]->Init(width, height);

		// illum buffer
		index = int(m_buffers.size());
		m_buffers.push_back(TTN_Framebuffer::Create());
		m_buffers[index]->AddColorTarget(GL_RGBA8);
		m_buffers[index]->AddDepthTarget();
		m_buffers[index]->Init(width, height);

		//load directional TTN_GBuffer shader
		index = int(m_shaders.size());
		m_shaders.push_back(TTN_Shader::Create());
		m_shaders[index]->LoadShaderStageFromFile("shaders/Post/ttn_passthrough_vert.glsl", GL_VERTEX_SHADER);
		m_shaders[index]->LoadShaderStageFromFile("shaders/ttn_gBuffer_directional_frag.glsl", GL_FRAGMENT_SHADER);
		m_shaders[index]->Link();

		//load ambient TTN_GBuffer shader
		index = int(m_shaders.size());
		m_shaders.push_back(TTN_Shader::Create());
		m_shaders[index]->LoadShaderStageFromFile("shaders/Post/ttn_passthrough_vert.glsl", GL_VERTEX_SHADER);
		m_shaders[index]->LoadShaderStageFromFile("shaders/ttn_gBuffer_ambient_frag.glsl", GL_FRAGMENT_SHADER);
		m_shaders[index]->Link();

		//allocates sun buffer
		m_sunBuffer.AllocateMemory(sizeof(TTN_DirectionalLight));

		//if sun enabled, send data
		if (m_sunEnabled) {
			m_sunBuffer.SendData(reinterpret_cast<void*>(&m_sun), sizeof(TTN_DirectionalLight));
		}
		TTN_PostEffect::Init(width, height);
	}

	void TTN_IlluminationBuffer::ApplyEffect(TTN_GBuffer::sgbufptr gBuffer)
	{
		//send direcitonal light data
		m_sunBuffer.SendData(reinterpret_cast<void*>(&m_sun), sizeof(TTN_DirectionalLight));

		if (m_sunEnabled) {
			//binds directional light shader
			m_shaders[TTN_Lights::DIRECTIONAL]->Bind();
			m_shaders[TTN_Lights::DIRECTIONAL]->SetUniformMatrix("u_LightSpaceMatrix", m_lightSpaceViewProj);
			m_shaders[TTN_Lights::DIRECTIONAL]->SetUniform("u_CamPos", m_camPos);

			//bind sun uniform
			m_sunBuffer.Bind(0);

			gBuffer->BindLighting();

			//binds and draws to illumination buffer
			m_buffers[1]->RenderToFSQ();

			gBuffer->UnbindLighting();

			//unbuinds the uniform buffers
			m_sunBuffer.Unbind(0);

			//unbind shader
			m_shaders[TTN_Lights::DIRECTIONAL]->UnBind();
		}

		//bind ambient shader
		m_shaders[TTN_Lights::AMBIENT]->Bind();

		//binds direcitonal light data
		m_sunBuffer.Bind(0);

		//binds for lighting
		gBuffer->BindLighting();
		m_buffers[1]->BindColorAsTexture(0, 4);
		m_buffers[0]->BindColorAsTexture(0, 5);

		m_buffers[0]->RenderToFSQ();

		m_buffers[0]->UnbindTexture(5);
		m_buffers[1]->UnbindTexture(4);

		gBuffer->UnbindLighting();

		//unbinds uniform buffer
		m_sunBuffer.Unbind(0);
		m_shaders[TTN_Lights::AMBIENT]->UnBind();
	}

	void TTN_IlluminationBuffer::DrawIllumBuffer()
	{
		m_shaders[m_shaders.size() - 1]->Bind();
		m_buffers[1]->BindColorAsTexture(0, 0);

		TTN_Framebuffer::DrawFullScreenQuad();

		m_buffers[1]->UnbindTexture(0);

		m_shaders[m_shaders.size() - 1]->UnBind();
	}

	void TTN_IlluminationBuffer::SetLightSpaceViewProj(glm::mat4 lightSpaceViewProj)
	{
		m_lightSpaceViewProj = lightSpaceViewProj;
	}

	void TTN_IlluminationBuffer::SetCamPos(glm::vec3 camPos)
	{
		m_camPos = camPos;
	}

	TTN_DirectionalLight& TTN_IlluminationBuffer::GetSunRef()
	{
		return m_sun;
	}

	void TTN_IlluminationBuffer::SetSun(TTN_DirectionalLight newSun)
	{
		m_sun = newSun;
	}

	void TTN_IlluminationBuffer::SetSun(glm::vec4 lightDir, glm::vec4 lightCol)
	{
		m_sun.m_lightDirection = lightDir;
		m_sun.m_lightColor = lightCol;
	}

	void TTN_IlluminationBuffer::EnableSun(bool enabled)
	{
		m_sunEnabled = enabled;
	}
}