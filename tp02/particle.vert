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

layout(location = VERTEX) in vec3 Position;
layout(location = NORMALS) in vec3 Normal;
layout(location = UVS) in vec2 TexCoord;

layout(location = POSITIONS) in vec3 Translation;
layout(location = VELOCITIES) in vec3 Velocity;
layout(location = FORCES) in vec3 Force;
//layout(location = ELAPSED_TIMES) in float ElapsedTime;
//layout(location = LIFE_TIMES) in float LifeTIme;
//layout(location = COLORS) in vec4 Color;
//layout(location = SIZES) in vec2 Size;

uniform mat4 VP;
uniform vec3 CameraRight;
uniform vec3 CameraUp;

out block
{
	vec2 TexCoord; 
	vec3 Position;
	vec3 Normal;
	vec4 Color;
} Out;

void main()
{	
	//TMP
	vec2 Size = vec2(1,1);
	vec4 Color = vec4(1,0,0,1);

	vec3 pos = Position;
	vec3 posWorldSpace = Translation + CameraRight * Position.x * Size.x + CameraUp * Position.z * Size.y;

	Out.Color = Color;
	Out.TexCoord = TexCoord;// * TextureRepetition;
	Out.Position = pos;
	Out.Normal =  Normal;//normalize( vec3(NormalMatrix * vec4(Normal, 0.0)) );

	gl_Position = VP * vec4(posWorldSpace, 1.0);
}
