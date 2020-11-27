//Titan Engine, by Atlas X Games 
// Mesh.h - header for the class that represents 3D meshes
#pragma once

//include the opengl wrap around classes
#include "VertexArrayObject.h"
//include glm features
#include "GLM/glm.hpp"
//import other required features
#include <vector>

namespace Titan {

	//class representing 3D meshes 
	class TTN_Mesh {
	public:
		//defines a special easier to use name for shared(smart) pointers to the class 
		typedef std::shared_ptr<TTN_Mesh> smptr;

		//creates and returns a shared(smart) pointer to the class 
		static inline smptr Create() {
			return std::make_shared<TTN_Mesh>();
		}
	public:
		//ensuring moving and copying is not allowed so we can control destructor calls through pointers
		TTN_Mesh(const TTN_Mesh& other) = delete;
		TTN_Mesh(TTN_Mesh& other) = delete;
		TTN_Mesh& operator=(const TTN_Mesh& other) = delete;
		TTN_Mesh& operator=(TTN_Mesh&& other) = delete;

	public:
		//constructor
		TTN_Mesh();

		//destructor
		~TTN_Mesh();

		//sets up the VAO for the mesh so it can acutally be rendered, called by the user (as they may change details of the mesh)
		void SetUpVao(int currentFrame = 0, int nextFrame = 0);

		//SETTERS 
		//sets the list of uvs for the mesh
		void SetUVs(std::vector<glm::vec2>& uvs);
		//sets the vertex colors of the mesh, returns wheter or not they were set succesfully
		bool SetColors(std::vector<glm::vec3>& colors);

		//Adders
		//adds a new set of verts to the class and creates a new vbo for them
		void AddVertices(std::vector<glm::vec3>& verts);
		//adds a new set of normals to the class and creates a new vbo for them
		void AddNormals(std::vector<glm::vec3>& norms);

		//GETTERS
		//Gets the pointer to the meshes vao
		TTN_VertexArrayObject::svaptr GetVAOPointer();
		//Gets the number of the vertices in the mesh
		int GetVertCount() { return m_Vertices[0].size(); }
		//Gets wheter or not the mesh has vertex colors
		bool GetHasVertColors() { return m_HasVertColors; }

	protected:
		//a vector containing all the vertices on the mesh 
		std::vector<std::vector<glm::vec3>> m_Vertices;
		//a vector containing all the normals of the mesh
		std::vector<std::vector<glm::vec3>> m_Normals;
		//a vector containing all the uvs of a mesh
		std::vector<glm::vec2> m_Uvs;
		//a vector containing all the vertex colors
		std::vector<glm::vec3> m_Colors;
		//a boolean for if the mesh has colors
		bool m_HasVertColors;

		//vbo smart pointers
		std::vector<TTN_VertexBuffer::svbptr> m_vertVbos;
		std::vector<TTN_VertexBuffer::svbptr> m_normVbos;
		TTN_VertexBuffer::svbptr m_UVsVbo;
		TTN_VertexBuffer::svbptr m_ColVbo;
		//smart pointer with the VAO for the mesh 
		TTN_VertexArrayObject::svaptr m_vao;
	};
}