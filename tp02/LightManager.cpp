#include "LightManager.h"

ShadowMap::ShadowMap(int _textureWidth, int _textureHeight) : textureWidth(_textureWidth), textureHeight(_textureHeight)
{
	glGenFramebuffers(1, &shadowFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);

	//initialyze shadowRenderBuffer : 
	glGenRenderbuffers(1, &shadowRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, shadowRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, textureWidth, _textureHeight);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, shadowRenderBuffer);

	//initialyze shadow texture : 
	glGenTextures(1, &shadowTexture);
	glBindTexture(GL_TEXTURE_2D, shadowTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, textureWidth, _textureHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(stderr, "Error on building shadow framebuffer\n");
		exit(EXIT_FAILURE);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ShadowMap::~ShadowMap()
{
	glDeleteTextures(1, &shadowTexture);
	glDeleteRenderbuffers(1, &shadowRenderBuffer);
	glDeleteFramebuffers(1, &shadowFrameBuffer);
}

LightManager::LightManager() //: globalIntensity(0.f)
{

}

void LightManager::init(GLuint glProgram_pointLight, GLuint glProgram_directionalLight, GLuint glProgram_spotLight)
{
	uniform_pointLight_pos = glGetUniformLocation(glProgram_pointLight, "pointLight.position");
	uniform_pointLight_col = glGetUniformLocation(glProgram_pointLight, "pointLight.color");
	uniform_pointLight_int = glGetUniformLocation(glProgram_pointLight, "pointLight.intensity");

	uniform_directionalLight_dir = glGetUniformLocation(glProgram_directionalLight, "directionalLight.direction");
	uniform_directionalLight_col = glGetUniformLocation(glProgram_directionalLight, "directionalLight.color");
	uniform_directionalLight_int = glGetUniformLocation(glProgram_directionalLight, "directionalLight.intensity");

	uniform_spotLight_dir = glGetUniformLocation(glProgram_spotLight, "spotLight.direction");
	uniform_spotLight_col = glGetUniformLocation(glProgram_spotLight, "spotLight.color");
	uniform_spotLight_int = glGetUniformLocation(glProgram_spotLight, "spotLight.intensity");
	uniform_spotLight_pos = glGetUniformLocation(glProgram_spotLight, "spotLight.position");
	uniform_spotLight_angle = glGetUniformLocation(glProgram_spotLight, "spotLight.angle");
}

void LightManager::setShadowMapCount(unsigned int count)
{
	shadowMaps.resize(count);	
}

int LightManager::getShadowMapCount()
{
	return shadowMaps.size();
}

void LightManager::bindShadowMapFBO(int index)
{
	assert(index >= 0 && index < shadowMaps.size());

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMaps[index].shadowFrameBuffer);
	glViewport(0, 0, shadowMaps[index].textureWidth, shadowMaps[index].textureHeight);
}

void LightManager::unbindShadowMapFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void LightManager::bindShadowMapTexture(int index)
{
	assert(index >= 0 && index < shadowMaps.size());

	glBindTexture(GL_TEXTURE_2D, shadowMaps[index].shadowTexture);
}

void LightManager::uniformPointLight(PointLight & light)
{
	glUniform3fv(uniform_pointLight_pos, 1, glm::value_ptr(light.position));
	glUniform3fv(uniform_pointLight_col, 1, glm::value_ptr(light.color));
	glUniform1f(uniform_pointLight_int, light.intensity);
}

void LightManager::uniformDirectionalLight(DirectionalLight & light)
{
	glUniform3fv(uniform_directionalLight_dir, 1, glm::value_ptr(light.direction));
	glUniform3fv(uniform_directionalLight_col, 1, glm::value_ptr(light.color));
	glUniform1f(uniform_directionalLight_int, light.intensity);
}

void LightManager::uniformSpotLight(SpotLight & light)
{
	glUniform3fv(uniform_spotLight_dir, 1, glm::value_ptr(light.direction));
	glUniform3fv(uniform_spotLight_col, 1, glm::value_ptr(light.color));
	glUniform1f(uniform_spotLight_int, light.intensity);
	glUniform3fv(uniform_spotLight_pos, 1, glm::value_ptr(light.position));
	glUniform1f(uniform_spotLight_angle, light.angle);
}
