#version 420
//gbuffer directional
layout(location = 0) in vec2 inUV;

struct DirectionalLight
{
	//Light direction (defaults to down, to the left, and a little forward)
	vec4 m_lightDirection;

	//Generic Light controls
	vec4 m_lightColor;

	//Ambience controls
	vec4 m_ambientColor;
	float m_ambientPower;
	
	//Power controls
	float m_lightAmbientPower;
	float m_lightSpecularPower;

	float m_minShadowBias;
	float m_maxShadowBias;
	int m_pcfFilterSamples;
};

layout (std140, binding = 0) uniform u_Lights
{
    DirectionalLight sun;
};

layout (binding = 15) uniform sampler2D s_depthMap;
layout (binding = 30) uniform sampler2DArray s_ShadowMap;	
uniform mat4 u_lightViewProj[4];
uniform float u_SplitRanges[4];
uniform mat4 u_vp;

//get gbuffer data
layout (binding = 0) uniform sampler2D s_albedoTex;
layout (binding = 1) uniform sampler2D s_normalsTex;
layout (binding = 2) uniform sampler2D s_specularTex;
layout (binding = 3) uniform sampler2D s_positionTex;

//get the light accumulation buffer
layout (binding = 4) uniform sampler2D s_lightAccumTex;

//toon shading
layout (binding = 9) uniform sampler2D s_DiffuseRamp;
uniform int u_UseDiffuseRamp = 0;
layout (binding = 10) uniform sampler2D s_SpecularRamp;
uniform int u_useSpecularRamp = 0;

uniform vec3 u_CamPos;

out vec4 frag_color;

float shadowCalc(vec3 worldPos, vec3 clipSpacePos, float bias) {
	//calculate depth of current fragment
	float depth = texture(s_depthMap, inUV).r;

	//figure out which shadowmap should be used based on the depth
	int shadowMapIndex;

	if(depth < u_SplitRanges[0]) {
		shadowMapIndex = 0;
	}
	else if (depth < u_SplitRanges[1]) {
		shadowMapIndex = 1;
	}
	else if (depth < u_SplitRanges[2]) {
		shadowMapIndex = 2;
	}
	else if (depth < u_SplitRanges[3]) {
		shadowMapIndex = 3;
	}
	else {
		return -1;
	}

	//get the size of a single texel 
	vec3 texelSize = 1.0 / textureSize(s_ShadowMap, 0);

	//now get the light space position, and projection coordinates from the index and light space matrices
	vec4 lightSpacePos = u_lightViewProj[shadowMapIndex] * vec4(worldPos, 1.0);
	vec3 projectionCoordinates = (lightSpacePos.xyz / lightSpacePos.w).xyz; 
	projectionCoordinates = projectionCoordinates * 0.5 + 0.5;

	if(projectionCoordinates.x < 0.0 || projectionCoordinates.x > 1.0 || projectionCoordinates.y < 0.0 || projectionCoordinates.y > 1.0)
		return -1.0;

	//get the acutal fragment depth from the projection coordinates
	float currentDepth = projectionCoordinates.z;

	//track the number of samples taken 
	int samplesTaken = 0;
	//track the shadow sampling
	float shadow = 0.0;

	for(int x = 0 - sun.m_pcfFilterSamples / 2; x <= sun.m_pcfFilterSamples / 2; x++) {
		for(int y = 0 - sun.m_pcfFilterSamples / 2; y <= sun.m_pcfFilterSamples / 2; y++) {
			//get the shadow depth from the projection coordinates, shadow map index and shadow maps themselves
			float shadowDepth = texture(s_ShadowMap, vec3((projectionCoordinates.xy + vec2(x, y) * texelSize.xy).xy, shadowMapIndex)).r;
			//check wheter there's a shadow
			shadow += (currentDepth - bias > shadowDepth) ? 0.0 : 1.0;
			samplesTaken++;
		}
	}
	
	//get the average softened shadow for this fragment
	shadow /= samplesTaken;

	//return the shadow
	return shadow;
}

// https://learnopengl.com/Advanced-Lighting/Advanced-Lighting
void main() {
    //albedo 
    vec4 textureColor = texture(s_albedoTex, inUV);
    //normals
    vec3 inNormal = (normalize(texture(s_normalsTex,inUV).rgb) * 2.0) - 1.0;
    //specular
    float texSpec = texture(s_specularTex,inUV).r;
    //positions
    vec3 fragPos = texture(s_positionTex,inUV).rgb;

	// Diffuse
	vec3 N = normalize(inNormal);
	vec3 lightDir = normalize(-sun.m_lightDirection.xyz);
	float dif = max(dot(N, lightDir), 0.0);
	vec3 diffuse = dif * sun.m_lightColor.rgb;// add diffuse intensity
	diffuse = mix(diffuse, texture(s_DiffuseRamp, vec2(dif, dif)).xyz, u_UseDiffuseRamp);

	// Specular
	vec3 viewDir  = normalize(u_CamPos - fragPos);
	vec3 h        = normalize(lightDir + viewDir);

	float spec = pow(max(dot(N, h), 0.0), 4.0); // Shininess coefficient (can be a uniform)
	vec3 specular = sun.m_lightSpecularPower * texSpec * spec * sun.m_lightColor.xyz; // Can also use a specular color
	specular = mix(specular, (texture(s_SpecularRamp, vec2(spec, spec)).xyz), u_useSpecularRamp);

	 //get the view position
	vec4 temp = (u_vp * vec4(fragPos, 1.0));
	vec3 clipPos = temp.xyz / temp.w;
	//do the shadow calculation
	float shadowBias = max(sun.m_maxShadowBias * (1.0 - dot(N, lightDir)), sun.m_minShadowBias);
	float shadow = shadowCalc(fragPos, clipPos, shadowBias);


	vec3 result = ((sun.m_ambientPower * sun.m_ambientColor.xyz) + // global ambient light
		 (diffuse + specular) // light factors from our single light, including shadow 
		);


	//if(shadow < -0.1)
		//result = vec3(1.0, 0.0, 0.8);

	//if the alpha is less than 31% than it is our clear colour
    if (textureColor.a < 0.31)
    {
		//so just set it to white so there isn't any lighting on the skybox
        result = vec3(1.0,1.0,1.0);
    }

	//pass out the frag color
	frag_color = vec4(result, textureColor.a);
}