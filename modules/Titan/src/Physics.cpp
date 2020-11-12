//Titan Engine, by Atlas X Games 
// Physics.cpp - source file for the class that represents physics bodies

//include the header
#include "Titan/Physics.h"
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
		//set up titan transform
		m_trans = TTN_Transform();
		m_trans.SetPos(glm::vec3(0.0f));
		m_trans.SetScale(glm::vec3(1.0f));

		//set up bullet collision shape
		m_colShape = new btBoxShape(btVector3(m_trans.GetScale().x / 2.0f, m_trans.GetScale().y / 2.0f, m_trans.GetScale().z / 2.0f));
		//set up bullet transform
		m_bulletTrans.setIdentity();
		m_bulletTrans.setOrigin(btVector3(m_trans.GetPos().x, m_trans.GetPos().y, m_trans.GetPos().z));
		m_bulletTrans.setRotation(btQuaternion(m_trans.GetRotQuat().x, m_trans.GetRotQuat().y, m_trans.GetRotQuat().z, m_trans.GetRotQuat().w));
		//setup up bullet motion state
		m_MotionState = new btDefaultMotionState(m_bulletTrans);

		//setup mass, static v dynmaic status, and local internia
		btVector3 localIntertia(0, 0, 0);
		m_Mass = 1.0f;
		m_IsStaticBody = false;
		if (m_IsStaticBody) {
			m_Mass = 0.0f;
			m_colShape->calculateLocalInertia(m_Mass, localIntertia);
		}

		//create the rigidbody
		btRigidBody::btRigidBodyConstructionInfo rbInfo(m_Mass, m_MotionState, m_colShape, localIntertia);
		m_body = new btRigidBody(rbInfo);

		m_hasGravity = true;

		m_InWorld = false;
	}

	//constructor that makes a physics body out of a position, rotation, and scale
	Titan::TTN_Physics::TTN_Physics(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, bool dynamic, float mass)
	{
		//set up titan transform
		m_trans = TTN_Transform();
		m_trans.SetPos(position);
		m_trans.RotateFixed(rotation);
		m_trans.SetScale(scale);

		//set up bullet collision shape
		m_colShape = new btBoxShape(btVector3(m_trans.GetScale().x / 2.0f, m_trans.GetScale().y / 2.0f, m_trans.GetScale().z / 2.0f));
		m_bulletTrans.setIdentity();
		//set up bullet transform
		m_bulletTrans.setOrigin(btVector3(m_trans.GetPos().x, m_trans.GetPos().y, m_trans.GetPos().z));
		m_bulletTrans.setRotation(btQuaternion(m_trans.GetRotQuat().x, m_trans.GetRotQuat().y, m_trans.GetRotQuat().z, m_trans.GetRotQuat().w));
		//setup up bullet motion state
		m_MotionState = new btDefaultMotionState(m_bulletTrans);

		//setup mass, static v dynmaic status, and local internia
		btVector3 localIntertia(0, 0, 0);
		m_Mass = mass;
		m_IsStaticBody = (!dynamic);
		if (m_IsStaticBody) {
			m_Mass = 0.0f;
			m_colShape->calculateLocalInertia(m_Mass, localIntertia);
		}

		//create the rigidbody
		btRigidBody::btRigidBodyConstructionInfo rbInfo(m_Mass, m_MotionState, m_colShape, localIntertia);
		m_body = new btRigidBody(rbInfo);

		m_hasGravity = true;

		m_InWorld = false;
	}

	TTN_Physics::~TTN_Physics()
	{}


	//updates the position of the physics body based on the velocity and deltaTime
	void TTN_Physics::Update(float deltaTime)
	{
		//updates the titan transform of the physics body

		//fetch the bullet transform
		if (m_body->getMotionState() != nullptr) {
			m_body->getMotionState()->getWorldTransform(m_bulletTrans);

		}
		else {
			m_bulletTrans = m_body->getWorldTransform();
		}

		//copy the position of the bullet transfrom into the titan transform
		m_trans.SetPos(glm::vec3((float)m_bulletTrans.getOrigin().getX(), (float)m_bulletTrans.getOrigin().getY(), (float)m_bulletTrans.getOrigin().getZ()));

		//copy the rotation
		btQuaternion rot = m_bulletTrans.getRotation();
		m_trans.SetRotationQuat(glm::quat(rot.getW(), rot.getX(), rot.getY(), rot.getZ()));
	}

	//reads the velocity out from bullet
	glm::vec3 TTN_Physics::GetLinearVelocity()
	{
		//get the linear velocity
		btVector3 velo = m_body->getLinearVelocity();
		//cast it to a glm vec3 and return it
		return glm::vec3((float)velo.getX(), (float)velo.getY(), (float)velo.getZ());
	}

	//gets the angular velocity out of bullet
	glm::vec3 TTN_Physics::GetAngularVelocity()
	{
		btVector3 velo = m_body->getAngularVelocity();
		return glm::vec3((float)velo.getX(), (float)velo.getY(), (float)velo.getZ());
	}

	void TTN_Physics::SetIsStatic(bool isStatic)
	{
		m_IsStaticBody = isStatic;
		if (m_IsStaticBody) {
			SetMass(0.0f);
		}
		else if (!m_IsStaticBody && m_Mass == 0.0f) {
			SetMass(1.0f);
		}
	}

	//sets the flag for if it's in the physics world or not
	void TTN_Physics::SetIsInWorld(bool inWorld)
	{
		m_InWorld = inWorld;
	}

	//sets the mass of the object
	void TTN_Physics::SetMass(float mass)
	{
		//save the mass
		m_Mass = mass;
		//update it in bullet
		//clear velocities
		btVector3 linearVelo = m_body->getLinearVelocity();
		btVector3 angularVelo = m_body->getAngularVelocity();
		m_body->setLinearVelocity(btVector3(0,0,0));
		m_body->setAngularVelocity(btVector3(0, 0, 0));
		//update mass
		m_body->setMassProps(m_Mass, btVector3(0,0,0));
		//check if the body is still dynamic
		if (m_Mass == 0.0f) {
			//if it isn't mark it as static
			m_IsStaticBody = true;
		}
		else {
			//if it is, mark it as dynamic, and restore the velocities
			m_IsStaticBody = false;
			m_body->setLinearVelocity(linearVelo);
			m_body->setAngularVelocity(angularVelo);
		}
	}

	void TTN_Physics::SetLinearVelocity(glm::vec3 velocity)
	{
		m_body->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
	}

	void TTN_Physics::SetAngularVelocity(glm::vec3 velocity)
	{
		m_body->setAngularVelocity(btVector3(velocity.x, velocity.y, velocity.z));
	}

	void TTN_Physics::SetHasGravity(bool hasGrav)
	{
		m_hasGravity = hasGrav;
	}

	void TTN_Physics::AddForce(glm::vec3 force)
	{
		m_body->applyCentralForce(btVector3(force.x, force.y, force.z));
	}

	void TTN_Physics::AddImpulse(glm::vec3 impulseForce)
	{
		m_body->applyCentralImpulse(btVector3(impulseForce.x, impulseForce.y, impulseForce.z));
	}

	void TTN_Physics::ClearForces()
	{
		m_body->clearForces();
	}
	
	


}