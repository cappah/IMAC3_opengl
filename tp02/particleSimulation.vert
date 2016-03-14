#version 410 core


precision highp float;
precision highp int;

uniform vec3 DeltaTime;

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
in vec3 inPositions;

//outputs : 
out vec3 outPositions; 

void main()
{	
	//TODO : physic stuffd

	outPositions = vec3(0,0,0);
}
