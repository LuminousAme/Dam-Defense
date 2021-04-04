#version 420 

//take in the uvs from the vertex shader
layout(location = 0) in vec2 inUV;

//pass on the fragment color
out vec4 frag_color; 

//take in the screen texture from a framebuffer
layout(binding = 0) uniform sampler2D s_SourceImage;
//and a uniform for how much to step before reaching the next fragment (may skip fragments) 
uniform float u_Step; 

//helpful : http://blog.trsquarelab.com/2015/12/a-simple-box-blur-implementation-in.html 
// http://amritamaz.net/blog/understanding-box-blur 

void main() {
	vec4 result = vec4(0.0);

	//add all the surrounding pixel colors together then divided to get the average 
	//sample the current fragment and add it's influence
	result += texture(s_SourceImage, inUV)  ;

	//sample the next fragment over in each direction and add it's influence
	result += texture(s_SourceImage, vec2(inUV.x + u_Step, inUV.y))  ;
	result += texture(s_SourceImage, vec2(inUV.x - u_Step, inUV.y))  ;

	//and continue with that pattern
	result += texture(s_SourceImage, vec2(inUV.x + 2.0 * u_Step, inUV.y))  ;
	result += texture(s_SourceImage, vec2(inUV.x - 2.0 * u_Step, inUV.y))  ;

	result += texture(s_SourceImage, vec2(inUV.x + 3.0 * u_Step, inUV.y))  ;
	result += texture(s_SourceImage, vec2(inUV.x - 3.0 * u_Step, inUV.y))  ;

	result += texture(s_SourceImage, vec2(inUV.x + 4.0 * u_Step, inUV.y))  ;
	result += texture(s_SourceImage, vec2(inUV.x - 4.0 * u_Step, inUV.y))  ;

	//divide by 9 to get the average
	result = result / 9.f;

	//save the result and pass it on 
	frag_color = vec4(result.rgb, 1.0);
}