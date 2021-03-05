#version 420

//mesh data from vert shader
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inColor;
layout(location = 4) in vec4 inFragPosLightSpace;

//material data
uniform sampler2D s_Diffuse;
uniform int u_UseDiffuse;
uniform float u_Shininess;

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

layout (binding = 30) uniform sampler2D s_ShadowMap;
uniform float shadowBias=0.005f;

//camera data
uniform vec3  u_CamPos;

//result
out vec4 frag_color;

//functions 
//vec3 CalcLight(vec3 pos, vec3 col, float ambStr, float specStr, float attenConst, float attenLine, float attenQuad, vec3 norm, vec3 viewDir, float textSpec);
vec3 CalcLight(vec3 pos, vec3 col, float ambStr, float specStr, float attenConst, float attenLine, float attenQuad, vec3 norm, vec3 viewDir, float textSpec, float shadow);
float ShadowCalculation(vec4 FragPosLightSpace);

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

		float shadow = ShadowCalculation(inFragPosLightSpace);

	//add the results from all the lights
	for(int i = 0; i < u_NumOfLights; i++) {
		result = result + CalcLight(u_LightPos[i], u_LightCol[i], u_AmbientLightStrength[i], u_SpecularLightStrength[i], 
					u_LightAttenuationConstant[i], u_LightAttenuationLinear[i], u_LightAttenuationQuadratic[i], 
					N, viewDir, 1.0, shadow);
	}

	//add that to the texture color if the texture color should be applied
	result = mix(result * inColor, result * inColor * textureColor.rgb, u_UseDiffuse);

	//calculate if it should be coloured as a black outline
	float edge = max(step(u_OutlineSize, abs(dot(viewDir, N))), u_hasOutline);

	//save the result and pass it on
	frag_color = vec4(result, textureColor.a) * vec4(vec3(edge), 1.0);
}

vec3 CalcLight(vec3 pos, vec3 col, float ambStr, float specStr, float attenConst, float attenLine, float attenQuad, vec3 norm, vec3 viewDir, float textSpec, float shadow) {
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
	
	return ((1.0-shadow)*(ambient + diffuse + specular) * attenuation);
}

float ShadowCalculation(vec4 FragPosLightSpace)
{
	// perspective division 
	vec3 projectionCoordinates = FragPosLightSpace.xyz / FragPosLightSpace.w;

	//transform values into [0,1] rnage
	projectionCoordinates = projectionCoordinates * 0.5 + 0.5;

	//get the closest dpeth value
	float closestDepth = texture(s_ShadowMap, projectionCoordinates.xy).r;

	//get current depth
	float currentDepth = projectionCoordinates.z;

	//check whether's theres a shadow 
	float shadow = currentDepth -shadowBias > closestDepth ? 1.0 : 0.0;

	vec2 texelSize = 1.0 / textureSize(s_ShadowMap, 0);
	for(int x = -1; x <= 1; ++x){
			for(int y = -1; y <= 1; ++y){
					float pcfDepth = texture(s_ShadowMap, projectionCoordinates.xy + vec2(x, y) * texelSize).r; 
					shadow += currentDepth - shadowBias > pcfDepth ? 1.0 : 0.0;        
				}    
		}
	shadow /= 2.0;

	//shadow =(0.5 + (shadow / 18.0));

	return shadow;
}