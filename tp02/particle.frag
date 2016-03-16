#version 410 core

uniform sampler2D Texture;

layout(location = 0) out vec4 FragColor;

in block
{
    vec2 TexCoord;
} In;

void main()
{
	//no lighting for the moment 

	FragColor = vec4(1,0,0,1);// texture2D(Texture, In.TexCoord) * In.Color;
}
