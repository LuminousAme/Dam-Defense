#include "Window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "GLM/glm.hpp"
#include <string>
#include <iostream>


GLFWwindow* gwindow = nullptr;

void Window::Init(int width, int height, const std::string& name)
{

	if (!glfwInit()) {	
		throw std::runtime_error("Failed to initialize GLFW");
	}


	glfwWindowHint(GLFW_SCALE_TO_MONITOR, true);

	gwindow = glfwCreateWindow(800, 800, "testing", nullptr, nullptr);
	glfwMakeContextCurrent(gwindow);

	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
		throw std::runtime_error("Failed to initialize Glad");
	}

	std::cout << glGetString(GL_RENDERER) << std::endl; std::cout << glGetString(GL_VERSION) << std::endl;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	while (!glfwWindowShouldClose(gwindow)) {
		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.1f, 1.0f); glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(gwindow);
	}

}

bool Window::ShouldClose()
{
	return glfwWindowShouldClose(gwindow);
}
