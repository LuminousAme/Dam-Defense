//Titan Engine, by Atlas X Games 
// Renderer2D.h - header for the class that allows 2D sprites to be rendered 
#pragma once

//include the texture2D, shader, and VAO classes
#include "Shader.h"
#include "Texture2D.h"
#include "VertexArrayObject.h"

namespace Titan {
	//class for rendering 2D sprites
	class TTN_Renderer2D {
	public:
		//default constructor
		TTN_Renderer2D();

		//constructor with data
		TTN_Renderer2D(TTN_Texture2D::st2dptr sprite, int renderOrderLayer = 0);

		//destructor
		~TTN_Renderer2D() = default;

		//copy, move, and assingment constrcutors for ENTT
		TTN_Renderer2D(const TTN_Renderer2D&) = default;
		TTN_Renderer2D(TTN_Renderer2D&&) = default;
		TTN_Renderer2D& operator=(TTN_Renderer2D&) = default;

		//setters
		void SetSprite(TTN_Texture2D::st2dptr sprite) { m_sprite = sprite; };
		void SetRenderOrderLayer(int renderOrderLayer) { m_RenderLayer = renderOrderLayer; };

		//getters
		TTN_Texture2D::st2dptr GetSprite() { return m_sprite; };
		int GetRenderOrderLayer() { return m_RenderLayer; }

		//renders the sprite
		void Render(glm::mat4 model, glm::mat4 VP);

		//set up for rendering (called on engine side)
		static void InitRenderer2D();

		//gets the texture
		const TTN_Texture2D::st2dptr GetTexture() const { return m_sprite; }
		//gets the render layer
		const int GetRenderLayer() const { return m_RenderLayer; }

	private:
		//the texture for the sprite being rendered 
		TTN_Texture2D::st2dptr m_sprite;

		//the render layer, to help control the order things should render
		int m_RenderLayer;

		//the shader program used to render sprites
		inline static TTN_Shader::sshptr s_shader = nullptr;

		//The vbos and vaos for sprites
		inline static TTN_VertexBuffer::svbptr s_vertPos = nullptr;
		inline static TTN_VertexBuffer::svbptr s_Uvs = nullptr;
		inline static TTN_VertexArrayObject::svaptr s_vao = nullptr;
	};
}