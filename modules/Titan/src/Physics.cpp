//Titan Engine, by Atlas X Games 
// Physics.cpp - source file for the class that represents physics bodies

//include the header
#include "Titan/Physics.h"
//include other required features
#include "Titan/ObjLoader.h"
#include <iostream>
#include "..\include\Titan\Physics.h"

namespace Titan {
	//initlization for static rendering variables
	TTN_Shader::sshptr TTN_Physics::shader = nullptr;
	TTN_Texture::stptr TTN_Physics::texture = nullptr;
	TTN_Material::smatptr TTN_Physics::mat = nullptr;
	TTN_Mesh::smptr TTN_Physics::mesh = nullptr;
	bool TTN_Physics::renderingSetUp = false;
	TTN_Renderer TTN_Physics::renderer = TTN_Renderer();

	//default constructor, constructs a basic 1x1x1 physics body around the origin
	TTN_Physics::TTN_Physics()
	{
		m_trans = TTN_Transform();
		m_trans.SetPos(glm::vec3(0.0f));
		m_trans.SetScale(glm::vec3(1.0f));

		m_Center = glm::vec3(0.0f, 0.0f, 0.f);
		m_Min = glm::vec3(-1.0f, -1.0f, -1.f);
		m_Max = glm::vec3(1.0f, 1.0f, 1.f);

		m_IsStaticBody = false;

		m_Velocity = glm::vec3(0.0f);
		m_ShouldBounce = true;

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

		m_IsStaticBody = false;
		m_ShouldBounce = true;
		
		m_Velocity = glm::vec3(0.0f);

		InitCorners();
		CalculateCorners();
	}

	//idk what this is here for tbh
	void TTN_Physics::Intersects(TTN_Physics& b1, TTN_Physics& b2)
	{
		if ((b1.GetMin().x <= b2.GetMax().x && b1.GetMax().x >= b2.GetMin().x) &&
			(b1.GetMin().y <= b2.GetMax().y && b1.GetMax().y >= b2.GetMin().y) &&
			(b1.GetMin().z <= b2.GetMax().z && b1.GetMax().z >= b2.GetMin().z))
		{

			std::cout << "Hello" << std::endl;
		}

	}

	//AABB coolision check, returns true if the two enters physics bodies are colliding, false if not
	bool TTN_Physics::Inter(TTN_Physics& b1, TTN_Physics& b2)
	{
		//if the 2 bodies are intersecting, then return true
		if ((b1.GetMin().x <= b2.GetMax().x && b1.GetMax().x >= b2.GetMin().x) &&
			(b1.GetMin().y <= b2.GetMax().y && b1.GetMax().y >= b2.GetMin().y) &&
			(b1.GetMin().z <= b2.GetMax().z && b1.GetMax().z >= b2.GetMin().z))
		{
			//calculate the contact normals
			glm::vec3 temp = b2.GetPosition() - b1.GetPosition();

			//find the largest component
			float largestComponentAbs = glm::abs(temp.x);
			float largestComponentSigned = temp.x;
			int largestAxis = 0;
			if (glm::abs(temp.y) > largestComponentAbs) {
				largestComponentAbs = glm::abs(temp.y);
				largestComponentSigned = temp.y;
				largestAxis = 1;
			}
			if (glm::abs(temp.z) > largestComponentAbs) {
				largestComponentAbs = glm::abs(temp.z);
				largestComponentSigned = temp.z;
				largestAxis = 2;
			}

			glm::vec3 normalForb1 = glm::vec3(0.0f); //normal of the edge of b2 that b1 is bouncing off of 
			glm::vec3 normalForb2 = glm::vec3(0.0f);; //normal of the edge of b1 that b2 is bouncing off of 

			//that largest component represents the two contact normals
			if (largestAxis == 0) normalForb2 = glm::vec3(largestComponentSigned, 0.0f, 0.0f);
			else if (largestAxis == 1)  normalForb2 = glm::vec3(0.0f, largestComponentSigned, 0.0f);
			else if (largestAxis == 2)  normalForb2 = glm::vec3(0.0f, 0.0f, largestComponentSigned);
			normalForb2 = glm::normalize(normalForb2);
			normalForb1 = normalForb2 * -1.0f;

			//resolve the collision
			//find the higher minmum and lower maximum on that axis 
			float higherMin, lowerMax;

			if (largestAxis == 0) {
				higherMin = (b1.GetMin().x >= b2.GetMin().x) ? b1.GetMin().x : b2.GetMin().x;
				lowerMax = (b1.GetMax().x <= b2.GetMax().x) ? b1.GetMax().x : b2.GetMax().x;
			}
			else if (largestAxis == 1) {
				higherMin = (b1.GetMin().y >= b2.GetMin().y) ? b1.GetMin().y : b2.GetMin().y;
				lowerMax = (b1.GetMax().y <= b2.GetMax().y) ? b1.GetMax().y : b2.GetMax().y;
			}
			else if (largestAxis == 2) {
				higherMin = (b1.GetMin().z >= b2.GetMin().z) ? b1.GetMin().z : b2.GetMin().z;
				lowerMax = (b1.GetMax().z <= b2.GetMax().z) ? b1.GetMax().z : b2.GetMax().z;
			}

			//we can then use those to caculate the pentration depth of the colision
			float depth = lowerMax - higherMin;

			//and then combining that with the normals we can resolve the colision

			//if both objects are dynamic
			if (!b1.GetIsStaticBody() && !b2.GetIsStaticBody()) {
				//resolve both of them by half of the depth
				b1.SetPos(b1.GetPosition() + normalForb1 * depth/2.0f);
				b2.SetPos(b2.GetPosition() + normalForb2 * depth / 2.0f);
			}
			//if only b1 is dynamic
			else if (!b1.GetIsStaticBody() && b2.GetIsStaticBody()) {
				//resolve only b1 by the full depth
				b1.SetPos(b1.GetPosition() + normalForb1 * depth);
			}
			//if only b2 is dyanmic
			else if (b1.GetIsStaticBody() && !b2.GetIsStaticBody()) {
				//resolve only b2 by the full depth
				b2.SetPos(b2.GetPosition() + normalForb2 * depth);
			}

			//the colision should now be resolved so the last thing to do is call the bounce for both bodies
			b1.Bounce(normalForb1);
			b2.Bounce(normalForb2);

			return true;
		}

		//if there was no collision return false
		else
			return false;
	}

	//causes the object to bounce when it collides with another object 
	void TTN_Physics::Bounce(glm::vec3 Contactnormal)
	{
		//only bounce the object if it is dynamic (acutally moves) and should bounce
		if (!m_IsStaticBody && m_ShouldBounce) {
			//make a vector to store the new velocity, start with the old velocity
			glm::vec3 newVelo = m_Velocity;

			//copy the direction of the normal from the contactNormal into the new vector
			if (Contactnormal.x != 0.0f)
				newVelo.x = Contactnormal.x;

			if (Contactnormal.y != 0.0f)
				newVelo.y = Contactnormal.y;

			if (Contactnormal.x != 0.0f)
				newVelo.y = Contactnormal.y;

			//normalize the new velocity
			newVelo = glm::normalize(newVelo);

			//multiply by the magnintude of the original velocity so the speed is the same 
			newVelo = newVelo * glm::length(m_Velocity);

			//we now have a new velocity that has bounced off the other object, so set our object's velocity to it
			m_Velocity = newVelo;
		}
	}

	//updates the position of the physics body based on the velocity and deltaTime
	void TTN_Physics::Update(float deltaTime)
	{
		if (!m_IsStaticBody) {
			//updates the position of the physics body
			SetPos(GetPosition() + (m_Velocity * deltaTime));
		}
	}

	//sets the position of the physics body
	void TTN_Physics::SetPos(glm::vec3 pos)
	{
		//pass the position to the underlying transform
		m_trans.SetPos(pos);
		m_Center = pos;
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

	//sets wheter or not this physics body is static(doesn't move)
	void TTN_Physics::SetIsStaticBody(bool isStatic)
	{
		m_IsStaticBody = isStatic;
		if (isStatic) m_ShouldBounce = false;
	}

	//sets the velocity of the physics body
	void TTN_Physics::SetVelocity(glm::vec3 Velo)
	{
		m_Velocity = Velo;
	}

	void TTN_Physics::SetShouldBounce(bool bounce)
	{
		if (!m_IsStaticBody)
			m_ShouldBounce = bounce;
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
		
		//setup the renderer
		renderer = TTN_Renderer(mesh, shader);
		renderer.SetMat(mat);

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

		renderer.GetMat()->GetAlbedo()->Bind(0);
		renderer.Render(m_trans.GetMatrix(), vp);
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

	//default collision constructor
	TTN_Collision::TTN_Collision()
		: m_Body1(nullptr), m_Body2(nullptr)
	{
		m_HasHappened = false;
	}

	//constructs a collision from 2 physics body references
	TTN_Collision::TTN_Collision(TTN_Physics* b1, TTN_Physics* b2)
		: m_Body1(b1), m_Body2(b2)
	{
		m_HasHappened = false;
	}

	//destructor
	TTN_Collision::~TTN_Collision()
	{
	}

	//Gets the first physics body in the collision object
	TTN_Physics* TTN_Collision::GetBody1()
	{
		return m_Body1;
	}

	//Gets the second physics body in the collision object
	TTN_Physics* TTN_Collision::GetBody2()
	{
		return m_Body2;
	}

	//returns a boolean representing wheter or not a collision has happened this frame
	bool TTN_Collision::GetHasCollided()
	{
		return m_HasHappened;
	}

	//sets the refernece for the first body
	void TTN_Collision::SetBody1(TTN_Physics* b)
	{
		m_Body1 = b;
	}

	//sets the reference for the second body
	void TTN_Collision::SetBody2(TTN_Physics* b)
	{
		m_Body2 = b;
	}

	//check for if a collision between this object's 2 physics bodies has occured, if so, save data about it
	void TTN_Collision::CheckCollision()
	{
		m_HasHappened = TTN_Physics::Inter(*m_Body1, *m_Body2);
	}
}