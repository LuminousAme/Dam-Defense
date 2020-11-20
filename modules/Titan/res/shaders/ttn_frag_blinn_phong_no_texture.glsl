#version 410

//mesh data from vert shader
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inColor;

//material stuff
uniform float u_Shininess;

//scene ambient lighting
uniform vec3  u_AmbientCol;
uniform float u_AmbientStrength;

//Specfic light stuff
uniform vec3  u_LightPos;
uniform vec3  u_LightCol;
uniform float u_AmbientLightStrength;
uniform float u_SpecularLightStrength;
uniform float u_LightAttenuationConstant;
uniform float u_LightAttenuationLinear;
uniform float u_LightAttenuationQuadratic;

//camera data
uniform vec3  u_CamPos;

//result
out vec4 frag_color;

// https://learnopengl.com/Advanced-Lighting/Advanced-Lighting
void main() {
	// Lecture 5
	vec3 ambient = u_AmbientLightStrength * u_LightCol;

	// Diffuse
	vec3 N = normalize(inNormal);
	vec3 lightDir = normalize(u_LightPos - inPos);

	float dif = max(dot(N, lightDir), 0.0);
	vec3 diffuse = dif * u_LightCol;// add diffuse intensity

	//Attenuation
	float dist = length(u_LightPos - inPos);
	float attenuation = 1.0f / (
		u_LightAttenuationConstant + 
		u_LightAttenuationLinear * dist +
		u_LightAttenuationQuadratic * dist * dist);

	// Specular
	vec3 viewDir  = normalize(u_CamPos - inPos);
	vec3 h        = normalize(lightDir + viewDir);
	float spec = pow(max(dot(N, h), 0.0), u_Shininess); // Shininess coefficient
	vec3 specular = u_SpecularLightStrength * spec * u_LightCol; // Can also use a specular color

	//combine everything
	vec3 result = (
		(u_AmbientCol * u_AmbientStrength) + // global ambient light
		(ambient + diffuse + specular) * attenuation // light factors from our single light
		) * inColor; // Object color

	//save the result and pass it on
	frag_color = vec4(result, 1.0f);
}