#version 410

//mesh data from c++ program
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inColor;
layout(location = 4) in vec3 inPosNextFrame;
layout(location = 5) in vec3 inNormalNextFrame;
layout(location = 6) in vec3 inTangent;
layout(location = 7) in vec3 inTangentNextFrame;

//mesh data to pass to the frag shader
layout(location = 0) out vec3 outPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outUV;
layout(location = 3) out vec3 outColor;
layout(location = 4) out vec4 outFragPosLightSpace;
layout(location = 5) out vec3 outTangent;
layout(location = 6) out vec3 outBiTangent;

//model, view, projection matrix
uniform mat4 MVP;
//model matrix only
uniform mat4 Model; 
//normal matrix
uniform mat3 NormalMat;
//lightspace matrix
uniform mat4 u_LightSpaceMatrix;

//uniform with the value of the interpolation 
uniform float t; 

void main() {
	//lerp the positions and normals 
	vec3 pos = mix(inPos, inPosNextFrame, t);
	vec3 normal = normalize(mix(inNormal, inNormalNextFrame, t));
	vec3 tangent = normalize(mix(inTangent, inTangentNextFrame, t));

	//apply the mvp matrix to the position
	vec4 newPos = MVP * vec4(pos, 1.0);

	//pass data onto the frag shader
	outPos = (Model * vec4(pos, 1.0)).xyz;
	outNormal = NormalMat * normal;
	tangent = NormalMat * tangent;
	outTangent = normalize(tangent - dot(tangent, outNormal) * outNormal);
	outBiTangent = cross(outNormal, outTangent);
	outUV = inUV;
	outColor = inColor;

	//pass out the light space fragment pos
	outFragPosLightSpace = u_LightSpaceMatrix * vec4(outPos, 1.0);

	//set the position of the vertex
	gl_Position = newPos;
}