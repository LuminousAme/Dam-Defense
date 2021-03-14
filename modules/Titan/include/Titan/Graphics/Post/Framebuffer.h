//Titan Engine by Atlas X Games
//Framebuffer.h - Header for the class that represents framebuffer textures
#pragma once

//include the precompile header
#include "Titan/ttn_pch.h"
//include other titan graphics features
#include "Titan/Graphics/Texture2D.h"
#include "Titan/Graphics/Shader.h"

namespace Titan {
	//depth target
	struct TTN_DepthTarget {
		//Destructor, unloads the texture
		~TTN_DepthTarget();
		//deletes the texture of the depth targets
		void Unload();
		//holds the depth target texture
		TTN_Texture2D::st2dptr m_texture;
	};

	//color target
	struct TTN_ColorTarget {
		//Destructor, unloads all the color targets
		~TTN_ColorTarget();
		//deletes the texture of the depth targets
		void Unload();
		//holds the color texture
		std::vector<TTN_Texture2D::st2dptr> m_textures;
		std::vector<GLenum> m_formats;
		std::vector<GLenum> m_buffers;
		//stores the number of color attachments this target has
		unsigned int m_numAttachments = 0;
	};

	//framebuffer class
	class TTN_Framebuffer {
	public:
		//defines a special easier to use name for shared(smart) pointers to the class
		typedef std::shared_ptr<TTN_Framebuffer> sfboptr;

		//creates and returns a shared(smart) pointer to the class
		static inline sfboptr Create() {
			return std::make_shared<TTN_Framebuffer>();
		}

	public:
		//default constructors and destructors
		TTN_Framebuffer();
		~TTN_Framebuffer();

		//unloads the framebuffer
		void Unload();

		//Init the framebuffer with a given size
		virtual void Init(unsigned width, unsigned height);

		//Inits the framebuffer
		void Init();

		//adds the depth target (we can only ever have 1 depth target)
		void AddDepthTarget();

		//adds color target (max number of color targets is gpu demendent)
		void AddColorTarget(GLenum format);

		//Binds depth buffer as a texture to a specified slot
		void BindDepthAsTexture(int textureSlot) const;
		//Binds a color buffer as a texture to a specified slot
		void BindColorAsTexture(unsigned colorBuffer, int textureSlot) const;
		//unbinds a texture
		void UnbindTexture(int textureSlot) const;

		//reshapes the framebuffer
		void Reshape(unsigned width, unsigned height);
		//sets the size of the frame
		void SetSize(unsigned width, unsigned height);

		//sets the viewport to fullscreen (using size of the framebuffer)
		void SetViewport() const;

		//binds the framebuffer
		void Bind() const;
		//unbinds the framebuffer
		void Unbind() const;

		//renders the framebuffer to a fullscreenquad
		void RenderToFSQ() const;

		//Draws the contents of the framebuffer to the back buffer
		void DrawToBackbuffer();

		//Clears the framebuffer
		void Clear();
		//checks if the framebuffer is okay
		bool CheckFBO();

		//initliazaes fullscreen quad
		//creates a VAO for a full screen quad, covering from -1 to 1
		static void InitFullScreenQuad();
		//Draws our fullscreen quad
		static void DrawFullScreenQuad();

		//Initial width and height is zero
		unsigned int m_width = 0;
		unsigned int m_height = 0;

	protected:
		//OpenGL framebuffer handle
		GLuint m_FBO;
		//Depth attachment, either one or none
		TTN_DepthTarget m_depth;
		//Color attachment, none, one, or more
		TTN_ColorTarget m_color;

		//Default filter is GL_NEAREST
		GLenum m_filter = GL_NEAREST;
		//Default filter is GL_CLAMP_TO_EDGE
		GLenum m_wrap = GL_CLAMP_TO_EDGE;

		//Clearflag is nothing by default
		GLbitfield m_clearFlag = 0;

		//is the framebuffer initialized
		bool _isInit = false;
		//depth attachment
		bool m_depthActive = false;

		//Full screen quad VBO handle
		inline static GLuint m_fullscreenQuadVBO = 0;
		//Full screen VAO handle
		inline static GLuint m_fullscreenQuadVAO = 0;

		//the maximum amount of color attachments
		static int m_maxColorAttachments;
		//is the fullscreen quad initliazed
		inline static bool m_isInitFSQ = false;
	};
}