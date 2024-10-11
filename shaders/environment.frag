
#version 330 core

/*default camera matrices. do not modify.*/
layout(std140) uniform camera
{
    mat4 projection;	/*camera's projection matrix*/
    mat4 view;			/*camera's view matrix*/
    mat4 pvm;			/*camera's projection*view*model matrix*/
    mat4 ortho;			/*camera's ortho projection matrix*/
    vec4 position;		/*camera's position in world space*/
};

/* set light ubo. do not modify.*/
struct light
{
	ivec4 att; 
	vec4 pos; // position
	vec4 dir;
	vec4 amb; // ambient intensity
	vec4 dif; // diffuse intensity
	vec4 spec; // specular intensity
	vec4 atten;
	vec4 r;
};
layout(std140) uniform lights
{
	vec4 amb;
	ivec4 lt_att; // lt_att[0] = number of lights
	light lt[4];
};
uniform samplerCube skybox;
uniform vec2 iResolution;
uniform float iTime;
uniform mat4 model;


/*input variables*/
in vec3 vtx_normal; // vtx normal in world space
in vec3 vtx_position; // vtx position in world space
in vec3 vtx_model_position; // vtx position in model space
in vec4 vtx_color;
in vec2 vtx_uv;
in vec3 vtx_tangent;
in vec2 fragCoord;

/*output variables*/
out vec4 frag_color;

float length2(vec2 p){
    return dot(p,p);
}
float noise(vec2 p){
	return fract(sin(fract(sin(p.x) * (43.13311)) + p.y) * 31.0011);
}
float worley(vec2 p) {
    //Set our distance to infinity
	float d = 1e30;
    //For the 9 surrounding grid points
	for (int xo = -1; xo <= 1; ++xo) {
		for (int yo = -1; yo <= 1; ++yo) {
            //Floor our vec2 and add an offset to create our point
			vec2 tp = floor(p) + vec2(xo, yo);
            //Calculate the minimum distance for this grid point
            //Mix in the noise value too!
			d = min(d, length2(p - tp - noise(tp)));
		}
	}
	return 3.0*exp(-4.0*abs(2.5*d - 1.0));
}
float fworley(vec2 p) {
    //Stack noise layers 
	return sqrt(sqrt(sqrt(
		worley(p*5.0 + 0.05*iTime) *
		sqrt(worley(p * 50.0 + 0.12 + -0.1*iTime)) *
		sqrt(sqrt(worley(p * -10.0 + 0.03*iTime))))));
}

void main()
{
	vec2 uv = fragCoord.xy / iResolution.xy;
    //Calculate an intensity
    float proceduralIntensity = fworley(uv * iResolution.xy / 1500.0);
    //Add some gradient
    proceduralIntensity *= exp(-length2(abs(0.7*uv - 1.0)));    
    //Make it blue!
    vec3 proceduralColor = proceduralIntensity * vec3(0.1, 1.1 * proceduralIntensity, pow(proceduralIntensity, 0.5 - proceduralIntensity));
    
    // Skybox Reflection
    //vec3 I = normalize(vtx_position - position.xyz);
    //vec3 R = reflect(I, normalize(vtx_normal));
    //vec3 skyboxColor = texture(skybox, R).rgb;
    
    // Blend procedural texture and skybox reflection
    //vec3 finalColor = mix(proceduralColor, skyboxColor, 0.5); // Adjust blend factor as needed
    
    frag_color = vec4(proceduralColor, 1.0);
}
