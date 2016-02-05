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

///////////////////////////////////////////////////

OmniShadowMap::OmniShadowMap(int _textureWidth, int _textureHeight) : textureWidth(_textureWidth), textureHeight(_textureHeight)
{
	glGenFramebuffers(1, &shadowFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);

	/*
	//initialyze shadowRenderBuffer : 
	glGenRenderbuffers(1, &shadowRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, shadowRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, textureWidth, textureHeight);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, shadowRenderBuffer);
	*/

	//initialyze shadow cube texture : 
	glGenTextures(1, &shadowTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, shadowTexture);
	for (int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, textureWidth, textureHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowTexture, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(stderr, "Error on building shadow framebuffer\n");
		exit(EXIT_FAILURE);
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

OmniShadowMap::~OmniShadowMap()
{
	glDeleteTextures(1, &shadowTexture);
	glDeleteRenderbuffers(1, &shadowRenderBuffer);
	glDeleteFramebuffers(1, &shadowFrameBuffer);
}



///////////////////////////////////////////////////////

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

void LightManager::setShadowMapCount(LightType lightType, unsigned int count)
{
	if (lightType == LightType::SPOT)
	{
		spot_shadowMaps.resize(count);
	}
	else if (lightType == LightType::DIRECTIONAL)
	{
		directional_shadowMaps.resize(count);
	}
	else
	{
		point_shadowMaps.resize(count);
	}
}

int LightManager::getShadowMapCount(LightType lightType)
{
	if (lightType == LightType::SPOT)
	{
		return spot_shadowMaps.size();
	}
	else if (lightType == LightType::DIRECTIONAL)
	{
		return directional_shadowMaps.size();
	}
	else
	{
		return point_shadowMaps.size();
	}
}

void LightManager::bindShadowMapFBO(LightType lightType, int index)
{
	if (lightType == LightType::SPOT)
	{
		assert(index >= 0 && index < spot_shadowMaps.size());
		glBindFramebuffer(GL_FRAMEBUFFER, spot_shadowMaps[index].shadowFrameBuffer);
		glViewport(0, 0, spot_shadowMaps[index].textureWidth, spot_shadowMaps[index].textureHeight);
	}
	else if (lightType == LightType::DIRECTIONAL)
	{
		assert(index >= 0 && index < directional_shadowMaps.size());
		glBindFramebuffer(GL_FRAMEBUFFER, directional_shadowMaps[index].shadowFrameBuffer);
		glViewport(0, 0, directional_shadowMaps[index].textureWidth, directional_shadowMaps[index].textureHeight);
	}
	else
	{
		assert(index >= 0 && index < point_shadowMaps.size());
		glBindFramebuffer(GL_FRAMEBUFFER, point_shadowMaps[index].shadowFrameBuffer);
		glViewport(0, 0, point_shadowMaps[index].textureWidth, point_shadowMaps[index].textureHeight);
	}

}

void LightManager::unbindShadowMapFBO(LightType lightType)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void LightManager::bindShadowMapTexture(LightType lightType, int index)
{
	if (lightType == LightType::SPOT)
	{
		assert(index >= 0 && index < spot_shadowMaps.size());
		glBindTexture(GL_TEXTURE_2D, spot_shadowMaps[index].shadowTexture);
	}
	else if (lightType == LightType::DIRECTIONAL)
	{
		assert(index >= 0 && index < directional_shadowMaps.size());
		glBindTexture(GL_TEXTURE_2D, directional_shadowMaps[index].shadowTexture);
	}
	else
	{
		assert(index >= 0 && index < point_shadowMaps.size());
		glBindTexture(GL_TEXTURE_CUBE_MAP, point_shadowMaps[index].shadowTexture);
	}
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
