//Titan Engine, by Atlas X Games 
// Tag.h - header for the class that represents tags to identify entities by
#pragma once

//include required features 
#include <string>

namespace Titan {
	class TTN_Tag {
	public:
		//default constructor
		TTN_Tag() { m_name = std::string(); }

		//constructor with data
		TTN_Tag(std::string name) { m_name = name; }

		//default destructor
		~TTN_Tag() = default;

		//copy, move, and assingment constrcutors for ENTT
		TTN_Tag(const TTN_Tag&) = default;
		TTN_Tag(TTN_Tag&&) = default;
		TTN_Tag& operator=(TTN_Tag&) = default;

		//sets the name of the object
		void SetName(std::string name) { m_name = name; }

		//gets the name of the object
		std::string getName() { return m_name; }

	private:
		//stores the name of the object
		std::string m_name;
	};
}
