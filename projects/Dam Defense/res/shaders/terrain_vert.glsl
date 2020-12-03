#version 420
//mesh data from c++ program
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

//mesh data to pass to the frag shader
layout(location = 0) out vec3 outPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outUV;
layout(location = 3) out float outHeight;

//texture
layout(binding=0)uniform sampler2D map;

//influnce the displacement map should have 
uniform float u_scale;

//model, view, projection matrix
uniform mat4 MVP;
//model matrix only
uniform mat4 Model; 
//normal matrix
uniform mat3 NormalMat;

void main() {
	//pass data onto the frag shader
	outNormal = NormalMat * inNormal;
	outUV = inUV;

	//displace the terrain based on the map
	vec3 vert = inPos;
	vert.y = vert.y + texture(map, inUV).r * u_scale;
	//pass the new position onto the frag shader
	outPos =  (Model * vec4(vert, 1.0)).xyz;
	outHeight = texture(map, inUV).r;
		
	vec4 newPos = MVP * vec4(vert, 1.0);
	gl_Position = newPos;
}