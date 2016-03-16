#version 410 core

#define VERTEX 0
#define NORMALS 1
#define UVS 2
#define POSITION 3
#define VELOCITIES 4
#define FORCES 5
#define ELAPSED_TIMES 6
#define LIFE_TIMES 7
#define COLORS 8
#define SIZES 9

layout(points) in;
layout(points) out;
layout(max_vertices = 30) out;

//uniforms :
uniform vec4 Color; //todo : remove this uniform
uniform sampler2D Texture;
uniform float DeltaTime;

//inputs :
in vec3 positions[]; 
in vec3 velocities[];
in vec3 forces[];
in float elapsedTimes[];
in float lifeTimes[];
in vec4 colors[];
in vec2 sizes[];

//outputs :
out vec3 outPositions; 
out vec3 outVelocities;
out vec3 outForces;
out float outElapsedTimes;
out float outLifeTimes;
out vec4 outColors;
out vec2 outSizes;

void main()
{
	float fixedMass = 0.1;

	//update time :
	outElapsedTimes = elapsedTimes[0] + DeltaTime;

	//forces update :
	outVelocities =  velocities[0] +  (DeltaTime / fixedMass)*vec3(0.0,0.1,0.0);
	outPositions = vec3(1,1,1);// positions[0] + DeltaTime*velocities[0];
	
	//todo :
	outColors = colors[0];
	outSizes = sizes[0];

	//remove ?
	outForces = forces[0];
	outLifeTimes = lifeTimes[0];

	EmitVertex();
	EndPrimitive();
}
