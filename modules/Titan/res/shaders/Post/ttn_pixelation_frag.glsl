#version 420

//take in the uvs from the vertex shader
layout(location = 0) in vec2 inUV;

//pass on the fragment color
out vec4 frag_color;

//take in the screen texture 
uniform sampler2D s_screenTex;

//pixel amount 
uniform float u_Pixels;

void main() 
{
    //horizontal dimension of pixel
 	float dx = 12.f*(1.f/u_Pixels);
    //vertical dimension of pixel
 	float dy = 10.f*(1.f/u_Pixels);

    //create the pixel
 	vec2 pixelate = vec2(dx*floor(inUV.x/dx), dy*floor(inUV.y/dy));
		
        //sample color at pixel coords to send to pixel
	frag_color =  texture2D(s_screenTex, pixelate); 
}