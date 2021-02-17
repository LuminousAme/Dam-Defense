#version 420

//take in the uvs from the vertex shader
layout(location = 0) in vec2 inUV;

//pass on the fragment color
out vec4 frag_color; 

//take in the screen texture from a framebuffer
layout(binding = 0) uniform sampler2D s_SourceImage;
//and a uniform for the threshold above which we extract 
uniform float u_Threshold; 

void main() {
	//get the starting color of the fragment
	vec4 color = texture(s_SourceImage, inUV);

	//calculate the normalized (0-1) brightness of the fragment
	float lumience = 0.2989 * color.r + 0.587 * color.g + 0.114 * color.b;

	//use that to decide if the fragment should be black (below threshold) or retain it's color (above threshold)
	frag_color = mix(vec4(vec3(0.0), 1.0), color, ceil(lumience - u_Threshold));
}