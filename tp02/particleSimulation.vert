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
out vec3 outPositions; 
out vec3 outVelocities;
out vec3 outForces;

out float outElapsedTimes;
out float outLifeTimes;
out vec4 outColors;
out vec2 outSizes;

//uniforms :
uniform float DeltaTime;

void main()
{	
	float fixedMass = 0.01f;

	outVelocities = vec3(0,0,0);//inVelocities +  (DeltaTime / fixedMass)*vec3(0.0,0.1,0.0); //inForces;
	outPositions = vec3(0,10,0);//inPositions + DeltaTime*inVelocities;
	outForces = vec3(0.0, 0.0, 0.0); //remove outForces ?

	outElapsedTimes = inElapsedTimes + DeltaTime;
	outLifeTimes = inLifeTimes; //remove outLifeTimes ?
	outColors = vec4(0,1,0,1);//inColors; //todo
	outSizes = inSizes; //todo
}
