//Titan Engine, by Atlas X Games 
// Renderer2D.h - header for the class that allows 2D sprites to be rendered 
#pragma once

//include the texture2D, shader, and VAO classes
#include "Titan/Graphics/Shader.h"
#include "Titan/Graphics/Texture2D.h"
#include "Titan/Graphics/VertexArrayObject.h"

namespace Titan {
	//class for rendering 2D sprites
	class TTN_Renderer2D {
	public:
		//default constructor
		TTN_Renderer2D();

		//constructor with data
		TTN_Renderer2D(TTN_Texture2D::st2dptr sprite, glm::vec4 color = glm::vec4(1.0f), int renderOrderLayer = 0);

		//destructor
		~TTN_Renderer2D() = default;

		//copy, move, and assingment constrcutors for ENTT
		TTN_Renderer2D(const TTN_Renderer2D&) = default;
		TTN_Renderer2D(TTN_Renderer2D&&) = default;
		TTN_Renderer2D& operator=(TTN_Renderer2D&) = default;

		//setters
		void SetSprite(TTN_Texture2D::st2dptr sprite) { m_sprite = sprite; }
		void SetColor(glm::vec4 color) { m_color = color; }
		void SetRenderOrderLayer(int renderOrderLayer) { m_RenderLayer = renderOrderLayer; }
		void SetVertMask(float maskPercentage) { m_veritcalMask = maskPercentage; }
		void SetHoriMask(float maskPercentage) { m_horizontalMask = maskPercentage; }
		void StartVertMaskAtBottom(bool startAtBottom) { m_startFromBottom = startAtBottom; }
		void StartHoriMaskAtRight(bool StartAtRight) { m_startFromRight = StartAtRight; }

		//getters
		TTN_Texture2D::st2dptr GetSprite() { return m_sprite; }
		glm::vec4 GetColor() { return m_color; }
		int GetRenderOrderLayer() { return m_RenderLayer; }
		float GetVertMask() { return m_veritcalMask; }
		float GetHoriMask() { return m_horizontalMask; }
		bool GetVertMaskStartsAtBottom() { return m_startFromBottom; }
		bool GetHoriMaskStartsAtRight() { return m_startFromRight; }

		//renders the sprite
		void Render(glm::mat4 model, glm::mat4 VP);

		//set up for rendering (called on engine side)
		static void InitRenderer2D();

		//binds the shader for rendering
		static void BindShader();

		//gets the texture
		const TTN_Texture2D::st2dptr GetTexture() const { return m_sprite; }
		//gets the render layer
		const int GetRenderLayer() const { return m_RenderLayer; }

	private:
		//the texture for the sprite being rendered 
		TTN_Texture2D::st2dptr m_sprite;

		//the color modifier for the sprite 
		glm::vec4 m_color;

		//the render layer, to help control the order things should render
		int m_RenderLayer;

		//masking controls
		float m_veritcalMask;
		bool m_startFromBottom;
		float m_horizontalMask;
		bool m_startFromRight;

		//the shader program used to render sprites
		inline static TTN_Shader::sshptr s_shader = nullptr;

		//The vbos and vaos for sprites
		inline static TTN_VertexBuffer::svbptr s_vertPos = nullptr;
		inline static TTN_VertexBuffer::svbptr s_Uvs = nullptr;
		inline static TTN_VertexArrayObject::svaptr s_vao = nullptr;
	};
}