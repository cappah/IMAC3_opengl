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
	GLuint m_uniformTexture;
	GLuint m_uniformTextureBlur;

public:
	MaterialBloom();
	MaterialBloom(const ShaderProgram& shaderProgram);
	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override;
	void glUniform_exposure(float exposure);
	void glUniform_Texture(int textureId);
	void glUniform_TextureBlur(int textureId);
};