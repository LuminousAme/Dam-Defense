#version 410

//mesh data from c++ program
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inColor;

//mesh data to pass to the frag shader
layout(location = 0) out vec3 outPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outUV;
layout(location = 3) out vec3 outColor;

//model, view, projection matrix
uniform mat4 MVP;
//model matrix only
uniform mat4 Model; 
//normal matrix
uniform mat3 NormalMat;

void main() {
	//calculate the position
	vec4 newPos = MVP * vec4(inPos, 1.0);

	//pass data onto the frag shader
	outPos = (Model * vec4(inPos, 1.0)).xyz;
	outNormal = NormalMat * inNormal;
	outUV = inUV;
	outColor = inColor;

	//set the position of the vertex
	gl_Position = newPos;
}

