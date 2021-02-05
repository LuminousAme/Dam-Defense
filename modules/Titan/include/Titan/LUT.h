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