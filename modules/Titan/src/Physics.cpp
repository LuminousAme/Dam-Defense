//Titan Engine, by Atlas X Games 
// Physics.cpp - source file for the class that represents physics bodies

//include the header
#include "Titan/Physics.h"
//include other required features
#include "Titan/ObjLoader.h"
#include <iostream>

namespace Titan {

	//initlization for static rendering variables
	TTN_Shader::sshptr TTN_Physics::shader = nullptr;
	TTN_Texture::stptr TTN_Physics::texture = nullptr;
	TTN_Material::smatptr TTN_Physics::mat = nullptr;
	TTN_Mesh::smptr TTN_Physics::mesh = nullptr;
	bool TTN_Physics::renderingSetUp = false;

	//default constructor, constructs a basic 1x1x1 physics body around the origin
	TTN_Physics::TTN_Physics()
	{
		m_trans = TTN_Transform();
		m_trans.SetPos(glm::vec3(0.0f));
		m_trans.SetScale(glm::vec3(1.0f));

		m_Center = glm::vec3(0.0f, 0.0f, 0.f);
		m_Min = glm::vec3(-1.0f, -1.0f, -1.f);
		m_Max = glm::vec3(1.0f, 1.0f, 1.f);

		InitCorners();
		CalculateCorners();
	}

	//constructor that makes a physics body out of a position, rotation, and scale
	Titan::TTN_Physics::TTN_Physics(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
	{
		m_trans = TTN_Transform();
		m_trans.SetPos(position);
		m_trans.RotateFixed(rotation);
		m_trans.SetScale(scale);

		m_Center = position;

		InitCorners();
		CalculateCorners();
	}

	void TTN_Physics::Projections()
	{

	}
		
	void TTN_Physics::CalcNormals(TTN_Physics& b1)
	{
		glm::vec3 pos = b1.GetCenter();
		glm::vec3 norm = glm::normalize(pos);
		b1.normals = norm;

	}	

	//AABB coolision check, returns true if the two enters physics bodies are colliding, false if not
	bool TTN_Physics::Inter(TTN_Physics& b1, TTN_Physics& b2)
	{
		//if the 2 bodies are intersecting, then return true
		if ((b1.GetMin().x <= b2.GetMax().x && b1.GetMax().x >= b2.GetMin().x) &&
			(b1.GetMin().y <= b2.GetMax().y && b1.GetMax().y >= b2.GetMin().y) &&
			(b1.GetMin().z <= b2.GetMax().z && b1.GetMax().z >= b2.GetMin().z))
		{
			return true;
		}
		//otherwise returns false
		else
			return false;
	}

	//sets the position of the physics body
	void TTN_Physics::SetPos(glm::vec3 pos)
	{
		//pass the position to the underlying transform
		m_trans.SetPos(pos);
		//recalculate the corners
		CalculateCorners();
	}

	//sets the scale of the phyiscs body
	void TTN_Physics::SetScale(glm::vec3 scale)
	{
		//pass the scale to the underlying transform
		m_trans.SetScale(scale);
		//recalculate the corners
		CalculateCorners();
	}

	//rotates the physics body around world axes
	void TTN_Physics::RotateFixed(glm::vec3 rotation)
	{
		//pass the rotation to the underlying transform
		m_trans.RotateFixed(rotation);
		//recalculate the corners
		CalculateCorners();
	}

	//rotates the physics body around local axes
	void TTN_Physics::RotateLocal(glm::vec3 rotation)
	{
		//pass the rotation to the underlying transform
		m_trans.RotateRelative(rotation);
		//recalculate the corners
		CalculateCorners();
	}

	//sets up the data required to render physic bodies 
	void TTN_Physics::SetUpPhysicsBoxRendering()
	{
		//reinit the variables
		TTN_Physics::shader = TTN_Shader::Create();
		TTN_Physics::texture = TTN_Texture::Create();
		TTN_Physics::mat = TTN_Material::Create();
		TTN_Physics::mesh = TTN_Mesh::Create();

		//load the shaders
		shader->LoadShaderStageFromFile("shaders/ttn_vert_phyiscsbody.glsl", GL_VERTEX_SHADER);
		shader->LoadShaderStageFromFile("shaders/ttn_frag_physicsbody.glsl", GL_FRAGMENT_SHADER);
		shader->Link();

		//load the mesh
		mesh = TTN_ObjLoader::LoadFromFile("physics_body_render.obj");
		mesh->SetUpVao();

		//load the texture
		texture->LoadFromFile("physics box texture.png");

		//set the texture
		mat->SetAlbedo(texture);
		//and set the material
		mesh->SetMat(mat);

		//the rendering can be flagged as set up
		renderingSetUp = true;
	}

	//returns wheter or not the rendering of physics body is set up (note physics body will still work even if not set up to render, they'll just be invisible)
	bool TTN_Physics::GetRenderingIsSetUp()
	{
		return renderingSetUp;
	}

	void TTN_Physics::Render(glm::mat4 vp)
	{
		//make sure the vao is acutally set up before continuing
		if (mesh->GetVAOPointer() == nullptr)
			//if it isn't, then stop then return so the later code doesn't break the entire program
			return;

		//bind the shader this model uses
		shader->Bind();
		//send the uniforms to openGL 
		shader->SetUniformMatrix("MVP", vp * m_trans.GetMatrix());
		mesh->GetMatPointer()->GetAlbedo()->Bind(0);
		//render the VAO
		mesh->GetVAOPointer()->Render();
		//unbind the shader
		shader->UnBind();
	}

	//recalculates all the corners of the physics body and sets the min and max value
	void TTN_Physics::CalculateCorners()
	{
		InitCorners();

		//go through them and apply the transformation
		for (int i = 0; i < 8; i++) {
			//multiply each corner by the corner by the transfomation matrix
			glm::vec4 temp = glm::vec4(m_Corners[i].x, m_Corners[i].y, m_Corners[i].z, 1.0f);
			glm::vec4 result = m_trans.GetMatrix() * temp;
			//and save the result
			m_Corners[i] = glm::vec3(result.x, result.y, result.z);

			//reset the min and max (to make sure it saves correctly when a change is made)
			if (i == 0) {
				m_Min = m_Corners[i];
				m_Max = m_Corners[i];
				m_Center = m_trans.GetPos();
			}

			//compare with the minimum on each axis
			if (m_Corners[i].x < m_Min.x) m_Min.x = m_Corners[i].x;
			if (m_Corners[i].y < m_Min.y) m_Min.y = m_Corners[i].y;
			if (m_Corners[i].z < m_Min.z) m_Min.z = m_Corners[i].z;

			//compare with the maximum on each axis
			if (m_Corners[i].x > m_Max.x) m_Max.x = m_Corners[i].x;
			if (m_Corners[i].y > m_Max.y) m_Max.y = m_Corners[i].y;
			if (m_Corners[i].z > m_Max.z) m_Max.z = m_Corners[i].z;
		}
	}

	//sets up the original corners of the physics body
	void TTN_Physics::InitCorners()
	{
		m_Corners[0] = glm::vec3(-1.0f, -1.0f, -1.0f);
		m_Corners[1] = glm::vec3(1.0f, -1.0f, -1.0f);
		m_Corners[2] = glm::vec3(-1.0f, -1.0f, 1.0f);
		m_Corners[3] = glm::vec3(1.0f, -1.0f, 1.0f);
		m_Corners[4] = glm::vec3(-1.0f, 1.0f, -1.0f);
		m_Corners[5] = glm::vec3(1.0f, 1.0f, -1.0f);
		m_Corners[6] = glm::vec3(-1.0f, 1.0f, 1.0f);
		m_Corners[7] = glm::vec3(1.0f, 1.0f, 1.0f);
	}
}