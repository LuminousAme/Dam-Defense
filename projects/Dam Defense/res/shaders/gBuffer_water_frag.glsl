#version 420

//mesh data from vert shader
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

//material data
layout(binding=0) uniform sampler2D waterText;
uniform int u_UseDiffuse;
//result, multiple render targets
//we can render color to all of these
layout(location = 0) out vec4 outColors;
layout(location = 1) out vec3 outNormals;
layout(location = 2) out vec3 outSpecs;
layout(location = 3) out vec3 outPositions;

void main() {
	//sample the textures
	vec4 textureColor = texture(waterText, inUV);

	//get the albedo from the diffuse / abledo texture map and output it
	outColors = mix(vec4(1.0), textureColor, u_UseDiffuse);

	//calculate the normal vectors and convert it from [-1, 1] range to [0, 1] range and output it 
	outNormals = (normalize(inNormal) * 0.5) + 0.5;

	//find the specular from the texture and output it 
	outSpecs = vec3(1.0);

	//output the world space positions
	outPositions = inPos;
}