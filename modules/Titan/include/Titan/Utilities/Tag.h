//Titan Engine, by Atlas X Games 
// Tag.h - header for the class that represents tags to identify entities by
#pragma once

//include required features 
#include "Titan/ttn_pch.h"

namespace Titan {
	class TTN_Tag {
	public:
		//default constructor
		TTN_Tag() { m_label = std::string(); }

		//constructor with data
		TTN_Tag(std::string label) { m_label = label; }

		//default destructor
		~TTN_Tag() = default;

		//copy, move, and assingment constrcutors for ENTT
		TTN_Tag(const TTN_Tag&) = default;
		TTN_Tag(TTN_Tag&&) = default;
		TTN_Tag& operator=(TTN_Tag&) = default;

		//sets the label of the object
		void SetLabel(std::string label) { m_label = label; }

		//gets the name of the object
		std::string getLabel() { return m_label; }


	private:
		//stores the name of the object
		std::string m_label;
	};

	class TTN_Name {
	public:
		//default constructor
		TTN_Name() { m_name = std::string(); }

		//constructor with data
		TTN_Name(std::string name) { m_name = name; }

		//default destructor
		~TTN_Name() = default;

		//copy, move, and assingment constrcutors for ENTT
		TTN_Name(const TTN_Name&) = default;
		TTN_Name(TTN_Name&&) = default;
		TTN_Name& operator=(TTN_Name&) = default;

		//sets the label of the object
		void SetName(std::string name) { m_name = name; }

		//gets the name of the object
		std::string getName() { return m_name; }


	private:
		//stores the name of the object
		std::string m_name;
	};

	class TTN_DeleteCountDown {
	public:
		//default constructor
		TTN_DeleteCountDown() { m_lifeLeft = 0;}

		//constructor with data
		TTN_DeleteCountDown(float lifeTime) { m_lifeLeft = lifeTime; }

		//default destructor
		~TTN_DeleteCountDown() = default;

		//copy, move, and assingment constrcutors for ENTT
		TTN_DeleteCountDown(const TTN_DeleteCountDown&) = default;
		TTN_DeleteCountDown(TTN_DeleteCountDown&&) = default;
		TTN_DeleteCountDown& operator=(TTN_DeleteCountDown&) = default;

		//update function
		void Update(float deltaTime) {
			m_lifeLeft -= deltaTime;
		}

		//setters
		void SetLifeLeft(float lifeLeft) { m_lifeLeft = lifeLeft; }
		//getters
		float GetLifeLeft() { return m_lifeLeft; }

	private:
		float m_lifeLeft;
	};
}