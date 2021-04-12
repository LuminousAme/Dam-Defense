#version 410
//mesh data from c++ program
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inUv;

//mesh data to pass to the frag shader
layout(location = 0) out vec2 outUV;

//model view projection matrix
uniform mat4 MVP;

void main() {
	//send the uvs onto the frag shader 
	outUV = inUv;
	//set the vertex position
	gl_Position = MVP * vec4(inPos, 1.0);
}