#pragma once

#include "Materials.h"

class MaterialBlur : public Material
{
private:
	GLuint m_uniformPassId;
	GLuint m_uniformTexture;

public:
	MaterialBlur();
	MaterialBlur(const ShaderProgram& shaderProgram);
	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override;
	void glUniform_passId(int passId);
	void glUniform_Texture(int textureId);
};

class MaterialBloom : public Material
{
private:
	GLuint m_uniformExposure;
	GLuint m_uniformGamma;
	GLuint m_uniformTexture;
	GLuint m_uniformTextureBlur;

public:
	MaterialBloom();
	MaterialBloom(const ShaderProgram& shaderProgram);
	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override;
	void glUniform_Texture(int textureId);
	void glUniform_TextureBlur(int textureId);
	void glUniform_Exposure(float exposure);
	void glUniform_Gamma(float gamma);
};

class MaterialAdd : public Material
{
private:
	GLuint m_uniformTexture01;
	GLuint m_uniformTexture02;

public:
	MaterialAdd();
	MaterialAdd(const ShaderProgram& shaderProgram);
	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override;
	void glUniform_Texture01(int textureId);
	void glUniform_Texture02(int textureId);
};

class MaterialFlares : public Material
{
private:
	GLuint m_uniformVP;
	GLuint m_uniformDepth;

public:
	MaterialFlares();
	MaterialFlares(const ShaderProgram& shaderProgram);
	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override;
	void glUniform_VP(const glm::mat4& VP) const;
	void glUniform_Depth(int textureId) const;

};

class MaterialSSAO : public Material
{
private:

public:
	MaterialSSAO();
	MaterialSSAO(const ShaderProgram& shaderProgram);
	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override;
};