#version 410 core

uniform vec4 Color; //todo : remove this uniform
uniform sampler2D Texture;

layout(location = 0) out vec4 FragColor;

in block
{
        vec2 TexCoord;
        vec3 Position;
        vec3 Normal;
		vec4 Color;
} In;

void main()
{
	//no lighting for the moment 

	FragColor = texture2D(Texture, In.TexCoord) * In.Color;
}
