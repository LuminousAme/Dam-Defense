#include "Titan/Scene.h"



	int main() {


		Titan::Window GameWindow(800, 800, "Dam Defense");


		while (!GameWindow.ShouldClose()) {
			glfwPollEvents();

			
			glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			glfwSwapBuffers(GameWindow.gwindow);
		}


		return 0;
	}


