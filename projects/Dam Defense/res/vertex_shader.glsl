#version 410

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;

layout(location = 4) out vec3 outColour;

uniform mat4 MVP;

void main() {
	
	gl_Position = MVP * vec4(inPos, 1.0);

	outColour = vec3(1.0, 0.0, 0.0);
}