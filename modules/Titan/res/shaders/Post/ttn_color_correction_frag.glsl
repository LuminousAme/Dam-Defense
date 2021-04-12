#version 420

//get the uvs from the vertex shader
layout(location = 0) in vec2 inUV;

//pass on the fragment color
out vec4 frag_color;

//get the frame and the color correction look up table from texture bindings
layout (binding = 0) uniform sampler2D u_FinishedFrame;
layout (binding = 30) uniform sampler3D u_TextColorGrade;

//uniform intensity of the effect
uniform float u_Intensity = 1.0;

void main() 
{
	//sample the framebuffer texture
	vec4 textureColor = texture(u_FinishedFrame, inUV);

	//grab some scale and offset vectors
	vec3 scale = vec3((64.0 - 1.0) / 64.0);
	vec3 offset = vec3(1.0 / (2.0 * 64.0));

	//and mix the base color with the corrected color based on the intensity and pass it on as the fragment color
	frag_color = vec4(mix(textureColor.rgb, texture(u_TextColorGrade, scale * textureColor.rgb + offset).rgb, u_Intensity), textureColor.a);
}