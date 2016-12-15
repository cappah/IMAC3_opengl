#include "stdafx.h"

#include "LightManager.h"

ShadowMap::ShadowMap(int _textureWidth, int _textureHeight) : textureWidth(_textureWidth), textureHeight(_textureHeight)
{
	glGenFramebuffers(1, &shadowFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);

	//initialyze shadowRenderBuffer : 
	glGenRenderbuffers(1, &shadowRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, shadowRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, textureWidth, textureHeight);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, shadowRenderBuffer);

	//initialyze shadow texture : 
	glGenTextures(1, &shadowTexture);
	glBindTexture(GL_TEXTURE_2D, shadowTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, textureWidth, textureHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//2*2 percentage close filtering : 
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(stderr, "Error on building shadow framebuffer\n");
		exit(EXIT_FAILURE);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ShadowMap::ShadowMap(ShadowMap&& other) noexcept
{
	textureWidth = other.textureWidth;
	textureHeight = other.textureHeight;
	shadowFrameBuffer = other.shadowFrameBuffer;
	shadowRenderBuffer = other.shadowRenderBuffer;
	shadowTexture = other.shadowTexture;

	other.textureWidth = 0.f;
	other.textureHeight = 0.f;
	other.shadowFrameBuffer = 0;
	other.shadowRenderBuffer = 0;
	other.shadowTexture = 0;
}

ShadowMap& ShadowMap::operator=(ShadowMap&& other) noexcept
{
	if (&other != this)
	{
		//we keep texture alive in openGl context.

		textureWidth = other.textureWidth;
		textureHeight = other.textureHeight;
		shadowFrameBuffer = other.shadowFrameBuffer;
		shadowRenderBuffer = other.shadowRenderBuffer;
		shadowTexture = other.shadowTexture;

		other.textureWidth = 0.f;
		other.textureHeight = 0.f;
		other.shadowFrameBuffer = 0;
		other.shadowRenderBuffer = 0;
		other.shadowTexture = 0;
	}

	return *this;
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
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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


OmniShadowMap::OmniShadowMap(OmniShadowMap&& other) noexcept
{
	textureWidth = other.textureWidth;
	textureHeight = other.textureHeight;
	shadowFrameBuffer = other.shadowFrameBuffer;
	shadowRenderBuffer = other.shadowRenderBuffer;
	shadowTexture = other.shadowTexture;

	other.textureWidth = 0.f;
	other.textureHeight = 0.f;
	other.shadowFrameBuffer = 0;
	other.shadowRenderBuffer = 0;
	other.shadowTexture = 0;
}

OmniShadowMap& OmniShadowMap::operator=(OmniShadowMap&& other) noexcept
{
	if (&other != this)
	{
		//we keep texture alive in openGl context.

		textureWidth = other.textureWidth;
		textureHeight = other.textureHeight;
		shadowFrameBuffer = other.shadowFrameBuffer;
		shadowRenderBuffer = other.shadowRenderBuffer;
		shadowTexture = other.shadowTexture;

		other.textureWidth = 0.f;
		other.textureHeight = 0.f;
		other.shadowFrameBuffer = 0;
		other.shadowRenderBuffer = 0;
		other.shadowTexture = 0;
	}

	return *this;
}

OmniShadowMap::~OmniShadowMap()
{
	glDeleteTextures(1, &shadowTexture);
	glDeleteRenderbuffers(1, &shadowRenderBuffer);
	glDeleteFramebuffers(1, &shadowFrameBuffer);
}



///////////////////////////////////////////////////////

LightManager::LightManager() : directionalShadowMapViewportSize(128), directionalShadowMapViewportNear(0.1f), directionalShadowMapViewportFar(100.f)
{

}

void LightManager::setShadowMapCount(LightType lightType, unsigned int count)
{
	if (lightType == LightType::SPOT)
	{
		spot_shadowMaps.resize(count);
	}
	else if (lightType == LightType::DIRECTIONAL)
	{
		int begin = std::max((int)(directional_shadowMaps.size() - 1), 0);
		for (int i = begin; i < count; i++)
		{
			directional_shadowMaps.push_back(ShadowMap(4096, 4096));
			//directional_shadowMaps.resize(count, ShadowMap(1024, 1024) ); // set texture size to 2048 for better precision with directional shadow maps.
		}
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

void LightManager::setLightingMaterials(std::shared_ptr<MaterialPointLight> pointLightMat, std::shared_ptr<MaterialDirectionalLight> directionalLightMat, std::shared_ptr<MaterialSpotLight> spotLightMat)
{
	m_pointLightMaterial = pointLightMat;
	m_directionalLightMaterial = directionalLightMat;
	m_spotLightMaterial = spotLightMat;
}

void LightManager::uniformPointLight(PointLight & light, const glm::mat4& view)
{
	m_pointLightMaterial->setUniformLightPosition( glm::vec3(view * glm::vec4(light.position, 1.0)) ); //Convert to eye space
	m_pointLightMaterial->setUniformLightColor(light.color);
	m_pointLightMaterial->setUniformLightIntensity(light.intensity);
}

void LightManager::uniformDirectionalLight(DirectionalLight & light, const glm::mat4& view)
{
	m_directionalLightMaterial->setUniformLightDirection(glm::vec3(view * glm::vec4(light.direction, 0.0))); //Convert to eye space
	m_directionalLightMaterial->setUniformLightColor(light.color);
	m_directionalLightMaterial->setUniformLightIntensity(light.intensity);
}

void LightManager::uniformSpotLight(SpotLight & light, const glm::mat4& view)
{
	m_spotLightMaterial->setUniformLightPosition(glm::vec3(view * glm::vec4(light.position, 1.0))); //Convert to eye space
	m_spotLightMaterial->setUniformLightDirection(glm::vec3(view * glm::vec4(light.direction, 0.0))); //Convert to eye space
	m_spotLightMaterial->setUniformLightAngle(light.angle);
	m_spotLightMaterial->setUniformLightColor(light.color);
	m_spotLightMaterial->setUniformLightIntensity(light.intensity);
}

float LightManager::getDirectionalShadowMapViewportSize() const
{
	return directionalShadowMapViewportSize;
}

float LightManager::getDirectionalShadowMapViewportNear() const
{
	return directionalShadowMapViewportNear;
}

float LightManager::getDirectionalShadowMapViewportFar() const
{
	return directionalShadowMapViewportFar;
}
