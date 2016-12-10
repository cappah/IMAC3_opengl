#include "PostProcessMaterials.h"

MaterialBlur::MaterialBlur()
	: Material()
{}

MaterialBlur::MaterialBlur(const ShaderProgram& shaderProgram)
	: Material(shaderProgram)
{
	setExternalParameters(shaderProgram.getExternalParameters());
}

void MaterialBlur::setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
{
	m_uniformPassId = MaterialHelper::getUniform(m_glProgramId, "PassId");
	m_uniformTexture = MaterialHelper::getUniform(m_glProgramId, "Texture");

	if (!checkError("Uniforms"))
		PRINT_ERROR("error in texture initialization.")
}

void MaterialBlur::glUniform_passId(int passId)
{
	GlHelper::pushParameterToGPU(m_uniformPassId, passId);
}

void MaterialBlur::glUniform_Texture(int textureId)
{
	GlHelper::pushParameterToGPU(m_uniformTexture, textureId);
}

///////////////////////////////

MaterialBloom::MaterialBloom()
	: Material()
{

}

MaterialBloom::MaterialBloom(const ShaderProgram& shaderProgram)
	: Material(shaderProgram)
{
	setExternalParameters(shaderProgram.getExternalParameters());
}

void MaterialBloom::setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
{
	m_uniformExposure = MaterialHelper::getUniform(m_glProgramId, "Exposure");
	m_uniformTexture = MaterialHelper::getUniform(m_glProgramId, "Texture");
	m_uniformTextureBlur = MaterialHelper::getUniform(m_glProgramId, "TextureBlur");

	if (!checkError("Uniforms"))
		PRINT_ERROR("error in texture initialization.")
}

void MaterialBloom::glUniform_exposure(float exposure)
{
	GlHelper::pushParameterToGPU(m_uniformExposure, exposure);
}

void MaterialBloom::glUniform_Texture(int textureId)
{
	GlHelper::pushParameterToGPU(m_uniformTexture, textureId);
}

void MaterialBloom::glUniform_TextureBlur(int textureId)
{
	GlHelper::pushParameterToGPU(m_uniformTextureBlur, textureId);
}