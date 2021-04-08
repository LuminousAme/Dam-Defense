#version 420

//mesh data from vert shader
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inColor;
layout(location = 5) in vec3 inTangent;
layout(location = 6) in vec3 inBiTangent;

//material data
layout(binding = 0) uniform sampler2D s_Diffuse;
uniform int u_UseDiffuse;
layout(binding = 1) uniform sampler2D s_Emissive;
layout(binding = 2) uniform sampler2D s_normalMap;
uniform int u_UseEmissive;
uniform float u_EmissiveStrenght;
uniform int u_useNormalMapping = 0;
uniform int u_useRimLighting = 0;
uniform float u_rimSize = 0.2;
uniform vec3 u_rimColor = vec3(1.0);
uniform vec3 u_CamPos;

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

	//add the rim lighting 
	vec3 viewDir  = normalize(u_CamPos - inPos);
	float rimLightContribution = 1.0 - max(dot(viewDir, ((outNormals * 2.0) - 1.0)), 0.0);
	float rimLighting = smoothstep(1.0 - clamp(u_rimSize, 0.0, 1.0), 1.0, rimLightContribution);
	outColors = mix(outColors, outColors + vec4(u_rimColor * vec3(rimLighting), 1.0), float(u_useRimLighting));
	//rimLighting = mix(0.0, rimLighting, float(u_useRimLighting));
	//rimLighting = ReMap(0.0, 1.0, 1.0, 0.0, rimLighting);

	//find the specular from the texture and output it 
	outSpecs = vec3(1.0, ReMap(0.0, 1.0, 1.0, 0.0, u_UseEmissive * u_EmissiveStrenght), 1.0);

	//output the world space positions
	outPositions = inPos;

	//output the emissive colour 
	outEmissive = texture(s_Emissive, inUV).rgb;
}