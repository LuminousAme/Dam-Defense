#version 420

//mesh data from vert shader
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

//material data
layout(binding=0) uniform sampler2D waterText;

//scene ambient lighting
uniform vec3  u_AmbientCol;
uniform float u_AmbientStrength;

//result
out vec4 frag_color;

void main() {
	//sample the textures
	vec4 textureColor = texture(waterText, inUV);

	if(textureColor.a < 0.01)
		discard;

	//combine everything
	vec3 result = u_AmbientCol * u_AmbientStrength; // global ambient light

	//add that to the texture color
	result = result * textureColor.rgb;

	//save the result and pass it on
	//frag_color = vec4(result, textureColor.a);
	frag_color = vec4(result, 1.0);
}