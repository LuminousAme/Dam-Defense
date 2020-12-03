#version 410

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 outPos;
layout(location = 1) out vec3 outNormal;

uniform mat4 MVP;

void main() {
	
	vec4 newPos = MVP * vec4(inPos, 1.0);

	outPos = newPos.xyz;
	outNormal = inNormal;

	gl_Position = newPos;
}