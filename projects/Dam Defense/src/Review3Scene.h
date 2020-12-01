//Dam Defense by Atlas X Games
//Review3Scene.h - header for the class that inherits from the scene class to make a scene represneting a demo of our final product for review 3
#pragma once

//#include "Titan/Particle.h"
#include "Titan/Application.h"
#include "Titan/ObjLoader.h"
#include <iostream>

using namespace Titan;

class Review3Scene : public TTN_Scene {
public:
	//default constructor
	Review3Scene();

	//sets up the scene
	void InitScene();

	//update the scene
	void Update(float deltaTime);

	//keyboard input
	void KeyDownChecks();
	void KeyChecks();
	void KeyUpChecks();

	//mouse input
	void MouseButtonDownChecks();
	void MouseButtonChecks();
	virtual void MouseButtonUpChecks();

	void CreateCannonball();
	void BoatPathing( entt::entity boat, int path);
	void Spawner(float deltatime, float SpawnTime);
	glm::vec3 Seek(glm::vec3 target, glm::vec3 velo, glm::vec3 pos);

public:
	//shader programs
	TTN_Shader::sshptr shaderProgam;
	TTN_Shader::sshptr shaderProgamTextured;
	TTN_Shader::sshptr shaderHeight;
	TTN_Shader::sshptr shaderProgramSkybox;
	TTN_Shader::sshptr shaderProgramAnimatedTextured;

	//meshes
	TTN_Mesh::smptr boatMesh;
	TTN_Mesh::smptr cannonMesh;
	TTN_Mesh::smptr plane;
	TTN_Mesh::smptr tree1Mesh;
	TTN_Mesh::smptr skyboxMesh;

	//textures
	TTN_Texture2D::st2dptr waterText;
	TTN_Texture2D::st2dptr cannonText;

	TTN_TextureCubeMap::stcmptr skyboxText;
	TTN_Texture2D::st2dptr heightmap;

	//materials
	TTN_Material::smatptr waterMat;
	TTN_Material::smatptr cannonMat;
	TTN_Material::smatptr skyboxMat;
	TTN_Material::smatptr heightMat;

	TTN_Mesh::smptr swordMesh;
	TTN_Texture2D::st2dptr swordText;
	TTN_Material::smatptr swordMat;

	float Timer = 0.F;
	

protected:
	//entities
	entt::entity camera;
	entt::entity light;
	entt::entity cannon;
	entt::entity water;
	entt::entity tree1;
	entt::entity tree2;
	entt::entity tree3;
	entt::entity tree4;
	entt::entity tree5;
	
	
	entt::entity debug;//used to look at positions on the map
	entt::entity boat;
	entt::entity boat2;

	entt::entity ball;
	std::vector<entt::entity> cannonballs;
	std::vector<entt::entity> boats;
	
	entt::entity skybox;
	entt::entity height;
	entt::entity testParticleSystem;

	//others
	glm::vec2 rotAmmount;
	glm::vec2 mousePos; 
	TTN_ParticleTemplate testParticle;
};