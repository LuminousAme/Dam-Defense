//Dam Defense, by Atlas X Games
//Game.h, the header file for the class that represents the main gameworld scene
#pragma once

//include required features from titan
#include "Titan/Application.h"
#include "Titan/ObjLoader.h"
#include "Titan/Interpolation.h"

using namespace Titan;

class Game : public TTN_Scene {
public:
	//default constructor
	Game();

	//sets up the scene
	void InitScene();

	//update the scene
	void Update(float deltaTime);

	//late render (terrain, water, etc.)
	void PostRender();

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
	TTN_Shader::sshptr shaderProgramWater;
	TTN_Shader::sshptr shaderProgramTerrain;

	//meshes
	TTN_Mesh::smptr cannonMesh;
	TTN_Mesh::smptr boat1Mesh;
	TTN_Mesh::smptr boat2Mesh;
	TTN_Mesh::smptr boat3Mesh;
	TTN_Mesh::smptr flamethrowerMesh;
	TTN_Mesh::smptr birdMesh;
	TTN_Mesh::smptr treeMesh[3];
	TTN_Mesh::smptr rockMesh[5];
	TTN_Mesh::smptr damMesh;

	TTN_Mesh::smptr skyboxMesh;
	TTN_Mesh::smptr sphereMesh; //used for cannonballs and particles
	TTN_Mesh::smptr terrainPlain;

	//textures
	TTN_Texture2D::st2dptr cannonText;
	TTN_TextureCubeMap::stcmptr skyboxText;
	TTN_Texture2D::st2dptr terrainMap;
	TTN_Texture2D::st2dptr sandText;
	TTN_Texture2D::st2dptr rockText;
	TTN_Texture2D::st2dptr grassText;
	TTN_Texture2D::st2dptr waterText;
	TTN_Texture2D::st2dptr boat1Text;
	TTN_Texture2D::st2dptr boat2Text;
	TTN_Texture2D::st2dptr boat3Text;
	TTN_Texture2D::st2dptr flamethrowerText;
	TTN_Texture2D::st2dptr birdText;
	TTN_Texture2D::st2dptr treeText;
	TTN_Texture2D::st2dptr damText;

	//materials
	TTN_Material::smatptr boat1Mat;
	TTN_Material::smatptr boat2Mat;
	TTN_Material::smatptr boat3Mat;
	TTN_Material::smatptr flamethrowerMat;
	TTN_Material::smatptr birdMat;
	TTN_Material::smatptr treeMat;
	TTN_Material::smatptr rockMat;
	TTN_Material::smatptr damMat;

	TTN_Material::smatptr cannonMat;
	TTN_Material::smatptr skyboxMat;
	TTN_Material::smatptr smokeMat;
	TTN_Material::smatptr fireMat;

//Entities
protected:
	entt::entity camera;
	entt::entity light;
	entt::entity skybox;
	entt::entity cannon;
	std::vector<entt::entity> cannonBalls;
	std::vector<entt::entity> boats;
	entt::entity smokePS;
	entt::entity terrain;
	entt::entity water;
	entt::entity birds[3];
	entt::entity trees[17];
	entt::entity dam;

	std::vector<entt::entity> flamethrowers;
	std::vector<entt::entity> flames;

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
	//the terrain scale
	float terrainScale;
	//water animation control
	float time;
	float waveSpeed;
	float waveBaseHeightIncrease;
	float waveHeightMultiplier;
	float waveLenghtMultiplier;

	//Stuff for waves and spawning enemies
	float Timer = 0.F;//timer for boat spawning (left side)
	float Timer2 = 0.F;//timer for boat spawning (right side)
	bool Spawning = true; //whether or not the spawners should be spawning

	float waveTimer = 0.F;//timer for waves
	float restTimer = 0.F;//timer for waves
	int wave = 0; // keep track of wave number

	bool Flaming = false; //if flamethrowers are active right now
	float FlameTimer = 0.0f; //flamethrower cooldown
	float FlameAnim = 0.0f; //flamethrower

	//bird control
	float birdTimer = 0.0f;
	glm::vec3 birdBase, birdTarget;

	//smoke particle
	TTN_ParticleTemplate smokeParticle;
	TTN_ParticleTemplate fireParticle;

//set up functions, called by InitScene()
protected:
	void SetUpAssets();
	void SetUpEntities();
	void SetUpOtherData();

//update functions, called by Update()
protected:
	void PlayerRotate(float deltaTime);
	void StopFiring();

	void BoatPathing(entt::entity boatt, int path);
	void SpawnerLS(float deltatime, float SpawnTime);
	void SpawnerRS(float deltatime, float SpawnTime);
	void Waves(int num, float restTime, float waveTime, float deltaTime);
	glm::vec3 Seek(glm::vec3 target, glm::vec3 velo, glm::vec3 pos);

	void Flamethrower();
	void Collisions();

	void DeleteFlamethrowers();

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