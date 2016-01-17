#version 410 core

#define POSITION	0
#define NORMAL		1
#define TEXCOORD	2
#define FRAG_COLOR	0

precision highp int;

uniform sampler2D Diffuse;
uniform sampler2D Specular;
uniform float Time;
uniform float specularPower;
uniform vec3 cameraPosition;


//lights struct : 

struct PointLight{
	vec3 position;
	vec3 color;
	float intensity; 
};

struct DirectionalLight{
	vec3 direction;
	vec3 color;
	float intensity; 
};

//light uniforms : 
uniform int pointLight_count;
uniform int directionalLight_count;
uniform PointLight pointLights[ 10 ];
uniform DirectionalLight directionalLights[ 10 ];


layout(location = FRAG_COLOR, index = 0) out vec3 FragColor;

in block
{
	vec2 TexCoord;
	vec3 Position;
	vec3 Normal;
} In;


vec3 computePointLight(PointLight light)
{
	vec3 n = In.Normal; 
	vec3 l = normalize(light.position - In.Position);
	float ndotl = clamp(dot(n,l), 0.0, 1.0);
	vec3 v = normalize(cameraPosition - In.Position);
	vec3 h = normalize(l+v);
	float ndoth = clamp(dot(n,h),0.0,1.0);
	float d = length(light.position - In.Position);

	vec3 diffuse = texture(Diffuse, In.TexCoord).rgb;
	diffuse *= light.color;

	vec3 specularColor = texture(Specular, In.TexCoord).rgb;
	vec3 specular = specularColor * pow(ndoth, specularPower);
	specular *= light.color;

	float intensity = light.intensity / (d*d);

	return intensity * (diffuse + specular) ; 
}


vec3 computeDirectionalLight(DirectionalLight light)
{

	vec3 n = In.Normal; 
	vec3 l = normalize(-light.direction);
	float ndotl = clamp(dot(n,l), 0.0, 1.0);
	vec3 v = normalize(cameraPosition - In.Position);
	vec3 h = normalize(l+v);
	float ndoth = clamp(dot(n,h),0.0,1.0);

	vec3 diffuse = texture(Diffuse, In.TexCoord).rgb;
	diffuse *= light.color;

	vec3 specularColor = texture(Specular, In.TexCoord).rgb;
	vec3 specular = specularColor * pow(ndoth, specularPower);
	specular *= light.color;

	float intensity = light.intensity;

	return intensity * (diffuse + specular) ;
}

void main()
{
	vec3 color;

	
	for(int i = 0; i < pointLight_count; i++)
	{
		color += computePointLight( pointLights[i] );
	}

	
	for(int i = 0; i < directionalLight_count; i++)
	{
		color += computeDirectionalLight(directionalLights[i]);
	}

	FragColor = color;
}
