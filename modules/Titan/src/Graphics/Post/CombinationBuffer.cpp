//Titan Engine by Atlas X Games
//CombinationBuffer.cpp - source for the class for the post processing effect that combines a framebuffer into this framebuffer

//include precompiled header
#include "Titan/ttn_pch.h"

//include the class
#include "Titan/Graphics/Post/CombinationBuffer.h"

namespace Titan {
	//init this effect
	void TTN_CombineFrameBuffer::Init(unsigned width, unsigned height)
	{
		//Set up framebuffers
		//creates a new framebuffer with a basic color and depth target to write save the result too 
		int index = (int)m_buffers.size();
		m_buffers.push_back(TTN_Framebuffer::Create());
		m_buffers[index]->AddColorTarget(GL_RGBA8);
		m_buffers[index]->AddDepthTarget();
		//initliaze the framebuffer
		m_buffers[index]->Init(width, height);
		index++;

		//creates another new framebuffer with a basic color and depth target to write and save the results too
		m_buffers.push_back(TTN_Framebuffer::Create());
		m_buffers[index]->AddColorTarget(GL_RGBA8);
		m_buffers[index]->AddDepthTarget();
		//initliaze the framebuffer
		m_buffers[index]->Init(width, height);


		//make a shader program we don't use, just writting it so we can control the number of framebuffers in here
		m_shaders.push_back(TTN_Shader::Create());

		//init the original post effect
		TTN_PostEffect::Init(width, height);
	}

	//applies this effect
	void TTN_CombineFrameBuffer::ApplyEffect(TTN_PostEffect::spostptr buffer)
	{
		//binds the shader
		BindShader(m_shaders.size() - 1);

		//draw the conents of the first framebuffer to the second framebuffer
		//binds the color
		BindColorAsTexture(0,0,0);
		//render to the second framebuffer
		m_buffers[1]->RenderToFSQ();
		//unbind the texutre
		UnbindTexture(0);

		//draw the conents of the previous effect into the first buffer
		//binds the color
		buffer->BindColorAsTexture(0, 0, 0);
		//render to the first framebuffer
		m_buffers[0]->RenderToFSQ();
		//unbind the texutre
		buffer->UnbindTexture(0);

		//draw the contents of the second buffer into the first buffer
		//binds the color
		BindColorAsTexture(1, 0, 0);
		//render to the second framebuffer
		m_buffers[0]->RenderToFSQ();
		//unbind the texutre
		UnbindTexture(0);

		//unbind the shader
		UnbindShader();

		//congrats, you've now just made the contents of the previous buffer the background for this buffer
		//without losing the contents of this buffer *yay*
	}
}