#version 410

//mesh data from c++ program
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inColor;
layout(location = 4) in vec3 inPosNextFrame;
layout(location = 5) in vec3 inNormalNextFrame;

//mesh data to pass to the frag shader
layout(location = 0) out vec3 outPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outUV;
layout(location = 3) out vec3 outColor;
layout(location = 4) out vec3 viewSpacePos;

//model, view, projection matrix
uniform mat4 MVP;
//model matrix only
uniform mat4 Model; 
//normal matrix
uniform mat3 NormalMat;
//view matrix only
uniform mat4 View;

//uniform with the value of the interpolation 
uniform float t; 

void main() {
	//lerp the positions and normals 
	vec3 pos = mix(inPos, inPosNextFrame, t);
	vec3 normal = normalize(mix(inNormal, inNormalNextFrame, t));

	//apply the mvp matrix to the position
	vec4 newPos = MVP * vec4(pos, 1.0);

	//pass data onto the frag shader
	outPos = (Model * vec4(pos, 1.0)).xyz;
	outNormal = NormalMat * normal;
	outUV = inUV;
	outColor = inColor;

	//viewspace position
	viewSpacePos = (View * Model * vec4(inPos, 1.0)).xyz;

	//set the position of the vertex
	gl_Position = newPos;
}