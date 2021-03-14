#version 410

layout (location = 0) in vec3 inPosition;
layout(location = 4) in vec3 inPosNextFrame;

//out lightspace matrix
uniform mat4 u_Model;
uniform float t;

void main()
{ 
	//lightspace matrix form the light's perspective
	gl_Position = u_Model * vec4(mix(inPosition, inPosNextFrame, t), 1.0);
}