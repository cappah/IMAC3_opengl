
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

struct SpotLight{
        vec3 direction;
        vec3 position;
        float angle;
        vec3 color;
        float intensity;
};

//light uniforms : 
uniform int pointLight_count;
uniform int directionalLight_count;
uniform int spotLight_count;
uniform PointLight pointLights[ 200 ];
uniform DirectionalLight directionalLights[ 30 ];
uniform SpotLight spotLights[ 30 ];



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
	specular /= ( (specularPower + 8) / (8*3.1415) );

	float intensity = light.intensity / (d*d);

	return intensity * light.color * 3.1415 * (diffuse + specular) * (ndotl) ; 
}


vec3 computeDirectionalLight(DirectionalLight light, vec3 p, vec3 n,  vec3 diffuse, vec3 specular, float specularPower)
{
	vec3 l = normalize(-light.direction);
	float ndotl = clamp(dot(n,l), 0.0, 1.0);
	vec3 v = normalize(CameraPosition - p);
	vec3 h = normalize(l+v);
	float ndoth = clamp(dot(n,h),0.0,1.0);

	diffuse /= 3.1415f;

	specular *= pow(ndoth, specularPower);
	specular /= ( (specularPower + 8) / (8*3.1415) );

	float intensity = light.intensity;

	return intensity * light.color * 3.1415 * (diffuse + specular) * (ndotl) ; 
}

vec3 computeSpotLight(SpotLight light, vec3 p, vec3 n,  vec3 diffuse, vec3 specular, float specularPower)
{
    vec3 l = normalize( light.position - p);
    float ndotl = clamp(dot(n,l), 0.0, 1.0);
    vec3 v = normalize(CameraPosition - p);
    vec3 h = normalize(l+v);
    float ndoth = clamp(dot(n,h),0.0,1.0);
    float d = length(light.position - p);

    float spotFactor = dot( -l, normalize(light.direction));

	diffuse /= 3.1415f;

    specular *= pow(ndoth, specularPower);
    specular /= ( (specularPower + 8) / (8*3.1415) );

    float intensity = light.intensity / (d*d) ;
    //if(spotFactor > light.angle * 0.5f)
    //    return vec3(0,0,0);

    float teta = dot(-l, normalize(light.direction));
    float phi = cos(light.angle);
    float omega = cos(light.angle*1.3f);

    float falloff = pow( ( teta - phi ) / ( phi - omega ) , 4.f );

	intensity *= clamp(falloff, 0, 1);	

    return intensity * light.color * 3.1415 * (diffuse + specular) * (ndotl) ; 
}

void main(void)
{
	vec3 color = vec3(0,0,0);

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
	vec3 n = normalBuffer.rgb;

	for(int i = 0; i < pointLight_count; i++)
	{
		color += computePointLight( pointLights[i], p, n, diffuse, specular, specularPower );
	}
	
	for(int i = 0; i < directionalLight_count; i++)
	{
		color += computeDirectionalLight(directionalLights[i], p, n, diffuse, specular, specularPower);
    }

    for(int i = 0; i < spotLight_count; i++)
    {
        color += computeSpotLight(spotLights[i], p, n, diffuse, specular, specularPower);
    }

    Color = vec4(color, 1.0);
}