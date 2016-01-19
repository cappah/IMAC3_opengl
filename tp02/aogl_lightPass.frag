#version 410 core

in block
{
    vec2 Texcoord;
} In;
 
uniform sampler2D ColorBuffer;
uniform sampler2D NormalBuffer;
uniform sampler2D DepthBuffer;
 
out vec4 Color;
 
 
// Uniforms : 

uniform sampler2D Diffuse;
uniform sampler2D Specular;
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
uniform PointLight pointLights[ 10 ];
uniform DirectionalLight directionalLights[ 10 ];
uniform SpotLight spotLights[ 10 ];

void main(void)
{
	// Read gbuffer values
	vec4 colorBuffer = texture(ColorBuffer, In.Texcoord).rgba;
	vec4 normalBuffer = texture(NormalBuffer, In.Texcoord).rgba;
	float depth = texture(DepthBuffer, In.Texcoord).r;

	// Unpack values stored in the gbuffer
	vec3 diffuseColor = colorBuffer.rgb;
	vec3 specularColor = colorBuffer.aaa;
	float specularPower = normalBuffer.a;
	vec3 n = normalBuffer.rgb;

    Color = vec4(diffuseColor.rgb, 1.0);
}