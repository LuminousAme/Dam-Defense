//Titan Engine by Atlas X Games
//Framebuffer.cpp - source file for the class that represents framebuffer textures

//include titan's precompile header
#include "Titan/ttn_pch.h"
//include the class
#include "Titan/Framebuffer.h"

namespace Titan {
	//destcutor for the depth target
	TTN_DepthTarget::~TTN_DepthTarget()
	{
		//unloads the depth target
		Unload();
	}

	//unloads the depth target
	void TTN_DepthTarget::Unload()
	{
		//Deletes the texture at the given handle
		glDeleteTextures(1, &m_texture->GetHandle());
	}

	//destructor for the color targets
	TTN_ColorTarget::~TTN_ColorTarget()
	{
		//unloads the color targets
		Unload();
	}

	//unloads all the color targets
	void TTN_ColorTarget::Unload()
	{
		//delete all the color target textures
		glDeleteTextures(m_numAttachments, &m_textures[0]->GetHandle());
	}

	//constructor for framebuffer
	TTN_Framebuffer::TTN_Framebuffer()
	{
	}

	//destructor for framebuffer
	TTN_Framebuffer::~TTN_Framebuffer()
	{
		//unloads the framebuffer
		Unload();
	}

	//unloads the framebuffer
	void TTN_Framebuffer::Unload()
	{
		//Deletes the framebuffer
		glDeleteFramebuffers(1, &m_FBO);
		//Sets init to false
		_isInit = false;
	}

	//Inits the framebuffer with a given size
	void TTN_Framebuffer::Init(unsigned width, unsigned height)
	{
		//Sets the size to width and height
		SetSize(width, height);

		//Inits framebuffer
		Init();
	}

	void TTN_Framebuffer::Init()
	{
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
			glBindTexture(GL_TEXTURE_2D, m_depth.m_texture->GetHandle());
			//Sets the texture data
			glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, m_width, m_height);

			//Set texture parameters
			glTextureParameteri(m_depth.m_texture->GetHandle(), GL_TEXTURE_MIN_FILTER, m_filter);
			glTextureParameteri(m_depth.m_texture->GetHandle(), GL_TEXTURE_MAG_FILTER, m_filter);
			glTextureParameteri(m_depth.m_texture->GetHandle(), GL_TEXTURE_WRAP_S, m_wrap);
			glTextureParameteri(m_depth.m_texture->GetHandle(), GL_TEXTURE_WRAP_T, m_wrap);

			//Sets up as a framebuffer texture
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth.m_texture->GetHandle(), 0);

			glBindTexture(GL_TEXTURE_2D, GL_NONE);
		}

		//If there is more than zero color attachments
			//We create them
		if (m_color.m_numAttachments)
		{
			//Because we have a color target we include a color buffer bit into clear flag
			m_clearFlag |= GL_COLOR_BUFFER_BIT;
			//Creates the GLuints to hold the new texture handles;
			GLuint* textureHandles = new GLuint[m_color.m_numAttachments];

			glGenTextures(m_color.m_numAttachments, textureHandles);

			//Loops through them
			for (unsigned i = 0; i < m_color.m_numAttachments; i++)
			{
				//create the texture pointers
				m_color.m_textures[i] = TTN_Texture2D::CreateEmpty();

				//set the handle
				m_color.m_textures[i]->GetHandle() = textureHandles[i];

				//Binds the texture
				glBindTexture(GL_TEXTURE_2D, m_color.m_textures[i]->GetHandle());
				//Sets the texture storage
				glTexStorage2D(GL_TEXTURE_2D, 1, m_color.m_formats[i], m_width, m_height);

				//Set texture parameters
				glTextureParameteri(m_color.m_textures[i]->GetHandle(), GL_TEXTURE_MIN_FILTER, m_filter);
				glTextureParameteri(m_color.m_textures[i]->GetHandle(), GL_TEXTURE_MAG_FILTER, m_filter);
				glTextureParameteri(m_color.m_textures[i]->GetHandle(), GL_TEXTURE_WRAP_S, m_wrap);
				glTextureParameteri(m_color.m_textures[i]->GetHandle(), GL_TEXTURE_WRAP_T, m_wrap);

				//Sets up as a framebuffer texture
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_color.m_textures[i]->GetHandle(), 0);
			}

			delete[] textureHandles;
		}

		//Make sure it's set up right
		CheckFBO();
		//Unbind buffer
		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
		//Set init to true
		_isInit = true;
	}

	//adds a depth target to the framebuffer
	void TTN_Framebuffer::AddDepthTarget()
	{
		//if there is a handle already, unload it
		if (m_depth.m_texture != nullptr && m_depth.m_texture->GetHandle())
		{
			m_depth.Unload();
		}
		//Make depth active true
		m_depthActive = true;
	}

	//adds a color target to the framebuffer
	void TTN_Framebuffer::AddColorTarget(GLenum format)
	{
		//Resizes the textures to number of attachments
		m_color.m_textures.resize(m_color.m_numAttachments + 1);
		//Add the format
		m_color.m_formats.push_back(format);
		//Add the color attachment buffer number
		m_color.m_buffers.push_back(GL_COLOR_ATTACHMENT0 + m_color.m_numAttachments);
		//Incremenets number of attachments
		m_color.m_numAttachments++;
	}

	//binds the texture in the framebuffer's depth buffer
	void TTN_Framebuffer::BindDepthAsTexture(int textureSlot) const
	{
		m_depth.m_texture->Bind(textureSlot);
	}

	//binds a given color buffer in the framebuffer as a texture
	void TTN_Framebuffer::BindColorAsTexture(unsigned colorBuffer, int textureSlot) const
	{
		m_color.m_textures[colorBuffer]->Bind(colorBuffer);
	}

	//unbinds a texutre in a given texture slot
	void TTN_Framebuffer::UnbindTexture(int textureSlot) const
	{
		//Binds texture to gl_none 
		glActiveTexture(GL_TEXTURE0 + textureSlot);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
	}

	//dynamically change the shape of the framebuffer and recreate it
	void TTN_Framebuffer::Reshape(unsigned width, unsigned height)
	{
		//Set size
		SetSize(width, height);
		//Unloads the framebuffer
		Unload();
		//Unload the depth target
		m_depth.Unload();
		//Unloads the color target
		m_color.Unload();
		//Inits the framebuffer
		Init();
	}

	//sets the side of the framebuffer
	void TTN_Framebuffer::SetSize(unsigned width, unsigned height)
	{
		//Sets the width and height
		m_width = width;
		m_height = height;
	}

	//sets the viewport to be fullscreen
	void TTN_Framebuffer::SetViewport() const
	{
		glViewport(0, 0, m_width, m_height);
	}

	//binds this current framebuffer
	void TTN_Framebuffer::Bind() const
	{
		//bind the buffer
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

		//add all the color buffers
		if (m_color.m_numAttachments)
		{
			glDrawBuffers(m_color.m_numAttachments, &m_color.m_buffers[0]);
		}
	}

	//unbinds the current framebuffer
	void TTN_Framebuffer::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
	}

	//Render the framebuffer to a full screen quad
	void TTN_Framebuffer::RenderToFSQ() const
	{
		//Sets viewport
		SetViewport();
		//Bind the framebuffer
		Bind();
		//Draw full screen quad
		DrawFullScreenQuad();
		//Unbind the framebuffer
		Unbind();
	}

	//draws to the backbuffer 
	void TTN_Framebuffer::DrawToBackbuffer()
	{
		//bind the read and draw framebuffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GL_NONE);

		//Blits the framebuffer to the back buffer
		glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, GL_NONE);
	}

	//clear the framebuffer
	void TTN_Framebuffer::Clear()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
		glClear(m_clearFlag);
		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
	}

	//check to make sure the framebuffer is set up properlly 
	bool TTN_Framebuffer::CheckFBO()
	{
		//Binds the framebuffer
		Bind();
		//Check the framebuffer status
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			printf("Framebuffer is not vibing\n");
			return false;
		}
		return true;
	}

	//prepare the fullscreen quad's data so it can be rendered
	void TTN_Framebuffer::InitFullScreenQuad()
	{
		//A vbo with Uvs and verts from
	//-1 to 1 for verts
	//0 to 1 for UVs
		float VBO_DATA[]
		{
			-1.f, -1.f, 0.f,
			1.f, -1.f, 0.f,
			-1.f, 1.f, 0.f,

			1.f, 1.f, 0.f,
			-1.f, 1.f, 0.f,
			1.f, -1.f, 0.f,

			0.f, 0.f,
			1.f, 0.f,
			0.f, 1.f,

			1.f, 1.f,
			0.f, 1.f,
			1.f, 0.f
		};
		//Vertex size is 6pts * 3 data points * sizeof (float)
		int vertexSize = 6 * 3 * sizeof(float);
		//texcoord size = 6pts * 2 data points * sizeof(float)
		int texCoordSize = 6 * 2 * sizeof(float);

		//Generates vertex array
		glGenVertexArrays(1, &m_fullscreenQuadVAO);
		//Binds VAO
		glBindVertexArray(m_fullscreenQuadVAO);

		//Enables 2 vertex attrib array slots
		glEnableVertexAttribArray(0); //Vertices
		glEnableVertexAttribArray(1); //UVS

		//Generates VBO
		glGenBuffers(1, &m_fullscreenQuadVBO);

		//Binds the VBO
		glBindBuffer(GL_ARRAY_BUFFER, m_fullscreenQuadVBO);
		//Buffers the vbo data
		glBufferData(GL_ARRAY_BUFFER, vertexSize + texCoordSize, VBO_DATA, GL_STATIC_DRAW);

#pragma warning(push)
#pragma warning(disable : 4312)
		//Sets first attrib array to point to the beginning of the data
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));
		//Sets the second attrib array to point to an offset in the data
		glVertexAttribPointer((GLuint)1, 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(vertexSize));
#pragma warning(pop)

		glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
		glBindVertexArray(GL_NONE);

		m_isInitFSQ = true;
	}

	//renders the fullscreen quad
	void TTN_Framebuffer::DrawFullScreenQuad()
	{
		glBindVertexArray(m_fullscreenQuadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(GL_NONE);
	}
}