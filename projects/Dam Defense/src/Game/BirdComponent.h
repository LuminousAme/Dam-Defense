//Dam Defense, by Atlas X Games
//EnemyComponent.h, the header file for the class that acts as a component for the indivual enemy logic
#pragma once

//include required features from titan
#include "Titan/Application.h"
#include "Titan/Utilities/Interpolation.h"

using namespace Titan;

//Bird component class
class BirdComponent {
public:
	//default constructor
	BirdComponent();
	//default destructor
	~BirdComponent() = default;
	//constructor with data
	BirdComponent(entt::entity bird, TTN_Scene* scene, float neighborhood, float speed, float diveSpeed, float aliWeight, float cohWeight, float sepWeight, float corWeight, float DiveWeight);

	//setters
	void SetBirdsVector(std::vector<entt::entity> birds) { m_allBirds = birds; }
	void SetTarget(entt::entity target = entt::null) { m_target = target; }
	void SetNeighborhoodDistance(float neighborhood) { m_neighborhoodDistance = neighborhood; }
	void SetSpeed(float speed) { m_speed = speed; }
	void SetDiveSpeed(float diveSpeed) { m_diveSpeed = diveSpeed; }
	void SetAlignmentWeight(float aliWeight) { m_AlignmentWeight = aliWeight; }
	void SetCohesionWeight(float cohWeight) { m_CohesionWeight = cohWeight; }
	void SetSeparationWeight(float sepWeight) { m_SeperationWeight = sepWeight; }
	void SetCorrectionWeight(float corWeight) { m_CorrectionWeight = corWeight; }
	void SetDiveWeight(float diveWeight) { m_DiveWeight = diveWeight; }

	//getters
	std::vector<entt::entity> GetBirdsVector() { return m_allBirds; }
	entt::entity GetTarget() { return m_target; }
	float GetNeighborhoodDistance() { return m_neighborhoodDistance; }
	float GetSpeed() { return m_speed; }
	float GetDiveSpeed() { return m_diveSpeed; }
	float GetAlignmentWeight() { return m_AlignmentWeight; }
	float GetCohensionWeight() { return m_CohesionWeight; }
	float GetSeparationWeight() { return m_SeperationWeight; }
	float GetCorrectionWeight() { return m_CorrectionWeight; }
	float GetDiveWeight() { return m_DiveWeight; }

	//update the bird's physics once a frame
	void Update(float deltaTime);

	//steering behaviours
	glm::vec3 NormalizedSeek(glm::vec3 target, glm::vec3 currentVelocity, glm::vec3 currentPosition, float speed);

private:
	entt::entity m_entityNumber; //the entity number for the current bird
	TTN_Scene* m_scene; //the scene this bird is in, used to access transforms and physics components
	std::vector<entt::entity> m_allBirds; //vector of all the birds in the scene
	float m_neighborhoodDistance; //the distance of the bird's neighbourhood
	entt::entity m_target = entt::null; //the target boat the birds are dive bombing

	float m_AcutalSpeed; //the real speed of the bird based on the base and dive speeds
	bool m_currentlyBombing; //wheter or not the bird is currently dive bombing a ship
	float m_timeSinceStateChange; //the time since the last time it changed between bombing or not bombing, used to control acutal speed

	float m_speed; //the speed of the bird
	float m_diveSpeed; //the speed of the bird when it's dive bombing an enemy shit
	float m_AlignmentWeight; //a multiplier for the allignment component of the boid behavoir
	float m_CohesionWeight; //a multiplier for the cohension component of the boid behavoir
	float m_SeperationWeight; //a multiplier for the separation of the boid behavoir
	float m_CorrectionWeight; //a multiplier for the correction component of the boid behaviour that tries to keep the birds within certain axises
	float m_DiveWeight; //a multipler for the bird bomb diving component of the boid behaviour that targets a given ship
};