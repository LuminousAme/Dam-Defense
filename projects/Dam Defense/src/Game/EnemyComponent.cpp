//Dam Defense, by Atlas X Games
//EnemyComponent.h, the source file for the class that acts as a component for the indivual enemy logic
#include "EnemyComponent.h"

EnemyComponent::EnemyComponent()
{
	m_entityNumber = entt::null;
	m_scene = nullptr;
	m_boatType = 0;
	m_path = 0;
	m_damageCooldown = 0.0f;
	m_ypos = 7.5f;
	m_alive = true;
	m_attacking = false;
	m_diff = 1.0f;
	m_cannonEntityRef = entt::null;
}

EnemyComponent::EnemyComponent(entt::entity boat, TTN_Scene* scene, int boatType, int path, float damageCooldown)
	: m_entityNumber(boat), m_scene(scene), m_boatType(boatType), m_path(path), m_damageCooldown(damageCooldown), m_ypos(0.0f), m_alive(true), m_attacking(false), m_diff(1.0f),
	m_cannonEntityRef(entt::null)
{
	//set the y position as approriate based on ship model
	switch (m_boatType) {
	case 0:
		m_ypos = -8.5f;
		break;
	case 1:
		m_ypos = -8.0f;
		break;
	case 2:
		m_ypos = -7.5f;
		break;
	}
}

//function to update the movement, position, rotation, etc. of the boat
void EnemyComponent::Update(float deltaTime)
{
	//if this enemy is alive, head towards the dam
	if (m_alive) {
		auto& pBoat = m_scene->Get<TTN_Physics>(m_entityNumber);
		auto& tBoat = m_scene->Get<TTN_Transform>(m_entityNumber);

		//left side middle path
		if (m_path == 0) {
			if (tBoat.GetPos().x <= 65.f) {
				//rotation for the green boats
				if (m_boatType == 0) {
					if (tBoat.GetRotation().y <= 75.0f)
						tBoat.RotateFixed(glm::vec3(0.0f, -0.55f * 60.0f * deltaTime, 0.0f));
				}

				//rotation for the red boats
				else if (m_boatType == 1) {
					if (tBoat.GetRotation().y <= -20.0f)
						tBoat.RotateFixed(glm::vec3(0.0f, -0.55f * 60.0f * deltaTime, 0.0f));
				}

				//rotation for the yellow boats
				else if (m_boatType == 2) {
					if (tBoat.GetRotation().y >= 20.0f)
						tBoat.RotateFixed(glm::vec3(0.0f, -0.55f * 60.0f * deltaTime, 0.0f));
				}

				//boat movememnt
				glm::vec3 tar = Arrive(glm::vec3(8.0f, m_ypos, m_ztarget), pBoat.GetLinearVelocity(), tBoat.GetPos(), m_targetDistance);
				(tar == glm::vec3(0.0f)) ? pBoat.SetLinearVelocity(glm::vec3(0.0f)) : pBoat.AddForce(tar);
			}
		}

		//far left path
		if (m_path == 1) {
			if (tBoat.GetPos().x <= 75.f) {
				//green boat rotation
				if (m_boatType == 0) {
					if (tBoat.GetRotation().y <= 83.0f)
						tBoat.RotateFixed(glm::vec3(0.0f, -0.75f * 60.0f * deltaTime, 0.0f));
				}

				//red boat rotation
				else if (m_boatType == 1) {
					if (tBoat.GetRotation().y <= -1.0f)
						tBoat.RotateFixed(glm::vec3(0.0f, -0.7f * 60.0f * deltaTime, 0.0f));
				}

				//yellow boat rotation
				else if (m_boatType == 2) {
					if (tBoat.GetRotation().y >= 1.0f)
						tBoat.RotateFixed(glm::vec3(0.0f, -0.7f * 60.0f * deltaTime, 0.0f));
				}

				//boat movememnet
				glm::vec3 tar = Arrive(glm::vec3(40.0f, m_ypos, m_ztarget), pBoat.GetLinearVelocity(), tBoat.GetPos(), m_targetDistance);
				(tar == glm::vec3(0.0f)) ? pBoat.SetLinearVelocity(glm::vec3(0.0f)) : pBoat.AddForce(tar);
			}
		}

		//center left
		if (m_path == 2) {
			//first part
			if (tBoat.GetPos().x <= 65.f && !(tBoat.GetPos().x <= 5.f)) {
				//green boat rotation
				if (m_boatType == 0) {
					if (tBoat.GetRotation().y <= 65.0f)
						tBoat.RotateFixed(glm::vec3(0.0f, -0.6f * 60.0f * deltaTime, 0.0f));
				}

				//red boat rotation
				else if (m_boatType == 1) {
					if (tBoat.GetRotation().y <= 15.0f)
						tBoat.RotateFixed(glm::vec3(0.0f, -0.15f * 60.0f * deltaTime, 0.0f));
				}

				//yellow boat rotation
				else if (m_boatType == 2) {
					if (tBoat.GetRotation().y >= 15.0f)
						tBoat.RotateFixed(glm::vec3(0.0f, -0.15f * 60.0f * deltaTime, 0.0f));
				}

				//boat movement
				pBoat.AddForce(Seek(glm::vec3(5.0f, m_ypos, 55.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));
			}

			//second part
			if (tBoat.GetPos().x <= 5.f) {
				//green boat rotation
				if (m_boatType == 0) {
					if (tBoat.GetRotation().y <= 89.0f)
						tBoat.RotateFixed(glm::vec3(0.0f, -0.95f * 60.0f * deltaTime, 0.0f));
				}

				//red boat rotation
				else if (m_boatType == 1) {
					if (tBoat.GetRotation().y <= 15.0f)
						tBoat.RotateFixed(glm::vec3(0.0f, -0.08f * 60.0f * deltaTime, 0.0f));
				}

				//yellow boat rotation
				else if (m_boatType == 2) {
					if (tBoat.GetRotation().y >= 1.0f)
						tBoat.RotateFixed(glm::vec3(0.0f, -0.08f * 60.0f * deltaTime, 0.0f));
				}

				glm::vec3 tar = Arrive(glm::vec3(4.0f, m_ypos, m_ztarget), pBoat.GetLinearVelocity(), tBoat.GetPos(), m_targetDistance);
				(tar == glm::vec3(0.0f)) ? pBoat.SetLinearVelocity(glm::vec3(0.0f)) : pBoat.AddForce(tar);
			}
		}

		//right middle path
		if (m_path == 3) {
			if (tBoat.GetPos().x >= -65.F) {
				//green boat rotation
				if (m_boatType == 0) {
					if (tBoat.GetRotation().y <= 69.0f)
						tBoat.RotateFixed(glm::vec3(0.0f, 0.5f * 60.0f * deltaTime, 0.0f));
				}

				//red boat rotation
				else if (m_boatType == 1) {
					if (tBoat.GetRotation().y >= 20.0f)
						tBoat.RotateFixed(glm::vec3(0.0f, 0.55f * 60.0f * deltaTime, 0.0f));
				}

				//yellow boat rotation
				else if (m_boatType == 2) {
					if (tBoat.GetRotation().y <= -20.0f)
						tBoat.RotateFixed(glm::vec3(0.0f, 0.55f * 60.0f * deltaTime, 0.0f));
				}

				//boat movement
				glm::vec3 tar = Arrive(glm::vec3(-8.0f, m_ypos, m_ztarget), pBoat.GetLinearVelocity(), tBoat.GetPos(), m_targetDistance);
				(tar == glm::vec3(0.0f)) ? pBoat.SetLinearVelocity(glm::vec3(0.0f)) : pBoat.AddForce(tar);
			}
		}

		//far right path
		if (m_path == 4) {
			if (tBoat.GetPos().x >= -75.f) {
				//green boat rotation
				if (m_boatType == 0) {
					if (tBoat.GetRotation().y <= 83.0f)
						tBoat.RotateFixed(glm::vec3(0.0f, 0.75f * 60.0f * deltaTime, 0.0f));
				}

				//red boat rotation
				else if (m_boatType == 1) {
					if (tBoat.GetRotation().y >= 1.0f)
						tBoat.RotateFixed(glm::vec3(0.0f, 0.7f * 60.0f * deltaTime, 0.0f));
				}

				//yellow boat rotation
				else if (m_boatType == 2) {
					if (tBoat.GetRotation().y <= -1.0f)
						tBoat.RotateFixed(glm::vec3(0.0f, 0.7f * 60.0f * deltaTime, 0.0f));
				}

				//boat movement
				glm::vec3 tar = Arrive(glm::vec3(-40.0f, m_ypos, m_ztarget), pBoat.GetLinearVelocity(), tBoat.GetPos(), m_targetDistance);
				(tar == glm::vec3(0.0f)) ? pBoat.SetLinearVelocity(glm::vec3(0.0f)) : pBoat.AddForce(tar);
			}
		}

		//right center path
		if (m_path == 5) {
			//part 1
			if (tBoat.GetPos().x >= -65.f && !(tBoat.GetPos().x >= -5.F)) {
				//green boat rotation
				if (m_boatType == 0) {
					if (tBoat.GetRotation().y <= 65.0f)
						tBoat.RotateFixed(glm::vec3(0.0f, 0.6f * 60.0f * deltaTime, 0.0f));
				}

				//red boat rotation
				else if (m_boatType == 1) {
					if (tBoat.GetRotation().y >= 15.0f)
						tBoat.RotateFixed(glm::vec3(0.0f, 0.20f * 60.0f * deltaTime, 0.0f));
				}

				//yellow boat rotation
				else if (m_boatType == 2) {
					if (tBoat.GetRotation().y <= -15.0f)
						tBoat.RotateFixed(glm::vec3(0.0f, 0.15f * 60.0f * deltaTime, 0.0f));
				}

				//boat movement
				pBoat.AddForce(Seek(glm::vec3(-5.0f, m_ypos, 55.0f), pBoat.GetLinearVelocity(), tBoat.GetPos()));
			}

			//part 2
			if (tBoat.GetPos().x >= -5.f) {
				//green boat rotation
				if (m_boatType == 0) {
					if (tBoat.GetRotation().y <= 89.0f)
						tBoat.RotateFixed(glm::vec3(0.0f, 0.95f * 60.0f * deltaTime, 0.0f));
				}

				//red boat rotation
				else if (m_boatType == 1) {
					if (tBoat.GetRotation().y <= 15.0f)
						tBoat.RotateFixed(glm::vec3(0.0f, 0.08f * 60.0f * deltaTime, 0.0f));
				}

				//yellow boat rotation
				else if (m_boatType == 2) {
					if (tBoat.GetRotation().y <= -1.0f)
						tBoat.RotateFixed(glm::vec3(0.0f, 0.08f * 60.0f * deltaTime, 0.0f));
				}

				//boat movement
				glm::vec3 tar = Arrive(glm::vec3(-4.0f, m_ypos, m_ztarget), pBoat.GetLinearVelocity(), tBoat.GetPos(), m_targetDistance);
				(tar == glm::vec3(0.0f)) ? pBoat.SetLinearVelocity(glm::vec3(0.0f)) : pBoat.AddForce(tar);
			}
		}
	}
	//otherwise sink the ship
	else {
		//grab a refernece to it's transform
		auto& tBoat = m_scene->Get<TTN_Transform>(m_entityNumber);

		//if the enemy is on the left
		if (m_path < 3) {
			//rotate the enemy ship
			if (tBoat.GetRotation().z < 75.0f) {
				tBoat.RotateFixed(glm::vec3(0.0f, 0.0f, 90.0f * 0.4f * deltaTime));
			}
		}
		//otherwise they must be on the right
		else {
			//rotate the enemy ship
			if (tBoat.GetRotation().z > -75.0f) {
				tBoat.RotateFixed(glm::vec3(0.0f, 0.0f, -90.0f * 0.4f * deltaTime));
			}
		}

		//sink the ship
		tBoat.SetPos(tBoat.GetPos() + glm::vec3(0.0f, -2.0f * deltaTime, 0.0f));
	}
}

//function to seek a targetted position, returns a velocity towards that position
glm::vec3 EnemyComponent::Seek(glm::vec3 target, glm::vec3 currentVelocity, glm::vec3 currentPosition)
{
	//base restriction (-10, 0 ,-10)
	//glm::vec3 maxVelo(-20.0f, 0.0f, -24.0f); //fast version
	glm::vec3 maxVelo(-10.0f * (m_diff / 100.f), 0.0f, -12.0f * (m_diff / 100.f));

	//gets the desired vector
	glm::vec3 desired = (currentPosition - target);
	desired = glm::normalize(desired) * maxVelo;
	glm::vec3 steering = desired - currentVelocity;
	glm::vec3 moveVelo = steering;

	//return it
	return moveVelo;
}

//function to arrive a targetted position, seeking it until it is within an acceptable distance, and then stopping
glm::vec3 EnemyComponent::Arrive(glm::vec3 target, glm::vec3 currentVelocity, glm::vec3 currentPosition, float accetpableDistance)
{
	//if they are within an acceptable distance of the target position, return a zeroed vector
	if (abs(glm::distance(target, currentPosition)) <= accetpableDistance) {
		m_attacking = true;
		return glm::vec3(0.0f);
	}
	//otherwise seek the target position
	return Seek(target, currentVelocity, currentPosition);
}