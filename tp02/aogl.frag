#version 410 core

#define POSITION	0
#define NORMAL		1
#define TEXCOORD	2
#define FRAG_COLOR	0

precision highp int;

uniform sampler2D Diffuse;
uniform sampler2D Specular;
uniform float Time;
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform float lightIntensity;
uniform float specularPower;
uniform vec3 cameraPosition;

layout(location = FRAG_COLOR, index = 0) out vec3 FragColor;

in block
{
	vec2 TexCoord;
	vec3 Position;
	vec3 Normal;
} In;

void main()
{

	vec3 color;

	float valSin = sin(30*(In.TexCoord.x+Time));
	float valCos = cos(30*int(In.TexCoord.x+Time));

	valSin = (valSin < 0.0) ? 0.0 : 1.0;
	valCos = (valCos < 0.0) ? 0.0 : 1.0;
	color = vec3(valSin, 0.0, valCos);

	vec3 n = In.Normal; 
	vec3 l = normalize(lightPosition - In.Position);

	float ndotl = clamp(dot(n,l), 0.0, 1.0);
	vec3 diffuse = texture(Diffuse, In.TexCoord).rgb;


	vec3 v = normalize(cameraPosition - In.Position);
	vec3 h = normalize(l+v);
	float ndoth = clamp(dot(n,h),0.0,1.0);
	vec3 specularColor = texture(Specular, In.TexCoord).rgb;
	vec3 specular = specularColor * pow(ndoth, specularPower);


	float d = length(lightPosition - In.Position);

	color =  lightIntensity * lightColor * (diffuse * (0.25+ndotl) + specular) / (d*d) ;

	FragColor = color;
}
