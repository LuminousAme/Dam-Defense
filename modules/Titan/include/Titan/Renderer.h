//Titan Engine, by Atlas X Games 
// Renderer.h - header for the class that allows meshes, 2D sprites, etc. be rendered 
#pragma once

//include the mesh and shader classes
#include "Mesh.h"
#include "Shader.h"

namespace Titan {

	//class that acts as a component to allow objects to be rendered in the game 
	class TTN_Renderer {
	public:
		//enum class to be explcit wheter something is a 3D mesh, a 2D sprite, etc.
		enum class RenderObjectType {
			SPRITE = 1, 
			MESH
		};

		//constructor that sets the mesh
		TTN_Renderer(TTN_Mesh mesh);

		//destructor
		~TTN_Renderer();

		//move and assingment constrcutors for ENTT
		TTN_Renderer(TTN_Renderer&& renderer) = default;

		TTN_Renderer& operator=(TTN_Renderer&& renderer) = default;

		//sets a new mesh
		void SetMesh(TTN_Mesh& mesh);
		void Render(glm::mat4 model, glm::mat4 proj, glm::mat4 view);

	protected:
		//a pointer to the shader that should be used to render this object
		TTN_Shader* m_Shader;
		//a smart pointer to the vao for the mesh
		TTN_VertexArrayObject::svaptr m_Vao;
	};
}