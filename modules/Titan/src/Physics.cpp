//Titan Engine, by Atlas X Games 
// Physics.cpp - source file for the class that represents physics bodies

//include the header
#include "Titan/Physics.h"
#include "Titan/GlutStuff.h"
//include other required features
#include "Titan/ObjLoader.h"
#include <iostream>

#include <stdio.h> //printf debugging

namespace Titan {

	TTN_PhysicsB::TTN_PhysicsB()
	{
		btCollisionShape* boxCol = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));

	}

	TTN_PhysicsB::TTN_PhysicsB(glm::vec3 pos, btDiscreteDynamicsWorld* m_world, btAlignedObjectArray<btCollisionShape*> parray)
	{
		static btDefaultMotionState* m_MotionState;

		btCollisionShape* boxCol = new btBoxShape(btVector3(2.0f, 2.0f, 2.0f)); //size
		parray.push_back(boxCol); //add hitbox to array

		m_mass = 2.0f;
		m_trans.setIdentity();
		m_trans.setOrigin(btVector3(pos.x, pos.y, pos.z));

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (m_mass != 0.f);

		btVector3 m_inertia(0, 0, 0);
		if (isDynamic)
			boxCol->calculateLocalInertia(m_mass,m_inertia);

		//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
		m_MotionState= new btDefaultMotionState(m_trans);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(m_mass, m_MotionState, boxCol, m_inertia);// first: mass in kg, last:local interia in btvec3
		btRigidBody* body = new btRigidBody(rbInfo);

		//add the body to the world
		m_world->addRigidBody(body);		

	}

	void TTN_PhysicsB::InitCollision()
	{
		// Build the broadphase which essentially detects overlapping aabb pairs
		//m_broadphase = new btDbvtBroadphase();

		//// Set up the collision configuration and dispatcher
		//m_config = new btDefaultCollisionConfiguration();
		//m_dispatcher = new btCollisionDispatcher(m_config);

		//// The actual physics solver
		//m_solver = new btSequentialImpulseConstraintSolver();

		//// The world.
		//m_world = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_config);
		//m_world->setGravity(btVector3(0, -9.80f, 0));

	}

	void TTN_PhysicsB::SetMass(btScalar mass)
	{
		m_mass = mass;
	}

	void TTN_PhysicsB::SetTrans(btVector3 trans)
	{
		m_trans.setOrigin(trans);
	}


	TTN_Debug::TTN_Debug() 
		: m_debugMode(0)
	{
		
	}

	TTN_Debug::~TTN_Debug()
	{
	}

	void TTN_Debug::drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor)
	{
		GLfloat points[6];

		points[0] = from.x();
		points[1] = from.y();
		points[2] = from.z();

		points[3] = to.x();
		points[4] = to.y();
		points[5] = to.z();

		btVector3 result = to - from;
		glm::vec3 colors = { result.x(), result.y(), result.z() };
		colors = glm::normalize(colors);
		GLuint indices[] = { 0,1 };

		GLuint vao, vbo, ebo;

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindVertexArray(vao);

		//UPLOADING VERTEX
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
		//UPLOADING INDEXES
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(GLuint) * 2, indices,GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,sizeof(GLfloat) * 3, (GLvoid*)0);
		glBindVertexArray(0);

		//use geometry
		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		////delete buffers
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
	}

	void TTN_Debug::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
	{
		drawLine(from, to, color, color);
	}

	void TTN_Debug::drawSphere(const btVector3& p, btScalar radius, const btVector3& color)
	{
	}

	void TTN_Debug::drawTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& color, btScalar alpha)
	{
	}

	void TTN_Debug::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
	{
	}

	void TTN_Debug::reportErrorWarning(const char* warningString)
	{
	}

	void TTN_Debug::draw3dText(const btVector3& location, const char* textString)
	{
	}

	void TTN_Debug::setDebugMode(int debugMode)
	{
		m_debugMode = debugMode;
	}






#pragma region test code
	//void TTN_Debug::setDebugMode(int debugmode)
	//{

	//}

	//void TTN_Debug::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {

	//	// Vertex data
	//	/*GLfloat points[12];

	//	points[0] = from.x();
	//	points[1] = from.y();
	//	points[2] = from.z();
	//	points[3] = color.x();
	//	points[4] = color.y();
	//	points[5] = color.z();

	//	points[6] = to.x();
	//	points[7] = to.y();
	//	points[8] = to.z();
	//	points[9] = color.x();
	//	points[10] = color.y();
	//	points[11] = color.z();

	//	glGenVertexArrays(1, &vao);
	//	glBindVertexArray(VertexArrayID);
	//	
	//	glGenBuffers(1, &vertexbuffer);
	//	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	//	glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);

	//	glEnableVertexAttribArray(0);
	//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
	//	glEnableVertexAttribArray(1);
	//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	//	glDrawArrays(GL_LINES, 0, 2);
	//	glBindVertexArray(0);*/
	//}

	//void TTN_Debug::lines(const btVector3& from, const btVector3& to)
	//{
	//	GLfloat points[6];

	//	points[0] = from.x();
	//	points[1] = from.y();
	//	points[2] = from.z();
	//	
	//	points[3] = to.x();
	//	points[4] = to.y();
	//	points[5] = to.z();
	//
	//	btVector3 result = to - from;
	//	glm::vec3 colors = { result.x(), result.y(), result.z() };
	//	colors = glm::normalize(colors);
	//	GLuint indices[] = { 0,1 };

	//	GLuint vao, vbo, ebo;

	//	glGenVertexArrays(1, &vao);
	//	glGenBuffers(1, &vbo);
	//	glGenBuffers(1, &ebo);

	//	glBindVertexArray(vao);

	//	//UPLOADING VERTEX
	//	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//	glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
	//	//UPLOADING INDEXES
	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	//	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
	//		sizeof(GLuint) * 2,
	//		indices,
	//		GL_STATIC_DRAW);
	//	glEnableVertexAttribArray(0);
	//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
	//		sizeof(GLfloat) * 3, (GLvoid*)0);
	//	glBindVertexArray(0);

	//	//use program
	///*	material_->use();
	//	material_->setValue(glm::value_ptr(colors), "ph_objectColor");
	//	material_->setProjectionMatrix(gmeinstance->getCurrentCamera()->getProjectionMatrix());
	//	material_->setViewMatrix(gmeinstance->getCurrentCamera()->getViewMatrix());*/

	//	//use geometry
	//	glBindVertexArray(vao);
	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	//	glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
	//	glBindVertexArray(0);

	//	//delete buffers
	//	glDeleteVertexArrays(1, &vao);
	//	glDeleteBuffers(1, &vbo);
	//	glDeleteBuffers(1, &ebo);
	//}

#pragma endregion











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
		m_ShouldBounce = (!m_IsStaticBody);
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