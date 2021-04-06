#version 410

//data from c++
//regular vbos
layout(location = 0) in vec3 inVertPos;
//instanced vbos
layout(location = 1) in vec4 inParticleColor;
layout(location = 2) in vec3 inInstancePosition;
layout(location = 3) in float inScale;

//data to send to the geometry shader 
out Vertex {
	float scale;
	vec4 color;
} vertex;

uniform mat4 u_model;

void main() {
	//calculate the position
	vec3 ParticlePos = inVertPos + inInstancePosition;

	//pass data onto the geometry shader 
	vertex.scale = inScale;

	//pass data onto the frag shader
	vertex.color = inParticleColor;

	//set the position of the vertex
	gl_Position = (u_model * vec4(ParticlePos, 1.0));
}