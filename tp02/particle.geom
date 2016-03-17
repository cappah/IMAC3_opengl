#version 410 core

layout (points) in;
layout (triangle_strip) out;
layout (max_vertices = 4) out;

uniform mat4 VP;
uniform vec3 CameraUp;
uniform vec3 CameraRight;

out block
{
    vec2 TexCoord;
} Out;

void main()
{
	vec3 Pos = gl_in[0].gl_Position.xyz;
	/*
	Pos -= (CameraRight * 0.5);
	Pos += (CameraUp * 0.5);
    gl_Position = VP * vec4(Pos, 1.0);
    Out.TexCoord = vec2(0.0, 0.0);
    EmitVertex();

    Pos -= CameraUp;
    gl_Position = VP * vec4(Pos, 1.0);
    Out.TexCoord = vec2(0.0, 1.0);
    EmitVertex();

    Pos += CameraUp;
    Pos += CameraRight;
    gl_Position = VP * vec4(Pos, 1.0);
    Out.TexCoord = vec2(1.0, 0.0);
    EmitVertex();

    Pos -= CameraUp;
    gl_Position = VP * vec4(Pos, 1.0);
    Out.TexCoord = vec2(1.0, 1.0);
    EmitVertex();*/

	Pos += vec3(-0.5,0.5,0);
    gl_Position = VP * vec4(Pos, 1.0);
    Out.TexCoord = vec2(0.0, 0.0);
    EmitVertex();

    Pos += vec3(0, -1, 0);
    gl_Position = VP * vec4(Pos, 1.0);
    Out.TexCoord = vec2(0.0, 1.0);
    EmitVertex();

    Pos += vec3(1, 0, 0);
    gl_Position = VP * vec4(Pos, 1.0);
    Out.TexCoord = vec2(1.0, 0.0);
    EmitVertex();

    Pos += vec3(0, 1, 0);
    gl_Position = VP * vec4(Pos, 1.0);
    Out.TexCoord = vec2(1.0, 1.0);
    EmitVertex();

    EndPrimitive();
}
