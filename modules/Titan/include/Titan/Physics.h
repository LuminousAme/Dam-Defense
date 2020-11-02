//Titan Engine, by Atlas X Games 
// Physics.h - header for the class that represents physics bodies
#pragma once

//include other headers
#include "Transform.h"
#include "Shader.h"
#include "Mesh.h"
#include "Material.h"
#include "Renderer.h"
//include glm features
#include "GLM/glm.hpp"
//import other required features
#include <vector>

namespace Titan {
	class TTN_Physics
	{
	public:

		//default constructor
		TTN_Physics();

		TTN_Physics(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
		~TTN_Physics() = default;

		//copy, move, and assingment constrcutors for ENTT
		TTN_Physics(const TTN_Physics&) = default;
		TTN_Physics(TTN_Physics&&) = default;
		TTN_Physics& operator=(TTN_Physics&) = default;

		static void Intersects(TTN_Physics& b1, TTN_Physics& b2);

		static bool Inter(TTN_Physics& b1, TTN_Physics& b2);

		void Bounce(glm::vec3 Contactnormal);

		void Update(float deltaTime);

		void SetPos(glm::vec3 pos);
		void SetScale(glm::vec3 scale);
		void RotateFixed(glm::vec3 rotation);
		void RotateLocal(glm::vec3 rotation);
		void SetIsStaticBody(bool isStatic);
		void SetVelocity(glm::vec3 Velo);
		void SetShouldBounce(bool bounce);

		glm::vec3 GetCenter() { return m_Center; }
		glm::vec3 GetMin() { return m_Min; }
		glm::vec3 GetMax() { return m_Max; }
		glm::vec3 GetPosition() { return m_trans.GetPos(); }
		glm::vec3 GetScale() { return m_trans.GetScale(); }
		bool GetIsStaticBody() { return m_IsStaticBody; }
		glm::vec3 GetVelocity() { return m_Velocity; }
		bool GetShouldBounce() { return m_ShouldBounce; }

		static void SetUpPhysicsBoxRendering();
		static bool GetRenderingIsSetUp();
		void Render(glm::mat4 vp);

	protected:
		glm::vec3 m_Center; //center of object
		glm::vec3 m_Min; //corner of smallest  coord
		glm::vec3 m_Max; //corner of largest coord

		glm::vec3 m_Scale; //scale of the object on each axis
		TTN_Transform m_trans; //transform with the position, rotation, and scale of the physics body
		glm::vec3 m_Corners[8]; //the positions of each corner of the physics body

		bool m_IsStaticBody;

		glm::vec3 m_Velocity;
		bool m_ShouldBounce;

		//data for rendering the physics box
		static TTN_Shader::sshptr shader;
		static TTN_Texture::stptr texture;
		static TTN_Material::smatptr mat;
		static TTN_Mesh::smptr mesh;
		static bool renderingSetUp;
		static TTN_Renderer renderer;


		void CalculateCorners();
		void InitCorners();
	};

	//class representing a collision
	class TTN_Collision {
	public:
		//defines a special easier to use name for shared(smart) pointers to the class 
		typedef std::shared_ptr<TTN_Collision> scolptr;

		//creates and returns a shared(smart) pointer to the class 
		static inline scolptr Create() {
			return std::make_shared<TTN_Collision>();
		}

	public:
		//ensuring moving and copying is not allowed so we can manually control destructor calls
		TTN_Collision(const TTN_Collision& other) = delete;
		TTN_Collision(TTN_Collision& other) = delete;
		TTN_Collision& operator=(const TTN_Collision& other) = delete;
		TTN_Collision& operator=(TTN_Collision&& other) = delete;

	public:
		//default constructor that creates a new collision object
		TTN_Collision();

		//contrscutor that creates a new collision object from two physics bodies
		TTN_Collision(TTN_Physics* b1, TTN_Physics* b2);

		//destructor
		~TTN_Collision();

		//Getters
		TTN_Physics* GetBody1();
		TTN_Physics* GetBody2();
		bool GetHasCollided();

		//Setters
		void SetBody1(TTN_Physics* b);
		void SetBody2(TTN_Physics* b);

		//checks if a collision has happened and saves data about it if it has
		void CheckCollision();

	protected:
		//the objects this collision tracks
		TTN_Physics* m_Body1;
		TTN_Physics* m_Body2;

		//bool for if the 2 physics body have collided this frame
		bool m_HasHappened;
	};

}