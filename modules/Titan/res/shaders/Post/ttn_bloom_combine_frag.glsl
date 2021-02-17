#version 420

layout(binding = 0) uniform sampler2D u_Scene;//no bloom
layout(binding = 1) uniform sampler2D u_Bloom;//blur

layout(location = 0) in vec2 inUV;

//uniform float u_Intensity;
//uniform float u_Gamma;

out vec4 frag_color;
//CODE FORM: https://learnopengl.com/Advanced-Lighting/Bloom
void main() 
{
    const float u_Gamma = 2.5;
    const float u_Intensity = 1.0f;

	vec3 hdrColor = texture(u_Scene, inUV).rgb;      
    vec3 bloomColor = texture(u_Bloom, inUV).rgb;
    
    //vec4 hdrColor = texture(u_Scene, inUV);      
    //vec4 bloomColor = texture(u_Bloom, inUV);
    
	hdrColor += bloomColor; // additive blending
	//hdrColor *= bloomColor; // multiplicative blending

	 // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor *u_Intensity); //exposure
    //vec3 result = vec3(1.0) - exp(-hdrColor);

    // gamma correct       
    result = pow(result, vec3(1.0 / u_Gamma));

    frag_color = vec4(result,1.0);

}