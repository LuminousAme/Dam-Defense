//Dam Defense, by Atlas X Games
//Game.h, the header file for the class that represents the main gameworld scene
#pragma once

//include required features from titan
#include "Titan/Application.h"
#include "Titan/ObjLoader.h"

using namespace Titan;

class Game : public TTN_Scene {
public:
	//default constructor
	Game();

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

//Assets
public:
	//shader programs
	TTN_Shader::sshptr shaderProgramUnTextured;
	TTN_Shader::sshptr shaderProgramTextured;
	TTN_Shader::sshptr shaderProgramSkybox;
	TTN_Shader::sshptr shaderProgramAnimatedTextured;
	//TTN_Shader::sshptr shaderProgramWater;
	//TTN_Shader::sshptr shaderProgramTerrain;

	//meshes
	TTN_Mesh::smptr cannonMesh;
	TTN_Mesh::smptr skyboxMesh;
	TTN_Mesh::smptr sphereMesh; //used for cannonballs and particles

	//textures
	TTN_Texture2D::st2dptr cannonText;
	TTN_TextureCubeMap::stcmptr skyboxText;
	
	//materials
	TTN_Material::smatptr cannonMat;
	TTN_Material::smatptr skyboxMat;
	TTN_Material::smatptr smokeMat;

//Entities
protected:
	entt::entity camera;
	entt::entity light;
	entt::entity skybox;
	entt::entity cannon;
	std::vector<entt::entity> cannonBalls;
	entt::entity smokePS;

//other data
protected:
	//position of the mouse in screenspace
	glm::vec2 mousePos;
	//the ammount the main player should be rotated
	glm::vec2 rotAmmount; 
	 //the direction the cannon is currently facing
	glm::vec3 playerDir;
	//a multiplier for the ammount of force should be applied to a cannonball when it is fired
	float cannonBallForce; 
	//the ammount of time in seconds that the player has to wait between shots
	float playerShootCooldown;
	float playerShootCooldownTimer;

	//smoke particle
	TTN_ParticleTemplate smokeParticle;

//set up functions, called by InitScene()
protected:
	void SetUpAssets();
	void SetUpEntities();
	void SetUpOtherData();

//update functions, called by Update()
protected:
	void PlayerRotate(float deltaTime);
	void StopFiring();

//other functions, ussually called in relation to something happening like player input or a collision
protected:
	void CreateCannonball();
	void DeleteCannonballs(); 
};

inline float SmoothStep(float t) {
	return (t * t * (3.0f - 2.0f * t));
}

inline float FastStart(float t) {
	return std::cbrt(t);
}

inline float SlowStart(float t) {
	return (t * t * t);
}