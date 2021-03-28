#version 420

//mesh data from vert shader
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec4 inClipSpace;

//material data
layout(binding=0) uniform sampler2D waterText;
layout(binding=1) uniform sampler2D voronoiText;
layout(binding=2) uniform sampler2D refractionText;
layout(binding=3) uniform sampler2D reflectionText;

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
	vec4 textureVoronoi = texture(voronoiText, inUV);
	vec4 textureVoronoiDark = texture(voronoiText, clamp(inUV + 0.05, vec2(0.0), vec2(1.0)));

	//mix the voronois into the base text's colour 
	textureVoronoi = pow(textureVoronoi, vec4(9.5));
	textureVoronoiDark = pow(textureVoronoiDark, vec4(9.5));
	textureColor = textureColor - (0.1 * normalize(textureVoronoiDark));
	textureColor = textureColor + textureVoronoi;

	//get the reflection and refraction components
	vec2 ndc = 0.5 * (inClipSpace.xy / inClipSpace.w) + 0.5;
	vec4 refractionColor = texture(refractionText, ndc);
	vec4 reflectionColor = texture(reflectionText, vec2(ndc.x, -1.0 * ndc.y));

	//get the albedo from the diffuse / abledo texture map and output it
	outColors = mix(vec4(1.0), textureColor, u_UseDiffuse);

	//calculate the normal vectors and convert it from [-1, 1] range to [0, 1] range and output it 
	outNormals = (normalize(inNormal) * 0.5) + 0.5;

	//find the specular from the texture and output it 
	outSpecs = vec3(1.0);

	//output the world space positions
	outPositions = inPos;
}