#version 420

//take in the uvs from the vertex shader
layout(location = 0) in vec2 inUV;

//pass on the fragment color
out vec4 frag_color;

//take in the screen texture 
uniform sampler2D s_screenTex;

// amount 
uniform float u_Amount;

//from: https://simonharris.co/making-a-noise-film-grain-post-processing-effect-from-scratch-in-threejs/
float random (vec2 p) {
  vec2 k1 = vec2( 23.14069263277926, // e^pi (Gelfond's constant)
    2.665144142690225 // 2^sqrt(2) (Gelfond–Schneider constant)
  );
   return fract(cos(dot(p,k1) ) * 12345.6789);
}

void main() 
{
  vec4 color = texture2D( s_screenTex, inUV );
  vec2 randomIntensity  = inUV;

  randomIntensity.y *= random(vec2(randomIntensity.y, u_Amount));
  //amount *= randomIntensity;

  color.rgb += random(randomIntensity )*0.25;
  frag_color = vec4(color.rgb, color.a);

}