#include <Logging.h>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <filesystem>
#include <fstream>


#include "Window.h"


GLFWwindow* window;


int main() { 
	Logger::Init(); 


	Window::Init(800, 800, "Dam Defense");
	
		
	/*while (!Window::ShouldClose()) {
		glfwPollEvents();
		glfwSwapBuffers(window);

	}*/
	
	Logger::Uninitialize();
	return 0; 
} 
