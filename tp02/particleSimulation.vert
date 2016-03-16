#version 410 core


precision highp float;
precision highp int;


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

//inputs : 
layout(location = POSITION) in vec3 inPositions;
layout(location = VELOCITIES) in vec3 inVelocities;
layout(location = FORCES) in vec3 inForces;
layout(location = ELAPSED_TIMES) in float inElapsedTimes;
layout(location = LIFE_TIMES) in float inLifeTimes;
layout(location = COLORS) in vec4 inColors;
layout(location = SIZES) in vec2 inSizes;

//outputs : 
out vec3 positions; 
out vec3 velocities;
out vec3 forces;
out float elapsedTimes;
out float lifeTimes;
out vec4 colors;
out vec2 sizes;


void main()
{	
	velocities =  inVelocities;//inVelocities +  (DeltaTime / fixedMass)*vec3(0.0,0.1,0.0); //inForces;
	positions = inPositions;//inPositions + DeltaTime*inVelocities;
	forces = inForces; //remove outForces ?
	elapsedTimes = inElapsedTimes;
	lifeTimes = inLifeTimes; //remove outLifeTimes ?
	colors = inColors;//inColors; //todo
	sizes = inSizes; //todo
}
