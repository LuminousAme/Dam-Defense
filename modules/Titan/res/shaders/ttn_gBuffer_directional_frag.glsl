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

layout (binding = 30) uniform sampler2D s_ShadowMap;	

//get gbuffer data
layout (binding = 0) uniform sampler2D s_albedoTex;
layout (binding = 1) uniform sampler2D s_normalsTex;
layout (binding = 2) uniform sampler2D s_specularTex;
layout (binding = 3) uniform sampler2D s_positionTex;

//get the light accumulation buffer
layout (binding = 4) uniform sampler2D s_lightAccumTex;

uniform mat4 u_LightSpaceMatrix;
uniform vec3 u_CamPos;

out vec4 frag_color;

float ShadowCalculation(vec4 fragPosLightSpace, float bias)
{
	//Perspective division
	vec3 projectionCoordinates = fragPosLightSpace.xyz / fragPosLightSpace.w;
	
	//Transform into a [0,1] range
	projectionCoordinates = projectionCoordinates * 0.5 + 0.5;
	
	//Get the closest depth value from light's perspective (using our 0-1 range)
	float closestDepth = texture(s_ShadowMap, projectionCoordinates.xy).r;

	//Get the current depth according to the light
	float currentDepth = projectionCoordinates.z;

	//Check whether there's a shadow
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

	//Return the value
	return shadow;
}

// https://learnopengl.com/Advanced-Lighting/Advanced-Lighting
void main() {

    //albedo 
    vec4 textureColor = texture(s_albedoTex, inUV);
    //normals
    vec3 inNormal = (normalize(texture(s_normalsTex,inUV).rgb)*2.0 )-1.0;
    //specular
    float texSpec = texture(s_specularTex,inUV).r;
    //positions
    vec3 fragPos = texture(s_positionTex,inUV).rgb;


	// Diffuse
	vec3 N = normalize(inNormal);
	vec3 lightDir = normalize(-sun.m_lightDirection.xyz);
	float dif = max(dot(N, lightDir), 0.0);
	vec3 diffuse = dif * sun.m_lightColor.rgb;// add diffuse intensity

	// Specular
	vec3 viewDir  = normalize(u_CamPos - fragPos);
	vec3 h        = normalize(lightDir + viewDir);

	float spec = pow(max(dot(N, h), 0.0), 4.0); // Shininess coefficient (can be a uniform)
	vec3 specular = sun.m_lightSpecularPower * texSpec * spec * sun.m_lightColor.xyz; // Can also use a specular color

	vec3 result = ((sun.m_ambientPower * sun.m_ambientColor.xyz) + // global ambient light
		(diffuse + specular) // light factors from our single light
		);

    
	//if the alpha is less than 31% than it is our clear colour
    if (textureColor.a < 0.31)
    {
		//so just set it to white so there isn't any lighting on the skybox
        result = vec3(1.0,1.0,1.0);
    }

	//pass out the frag color
	frag_color = vec4(result, textureColor.a);
}