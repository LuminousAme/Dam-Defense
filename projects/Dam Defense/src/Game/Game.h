//Dam Defense, by Atlas X Games
//Game.h, the header file for the class that represents the main gameworld scene
#pragma once

//include required features from titan
#include "Titan/Application.h"
#include "Titan/Systems/Sound.h"
#include "EnemyComponent.h"
#include "BirdComponent.h"

using namespace Titan;

class Game : public TTN_Scene {
public:
	//default constructor
	Game();

	//default destrcutor
	~Game() = default;

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
	void MouseButtonUpChecks();

	bool GetShouldShop() { return shouldShop; };
	void SetShouldShop(bool menu) { shouldShop = menu; }

	bool GetGameIsPaused() { return m_paused; }
	void SetGameIsPaused(bool paused) { m_paused = paused; }

	bool GetGameIsOver() { return m_gameOver; }
	void SetGameIsOver(bool over) { m_gameOver = over; }

	bool GetGameWin() { return m_gameWin; }
	void SetGameWin(bool win) { m_gameWin = win; }

	bool GetGameShouldRestart() { return m_restart; }
	void SetGameShouldRestart(bool restart) { m_restart = restart; }

	float GetDamHealth() { return Dam_health; }

	float GetMouseSensitivity() { return mouseSensetivity; }
	void SetMouseSensitivity(float sensitivity) { mouseSensetivity = sensitivity; }

	void SetMasterVolume(float vol) { masterVolume = (int)vol; }
	void SetMusicVolume(float vol) { musicVolume = (int)vol; }
	void SetSFXVolume(float vol) { sfxVolume = (int)vol; }

	void SetDiff(float diff) { difficulty = diff; }
	float GetDiff() { return difficulty; }

	void SetNoLut(bool lut) {
		if (lut) {
			m_applyWarmLut = false;
			m_applyCoolLut = false;
			m_applyCustomLut = false;
		}
	}

	void SetWarmLut(bool lut) { m_applyWarmLut = lut; }

	float GetFlameThrowerCoolDownTime() {
		if (Flaming)
			return std::clamp(TTN_Interpolation::ReMap(FlameActiveTime, 0.0f, FlameThrowerCoolDown, FlameActiveTime, FlameAnim), 0.0f, FlameTimer);

		return std::clamp(FlameTimer, 0.0f, FlameThrowerCoolDown);
	}
	float GetRealFlameThrowerCoolDownTime() {
		return std::clamp(FlameTimer, 0.0f, FlameThrowerCoolDown);
	}
	float GetFlameThrowerMaxCoolDownTime() { return FlameThrowerCoolDown; }

	float GetBirdCoolDownTime() {
		if (BombTimer > (BirdBombCooldown - FlameActiveTime))
			return std::clamp(TTN_Interpolation::ReMap(FlameActiveTime, 0.0f, BirdBombCooldown, FlameActiveTime, BirdBombCooldown - BombTimer), 0.0f, BombTimer);

		return std::clamp(BombTimer, 0.0f, BirdBombCooldown);
	}
	float GetRealBirdCoolDownTime() {
		return std::clamp(BombTimer, 0.0f, BirdBombCooldown);
	}
	float GetBirdBombMaxCoolDown() { return BirdBombCooldown; }

	float GetWaveProgress() {
		if (m_firstWave)
			return 0.0f;

		float percentBoatsRemaining = (float)m_boatsRemainingThisWave / (float)(m_enemiesPerWave * m_currentWave);
		return TTN_Interpolation::ReMap(1.0f, 0.0f, 0.0f, 1.0f, percentBoatsRemaining);
	}

	unsigned GetScore() { return m_score; }
	void SetScore(unsigned score) { m_score = score; }

	unsigned GetWave() { return m_currentWave; }
	bool GetWaveOver() { return playJingle; }

	void SetArcade(bool arcade) { m_arcade = arcade; }
	bool GetArcade() { return m_arcade; }

	//function to restart the game reseting all the data
	void RestartData();
	//Assets
public:
#pragma region assets
	//shader programs
	TTN_Shader::sshptr shaderProgramUnTextured;
	TTN_Shader::sshptr shaderProgramTextured;
	TTN_Shader::sshptr shaderProgramSkybox;
	TTN_Shader::sshptr shaderProgramAnimatedTextured;
	TTN_Shader::sshptr shaderProgramWater;
	TTN_Shader::sshptr shaderProgramTerrain;
	TTN_Shader::sshptr shaderDepth;

	//meshes
	TTN_Mesh::smptr cannonMesh;
	TTN_Mesh::smptr boat1Mesh;
	TTN_Mesh::smptr boat2Mesh;
	TTN_Mesh::smptr boat3Mesh;
	TTN_Mesh::smptr flamethrowerMesh;
	TTN_Mesh::smptr birdMesh;
	TTN_Mesh::smptr enemyCannonMesh;
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
	TTN_Texture2D::st2dptr enemyCannonText;

	//materials
	TTN_Material::smatptr boat1Mat;
	TTN_Material::smatptr boat2Mat;
	TTN_Material::smatptr boat3Mat;
	TTN_Material::smatptr flamethrowerMat;
	TTN_Material::smatptr birdMat;
	TTN_Material::smatptr treeMat;
	TTN_Material::smatptr rockMat;
	TTN_Material::smatptr damMat;
	TTN_Material::smatptr enemyCannonMat;
	std::vector<TTN_Material::smatptr> m_mats;

	TTN_Material::smatptr cannonMat;
	TTN_Material::smatptr skyboxMat;
	TTN_Material::smatptr smokeMat;
	TTN_Material::smatptr fireMat;
	TTN_Texture2D::st2dptr healthBar;

	//sound
	TTN_AudioEngine& engine = TTN_AudioEngine::Instance();

	TTN_AudioEventHolder::saehptr m_cannonFiringSounds;
	TTN_AudioEventHolder::saehptr m_splashSounds;
	TTN_AudioEventHolder::saehptr m_music;
	TTN_AudioEventHolder::saehptr m_jingle;
	TTN_AudioEventHolder::saehptr m_flamethrowerSound;

#pragma endregion

	//Entities
protected:
	entt::entity camera;
	entt::entity light;
	entt::entity skybox;
	entt::entity cannon;
	std::vector<std::pair<entt::entity, bool>> cannonBalls;
	std::vector<entt::entity> boats;
	std::vector<entt::entity> enemyCannons;//enemy
	entt::entity smokePS;
	entt::entity terrain;
	entt::entity water;
	//entt::entity birds[3];
	std::vector<entt::entity> birds;
	entt::entity dam;

	std::vector<entt::entity> flamethrowers;
	std::vector<entt::entity> flames;

	/////// OTHER DATA ///////////
#pragma region Data
protected:
	bool shouldShop;
	/////// Player control data/////////
	float cannonBallForce = 3600.0f;;//a multiplier for the ammount of force should be applied to a cannonball when it is fired
	float playerShootCooldown = 0.7f;//the ammount of time in seconds that the player has to wait between shots

	glm::vec2 mousePos;//position of the mouse in screenspace
	glm::vec2 rotAmmount;//the ammount the main player should be rotated
	glm::vec3 playerDir;//the direction the cannon is currently facing
	float playerShootCooldownTimer;//how much time until the player can shoot again
	float mouseSensetivity = 50.0f; //the mouse senestivity for the player
	bool firstFrame; //if it's the first frame of the game

	//////// GAMEPLAY DATA ////////////
	int lastWave = 3; //the wave the player needs to reach and beat to win
	float damage = 2.0f; //damage of boats (dam health is 100.f)
	unsigned m_score = 0;

	/////// Terrain and water control data ////////
	float terrainScale = 0.15f;//the terrain scale

	float water_time; //the current time for the water, used in it's sin wave
	float water_waveSpeed;//the speed of the waves
	float water_waveBaseHeightIncrease;//the base height of the waves
	float water_waveHeightMultiplier;//how much the waves should grow
	float water_waveLenghtMultiplier;//how long the waves should be

	//////// DAM FLAMETHROWER, AND BIRD BOMB CONTROL DATA ///////
	float FlameThrowerCoolDown = 30.0f; //how long the player has to wait between flamethrower uses
	float FlameActiveTime = 3.0f; //how long the flamethrower lasts
	float BirdBombCooldown = 15.0f; //how long the player has to wait between bird bomb uses
	const float Dam_MaxHealth = 100; //the maximum health of the dam

	bool Flaming; //if flamethrowers are active right now
	float FlameTimer; //flamethrower cooldown
	float FlameAnim; //flamethrower duration
	bool Bombing; //if the birds are currently seeking after a boat
	float BombTimer; //bird bomb cooldown
	float Dam_health;//the current health on the dam

	//////// BIRD CONTROL DATA ///////////////
	float birdNeighbourHoodDistance = 10.0f;
	float birdBaseSpeed = 5.0f;
	float birdDiveSpeed = 25.0f;
	float birdAligmentWeight = 0.5f;
	float birdCohensionWeight = 0.75f;
	float birdSeperationWeight = 1.25f;
	float birdCorrectionWeight = 0.05f;
	float birdDiveWeight = 2.0f;

	int birdNum = 20; //number of birds

	///////////SCENE CONTROL DATA///////////
	bool m_paused; //wheter or not the scene is paused
	bool m_gameOver = false; //wheter or not the player has yet gameover
	bool m_gameWin = false;//wheter or not the player has won
	bool m_restart;//wheter or not the game is restarting
	bool m_arcade = false; //wheter or not the game is in arcade mode
	float m_InputDelayTime = 0.3f; //the ammount of time before it accepts player input, used when hte player is moving in and out of the scene
	float m_InputDelay; //the time remaining before it accepts player input, used when the player is moving in and out of the scene

	/////////////ENEMY AND WAVE CONTROLS//////////////////
	float muzzleFlashCD = 1.50f; // time for muzzle flash
	float m_timeBetweenEnemyWaves = 5.0f; //rest time between waves
	float m_timeBetweenEnemySpawns = 2.0f; //cooldown between when boats spawn
	int m_enemiesPerWave = 5; //how many enemy enemies should it add to each wave, so wave number * this is the number of enemies in any given wave

	int m_currentWave = 0; //the current wave
	float m_timeTilNextWave; //the timer until the next wave starts, used after a wave has ended
	float m_timeUntilNextSpawn; //the timer until the next boat spawns
	int m_boatsRemainingThisWave; //the number of boats that need to be destoryed before the wave starts again
	int m_boatsStillNeedingToSpawnThisWave; //the number of boats that still need to be spawned before the wave can end
	bool m_rightSideSpawn = true; //wheter or not it should be using the right (true) or left (false) spawner
	bool m_waveInProgress;
	bool m_firstWave = true;
	float difficulty = 100.0f;

	/////////// SOUND CONTROL///////////////
	//control melody
	float melodyTimeTracker = 0.0f;
	int timesMelodiesPlayed = 0;
	int timesMelodyShouldPlay; //will be random between 2 and 3 every time
	bool melodyFinishedThisFrame = false;
	bool partialMelody = false;
	bool fullMelodyFinishedThisFrame = false;

	//jingle controls
	bool playJingle = false;
	float timeSinceJingleStartedPlaying = 0.0f;

	//volume controls
	int masterVolume = 100;
	int musicVolume = 20;
	int sfxVolume = 5;

#pragma endregion

	///////PARTICLE TEMPLATES//////////
	TTN_ParticleTemplate smokeParticle;//smoke burst particles
	TTN_ParticleTemplate fireParticle;//fire particles
	TTN_ParticleTemplate expolsionParticle;//expolsion particles
	TTN_ParticleTemplate birdParticle;//bird expolsion particles
	TTN_ParticleTemplate gunParticle;//enemy ship gun particles

	//set up functions, called by InitScene()
protected:
	void SetUpAssets();
	void SetUpEntities();
	void SetUpOtherData();

	//update functions, called by Update()
protected:
	void PlayerRotate(float deltaTime);
	void StopFiring();

	//functions for spawning enemies
	void SpawnBoatLeft();
	void SpawnBoatRight();
	void WaveUpdate(float deltaTime);

	//flamethrowers, collisions, and damages
	void Flamethrower();
	void FlamethrowerUpdate(float deltaTime);
	void Collisions();
	void Damage(float deltaTime);

	//sounds
	void GameSounds(float dt);
	//misc
	void BirdBomb();
	void MakeABird();
	void BirdUpate(float deltaTime);
	void ImGui();

	//other functions, ussually called in relation to something happening like player input or a collision
protected:
	void CreateCannonball();
	void DeleteCannonballs();

	void CreateExpolsion(glm::vec3 location);
	void CreateBirdExpolsion(glm::vec3 location);
	void CreateMuzzleFlash(glm::vec3 location, entt::entity e);

	//CG assingment 2 stuff
protected:
	//color correction effect
	TTN_ColorCorrect::scolcorptr m_colorCorrectEffect;
	//bools for imgui controls
	bool m_applyWarmLut;
	bool m_applyCoolLut;
	bool m_applyCustomLut;

	bool m_noLighting;
	bool m_ambientOnly;
	bool m_specularOnly;
	bool m_ambientAndSpecular;
	bool m_ambientSpecularAndOutline;
	//float to control outline size
	float m_outlineSize = 0.2f;

	//variables for if the specular and diffuse ramps should be used
	bool m_useDiffuseRamp = false;
	bool m_useSpecularRamp = false;
	bool m_useTextures = true;
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

inline float ZeroOneZero(float t) {
	return (-4.0f * (t * t) + 4.0f * t);
}