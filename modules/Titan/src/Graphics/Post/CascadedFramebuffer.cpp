//Titan Engine by Atlas X Games
//CascadedFrameBuffer.cpp - source file for the class that represents framebuffer textures with a 3D texture array depth target

//include titan's precompile header
#include "Titan/ttn_pch.h"
//include the class
#include "Titan/Graphics/Post/CascadedFrameBuffer.h"

namespace Titan {
	//default constructor
	TTN_CascadedFrameBuffer::TTN_CascadedFrameBuffer()
		: TTN_Framebuffer()
	{
	}

	//initlizaes the framebuffer
	void TTN_CascadedFrameBuffer::Init(unsigned width, unsigned height)
	{
		//set the size
		SetSize(width, height);

		//if the fullscreen quad has not been initliazed, initliaze it 
		if (!m_isInitFSQ)
			InitFullScreenQuad();

		//Generates the FBO
		glGenFramebuffers(1, &m_FBO);
		//Bind it
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

		if (m_depthActive)
		{
			//because we have depth we need to clear our depth bit
			m_clearFlag |= GL_DEPTH_BUFFER_BIT;

			//create the pointer for the texture
			m_depth.m_texture = TTN_Texture2D::CreateEmpty();

			//Generate the texture
			glGenTextures(1, &m_depth.m_texture->GetHandle());
			//Binds the texture
			glBindTexture(GL_TEXTURE_2D_ARRAY, m_depth.m_texture->GetHandle());

			//create the 3D depth texture and sets it's data
			glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT32F, m_width, m_height, 4);

			//Set texture parameters
			glTextureParameteri(m_depth.m_texture->GetHandle(), GL_TEXTURE_MIN_FILTER, m_filter);
			glTextureParameteri(m_depth.m_texture->GetHandle(), GL_TEXTURE_MAG_FILTER, m_filter);
			glTextureParameteri(m_depth.m_texture->GetHandle(), GL_TEXTURE_WRAP_S, m_wrap);
			glTextureParameteri(m_depth.m_texture->GetHandle(), GL_TEXTURE_WRAP_T, m_wrap);

			//Sets up as a framebuffer texture
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depth.m_texture->GetHandle(), 0);

			glDrawBuffer(GL_NONE);

			//unbinds the depth attachment
			glBindTexture(GL_TEXTURE_2D_ARRAY, GL_NONE);
		}

		//Make sure it's set up right
		CheckFBO();
		//Unbind buffer
		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
		//Set init to true
		_isInit = true;
	}
}