#version 420
//ambient gbuffer
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
    DirectionalLight ambience;
};


layout (binding = 0) uniform sampler2D s_albedoTex;
layout (binding = 4) uniform sampler2D s_lightAccumTex;

out vec4 frag_color;


void main(){

    //albedo 
    vec4 textureColor = texture(s_albedoTex, inUV); 

    //lights 
    vec4 lightAccum = texture(s_lightAccumTex, inUV);

    //ambient calculation
    vec3 ambient = ambience.m_lightAmbientPower * ambience.m_ambientColor.rgb;

    //the result of all lighting
    vec3 result = (ambient + lightAccum.rgb) * textureColor.rgb;

    //the light accumulation
    frag_color = vec4 (result,1.0);
}