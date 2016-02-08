#version 410 core
 
in block
{
vec2 Texcoord;
} In;
 
uniform sampler2D Texture;
uniform float Gamma = 1.0;
 
layout(location = 0, index = 0) out vec4 Color;
 
void main(void)
{
	vec3 color = texture(Texture, In.Texcoord).rgb;
	color.r = pow(color.r, 1.0/Gamma);
	color.g = pow(color.g, 1.0/Gamma);
	color.b = pow(color.b, 1.0/Gamma);
	Color = vec4(color, 1.0);
}