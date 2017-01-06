#pragma once

#include <vector>
#include "glew/glew.h"
#include "glm/common.hpp"
#include "IDrawable.h"

class RenderDatas;

class MaterialAggregation
{
public:
	virtual void initParameters(GLuint programID) = 0;
	virtual void pushParametersToGPU(const RenderDatas& renderDatas) const = 0;
};

class PerInstanceMaterialAggregation
{
public:
	virtual void initParameters(GLuint programID) = 0;
	virtual void pushParametersToGPU(const IDrawable& drawable, const RenderDatas& renderDatas, int& boundTextureCount) const = 0;
};

class MaterialAggregationWithSkeleton
{
private:
	std::vector<GLuint> uniform_bonesTransform;
	GLuint uniform_useSkeleton;

public:
	virtual void initParameters(GLuint programID);
	virtual void pushParametersToGPU(const IDrawable& drawable, const RenderDatas& renderDatas, int& boundTextureCount) const;

private:
	void setUniformBonesTransform(unsigned int idx, const glm::mat4& boneTransform) const;
	void setUniformUseSkeleton(bool useSkeleton) const;
};


class MaterialAggregationMesh
{
private:
	GLuint uniform_ModelMatrix;
	GLuint uniform_ViewMatrix;
	GLuint uniform_ProjectionMatrix;

public:
	virtual void initParameters(GLuint programID);
	virtual void pushParametersToGPU(const IDrawable& drawable, const RenderDatas& renderDatas, int& boundTextureCount) const;

private:
	void setUniformModelMatrix(const glm::mat4& modelMatrix) const;
	void setUniformViewMatrix(const glm::mat4& viewMatrix) const;
	void setUniformProjectionMatrix(const glm::mat4& projectionMatrix) const;
};


class MaterialAggregationBillboard
{
private:
	GLuint uniform_MVP;
	GLuint uniform_Scale;
	GLuint uniform_Translation;
	GLuint uniform_Texture;
	GLuint uniform_CameraRight;
	GLuint uniform_CameraUp;
	GLuint uniform_Color;

public:
	virtual void initParameters(GLuint programID);
	virtual void pushParametersToGPU(const IDrawable& drawable, const RenderDatas& renderDatas, int& boundTextureCount) const;

private:
	void setUniformMVP(const glm::mat4& VP) const;
	void setUniformScale(const glm::vec2& scale) const;
	void setUniformTranslation(const glm::vec3& translation) const;
	void setUniformTexture(int texId) const;
	void setUniformCameraRight(const glm::vec3& camRight) const;
	void setUniformCameraUp(const glm::vec3& camUp) const;
	void setUniformColor(const glm::vec4& color) const;
};


class MaterialAggregationParticles
{
private:
	GLuint m_uniformVP;
	GLuint m_uniformTexture;
	GLuint m_uniformCameraRight;
	GLuint m_uniformCameraUp;

public:
	virtual void initParameters(GLuint programID);
	virtual void pushParametersToGPU(const IDrawable& drawable, const RenderDatas& renderDatas, int& boundTextureCount) const;

private:
	void glUniform_VP(const glm::mat4& VP) const;
	void setUniformTexture(int texId) const;
	void setUniformCameraRight(const glm::vec3& camRight) const;
	void setUniformCameraUp(const glm::vec3& camUp) const;
};

