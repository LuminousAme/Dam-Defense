//Titan Engine, by Atlas X Games 
// Renderer.h - header for the class that allows meshes to be rendered 
#pragma once

//include the mesh, shader, and material classes
#include "Titan/Graphics/Mesh.h"
#include "Titan/Graphics/Shader.h"
#include "Titan/Graphics/Material.h"

namespace Titan {
	//class that acts as a component to allow objects to be rendered in the game 
	class TTN_Renderer {
	public:
		//constructor that sets the mesh
		TTN_Renderer(TTN_Mesh::smptr mesh);
		//constructor that sets the mesh and the shader
		TTN_Renderer(TTN_Mesh::smptr mesh, TTN_Shader::sshptr shader, TTN_Material::smatptr material = nullptr, int Renderlayer = 0);
		//default constructor
		TTN_Renderer();

		//init shadows function
		static void InitShadowRendering();

		//destructor
		~TTN_Renderer();

		//copy, move, and assingment constrcutors for ENTT
		TTN_Renderer(const TTN_Renderer&) = default;
		TTN_Renderer(TTN_Renderer&&) = default;
		TTN_Renderer& operator=(TTN_Renderer&) = default;

		//binds the shadow depth shader
		static void BindShadowShader() { s_simpleShadowShader->Bind(); }
		//unbinds the shadow depth shader
		static void UnBindShadowShader() { s_simpleShadowShader->UnBind(); }

		//sets a new mesh
		void SetMesh(TTN_Mesh::smptr mesh);
		//sets a shader
		void SetShader(TTN_Shader::sshptr shader);
		//sets a materail
		void SetMat(TTN_Material::smatptr mat);
		//sets the renderlayer
		void SetRenderLayer(int renderLayer);
		//set csat shadows
		void SetCastShadows(bool castShadows) { m_castShadows = castShadows; }

		//gets the mesh
		const TTN_Mesh::smptr GetMesh() const { return m_mesh; }
		//gets the shader
		const TTN_Shader::sshptr GetShader() const { return m_Shader;  }
		//gets the material 
		const TTN_Material::smatptr GetMat() const { return m_Mat; }
		//gets the render layer
		const int GetRenderLayer() const { return m_RenderLayer; }
		//get should cast shadows or not
		const int GetCastShadows() const { return m_castShadows; }
		//gets if shadows are init for rendering or not
		static bool GetShadowsAreInit() { return s_shaderRenderingIsInit; }

		void Render(glm::mat4 model, glm::mat4 VP, glm::mat4 lightSpaceMat);

	private:
		//a pointer to the shader that should be used to render this object
		TTN_Shader::sshptr m_Shader;
		//a pointer to the mesh that this should render
		TTN_Mesh::smptr m_mesh;
		//a pointer to the material that should be used on the mesh
		TTN_Material::smatptr m_Mat;
		//the render layer, to help control the order things should render
		int m_RenderLayer;
		//should cast shadows or not
		bool m_castShadows = false;

		inline static TTN_Shader::sshptr s_simpleShadowShader = nullptr;
		inline static bool s_shaderRenderingIsInit = false;
	};
}