#include "Titan/Window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "GLM/glm.hpp"
#include <string>
#include <iostream>


namespace Titan {

	GLFWwindow* Window::gwindow = nullptr;

	Window::Window(int w, int h, const std::string& name)
	{
		width = w;
		height = h;
		title == name;
		Init();
	}

	Window::~Window()
	{
		glfwTerminate();
		glfwDestroyWindow(gwindow);
	}


	bool Window::Init()
	{
		if (!glfwInit()) {
			throw std::runtime_error("Failed to initialize GLFW");
		}


		glfwWindowHint(GLFW_SCALE_TO_MONITOR, true);

		gwindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
		glfwMakeContextCurrent(gwindow);

		if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
			throw std::runtime_error("Failed to initialize Glad");
		}

		std::cout << glGetString(GL_RENDERER) << std::endl; std::cout << glGetString(GL_VERSION) << std::endl;

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		glfwSwapBuffers(gwindow);


	}

	bool Window::ShouldClose()
	{
		return glfwWindowShouldClose(gwindow);
	}







}