//include titan's precompile header
#include "Titan/ttn_pch.h"
#include "Titan/Graphics/GBuffer.h"

namespace Titan {
	void TTN_GBuffer::Init(unsigned width, unsigned height)
	{
		//store height and width of window
		m_windowWidth = width;
		m_windowHeight = height;

		//adds color targets to g buffer
		m_gBuffer.AddColorTarget(GL_RGBA8); //albedo buffer, needs all channels
		m_gBuffer.AddColorTarget(GL_RGB8); //normals buffer, does not need alpha
		m_gBuffer.AddColorTarget(GL_RGB8); //specular buffer, technivcally only needs 1 channel


		//obtain positional data using depth buffer.
		//here we use position buffer
		m_gBuffer.AddColorTarget(GL_RGB32F);

		m_gBuffer.AddColorTarget(GL_RGB8); //emissive buffer

		//add a depth buffer
		m_gBuffer.AddDepthTarget();

		//init framebufer
		m_gBuffer.Init(width, height);

		//init passthrough shader
		m_passThrough = TTN_Shader::Create();
		m_passThrough->LoadShaderStageFromFile("shaders/Post/ttn_passthrough_vert.glsl", GL_VERTEX_SHADER);
		m_passThrough->LoadShaderStageFromFile("shaders/Post/ttn_passthrough_frag.glsl", GL_FRAGMENT_SHADER);
		m_passThrough->Link();
	}

	void TTN_GBuffer::Bind()
	{
		m_gBuffer.Bind();
	}

	void TTN_GBuffer::BindLighting()
	{
		m_gBuffer.BindColorAsTexture((unsigned int)TTN_Target::ALBEDO, 0);
		m_gBuffer.BindColorAsTexture((unsigned int)TTN_Target::NORMAL, 1);
		m_gBuffer.BindColorAsTexture((unsigned int)TTN_Target::SPECULAR, 2);
		m_gBuffer.BindColorAsTexture((unsigned int)TTN_Target::POSITION, 3);
		m_gBuffer.BindColorAsTexture((unsigned int)TTN_Target::EMISSIVE, 4);
		m_gBuffer.BindDepthAsTexture(15);
	}

	void TTN_GBuffer::Clear()
	{
		m_gBuffer.Clear();
	}

	void TTN_GBuffer::Unbind()
	{
		m_gBuffer.Unbind();
	}

	void TTN_GBuffer::UnbindLighting()
	{
		m_gBuffer.UnbindTexture(0);
		m_gBuffer.UnbindTexture(1);
		m_gBuffer.UnbindTexture(2);
		m_gBuffer.UnbindTexture(3);
	}

	void TTN_GBuffer::DrawBuffersToScreen()
	{
		//bind passthroguh shader
		m_passThrough->Bind();

		//set viewport to top left
		glViewport(0, m_windowHeight / 2.f, m_windowWidth / 2.f, m_windowHeight / 2.f);
		m_gBuffer.BindColorAsTexture((unsigned int)TTN_Target::ALBEDO, 0);
		m_gBuffer.DrawFullScreenQuad();
		m_gBuffer.UnbindTexture(0);

		//set viewport to top right
		glViewport(m_windowWidth / 2.f, m_windowHeight / 2.f, m_windowWidth / 2.f, m_windowHeight / 2.f);
		m_gBuffer.BindColorAsTexture((unsigned int)TTN_Target::NORMAL, 0);
		m_gBuffer.DrawFullScreenQuad();
		m_gBuffer.UnbindTexture(0);

		//set viewport to bottom left
		glViewport(0, 0, m_windowWidth / 2.f, m_windowHeight / 2.f);
		m_gBuffer.BindColorAsTexture((unsigned int)TTN_Target::SPECULAR, 0);
		m_gBuffer.DrawFullScreenQuad();
		m_gBuffer.UnbindTexture(0);

		//set viewport to bottom right
		glViewport(m_windowWidth / 2.f, 0, m_windowWidth / 2.f, m_windowHeight / 2.f);
		m_gBuffer.BindColorAsTexture((unsigned int)TTN_Target::POSITION, 0);
		m_gBuffer.DrawFullScreenQuad();
		m_gBuffer.UnbindTexture(0);

		//unbind passthorugh shader
		m_passThrough->UnBind();
	}

	void TTN_GBuffer::DrawPositionBuffer()
	{
		//bind passthroguh shader
		m_passThrough->Bind();

		//bind and draw the position buffer
		m_gBuffer.BindColorAsTexture((unsigned int)TTN_Target::POSITION, 0);
		m_gBuffer.DrawFullScreenQuad();
		//unbind the position buffer
		m_gBuffer.UnbindTexture(0);

		//unbind passthorugh shader
		m_passThrough->UnBind();
	}

	void TTN_GBuffer::DrawNormalBuffer()
	{
		//bind passthroguh shader
		m_passThrough->Bind();

		//bind and draw the normal buffer
		m_gBuffer.BindColorAsTexture((unsigned int)TTN_Target::NORMAL, 0);
		m_gBuffer.DrawFullScreenQuad();
		//unbind the normal buffer
		m_gBuffer.UnbindTexture(0);

		//unbind passthorugh shader
		m_passThrough->UnBind();
	}

	void TTN_GBuffer::DrawAlbedoBuffer()
	{
		//bind passthroguh shader
		m_passThrough->Bind();

		//bind and draw the abledo buffer
		m_gBuffer.BindColorAsTexture((unsigned int)TTN_Target::ALBEDO, 0);
		m_gBuffer.DrawFullScreenQuad();
		//unbind the abledo buffer
		m_gBuffer.UnbindTexture(0);

		//unbind passthorugh shader
		m_passThrough->UnBind();
	}

	void TTN_GBuffer::DrawEmissiveBuffer()
	{
		//bind passthroguh shader
		m_passThrough->Bind();

		//bind and draw the abledo buffer
		m_gBuffer.BindColorAsTexture((unsigned int)TTN_Target::EMISSIVE, 0);
		m_gBuffer.DrawFullScreenQuad();
		//unbind the abledo buffer
		m_gBuffer.UnbindTexture(0);

		//unbind passthorugh shader
		m_passThrough->UnBind();
	}

	void TTN_GBuffer::Reshape(unsigned width, unsigned height)
	{
		//store new width and height
		m_windowWidth = width;
		m_windowHeight = height;

		//reshape framebuffer
		m_gBuffer.Reshape(width, height);
	}
}