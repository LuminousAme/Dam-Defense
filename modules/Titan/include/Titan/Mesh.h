//Titan Engine, by Atlas X Games 
// Mesh.h - header for the class that represents 3D meshes
#pragma once

//include the opengl wrap around classes
#include "Shader.h"
#include "VertexArrayObject.h"
//include glm features
#include "GLM/glm.hpp"
//import other required features
#include <vector>

namespace Titan {

	//class representing 3D meshes 
	class TTN_Mesh {
	public:
		//constructor
		TTN_Mesh();

		//constructor with data built in 
		TTN_Mesh(std::vector<glm::vec3>& verts, std::vector<glm::vec3>& norms, std::vector<glm::vec2>& uvs);

		//destructor
		~TTN_Mesh();

		//SETTERS 
		//sets the vertices of the mesh and loads them into a position VBO
		void setVertices(std::vector<glm::vec3>& verts);
		//sets the normals of the mesh and loads them into a normal VBO
		void setNormals(std::vector<glm::vec3>& norms);
		//sets the UVs of the mesh and loads them into a uv VBO
		void setUVs(std::vector<glm::vec2>& uvs);

		//GETTERS
		//Gets the pointer to a VBO
		TTN_VertexBuffer::svbptr GetVBOPointer(int location);

	protected:
		//a vector containing all the vertices on the mesh 
		std::vector<glm::vec3> m_Vertices;
		//a vector containing all the normals of the mesh
		std::vector<glm::vec3> m_Normals;
		//a vector containing all the uvs of a mesh
		std::vector<glm::vec2> m_Uvs;

		//array of VBO smart pointers, 0 for pos, 1 for normals, 2 for uvs. 
		TTN_VertexBuffer::svbptr m_vbos[3];

		//creates a VBO for the attribute we want 
		template<typename T>
		void CreateVBO(unsigned int attribute, GLint elementLenght, const std::vector<T>& data)
		{
			//ensure we acutally have data to send before continuing
			if (data.size() != 0) {
				//if it does have data, then load it into the approriate vbo
				m_vbos[attribute]->LoadData(data, elementLenght);
			}
		}
	};
}