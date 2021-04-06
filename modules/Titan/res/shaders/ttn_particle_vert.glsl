#version 410

//data from c++
//regular vbos
layout(location = 0) in vec3 inVertPos;
layout(location = 2) in vec2 inVertUV;
//instanced vbos
layout(location = 3) in vec4 inParticleColor;
layout(location = 4) in vec3 inInstancePosition;
layout(location = 5) in float inScale;

//mesh data to pass to the frag shader
layout(location = 0) out vec3 outPos;
layout(location = 2) out vec2 outUV;
layout(location = 3) out vec4 outColor;

uniform mat4 u_model;
uniform mat4 u_mvp;
uniform mat3 u_normalMat;

void main() {
	//calculate the position
	vec3 ParticlePos = inVertPos * inScale + inInstancePosition;

	//pass data onto the frag shader
	outPos = (u_model * vec4(ParticlePos, 1.0)).xyz;
	outUV = inVertUV;
	outColor = inParticleColor;

	//set the position of the vertex
	gl_Position = (u_mvp * vec4(ParticlePos, 1.0));
}