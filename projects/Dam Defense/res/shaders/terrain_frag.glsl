#version 420

//mesh data from vert shader
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in float inHeight;

//material data
layout(binding=1) uniform sampler2D s_base;
layout(binding=2) uniform sampler2D s_second;
layout(binding=3) uniform sampler2D s_third;

//scene ambient lighting
uniform vec3  u_AmbientCol;
uniform float u_AmbientStrength;

//result
out vec4 frag_color;

void main() {
	//sample the textures
	vec4 textureColor = vec4(1.0, 1.0, 1.0, 1.0);
	if(inHeight <= 0.5) {
		textureColor = mix(texture(s_base, inUV), texture(s_second, inUV), smoothstep(0.0, 0.5, inHeight));
	}
	else {
		textureColor = mix(texture(s_second, inUV), texture(s_third, inUV), smoothstep(0.5, 1.0, inHeight));
	}

	if(textureColor.a < 0.01)
		discard;

	//combine everything
	vec3 result = u_AmbientCol * u_AmbientStrength; // global ambient light

	//add that to the texture color
	result = result * textureColor.rgb;

	//save the result and pass it on
	frag_color = vec4(result, textureColor.a);
}