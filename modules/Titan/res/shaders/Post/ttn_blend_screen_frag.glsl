#version 420

//take in the uvs from the vertex shader
layout(location = 0) in vec2 inUV;

//pass on the fragment color
out vec4 frag_color; 

//take in the screen texture from a framebuffer
layout(binding = 0) uniform sampler2D s_baseImage;
layout(binding = 1) uniform sampler2D s_topImage;

void main() {
	//grab the color of the fragment from both the base image and the layer on top of it 
	vec4 base = texture(s_baseImage, inUV);
	vec4 top = texture(s_topImage, inUV);

	//apply the "screen" blending equation and pass on the result
	frag_color = vec4(1.0) - (vec4(1.0) - base) * (vec4(1.0) - top);
}