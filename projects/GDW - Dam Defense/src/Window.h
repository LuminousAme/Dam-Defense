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



	static GLFWwindow* gwindow;  //the window


	Window() = default;
	~Window() = default;
};

