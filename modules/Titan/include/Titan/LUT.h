#pragma once

//include the precompile header with a bunch of system
#include "ttn_pch.h"

#include <vector>
#include <fstream>
#include <string>
#include <glad/glad.h>
#include "glm/common.hpp"

class  TTN_LUT3D
{
public:
	//defines a special easier to use name for shared(smart) pointers to the class 
	typedef std::shared_ptr<TTN_LUT3D> st2dptr;

	//creates and returns a shared(smart) pointer to the class 
	static inline st2dptr Create() {
		return std::make_shared<TTN_LUT3D>();
	}

	//Creates a new empty texture pointer, this is used by the framebuffer class 
	static inline st2dptr CreateEmpty() {
		return std::make_shared<TTN_LUT3D>();
	}

	 TTN_LUT3D();
	 TTN_LUT3D(std::string path);
	void loadFromFile(std::string path);
	void bind();
	void unbind();

	void bind(int textureSlot);
	void unbind(int textureSlot);
private:
	GLuint _handle = GL_NONE;
	std::vector<glm::vec3> data;
};