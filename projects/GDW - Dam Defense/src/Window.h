#pragma once

#include "GLFW/glfw3.h"

#include <glad/glad.h>
#include "GLM/glm.hpp"
#include <string>


class Window
{
public:
	static void Init(int width, int height, const std::string& name);

	static bool ShouldClose();



	//static GLFWwindow* gwindow;  //gets linker error for some reason


	Window() = default;
	~Window() = default;
};

