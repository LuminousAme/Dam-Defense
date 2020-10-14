#include <Logging.h>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <filesystem>
#include <fstream>


#include "Window.h"
#include "VertexBuffer.h"

GLFWwindow* window;

GLuint shader_program;

bool loadShaders() {
	// Read Shaders from file
	std::string vert_shader_str;
	std::ifstream vs_stream("vertex_shader.glsl", std::ios::in);
	if (vs_stream.is_open()) {
		std::string Line = "";
		while (getline(vs_stream, Line))
			vert_shader_str += "\n" + Line;
		vs_stream.close();
	}
	else {
		printf("Could not open vertex shader!!\n");
		return false;
	}
	const char* vs_str = vert_shader_str.c_str();

	std::string frag_shader_str;
	std::ifstream fs_stream("frag_shader.glsl", std::ios::in);
	if (fs_stream.is_open()) {
		std::string Line = "";
		while (getline(fs_stream, Line))
			frag_shader_str += "\n" + Line;
		fs_stream.close();
	}
	else {
		printf("Could not open fragment shader!!\n");
		return false;
	}
	const char* fs_str = frag_shader_str.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_str, NULL);
	glCompileShader(vs);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fs_str, NULL);
	glCompileShader(fs);

	shader_program = glCreateProgram();
	glAttachShader(shader_program, fs);
	glAttachShader(shader_program, vs);
	glLinkProgram(shader_program);

	return true;
}



int main() { 
	Logger::Init(); 


	Window::Init(800, 800, "Dam Defense");
	
	static const GLfloat points[] = {
		-0.5f, -0.5f, 0.1f,
		 0.5f, -0.5f, 0.1f,
		-0.5f,  0.5f, 0.1f
	};

	static const GLfloat colors[] = {
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f
	};


	VertexBuffer* pos_vbo = new VertexBuffer(points,0);
	VertexBuffer* col_vbo = new VertexBuffer(colors, 1);
	//GLuint pos_vbo = 0;
	//VertexBuffer p(points, pos_vbo);
	//p.Create(pos_vbo);
	//
	//GLuint col_vbo = 1;
	//VertexBuffer c(colors, col_vbo);
	//c.Create( col_vbo);
	

	//GLuint pos_vbo = 0;
	//glGenBuffers(1, &pos_vbo);
	//glBindBuffer(GL_ARRAY_BUFFER, pos_vbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

	//GLuint col_vbo = 1;
	//glGenBuffers(1, &col_vbo);
	//glBindBuffer(GL_ARRAY_BUFFER, col_vbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

	//glBindBuffer(GL_ARRAY_BUFFER, pos_vbo);
	////                   index,  size,  data type,  normalize?,  stride,  pointer 
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	//glBindBuffer(GL_ARRAY_BUFFER, col_vbo);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(0); //pos
	glEnableVertexAttribArray(1); //colors

		//load our shaders
	if (!loadShaders()) {
		return 1;
	}


	while (!Window::ShouldClose()) {
		glfwPollEvents();

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader_program);

	/*	p.Draw(0, 3);
		p.Draw(3, 3);*/

		glDrawArrays(GL_TRIANGLES, 0, 3);
		//glDrawArrays(GL_TRIANGLES, 3, 3);

		//glfwSwapBuffers(window);
		glfwSwapBuffers(Window::gwindow);
	
	}

	
	Logger::Uninitialize();
	return 0; 
} 
