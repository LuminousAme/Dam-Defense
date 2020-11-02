#version 410

//mesh data from vert shader
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inColor;

//result
out vec4 frag_color;

//texture
uniform sampler2D s_Diffuse;

void main() {
	// Get the albedo from the diffuse / albedo map
	vec4 textureColor = texture(s_Diffuse, inUV);

	//combine the texture and in color 
	vec3 result = inColor * textureColor.rgb;

	//save the result and pass it on
	frag_color = vec4(result, textureColor.a);
}