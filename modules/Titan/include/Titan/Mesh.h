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

		//constructor with data built in (minus colors)
		TTN_Mesh(std::vector<glm::vec3>& verts, std::vector<glm::vec3>& norms, std::vector<glm::vec2>& uvs);

		//constructor with data built in (including colors)
		TTN_Mesh(std::vector<glm::vec3>& verts, std::vector<glm::vec3>& norms, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& colors);

		//destructor
		~TTN_Mesh();

		//sets up the VAO for the mesh so it can acutally be rendered, called by the user (as they may change details of the mesh)
		void SetUpVao();

		//SETTERS 
		//sets the vertices of the mesh and loads them into a position VBO
		void setVertices(std::vector<glm::vec3>& verts);
		//sets the normals of the mesh and loads them into a normal VBO
		void setNormals(std::vector<glm::vec3>& norms);
		//sets the UVs of the mesh and loads them into a uv VBO
		void setUVs(std::vector<glm::vec2>& uvs);
		//sets the vertex colors of the mesh, returns wheter or not they were set succesfully
		bool SetColors(std::vector<glm::vec3>& colors);

		//GETTERS
		//Gets the pointer to the meshes vao
		TTN_VertexArrayObject::svaptr GetVAOPointer();
		//Gets the number of the vertices in the mesh
		int GetVertCount() { return m_Vertices.size(); }
		//Gets wheter or not the mesh has vertex colors
		bool GetHasVertColors() { return m_HasVertColors; }

	protected:
		//a vector containing all the vertices on the mesh 
		std::vector<glm::vec3> m_Vertices;
		//a vector containing all the normals of the mesh
		std::vector<glm::vec3> m_Normals;
		//a vector containing all the uvs of a mesh
		std::vector<glm::vec2> m_Uvs;
		//a vector containing all the vertex colors
		std::vector<glm::vec3> m_Colors;
		//a boolean for if the mesh has colors
		bool m_HasVertColors;

		//array of VBO smart pointers, 0 for pos, 1 for normals, 2 for uvs, 3 for colors. 
		TTN_VertexBuffer::svbptr m_vbos[4];
		//smart pointer with the VAO for the mesh 
		TTN_VertexArrayObject::svaptr m_vao;

		//creates a VBO for the attribute we want 
		template<typename T>
		void CreateVBO(unsigned int attribute, GLint elementLenght, const std::vector<T>& data)
		{
			if (m_vbos[attribute] == nullptr) {
				m_vbos[attribute] = TTN_VertexBuffer::Create();
			}

			//ensure we acutally have data to send before continuing
			if (data.size() != 0) {
				//if it does have data, then load it into the approriate vbo
				m_vbos[attribute]->LoadData(data.data(), elementLenght);
			}
		}
	};
}