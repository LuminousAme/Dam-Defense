#version 420

//mesh data from vert shader
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inColor;
layout(location = 5) in vec3 inTangent;
layout(location = 6) in vec3 inBiTangent;

//material data
uniform sampler2D s_Diffuse;
uniform int u_UseDiffuse;
uniform sampler2D s_Emissive;
uniform sampler2D s_normalMap;
uniform int u_UseEmissive;
uniform float u_EmissiveStrenght;
uniform int u_useNormalMapping = 0;
uniform int u_useRimLighting = 0;

//result, multiple render targets
//we can render color to all of these
layout(location = 0) out vec4 outColors;
layout(location = 1) out vec3 outNormals;
layout(location = 2) out vec3 outSpecs;
layout(location = 3) out vec3 outPositions;
layout(location = 4) out vec3 outEmissive;

float ReMap(float oMin, float oMax, float nMin, float nMax, float val) {
	float t = (val - oMin) / (oMax - oMin);

	return mix(nMin, nMax, t);
}

void main() {
	//get the albedo from the diffuse / abledo texture map and output it
	outColors = mix(vec4(inColor, 1.0), texture(s_Diffuse, inUV) * vec4(inColor, 1.0), u_UseDiffuse);

	//calculate the normal vectors and convert it from [-1, 1] range to [0, 1] range and output it 
	mat3 TBN = mat3(inTangent, inBiTangent, inNormal);
	vec3 normalMapNormal = texture(s_normalMap, inUV).rgb;
	normalMapNormal = normalMapNormal * 2.0 - 1.0;
	normalMapNormal = normalize(TBN * normalMapNormal);
	outNormals = (mix(normalize(inNormal), normalMapNormal, u_useNormalMapping) * 0.5) + 0.5;

	//find the specular from the texture and output it 
	outSpecs = vec3(1.0, ReMap(0.0, 1.0, 1.0, 0.0, u_UseEmissive * u_EmissiveStrenght), ReMap(0.0, 1.0, 1.0, 0.0, float(u_useRimLighting)));

	//output the world space positions
	outPositions = inPos;

	//output the emissive colour 
	outEmissive = texture(s_Emissive, inUV).rgb;
}