//Titan Engine, by Atlas X Games 
// Tag.h - header for the class that represents tags to identify entities by
#pragma once

//include required features 
#include <string>

namespace Titan {
	class TTN_Tag {
	public:
		//default constructor
		TTN_Tag() { m_name = std::string(), m_path = 0; }

		//constructor with data
		TTN_Tag(std::string name) { m_name = name, m_path = 0; }
		TTN_Tag(int path) { m_path = path, m_name = std::string(); }
		TTN_Tag(std::string name, int path) { m_path = path, m_name = name; }

		//default destructor
		~TTN_Tag() = default;

		//copy, move, and assingment constrcutors for ENTT
		TTN_Tag(const TTN_Tag&) = default;
		TTN_Tag(TTN_Tag&&) = default;
		TTN_Tag& operator=(TTN_Tag&) = default;

		//sets the name of the object
		void SetName(std::string name) { m_name = name; }
		void SetPath(int path) { m_path = path; }

		//gets the name of the object
		std::string getName() { return m_name; }
		int getPath() { return m_path; }

	private:
		//stores the name of the object
		std::string m_name;
		int m_path; //path of the boat entity
	};
}
