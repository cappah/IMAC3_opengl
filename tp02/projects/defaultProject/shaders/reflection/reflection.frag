#version 410 core

// Outputs : 
layout(location = 0) out vec4 outColor;

uniform sampler2D ReflexionTexture;

void main(void)
{
    vec2 texCoord = gl_FragCoord.xy / textureSize(ReflexionTexture, 0);
    vec3 color = texture(ReflexionTexture, texCoord).rgb;
    outColor = vec4(color, 1.0);
}
