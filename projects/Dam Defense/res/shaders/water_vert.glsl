#version 420
//mesh data from c++ program
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

//mesh data to pass to the frag shader
layout(location = 0) out vec3 outPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outUV;

//model, view, projection matrix
uniform mat4 MVP;
//model matrix only
uniform mat4 Model;
//normal matrix
uniform mat3 NormalMat;

//animate the water
uniform float time;
//controls for the animation
uniform float speed;
uniform float baseHeight;
uniform float heightMultiplier;
uniform float waveLenghtMultiplier;

void main() {
	//pass data onto the frag shader
	vec3 norm = inNormal;
	outUV = inUV;

	//displace the mesh based on a sin function, it's z position, etc.
	vec3 vert = inPos;

	//create animated waves
	float freq = speed * waveLenghtMultiplier;
	float angle = (time + vert.z) * freq;
	vert.y = heightMultiplier * sin(angle) + baseHeight;

	//adjust the normals to match the waves
	norm = normalize(vec3(0.0, 1.0, 1.0 * -waveLenghtMultiplier * freq * cos(angle)));

	//pass the new position onto the frag shader
	outPos = (Model * vec4(vert, 1.0)).xyz;

	//pass the new normal onto the frag shader
	outNormal = NormalMat * inNormal;
	
	gl_Position = MVP * vec4(vert, 1.0);
}