//Titan Engine, by Atlas X Games 
// Physics.h - header for the class that represents physics bodies
#pragma once

//include other headers
#include "Transform.h"
#include "Shader.h"
#include "Mesh.h"
#include "Material.h"
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
		
		//aabb
		TTN_Physics(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

		~TTN_Physics() = default;

		//copy, move, and assingment constrcutors for ENTT
		TTN_Physics(const TTN_Physics&) = default;
		TTN_Physics(TTN_Physics&&) = default;
		TTN_Physics& operator=(TTN_Physics&) = default;

		//sat stuff
		static void Projections();
		void GetNormals();

		void CalcNormals(TTN_Physics& b1);

		glm::vec3 axis;
		glm::vec3 normals;
		glm::vec3 points;

		static bool Inter(TTN_Physics& b1, TTN_Physics& b2);

		void SetPos(glm::vec3 pos);
		void SetScale(glm::vec3 scale);
		void RotateFixed(glm::vec3 rotation);
		void RotateLocal(glm::vec3 rotation);

		glm::vec3 GetCenter() { return m_Center; }
		glm::vec3 GetMin() { return m_Min; }
		glm::vec3 GetMax() { return m_Max; }

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

		//data for rendering the physics box
		static TTN_Shader::sshptr shader;
		static TTN_Texture::stptr texture;
		static TTN_Material::smatptr mat;
		static TTN_Mesh::smptr mesh;
		static bool renderingSetUp;

		void CalculateCorners();
		void InitCorners();




	};

}

