#version 420

//mesh data from vert shader
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inColor;
layout(location = 4) in vec3 inViewSpacePosition;

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

//material data
uniform sampler2D s_Diffuse;
uniform int u_UseDiffuse;
uniform float u_Shininess;
uniform int u_recievesShadows;

//scene ambient lighting
uniform vec3  u_AmbientCol;
uniform float u_AmbientStrength;

//uniforms for different lighting effects
uniform int u_hasAmbientLighting;
uniform int u_hasSpecularLighting;
uniform int u_hasOutline;
uniform float u_OutlineSize;

//ramps for toon shading
layout(binding = 10)uniform sampler2D s_diffuseRamp;
uniform int u_useDiffuseRamp;
layout(binding = 11)uniform sampler2D s_specularRamp;
uniform int u_useSpecularRamp;

//Specfic light stuff
uniform vec3  u_LightPos[16];
uniform vec3  u_LightCol[16];
uniform float u_AmbientLightStrength[16];
uniform float u_SpecularLightStrength[16];
uniform float u_LightAttenuationConstant[16];
uniform float u_LightAttenuationLinear[16];
uniform float u_LightAttenuationQuadratic[16];

uniform int u_NumOfLights;

layout (binding = 30) uniform sampler2DArray s_ShadowMap;
uniform float shadowBias=0.005f;

//lightspace matrix
uniform mat4 u_LightSpaceMatrix[4];
uniform float u_shadowSplits[4];
uniform float u_farPlane;

//camera data
uniform vec3  u_CamPos;

//result
out vec4 frag_color;

//functions 
vec3 CalcLight(vec3 pos, vec3 col, float ambStr, float specStr, float attenConst, float attenLine, float attenQuad, vec3 norm, vec3 viewDir, float textSpec);
vec3 CalcDirLight(vec3 norm, vec3 viewDir, float textSpec, float shadow);
float ShadowCalculation();

// https://learnopengl.com/Advanced-Lighting/Advanced-Lighting
void main() {
	//calcualte the vectors needed for lighting
	vec3 N = normalize(inNormal);
	vec3 viewDir  = normalize(u_CamPos - inPos);
	//sample the textures
	vec4 textureColor = texture(s_Diffuse, inUV);

	if(textureColor.a < 0.01)
		discard;

	//combine everything
	vec3 result = u_AmbientCol * u_AmbientStrength * u_hasAmbientLighting; // global ambient light

	//check the shadow map
	float useShadows = u_recievesShadows;
	float shadow = mix(0.0, ShadowCalculation(), useShadows);

	//add the results from all the point lights
	for(int i = 0; i < u_NumOfLights; i++) {
		result = result + CalcLight(u_LightPos[i], u_LightCol[i], u_AmbientLightStrength[i], u_SpecularLightStrength[i], 
					u_LightAttenuationConstant[i], u_LightAttenuationLinear[i], u_LightAttenuationQuadratic[i], 
					N, viewDir, 1.0);
	}

	//add the directional light to the lighting 
	result += CalcDirLight(N, viewDir, 1.0, shadow);

	//add that to the texture color if the texture color should be applied
	result = mix(result * inColor, result * inColor * textureColor.rgb, u_UseDiffuse);

	//calculate if it should be coloured as a black outline
	float edge = max(step(u_OutlineSize, abs(dot(viewDir, N))), u_hasOutline);

	//save the result and pass it on
	frag_color = vec4(result, textureColor.a) * vec4(vec3(edge), 1.0);
}

vec3 CalcLight(vec3 pos, vec3 col, float ambStr, float specStr, float attenConst, float attenLine, float attenQuad, vec3 norm, vec3 viewDir, float textSpec) {
	//ambient 
	vec3 ambient = ambStr * col * u_hasAmbientLighting;

	//diffuse
	vec3 lightDir = normalize(pos - inPos);
	float dif = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = dif * col;
	diffuse = mix(diffuse, texture(s_diffuseRamp, vec2(dif, dif)).xyz, u_useDiffuseRamp);
	diffuse = diffuse * u_hasAmbientLighting * u_hasSpecularLighting;

	//attenuation
	float dist = length(pos - inPos);
	float attenuation = 1.0f / (
		attenConst + 
		attenLine * dist +
		attenQuad * dist * dist);

	//specular
	vec3 halfWay =  normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfWay), 0.0), u_Shininess); 
	vec3 specular = specStr * textSpec * spec * col;
	specular = mix(specular, (texture(s_specularRamp, vec2(spec, spec)).xyz), u_useSpecularRamp);
	specular = specular * u_hasSpecularLighting;
	
	return ((ambient + diffuse + specular) * attenuation);
}

vec3 CalcDirLight(vec3 norm, vec3 viewDir, float textSpec, float shadow) {
	//ambient
	vec3 ambient = sun.m_ambientPower * sun.m_ambientColor.xyz * u_hasAmbientLighting;

	//difuse
	vec3 lightDir = normalize(-sun.m_lightDirection.xyz);
	float dif = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = dif * sun.m_lightColor.xyz;
	diffuse = mix(diffuse, texture(s_diffuseRamp, vec2(dif, dif)).xyz, u_useDiffuseRamp);
	diffuse = diffuse * u_hasAmbientLighting * u_hasSpecularLighting;

	//specular
	vec3 halfWay =  normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfWay), 0.0), u_Shininess); 
	vec3 specular = sun.m_lightSpecularPower * textSpec * spec * sun.m_lightColor.xyz;
	specular = mix(specular, texture(s_specularRamp, vec2(spec, spec)).xyz, u_useSpecularRamp);
	specular = specular * u_hasSpecularLighting;

	//add them together and return
	return ambient + (1.0 - shadow) * (diffuse + specular);
}

float ShadowCalculation() {
	//figure out the texture depth
	vec4 FragPosLightSpace = vec4(0.0);
	int textureDepth = 3;
	float depth = inViewSpacePosition.z / u_farPlane;

	for(int i = 3; i >= 0; i--) {
		textureDepth = (depth < u_shadowSplits[i]) ? i : textureDepth;
	}

	//using the approriate texture depth, get the light space frag position 
	FragPosLightSpace = u_LightSpaceMatrix[textureDepth] * vec4(inPos, 1.0);

	//perspective division
	vec3 projectionCoordinates = FragPosLightSpace.xyz / FragPosLightSpace.w;

	//transform to a [0,1] range
	projectionCoordinates = projectionCoordinates * 0.5 + 0.5;

	//Get current depth acoording to our camera
	float currentDepth = projectionCoordinates.z; 

	//sample for the shadows
	float shadow = 0.0; 
	//track the number of samples that have been taken
	int samplesTaken = 0;
	//get the size of a single texel
	vec2 texelSize = vec2(1.0 / textureSize(s_ShadowMap, 0).x, 1.0 / textureSize(s_ShadowMap, 0).y);

	//caculate the shadow bias based on the angle between the view and the surface 
	float shadowBias = max(sun.m_maxShadowBias * (1.0 - dot(normalize(inNormal), normalize(sun.m_lightDirection.xyz))), sun.m_minShadowBias);

	//sample the shadow map at the correct point and in the surronding texels, up to the number of samples set in the light and add it to the shadow
	for(int x = 0 - sun.m_pcfFilterSamples / 2; x <= sun.m_pcfFilterSamples / 2; x++)
	{
		for(int y = 0 - sun.m_pcfFilterSamples / 2; y <= sun.m_pcfFilterSamples / 2; y++)
		{
			float depth = texture(s_ShadowMap, vec3((projectionCoordinates.xy + vec2(x, y) * texelSize), textureDepth)).r;
			shadow += currentDepth - shadowBias > depth ? 1.0 : 0.0;
			samplesTaken++;
		}
	}
	//and divide the shadow result by the number of samples taken to get the average softened shadow for this fragment
	shadow /= samplesTaken;

	shadow = projectionCoordinates.z > 1.0 ? 0.0 : shadow;

	//check wheter there's a shadow and return it 
	return shadow;
}