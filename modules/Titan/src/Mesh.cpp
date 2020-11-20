//Titan Engine, by Atlas X Games 
// Mesh.cpp - source file for the class that represents 3D meshes

//include the header
#include "Titan/Mesh.h"

namespace Titan {
	//constructor, creates a mesh
	TTN_Mesh::TTN_Mesh()
	{
		//initliaze the array of VBO pointers
		for (int i = 0; i < 4; i++)
			m_vbos[i]->Create();

		//initliaze the vao
		m_vao->Create();

		//set the mesh to not having vertex colors
		m_HasVertColors = false;
	}

	//Constructor, creates a mesh with the data given
	TTN_Mesh::TTN_Mesh(std::vector<glm::vec3>& verts, std::vector<glm::vec3>& norms, std::vector<glm::vec2>& uvs)
	{
		//initliaze the array of VBO pointers
		for (int i = 0; i < 3; i++)
			m_vbos[i]->Create();

		//initliaze the vao
		m_vao->Create();

		//set the vertices
		setVertices(verts);
		//set the normals
		setNormals(norms);
		//set the uvs
		setUVs(uvs);

		//set the mesh to not having vertex colors
		m_HasVertColors = false;
	}

	//Constructor, creates a mesh with the data given
	TTN_Mesh::TTN_Mesh(std::vector<glm::vec3>& verts, std::vector<glm::vec3>& norms, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& colors)
	{
		//initliaze the array of VBO pointers
		for (int i = 0; i < 3; i++)
			m_vbos[i]->Create();

		//initliaze the vao
		m_vao->Create();

		//set the vertices
		setVertices(verts);
		//set the normals
		setNormals(norms);
		//set the uvs
		setUVs(uvs);
		//set up the colors
		SetColors(colors);
	}

	//destructor
	TTN_Mesh::~TTN_Mesh()
	{
	}

	//sets up the VAO for the mesh so it can acutally be rendered, needs to be called by the user in case they change the mesh
	void TTN_Mesh::SetUpVao()
	{
		//creates a new vao
		m_vao = TTN_VertexArrayObject::Create();

		//load the vbos from the mesh into the vao 
		m_vao->AddVertexBuffer(m_vbos[0], { BufferAttribute(0, 3, GL_FLOAT, false, sizeof(float) * 3, 0, AttribUsage::Position) });
		m_vao->AddVertexBuffer(m_vbos[1], { BufferAttribute(1, 3, GL_FLOAT, false, sizeof(float) * 3, 0, AttribUsage::Normal) });
		m_vao->AddVertexBuffer(m_vbos[2], { BufferAttribute(2, 2, GL_FLOAT, false, sizeof(float) * 2, 0, AttribUsage::Texture) });
		if (m_HasVertColors) m_vao->AddVertexBuffer(m_vbos[3], { BufferAttribute(3, 3, GL_FLOAT, false, sizeof(float) * 2, 0, AttribUsage::Color) });
	}

	//sets the vertices and puts them in a position vbo
	void TTN_Mesh::setVertices(std::vector<glm::vec3>& verts)
	{
		//copy the vertices' postion
		m_Vertices = verts;
		//send them to a vbo
		CreateVBO(0, verts.size(), verts);
	}

	void TTN_Mesh::setNormals(std::vector<glm::vec3>& norms)
	{
		//copy the normals
		m_Normals = norms;
		//send them to a vbo
		CreateVBO(1, norms.size(), norms);
	}

	void TTN_Mesh::setUVs(std::vector<glm::vec2>& uvs)
	{
		//copy the uvs
		m_Uvs = uvs;
		//send them to a vbo
		CreateVBO(2, uvs.size(), uvs);
	}


	bool TTN_Mesh::SetColors(std::vector<glm::vec3>& colors)
	{
		//make sure the correct number of colors was entered
		if (colors.size() == m_Vertices.size())
		{
			//copy the colors
			m_Colors = colors;
			//set the mesh to have vertex colors (note, they still won't render if the shader is not set to render them)
			m_HasVertColors = true;
			//send them to a vbo
			CreateVBO(3, colors.size(), colors);
			//return true as the colors were set successfully
			return true;
		}

		return false;
	}

	//gets the pointer to the meshes vao 
	TTN_VertexArrayObject::svaptr TTN_Mesh::GetVAOPointer()
	{
		return m_vao;
	}
}