#version 420

layout(location = 0) in vec2 inUV;

layout(binding = 0) uniform sampler2D  s_screenTex; //Source image
uniform float u_Threshold;

out vec4 frag_color;
//based off shader form spritelib
void main() 
{
	vec4 color = texture(s_screenTex, inUV);
	
	//get brightness from tutorial 4 video
	float luminence = 0.2989 * color.r + 0.587 * color.g + 0.114 * color.b;

	if (luminence > u_Threshold) //check if the image is light enough
	{
		frag_color = color;//return the color
	}
	else
	{
		frag_color = vec4(0.0, 0.0, 0.0, 1.0);//return no
	}
}