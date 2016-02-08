
#version 410 core

in block
{
    vec2 Texcoord;
} In;

out vec4 Color;
 

// Uniforms : 


uniform sampler2D ColorBuffer;
uniform sampler2D NormalBuffer;
uniform sampler2D DepthBuffer;
uniform mat4 ScreenToWorld;
uniform vec3 CameraPosition;
 
 //shadow : 
 uniform samplerCube Shadow;
 uniform float FarPlane;

//lights struct : 

struct PointLight{
	vec3 position;
	vec3 color;
	float intensity; 
};

//light uniforms : 
uniform PointLight pointLight;


vec3 computePointLight(PointLight light, vec3 p, vec3 n,  vec3 diffuse, vec3 specular, float specularPower)
{
	vec3 l = normalize(light.position - p);
	float ndotl = clamp(dot(n,l), 0.0, 1.0);
	vec3 v = normalize(CameraPosition - p);
	vec3 h = normalize(l+v);
	float ndoth = clamp(dot(n,h),0.0,1.0);
	float d = length(light.position - p);

	diffuse /= 3.1415f;

	specular *= pow(ndoth, specularPower);
	specular /= ( (specularPower + 8.0) / (8.0*3.1415) );

	float intensity = light.intensity / (d*d);

	return intensity * light.color * 3.1415 * (diffuse + specular) * (ndotl) ; 
}


void main(void)
{
	// Read gbuffer values
	vec4 colorBuffer = texture(ColorBuffer, In.Texcoord).rgba;
	vec4 normalBuffer = texture(NormalBuffer, In.Texcoord).rgba;
	float depth = texture(DepthBuffer, In.Texcoord).r;

	// Convert texture coordinates into screen space coordinates
	vec2 xy = In.Texcoord * 2.0 -1.0;
	// Convert depth to -1,1 range and multiply the point by ScreenToWorld matrix
	vec4 wP = vec4(xy, depth * 2.0 -1.0, 1.0) * ScreenToWorld;
	// Divide by w
	vec3 p = vec3(wP.xyz / wP.w);

	vec3 diffuse = colorBuffer.rgb;
	vec3 specular = colorBuffer.aaa;
	float specularPower = normalBuffer.a;
	vec3 n = normalBuffer.rgb*2.0 -1.0;

	vec3 color = computePointLight( pointLight, p, n, diffuse, specular, specularPower * 100 );

	//shadow
	float shadowBias = 0.05f;
	vec3 lightToFrag = p - pointLight.position;
	float clothestDepth = texture(Shadow, lightToFrag).r;
	clothestDepth *= FarPlane;
	float currentDepth = length(lightToFrag);

	if(clothestDepth + shadowBias > currentDepth)
		Color = vec4(color, 1.0);
	else
		Color = vec4(0.0, 0.0, 0.0, 1.0);
}