//Titan Engine, by Atlas X Games 
// Mesh.cpp - source file for the class that represents 3D meshes

//precompile header
#include "Titan/ttn_pch.h"
//include the header
#include "Titan/Graphics/Mesh.h"

namespace Titan {
	//constructor, creates a mesh
	TTN_Mesh::TTN_Mesh()
	{
		//initliaze the vao
		m_vao->Create();

		//set the mesh to not having vertex colors
		m_HasVertColors = false;
	}

	//destructor
	TTN_Mesh::~TTN_Mesh()
	{
	}

	//sets up the VAO for the mesh so it can acutally be rendered, needs to be called by the user in case they change the mesh
	void TTN_Mesh::SetUpVao(int currentFrame, int nextFrame)
	{
		//if we don't have a vao, creates a new vao
		if (m_vao == nullptr)
			m_vao = TTN_VertexArrayObject::Create();
		//if we do have a vao, clear it's vertex buffers
		else
			m_vao->ClearVertexBuffers();

		//load the vbos from the mesh into the vao 
		m_vao->AddVertexBuffer(m_vertVbos[currentFrame], { BufferAttribute(0, 3, GL_FLOAT, false, sizeof(float) * 3, 0, AttribUsage::Position) });
		m_vao->AddVertexBuffer(m_normVbos[currentFrame], { BufferAttribute(1, 3, GL_FLOAT, false, sizeof(float) * 3, 0, AttribUsage::Normal) });
		m_vao->AddVertexBuffer(m_UVsVbo, { BufferAttribute(2, 2, GL_FLOAT, false, sizeof(float) * 2, 0, AttribUsage::Texture) });
		if (m_HasVertColors) m_vao->AddVertexBuffer(m_ColVbo, { BufferAttribute(3, 3, GL_FLOAT, false, sizeof(float) * 2, 0, AttribUsage::Color) });
		m_vao->AddVertexBuffer(m_vertVbos[nextFrame], {BufferAttribute(4, 3, GL_FLOAT, false, sizeof(float) * 3, 0, AttribUsage::Position) });
		m_vao->AddVertexBuffer(m_normVbos[nextFrame], { BufferAttribute(5, 3, GL_FLOAT, false, sizeof(float) * 3, 0, AttribUsage::Normal) });
		m_vao->AddVertexBuffer(m_tangVbos[currentFrame], { BufferAttribute(6, 3, GL_FLOAT, false, sizeof(float) * 3, 0, AttribUsage::Tagent) });
		m_vao->AddVertexBuffer(m_tangVbos[nextFrame], { BufferAttribute(7, 3, GL_FLOAT, false, sizeof(float) * 3, 0, AttribUsage::Tagent) });
	}

	void TTN_Mesh::SetUVs(std::vector<glm::vec2>& uvs)
	{
		//create a new vbo for the uvs
		m_UVsVbo = TTN_VertexBuffer::Create();

		//copy the list of uvs
		m_Uvs = uvs;

		//add the uvs to the vbo
		if (uvs.size() != 0) {
			m_UVsVbo->LoadData(uvs.data(), uvs.size());
		}
	}

	bool TTN_Mesh::SetColors(std::vector<glm::vec3>& colors)
	{
		//make sure the correct number of colors was entered
		if (colors.size() == m_Vertices[0].size())
		{
			//make the vbo pointer
			m_ColVbo = TTN_VertexBuffer::Create();

			//copy the colors
			m_Colors = colors;
			//set the mesh to have vertex colors (note, they still won't render if the shader is not set to render them)
			m_HasVertColors = true;
			//send them to a vbo
			if (colors.size() != 0) {
				m_ColVbo->LoadData(colors.data(), colors.size());
			}

			//return true as the colors were set successfully
			return true;
		}

		return false;
	}

	//adds a list of vertices to the mesh object
	void TTN_Mesh::AddVertices(std::vector<glm::vec3>& verts)
	{
		//create a new vbo pointer for it
		TTN_VertexBuffer::svbptr newVertVbo = TTN_VertexBuffer::Create();

		//copy the list of verts
		m_Vertices.push_back(verts);

		//add those verts to the new vbo
		if (verts.size() != 0) {
			newVertVbo->LoadData(verts.data(), verts.size());
		}

		//and add that vbo to the list of vert vbos
		m_vertVbos.push_back(newVertVbo);
	}
	
	//adds a list of normals to the mesh object
	void TTN_Mesh::AddNormals(std::vector<glm::vec3>& norms)
	{
		//create a new vbo pointer for it
		TTN_VertexBuffer::svbptr newNormVbo = TTN_VertexBuffer::Create();

		//copy the list of normals
		m_Normals.push_back(norms);

		//add those normals to the new vbo
		if (norms.size() != 0) {
			newNormVbo->LoadData(norms.data(), norms.size());
		}

		//and add that vbo to the list of vert vbos
		m_normVbos.push_back(newNormVbo);
	}

	void TTN_Mesh::AddTangents(std::vector<glm::vec3>& tangs)
	{
		//create a new vbo pointer for it
		TTN_VertexBuffer::svbptr newTangVBO = TTN_VertexBuffer::Create();

		//copy the list of normals
		m_tangents.push_back(tangs);

		//add those normals to the new vbo
		if (tangs.size() != 0) {	
			newTangVBO->LoadData(tangs.data(), tangs.size());
		}

		//and add that vbo to the list of vert vbos
		m_tangVbos.push_back(newTangVBO);
	}

	//gets the pointer to the meshes vao 
	TTN_VertexArrayObject::svaptr TTN_Mesh::GetVAOPointer()
	{
		return m_vao;
	}
}