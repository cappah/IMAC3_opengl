#version 410 core


precision highp float;
precision highp int;

#define VERTEX 0
#define NORMALS 1
#define UVS 2
#define POSITIONS 3
#define VELOCITIES 4
#define FORCES 5
#define ELAPSED_TIMES 6
#define LIFE_TIMES 7
#define COLORS 8
#define SIZES 9

layout(location = POSITIONS) in vec3 Position;
//layout(location = SIZES) in vec3 Size;
//layout(location = COLORS) in vec3 Color;

void main()
{	
	gl_Position = vec4(0,0,0,1);//vec4(Position, 1.0);
}
