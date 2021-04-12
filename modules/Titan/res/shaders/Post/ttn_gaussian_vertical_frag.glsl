#version 420 

//take in the uvs from the vertex shader
layout(location = 0) in vec2 inUV;

//pass on the fragment color
out vec4 frag_color; 

//take in the screen texture from a framebuffer
layout(binding = 0) uniform sampler2D s_SourceImage;
//and a uniform for how much to step before reaching the next fragment (may skip fragments) 
uniform float u_Step; 
//the weights for the blur
uniform float u_Weights[5];

void main() {
	vec4 result = vec4(0.0);

	//sample the current fragment and add it's influence
	result += texture(s_SourceImage, inUV) * u_Weights[0];

	//sample the next fragment over in each direction and add it's influence
	result += texture(s_SourceImage, vec2(inUV.x, inUV.y + u_Step)) * u_Weights[1];
	result += texture(s_SourceImage, vec2(inUV.x, inUV.y - u_Step)) * u_Weights[1];

	//and continue with that pattern
	result += texture(s_SourceImage, vec2(inUV.x, inUV.y + 2.0 * u_Step)) * u_Weights[2];
	result += texture(s_SourceImage, vec2(inUV.x, inUV.y - 2.0 * u_Step)) * u_Weights[2];

	result += texture(s_SourceImage, vec2(inUV.x, inUV.y + 3.0 * u_Step)) * u_Weights[3];
	result += texture(s_SourceImage, vec2(inUV.x, inUV.y - 3.0 * u_Step)) * u_Weights[3];

	result += texture(s_SourceImage, vec2(inUV.x, inUV.y + 4.0 * u_Step)) * u_Weights[4];
	result += texture(s_SourceImage, vec2(inUV.x, inUV.y - 4.0 * u_Step)) * u_Weights[4];

	//save the result and pass it on 
	frag_color = vec4(result.rgb, 1.0);
}