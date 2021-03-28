#version 420

//take in the uvs from the vertex shader
layout(location = 0) in vec2 inUV;

//pass on the fragment color
out vec4 frag_color;

//grid fidelty multipler
uniform float u_gridFidelty;
//current time
uniform float time;
//speed
uniform float u_speed = 1.0;

//based on and partly copied from the Art Of Code's voronoi algorthrim: https://youtu.be/l-07BXzNdPw

//creates a random number within certain bounds 
vec2 random(vec2 p) {
	vec3 a = fract(p.xyx * vec3(123.34, 234.34, 345.65));

	a = a + dot(a, a + 34.45);

	return vec2(a.x * a.y * a.z, a.y * a.z * a.z);
}

void main() {
	//convert uvs from 0-1 range to -1 to 1
	vec2 projCoords = (2.0 * inUV) - 1.0;

	//generate the grid coordinate values
	vec2 gridProjCoords = fract(projCoords * u_gridFidelty) - 0.5;

	//generate the id for the current grid value
	vec2 id = floor(projCoords * u_gridFidelty);
	
	//keep track of the minimum distance for the fragment 
	float minDist = 1000.0; //start it at an impossibly high value so it will be overwritten later

	//loop through 9 times calculating the minimum distance 
	for(int x = -1; x <= 1; x++) {
		for(int y = -1; y <= 1; y++) {
			//get the offset vector
			vec2 offset = vec2(x, y);

			//get the random number for the fragment
			vec2 rand = random(id + offset);

			//and get the voronoi point for that number 
			vec2 val = rand * (time * u_speed);
			vec2 point = offset + (((sin(val) + sin(2.2 * val + 3.4) + sin(1.3 * val + 1.3) + sin(5.3 * val + 5.5))) * 0.25);
			//vec2 point = offset + sin(val) + sin(2.2 * val + 3.4) * 0.5;

			//get the distance between the point and the grid coordinate 
			float dist = length(gridProjCoords - point);

			//get the minimum distance
			minDist = mix(minDist, dist, float(int(dist < minDist)));
		}
	}

	//save the minimum distance for the fragment as the color 
	frag_color = vec4(vec3(minDist), 1.0);
}