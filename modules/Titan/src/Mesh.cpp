//Titan Engine, by Atlas X Games 
// Mesh.cpp - source file for the class that represents 3D meshes

//include the header
#include "Titan/Mesh.h"

namespace Titan {
	//constructor, creates a mesh
	TTN_Mesh::TTN_Mesh()
	{
		//initliaze the array of VBO pointers
		for (int i = 0; i < sizeof(m_vbos); i++)
			m_vbos[i]->Create();
	}

	//Constructor, creates a mesh with the data given
	TTN_Mesh::TTN_Mesh(std::vector<glm::vec3>& verts, std::vector<glm::vec3>& norms, std::vector<glm::vec2>& uvs)
	{
		//initliaze the array of VBO pointers
		for (int i = 0; i < sizeof(m_vbos); i++)
			m_vbos[i]->Create();

		//set the vertices
		setVertices(verts);
		//set the normals
		setNormals(norms);
		//set the uvs
		setUVs(uvs);
	}

	//destructor
	TTN_Mesh::~TTN_Mesh()
	{
	}

	//sets the vertices and puts them in a position vbo
	void TTN_Mesh::setVertices(std::vector<glm::vec3>& verts)
	{
		//copy the vertices' postion
		m_Vertices = verts;
		//send them to a vbo
		CreateVBO(0, 3, verts);
	}

	void TTN_Mesh::setNormals(std::vector<glm::vec3>& norms)
	{
		//copy the normals
		m_Normals = norms;
		//send them to a vbo
		CreateVBO(1, 3, norms);
	}

	void TTN_Mesh::setUVs(std::vector<glm::vec2>& uvs)
	{
		//copy the uvs
		m_Uvs = uvs;
		//send them to a vbo
		CreateVBO(2, 2, uvs);
	}

	//gets the pointer to a VBO
	TTN_VertexBuffer::svbptr TTN_Mesh::GetVBOPointer(int location)
	{
		return m_vbos[location];
	}
}