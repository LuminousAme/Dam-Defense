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

		//make diffuse and specular ramps
		m_diffuseRamp = m_diffuseRamp->Create();
		m_diffuseRamp->Clear();
		m_specularRamp = m_specularRamp->Create();
		m_specularRamp->Clear();

		m_useDiffuseRamp = false;
		m_useSpecularRamp = false;

		TTN_PostEffect::Init(width, height);
	}

	void TTN_IlluminationBuffer::ApplyEffect(TTN_GBuffer::sgbufptr gBuffer)
	{
		//send direcitonal light data
		m_sunBuffer.SendData(reinterpret_cast<void*>(&m_sun), sizeof(TTN_DirectionalLight));

		if (m_sunEnabled) {
			//binds directional light shader
			m_shaders[TTN_Lights::DIRECTIONAL]->Bind();
			m_shaders[TTN_Lights::DIRECTIONAL]->SetUniform("u_CamPos", m_camPos);
			m_shaders[TTN_Lights::DIRECTIONAL]->SetUniform("u_SplitRanges", m_splitRanges[0], 4);
			m_shaders[TTN_Lights::DIRECTIONAL]->SetUniformMatrix("u_lightViewProj", m_lightSpaceViewProj[0], 4);
			m_shaders[TTN_Lights::DIRECTIONAL]->SetUniformMatrix("u_vp", m_viewMat);

			//bind sun uniform
			m_sunBuffer.Bind(0);

			//bind the gBuffer for lighting
			gBuffer->BindLighting();

			//bind the shadow buffer's depth
			m_shadowBuffer->BindDepthAsTexture(30);

			//bind the ramps
			m_diffuseRamp->Bind(9);
			m_specularRamp->Bind(10);
			m_shaders[TTN_Lights::DIRECTIONAL]->SetUniform("u_UseDiffuseRamp", int(m_useDiffuseRamp));
			m_shaders[TTN_Lights::DIRECTIONAL]->SetUniform("u_useSpecularRamp", int(m_useSpecularRamp));

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

	void TTN_IlluminationBuffer::SetViewMat(glm::mat4 view)
	{
		m_viewMat = view;
	}

	void TTN_IlluminationBuffer::SetLightSpaceMatrices(glm::mat4 mats[])
	{
		m_lightSpaceViewProj[0] = mats[0];
		m_lightSpaceViewProj[1] = mats[1];
		m_lightSpaceViewProj[2] = mats[2];
		m_lightSpaceViewProj[3] = mats[3];
	}

	void TTN_IlluminationBuffer::SetSplitRanges(float splits[])
	{
		m_splitRanges[0] = splits[0];
		m_splitRanges[1] = splits[1];
		m_splitRanges[2] = splits[2];
		m_splitRanges[3] = splits[3];
	}

	void TTN_IlluminationBuffer::SetFarClip(float farClip)
	{
		m_farClip = farClip;
	}

	void TTN_IlluminationBuffer::SetCamPos(glm::vec3 camPos)
	{
		m_camPos = camPos;
	}

	void TTN_IlluminationBuffer::SetShadowBuffer(TTN_Framebuffer::sfboptr shadowBuffer)
	{
		m_shadowBuffer = shadowBuffer;
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