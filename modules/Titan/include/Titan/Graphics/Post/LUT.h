//Titan Engine by Atlas X Games
//LUT.h - Header for the class that represents 3D look up tables for color correction
#pragma once

//include the precompile header with a bunch of system
#include "Titan/ttn_pch.h"

namespace Titan {
	//class the represents a 3D cube look up table
	class  TTN_LUT3D
	{
	public:
		//defines a special easier to use name for shared(smart) pointers to the class 
		typedef std::shared_ptr<TTN_LUT3D> sltptr;

		//creates and returns a shared(smart) pointer to the class 
		static inline sltptr Create() {
			return std::make_shared<TTN_LUT3D>();
		}

		//creates and returns a shared(smart) pointer to the class with a value loaded
		static inline sltptr Create(std::string path) {
			return std::make_shared<TTN_LUT3D>(path);
		}

	public:
		//default constructor
		TTN_LUT3D();
		//constructor that takes the path to a .cube file
		TTN_LUT3D(std::string path);
		
		//load in from a cube file
		void loadFromFile(std::string path);
		//bind and unbind the look up table
		void bind();
		void unbind();

		//bind and unbind to and from a specfic texture slot
		void bind(int textureSlot);
		void unbind(int textureSlot);

	private:
		//Gl handle and data
		GLuint m_handle = GL_NONE;
		std::vector<glm::vec3> data;
	};
}