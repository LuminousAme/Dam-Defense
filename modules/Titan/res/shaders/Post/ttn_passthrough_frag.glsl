#version 420
//passthrough fraG

//take in the uvs from the vertex shader
layout(location = 0) in vec2 inUV;

//pass on the fragment color
out vec4 frag_color;

//take in the screen texture from a framebuffer
layout (binding = 0) uniform sampler2D s_screenTex;
//and a transparency uniform
uniform float u_Transparency = 1.0f;

void main() 
{
	//just sample the texture
	vec4 source = texture(s_screenTex, inUV);

	//and apply it to the full screen quad
	frag_color = vec4(source.rgb, u_Transparency);
}