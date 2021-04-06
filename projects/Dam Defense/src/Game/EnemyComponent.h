//Dam Defense, by Atlas X Games
//EnemyComponent.h, the header file for the class that acts as a component for the indivual enemy logic
#pragma once

//include required features from titan
#include "Titan/Application.h"
#include "Titan/Utilities/Interpolation.h"

using namespace Titan;

//Enemy component class
class EnemyComponent {
public:
	//default constructor
	EnemyComponent();
	//default destructor
	~EnemyComponent() = default;
	//constructor with data
	EnemyComponent(entt::entity boat, TTN_Scene* scene, int boatType, int path, float damageCooldown);

	//setters
	void SetBoatType(int type) { m_boatType = type; }
	void SetPath(int path) { m_path = path; }
	void SetCooldown(float cooldown) { m_damageCooldown = cooldown; }
	void SetAlive(bool alive) { m_alive = alive; }
	void SetAttacking(bool attacking) { m_attacking = attacking; }
	void SetCannonEntity(entt::entity entity) { m_cannonEntityRef = entity; }
	void SetDifficulty(float d) { m_diff = d; }
	void SetMuzzleCD(float cd) { muzzleFlashCD = cd; }
	//getters
	int GetBoatType() { return m_boatType; }
	int GetDifficulty() { return m_diff; }
	int GetPath() { return m_path; }
	float GetCooldown() { return m_damageCooldown; }
	bool GetAlive() { return m_alive; }
	bool GetAttacking() { return m_attacking; }
	entt::entity GetCannonEntity() { return m_cannonEntityRef; }
	glm::vec3 GetDirection() { return direction; }
	static float GetZTarget() { return m_ztarget; }
	static float GetZTargetDistance() { return m_targetDistance; }
	float GetMuzzleCD() { return muzzleFlashCD; }

	//Updates the boat's physics
	void Update(float deltaTime);

	//steering behaviours
	glm::vec3 Seek(glm::vec3 target, glm::vec3 currentVelocity, glm::vec3 currentPosition);
	glm::vec3 Arrive(glm::vec3 target, glm::vec3 currentVelocity, glm::vec3 currentPosition, float accetpableDistance);

	glm::vec3 Flee(glm::vec3 target, glm::vec3 currentVelocity, glm::vec3 currentPosition);
	glm::vec3 Evade(glm::vec3 targetPos, glm::vec3 targetVelocity, glm::vec3 currentVelocity, glm::vec3 currentPosition);

protected:
	entt::entity m_entityNumber; //the entity number for the current boat, used to access it's transforms and physics
	TTN_Scene* m_scene; //the pointer to the scene the boat is in, used to access the boat's transforms and physics
	int m_boatType; // 0 is green, 1 is red, 2 is yellow
	int m_path; //which path the boat is taking, 0 left side middle, 1 left side far, 2 left side close, 3 right side middle, 4 right side far, 5 right side close
	float m_damageCooldown; //cooldown between the shots from the enemy ships

	float m_diff;// how fast ships go

	float m_ypos; //the y position of the boat, determined by the boat type
	inline static float m_ztarget = 10.0f / 10.0f; //the target z position near the dam that the enemy ship seeks
	inline static float m_targetDistance = 5.0f / 10.0f; //how close it's allowed to get to that point

	bool m_alive; //wheter or not a ship is alive
	bool m_attacking; //whether or not a ship is attacking
	entt::entity m_cannonEntityRef;

	//max velocity of boat
	glm::vec3 maxVelo = glm::vec3 ((-10.0f / 10.0f)* (m_diff / 100.f), 0.0f, (-12.0f / 10.0f)* (m_diff / 100.f));

	glm::vec3 direction; //current direction of the boat
	glm::vec3 deathDirection; //direction the boat was facing when it died
	bool wasAliveLastFrame; //a variable for if the ship was alive last frame
	float timeSinceDeath; //the ammount of time that has passed since the ship died
	float timeSinking = 2.5f; //the ammount of time it should be rotating while it sinks
	float muzzleFlashCD; //cooldown for muzzle flash
};