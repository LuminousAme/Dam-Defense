// Dam Defense, by Atlas X Games
//BirdComponent.h, the source file for the class that acts as a component for the indivual bird logic
#include "BirdComponent.h"

//default constructor
BirdComponent::BirdComponent()
{
	m_entityNumber = entt::null;
	m_scene = nullptr;
	m_target = entt::null;
	m_allBirds = std::vector<entt::entity>();
	m_neighborhoodDistance = 0.0f;
	m_speed = 0.0f;
	m_diveSpeed = 0.0f;
	m_AlignmentWeight = 0.0f;
	m_CohesionWeight = 0.0f;
	m_SeperationWeight = 0.0f;
	m_CorrectionWeight = 0.0f;
	m_DiveWeight = 0.0f;
	m_AcutalSpeed = m_speed;
	m_currentlyBombing = false;
	m_timeSinceStateChange = 10.0f;
}

//constructor with data
BirdComponent::BirdComponent(entt::entity bird, TTN_Scene* scene, float neighborhood, float speed, float diveSpeed, float aliWeight, float cohWeight, float sepWeight, float corWeight, float DiveWeight)
	: m_entityNumber(bird), m_scene(scene), m_neighborhoodDistance(neighborhood), m_speed(speed), m_diveSpeed(diveSpeed),
	m_AlignmentWeight(aliWeight), m_CohesionWeight(cohWeight), m_SeperationWeight(sepWeight), m_CorrectionWeight(corWeight), m_DiveWeight(DiveWeight)
{
	m_target = entt::null;
	m_allBirds = std::vector<entt::entity>();
	m_AcutalSpeed = m_speed;
	m_currentlyBombing = false;
	m_timeSinceStateChange = 10.0f;
}

//update the bird every frame
void BirdComponent::Update(float deltaTime)
{
	//update time since state change
	m_timeSinceStateChange += deltaTime;

	//Get the physics and transform of this bird
	TTN_Transform& thisTrans = m_scene->Get<TTN_Transform>(m_entityNumber);
	TTN_Physics& thisPhyiscs = m_scene->Get<TTN_Physics>(m_entityNumber);

	//the velocity components
	glm::vec3 alignmentComponent = glm::vec3(0.0f);
	glm::vec3 cohensionCenterOfMass = glm::vec3(0.0f);
	glm::vec3 cohensionComponent = glm::vec3(0.0f);
	glm::vec3 separationComponent = glm::vec3(0.0f);
	glm::vec3 seekingComponent = glm::vec3(0.0f);

	//the number of neighbours nearby
	int neighbourCount = 0;

	//the neighbourhood distance
	float neighbourhood = m_neighborhoodDistance;
	if (!m_currentlyBombing && m_timeSinceStateChange < 3.0f) {
		neighbourhood *= 10.0f;
	}

	//loop through every other bird
	for (auto bird : m_allBirds) {
		//if the bird is not this bird
		if (m_entityNumber != bird) {
			//and if the bird is in the neighbour hood
			TTN_Transform& birdTrans = m_scene->Get<TTN_Transform>(bird);
			if (glm::distance(thisTrans.GetGlobalPos(), birdTrans.GetGlobalPos()) <= neighbourhood) {
				//increase the neighbour count
				neighbourCount++;

				//grab a reference to their phyiscs component
				TTN_Physics& birdPhys = m_scene->Get<TTN_Physics>(bird);

				//add the birds velocity to the alingment component
				alignmentComponent += birdPhys.GetLinearVelocity();

				//add the bird's position to the cohension's center of mass
				cohensionCenterOfMass += birdTrans.GetGlobalPos();

				//add the difference between the birds and this bird's position to the seperation component
				separationComponent += birdTrans.GetGlobalPos() - thisTrans.GetGlobalPos();
			}
		}
	}

	//now that we have the base information from the entire neigbourhood we can form the acutal vectors
	if (neighbourCount > 0) {
		//make sure the alignment component is acutally averaged and then normalize it
		alignmentComponent = glm::normalize(alignmentComponent / (float)neighbourCount);
		//make sure the cohension center of mass is acutally average, then turn it into the velocity towards that point, and normalize it
		cohensionComponent = glm::normalize((cohensionCenterOfMass / (float)neighbourCount) - thisTrans.GetGlobalPos());
		//make sure the separation component is averaged, negated, and then normalized
		separationComponent = glm::normalize(-1.0f * (separationComponent / (float)neighbourCount));
	}

	//get the current velocity and sum the steering behaviours into it
	glm::vec3 currentVelo = thisPhyiscs.GetLinearVelocity();

	//now that we have the components for the basic boid behaviour, let's also do the seek behaviour

	//if the bird has a target to bird bomb, then seek that target
	if (m_target != entt::null) {
		//reset timer
		if (!m_currentlyBombing)
			m_timeSinceStateChange = 0.0f;
		//reset flag
		m_currentlyBombing = true;
		//seek
		seekingComponent = NormalizedSeek(m_scene->Get<TTN_Transform>(m_target).GetGlobalPos(), thisPhyiscs.GetLinearVelocity(), thisTrans.GetGlobalPos(), m_diveSpeed);
	}
	//otherwise see if it has to correct for it's current position
	else {
		//reset timer
		if (m_currentlyBombing) {
			m_timeSinceStateChange = 0.0f;
		}

		//reset flag
		m_currentlyBombing = false;

		//correct for position

		//if it's too far in the positive x direction, start moving it towards the negative x direction
		if (thisTrans.GetGlobalPos().x >= 80.0f / 10.0f) {
			seekingComponent.x += -1.0f;
		}
		//if it's too far in the negative x direction, start moving it towards the positive x direction
		else if (thisTrans.GetGlobalPos().x <= -80.0f / 10.0f) {
			seekingComponent.x += 1.0f;
		}

		//if it's too far in the positive y direction, start moving it towards the negative y direction
		if (thisTrans.GetGlobalPos().y >= 35.0f / 10.0f && currentVelo.y > -0.6f / 10.0f) {
			seekingComponent.y += -1.0f;
		}
		//if it's too far in the negative y direction, start moving it towards the positive y direction
		else if (thisTrans.GetGlobalPos().y <= 15.0f / 10.0f && currentVelo.y < 0.6f / 10.0f) {
			seekingComponent.y += 1.0f;
		}
		else if (thisTrans.GetGlobalPos().y > 15.0f / 10.0f && thisTrans.GetGlobalPos().y < 35.0f / 10.0f && std::abs(currentVelo.y) > 0.6f / 10.0f) {
			seekingComponent -= glm::normalize(currentVelo).y;
		}

		//if it's too far in the positve z direction, start moving it towards the negative z direction
		if (thisTrans.GetGlobalPos().z >= 110.0f / 10.0f) {
			seekingComponent.z += -1.0f;
		}
		//if it's too far in the negative z direction, start moving it towards the positive z direction
		else if (thisTrans.GetGlobalPos().z <= 10.0f / 10.0f) {
			seekingComponent.z += 1.0f;
		}

		//and normalize the seeking component
		if (seekingComponent != glm::vec3(0.0f)) seekingComponent = glm::normalize(seekingComponent);
	}

	//if it is too low force it to come back up
	if (thisTrans.GetGlobalPos().y < 0.0f) {
		if (m_currentlyBombing && thisTrans.GetGlobalPos().y < -8.0f / 10.0f)
			currentVelo.y = 0.0f;

		thisTrans.SetPos(glm::vec3(thisTrans.GetGlobalPos().x, std::clamp(thisTrans.GetGlobalPos().y, -8.0f / 10.0f, 35.0f / 10.0f), thisTrans.GetGlobalPos().z));
	}

	//if it's not bombing and has only been a few seconds since last bombing, turn off the allignment and cohension components and clamp the y value of the velocity
	if (!m_currentlyBombing && m_timeSinceStateChange < 1.0f) {
		alignmentComponent = glm::vec3(0.0f);
		cohensionComponent = glm::vec3(0.0f);
	}

	//add in the boid behaviour
	currentVelo += m_AlignmentWeight * alignmentComponent + m_CohesionWeight * cohensionComponent + m_SeperationWeight * separationComponent;

	//add in the seeking behvaiour, normalize, and multiply by approriate speed
	if (m_target != entt::null) {
		currentVelo -= m_DiveWeight * seekingComponent;
		m_AcutalSpeed = TTN_Interpolation::Lerp(m_speed, m_diveSpeed, TTN_Interpolation::ReMap(0.0f, 0.1f, 0.0f, 1.0f, std::clamp(m_timeSinceStateChange, 0.0f, 0.1f)));
	}
	else {
		currentVelo += m_CorrectionWeight * seekingComponent;
		m_AcutalSpeed = TTN_Interpolation::Lerp(m_diveSpeed, m_speed, TTN_Interpolation::ReMap(0.0f, 3.0f, 0.0f, 1.0f, std::clamp(m_timeSinceStateChange, 0.0f, 3.0f)));
	}

	currentVelo = m_AcutalSpeed * glm::normalize(currentVelo);

	//have it go up further if it's right after the dive bombing
	if (!m_currentlyBombing && m_timeSinceStateChange < 2.0f) {
		currentVelo += m_SeperationWeight * separationComponent * 2.0f;
		currentVelo.y = 1.0f * m_diveSpeed * (1.0f - TTN_Interpolation::ReMap(0.0f, 2.0f, 0.0f, 1.0f, m_timeSinceStateChange));
	}

	//handle if it's looking straight up
	if (glm::normalize(currentVelo) == glm::vec3(0.0f, 1.0f, 0.0f)) currentVelo += glm::vec3(0.1f / 10.0f, 0.0f, 0.1f / 10.0f);

	//rotate the bird to be facing along the angle it is travelling
	thisTrans.LookAlong(glm::normalize(currentVelo), glm::vec3(0.0f, 1.0f, 0.0f));

	//and set the velocity inside of the physics body
	thisPhyiscs.SetLinearVelocity(currentVelo);
}

//seek behaviour that normalizes it's result before returning
glm::vec3 BirdComponent::NormalizedSeek(glm::vec3 target, glm::vec3 currentVelocity, glm::vec3 currentPosition, float speed)
{
	//gets the desired vector
	glm::vec3 desired = glm::normalize(currentPosition - target) * speed;
	glm::vec3 steering = desired - currentVelocity;

	//return it
	return glm::normalize(steering);
}