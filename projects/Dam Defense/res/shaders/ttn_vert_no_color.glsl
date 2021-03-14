#version 410
//vert no color res
//mesh data from c++ program
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

//mesh data to pass to the frag shader
layout(location = 0) out vec3 outPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outUV;
layout(location = 3) out vec3 outColor;
layout(location = 4) out vec4 outFragPosLightSpace;


//model, view, projection matrix
uniform mat4 MVP;
//model matrix only
uniform mat4 Model; 
//normal matrix
uniform mat3 NormalMat;
//light spac ematrix
uniform mat4 u_LightSpaceMatrix;


void main() {
	//calculate the position
	vec4 newPos = MVP * vec4(inPos, 1.0);

	//pass data onto the frag shader
	outPos = (Model * vec4(inPos, 1.0)).xyz;
	outNormal = NormalMat * inNormal;
	outUV = inUV;
	outColor = vec3(1.0, 1.0, 1.0);

	//pass out the light space fragment pos
	outFragPosLightSpace = u_LightSpaceMatrix* vec4(outPos, 1.0);

	//set the position of the vertex
	gl_Position = newPos;
}