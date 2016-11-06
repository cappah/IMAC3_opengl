#pragma once

#include <memory>

#include "glew/glew.h"

#include "glm/glm.hpp"
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4, glm::ivec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/type_ptr.hpp" // glm::value_ptr

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "Utils.h"
#include "Texture.h"

#include "ISerializable.h"
#include "ResourcePointer.h"
#include "ShaderParameters.h"
#include "ShaderProgram.h"
#include "FileHandler.h"
#include "Resource.h"

//Statics :

static const unsigned int MAX_BONE_COUNT = 100;

//Helpers :
namespace MaterialHelper {

	GLuint findUniform(const std::string& uniformName, const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters);
	std::vector<GLuint> findUniforms(const std::string& uniformName, int count, const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters);

}

//Materials :
class Material : public Resource, public ISerializable
{
protected:
	GLuint m_glProgramId;
	std::vector<std::shared_ptr<InternalShaderParameterBase>> m_internalParameters;
	std::map<std::string, std::shared_ptr<ExternalShaderParameterBase>> m_externalParameters;

	std::string m_glProgramName; //a key to found the program this material is based on.

public:
	Material();
	Material(const std::string& glProgramName, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters);
	Material(const std::string& glProgramName, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters);
	Material(const ShaderProgram& shaderProgram);
	virtual ~Material();
	virtual void init(const FileHandler::CompletePath& path) override; //TODO
	void drawUI();
	void pushInternalsToGPU(int& boundTextureCount);
	virtual void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters);
	void use();
	template<typename T>
	T* getInternalData(const std::string& parameterName) const
	{
		auto& found = std::find_if(m_internalParameters.begin(), m_internalParameters.end(), [parameterName](const std::shared_ptr<InternalShaderParameterBase>& item) { return item->getName() == parameterName; });
		void* foundData = nullptr;
		if(found != m_internalParameters.end())
			(*found)->getData(foundData);
		return static_cast<T*>(foundData);
	}
	template<typename T>
	void setInternalData(const std::string& parameterName, const T* data)
	{
		auto& found = std::find_if(m_internalParameters.begin(), m_internalParameters.end(), [parameterName](const std::shared_ptr<InternalShaderParameterBase>& item) { return item->getName() == parameterName; });
		(*found)->setData(data);
	}

	//TODO 10
	//template<typename T, typename U>//= ShaderParameter::IsNotArray>
	//ExternalShaderParameter<T, U>& getExternalParameter(const std::string& parameterName) const
	//{
	//	auto& foundParameter = m_externalParameters.find(parameterName);
	//	if (foundParameter != m_externalParameters.end())
	//	{
	//		return *static_cast<ExternalShaderParameter<T, U>*>(foundParameter->second().get());
	//	}
	//}

	void loadFromShaderProgramDatas(GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters);
	// Save and load internal parameters 
	virtual void save(Json::Value & entityRoot) const override;
	virtual void load(const Json::Value & entityRoot) override;

	void save(const FileHandler::CompletePath& path) const;

};

//Default materials :
class Material3DObject : public Material
{
private:
	GLuint uniform_MVP;
	GLuint uniform_normalMatrix;
	std::vector<GLuint> uniform_bonesTransform;
	GLuint uniform_useSkeleton;

public:
	Material3DObject()
		: Material()
	{}

	Material3DObject(const ShaderProgram& shaderProgram)
		: Material(shaderProgram)
	{
		setExternalParameters(shaderProgram.getExternalParameters());
	}

	virtual ~Material3DObject()
	{}

	Material3DObject(const std::string& glProgramName, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: Material(glProgramName, glProgramId, internalParameters)
	{
		setExternalParameters(externalParameters);
	}

	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		uniform_MVP = MaterialHelper::findUniform("MVP", externalParameters);
		uniform_normalMatrix = MaterialHelper::findUniform("normalMatrix", externalParameters);
		uniform_bonesTransform = MaterialHelper::findUniforms("bonesTransform", MAX_BONE_COUNT, externalParameters);
		uniform_useSkeleton = MaterialHelper::findUniform("useSkeleton", externalParameters);
	}

	void setUniform_MVP(glm::mat4& mvp)
	{
		GlHelper::pushParameterToGPU(uniform_MVP, mvp);
	}
	void setUniform_normalMatrix(glm::mat4& normalMatrix)
	{
		GlHelper::pushParameterToGPU(uniform_normalMatrix, normalMatrix);
	}
	void setUniformBonesTransform(unsigned int idx, const glm::mat4& boneTransform)
	{
		GlHelper::pushParameterToGPU(uniform_bonesTransform[idx], boneTransform);
	}
	void setUniformUseSkeleton(bool useSkeleton)
	{
		GlHelper::pushParameterToGPU(uniform_useSkeleton, useSkeleton);
	}
};

class MaterialLit : public Material3DObject
{
private:

	//internals
	//GLuint uniform_textureDiffuse;
	//GLuint uniform_textureSpecular;
	//GLuint uniform_textureBump;
	//GLuint uniform_specularPower;
	//GLuint uniform_textureRepetition;

public:
	MaterialLit()
		: Material3DObject()
	{}

	MaterialLit(const ShaderProgram& shaderProgram)
		: Material3DObject(shaderProgram)
	{

	}

	MaterialLit(const std::string& glProgramName, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: Material3DObject(glProgramName, glProgramId, internalParameters, externalParameters)
	{

	}
};

class MaterialUnlit : public Material3DObject
{
	GLuint uniform_color;

public:
	MaterialUnlit()
		: Material3DObject()
	{}

	MaterialUnlit(const ShaderProgram& shaderProgram)
		: Material3DObject(shaderProgram)
	{
		setExternalParameters(shaderProgram.getExternalParameters());
	}

	MaterialUnlit(const std::string& glProgramName, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: Material3DObject(glProgramName, glProgramId, internalParameters, externalParameters)
	{
		setExternalParameters(externalParameters);
	}

	void setUniform_color(glm::vec3 color)
	{
		GlHelper::pushParameterToGPU<glm::vec3>(uniform_color, color);
	}

	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		uniform_color = MaterialHelper::findUniform("color", externalParameters);
	}
};

class MaterialInstancedUnlit : public Material
{
private:
	GLuint uniform_VP;
	GLuint uniform_color;

public:
	MaterialInstancedUnlit()
		: Material()
	{}

	MaterialInstancedUnlit(const ShaderProgram& shaderProgram)
		: Material(shaderProgram)
	{
		setExternalParameters(shaderProgram.getExternalParameters());
	}

	MaterialInstancedUnlit(const std::string& glProgramName, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: Material(glProgramName, glProgramId, internalParameters, externalParameters)
	{
		setExternalParameters(externalParameters);
	}

	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		uniform_VP = MaterialHelper::findUniform("VP", externalParameters);
		uniform_color = MaterialHelper::findUniform("color", externalParameters);
	}

	void setUniform_color(const glm::vec3& color) const
	{
		GlHelper::pushParameterToGPU<glm::vec3>(uniform_color, color);

	}
	void setUniform_VP(const glm::mat4& VP) const
	{
		GlHelper::pushParameterToGPU<glm::mat4>(uniform_VP, VP);
	}
};

struct MaterialDebugDrawer : public Material
{
private:
	GLuint uniform_MVP;

public:
	MaterialDebugDrawer()
		: Material()
	{}

	MaterialDebugDrawer(const ShaderProgram& shaderProgram)
		: Material(shaderProgram)
	{
		setExternalParameters(shaderProgram.getExternalParameters());
	}

	MaterialDebugDrawer(const std::string& glProgramName, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: Material(glProgramName, glProgramId, internalParameters, externalParameters)
	{
		setExternalParameters(externalParameters);
	}

	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		uniform_MVP = MaterialHelper::findUniform("MVP", externalParameters);
	}

	void setUniform_MVP(const glm::mat4& MVP)
	{
		GlHelper::pushParameterToGPU<glm::mat4>(uniform_MVP, MVP);
	}
};


class MaterialSkybox : public Material
{
private:
	GLuint uniform_VP;

public:
	MaterialSkybox()
		: Material()
	{}

	MaterialSkybox(const ShaderProgram& shaderProgram)
		: Material(shaderProgram)
	{
		setExternalParameters(shaderProgram.getExternalParameters());
	}

	MaterialSkybox(const std::string& glProgramName, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: Material(glProgramName, glProgramId, internalParameters, externalParameters)
	{
		setExternalParameters(externalParameters);
	}

	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		uniform_VP = MaterialHelper::findUniform("VP", externalParameters);
	}

	void setUniform_VP(const glm::mat4& VP)
	{
		GlHelper::pushParameterToGPU<glm::mat4>(uniform_VP, VP);
	}
};


class MaterialShadow : public Material
{

};


class MaterialBillboard : public Material
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
	MaterialBillboard()
		: Material()
	{}

	MaterialBillboard(const ShaderProgram& shaderProgram)
		: Material(shaderProgram)
	{
		setExternalParameters(shaderProgram.getExternalParameters());
	}

	MaterialBillboard(const std::string& glProgramName, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: Material(glProgramName, glProgramId, internalParameters, externalParameters)
	{
		setExternalParameters(externalParameters);
	}

	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		uniform_MVP = MaterialHelper::findUniform("MVP", externalParameters);
		uniform_Scale = MaterialHelper::findUniform("Scale", externalParameters);
		uniform_Translation = MaterialHelper::findUniform("Translation", externalParameters);
		uniform_Texture = MaterialHelper::findUniform("Texture", externalParameters);
		uniform_CameraRight = MaterialHelper::findUniform("CameraRight", externalParameters);
		uniform_CameraUp = MaterialHelper::findUniform("CameraUp", externalParameters);
		uniform_Color = MaterialHelper::findUniform("Color", externalParameters);
	}

	void setUniformMVP(const glm::mat4& VP)
	{
		GlHelper::pushParameterToGPU<glm::mat4>(uniform_MVP, VP);
	}
	void setUniformScale(const glm::vec2& scale)
	{
		GlHelper::pushParameterToGPU(uniform_MVP, scale);
	}
	void setUniformTranslation(const glm::vec3& translation)
	{
		GlHelper::pushParameterToGPU(uniform_MVP, translation);
	}
	void setUniformTexture(int texId)
	{
		GlHelper::pushParameterToGPU(uniform_MVP, texId);
	}
	void setUniformCameraRight(const glm::vec3& camRight)
	{
		GlHelper::pushParameterToGPU(uniform_MVP, camRight);
	}
	void setUniformCameraUp(const glm::vec3& camUp)
	{
		GlHelper::pushParameterToGPU(uniform_MVP, camUp);
	}
	void setUniformColor(const glm::vec4& color)
	{
		GlHelper::pushParameterToGPU(uniform_MVP, color);
	}
};


struct MaterialTerrain : public Material3DObject
{
private:
	//internals

	GLuint uniform_SpecularPower;
	GLuint uniform_TextureRepetition;
	GLuint uniform_LayoutOffset;
	GLuint uniform_FilterTexture;

	GLuint uniform_DiffuseTexture;
	GLuint uniform_BumpTexture;
	GLuint uniform_SpecularTexture;


public:
	MaterialTerrain()
		: Material3DObject()
	{}

	MaterialTerrain(const ShaderProgram& shaderProgram)
		: Material3DObject(shaderProgram)
	{
		setExternalParameters(shaderProgram.getExternalParameters());
	}

	MaterialTerrain(const std::string& glProgramName, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: Material3DObject(glProgramName, glProgramId, internalParameters, externalParameters)
	{
		setExternalParameters(externalParameters);
	}

	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		uniform_LayoutOffset = MaterialHelper::findUniform("LayoutOffset", externalParameters);
		uniform_FilterTexture = MaterialHelper::findUniform("FilterTexture", externalParameters);
		uniform_TextureRepetition = MaterialHelper::findUniform("TextureRepetition", externalParameters);
		uniform_SpecularPower = MaterialHelper::findUniform("SpecularPower", externalParameters);

		uniform_DiffuseTexture = MaterialHelper::findUniform("DiffuseTexture", externalParameters);
		uniform_BumpTexture = MaterialHelper::findUniform("BumpTexture", externalParameters);
		uniform_SpecularTexture = MaterialHelper::findUniform("SpecularTexture", externalParameters);
	}

	void setUniformLayoutOffset(const glm::vec2& layoutOffset)
	{
		GlHelper::pushParameterToGPU(uniform_LayoutOffset, layoutOffset);
	}
	void setUniformFilterTexture(int textureId)
	{
		GlHelper::pushParameterToGPU(uniform_FilterTexture, textureId);
	}
	void setUniformTextureRepetition(glm::vec2 repetition)
	{
		GlHelper::pushParameterToGPU(uniform_TextureRepetition, repetition);
	}
	void setUniformDiffuseTexture(int textureId)
	{
		GlHelper::pushParameterToGPU(uniform_DiffuseTexture, textureId);
	}
	void setUniformBumpTexture(int textureId)
	{
		GlHelper::pushParameterToGPU(uniform_BumpTexture, textureId);
	}
	void setUniformSpecularTexture(int textureId)
	{
		GlHelper::pushParameterToGPU(uniform_SpecularTexture, textureId);
	}
	void setUniformSpecularPower(float specularPower)
	{
		GlHelper::pushParameterToGPU(uniform_SpecularPower, specularPower);
	}

};


class MaterialTerrainEdition : public Material
{
private:

	GLuint uniform_textureDiffuse;
	GLuint uniform_textureSpecular;
	GLuint uniform_textureBump;

	GLuint uniform_textureRepetition;
	GLuint uniform_textureFilter;
	GLuint uniform_filterValues;

public:
	MaterialTerrainEdition()
		: Material()
	{}

	MaterialTerrainEdition(const ShaderProgram& shaderProgram)
		: Material(shaderProgram)
	{
		setExternalParameters(shaderProgram.getExternalParameters());
	}

	MaterialTerrainEdition(const std::string& glProgramName, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: Material(glProgramName, glProgramId, internalParameters, externalParameters)
	{
		setExternalParameters(externalParameters);
	}

	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		uniform_filterValues = MaterialHelper::findUniform("FilterValues", externalParameters);
		uniform_textureFilter = MaterialHelper::findUniform("FilterTexture", externalParameters);
		uniform_textureRepetition = MaterialHelper::findUniform("TextureRepetition", externalParameters);

		uniform_textureDiffuse = MaterialHelper::findUniform("Diffuse", externalParameters);
		uniform_textureSpecular = MaterialHelper::findUniform("Specular", externalParameters);
		uniform_textureBump = MaterialHelper::findUniform("Bump", externalParameters);
	}

	void setUniformLayoutOffset(const glm::vec2& layoutOffset)
	{
		GlHelper::pushParameterToGPU(uniform_filterValues, layoutOffset);
	}
	void setUniformFilterTexture(int textureId)
	{
		GlHelper::pushParameterToGPU(uniform_textureFilter, textureId);
	}
	void setUniformTextureRepetition(glm::vec2 repetition)
	{
		GlHelper::pushParameterToGPU(uniform_textureRepetition, repetition);
	}
	void setUniformDiffuseTexture(int textureId)
	{
		GlHelper::pushParameterToGPU(uniform_textureDiffuse, textureId);
	}
	void setUniformBumpTexture(int textureId)
	{
		GlHelper::pushParameterToGPU(uniform_textureBump, textureId);
	}
	void setUniformSpecularTexture(int textureId)
	{
		GlHelper::pushParameterToGPU(uniform_textureSpecular, textureId);
	}

};

class MaterialDrawOnTexture : public Material
{
private:
	GLuint uniform_drawPosition;
	GLuint uniform_colorToDraw;
	GLuint uniform_drawRadius;
	GLuint uniform_textureToDrawOn;

public:
	MaterialDrawOnTexture()
		: Material()
	{}

	MaterialDrawOnTexture(const ShaderProgram& shaderProgram)
		: Material(shaderProgram)
	{
		setExternalParameters(shaderProgram.getExternalParameters());
	}

	MaterialDrawOnTexture(const std::string& glProgramName, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: Material(glProgramName, glProgramId, internalParameters, externalParameters)
	{
		setExternalParameters(externalParameters);
	}

	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		uniform_drawPosition = MaterialHelper::findUniform("DrawPosition", externalParameters);
		uniform_colorToDraw = MaterialHelper::findUniform("DrawColor", externalParameters);
		uniform_drawRadius = MaterialHelper::findUniform("DrawRadius", externalParameters);
		uniform_textureToDrawOn = MaterialHelper::findUniform("Texture", externalParameters);
	}

	void setUniformDrawPosition(const glm::vec2& position)
	{
		GlHelper::pushParameterToGPU(uniform_drawPosition, position);
	}
	void setUniformColorToDraw(const glm::vec4& color)
	{
		GlHelper::pushParameterToGPU(uniform_colorToDraw, color);
	}
	void setUniformDrawRadius(float radius)
	{
		GlHelper::pushParameterToGPU(uniform_drawRadius, radius);
	}
	void setUniformTextureToDrawOn(int textureId)
	{
		GlHelper::pushParameterToGPU(uniform_textureToDrawOn, textureId);
	}
};

class MaterialGrassField : public Material
{
	GLuint uniform_time;
	GLuint uniform_Texture;
	GLuint uniform_VP;


public:
	MaterialGrassField()
		: Material()
	{}

	MaterialGrassField(const ShaderProgram& shaderProgram)
		: Material(shaderProgram)
	{
		setExternalParameters(shaderProgram.getExternalParameters());
	}

	MaterialGrassField(const std::string& glProgramName, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: Material(glProgramName, glProgramId, internalParameters, externalParameters)
	{
		setExternalParameters(externalParameters);
	}

	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		uniform_time = MaterialHelper::findUniform("Time", externalParameters);
		uniform_Texture = MaterialHelper::findUniform("Texture", externalParameters);
		uniform_VP = MaterialHelper::findUniform("VP", externalParameters);
	}

	void setUniformTime(float time)
	{
		GlHelper::pushParameterToGPU(uniform_time, time);
	}
	void setUniformTexture(int texId)
	{
		GlHelper::pushParameterToGPU(uniform_Texture, texId);
	}
	void setUniformVP(const glm::mat4& VP)
	{
		GlHelper::pushParameterToGPU(uniform_VP, VP);
	}
};

class MaterialParticlesCPU : public Material
{
private:
	GLuint m_uniformVP;
	GLuint m_uniformTexture;
	GLuint m_uniformCameraRight;
	GLuint m_uniformCameraUp;

public:
	MaterialParticlesCPU()
		: Material()
	{}

	MaterialParticlesCPU(const ShaderProgram& shaderProgram)
		: Material(shaderProgram)
	{
		setExternalParameters(shaderProgram.getExternalParameters());
	}

	MaterialParticlesCPU(const std::string& glProgramName, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: Material(glProgramName, glProgramId, internalParameters, externalParameters)
	{
		setExternalParameters(externalParameters);
	}

	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		m_uniformVP = MaterialHelper::findUniform("VP", externalParameters);
		m_uniformTexture = MaterialHelper::findUniform("Texture", externalParameters);
		m_uniformCameraRight = MaterialHelper::findUniform("CameraRight", externalParameters);
		m_uniformCameraUp = MaterialHelper::findUniform("CameraUp", externalParameters);
	}

	void glUniform_VP(const glm::mat4& VP)
	{
		GlHelper::pushParameterToGPU(m_uniformVP, VP);
	}
	void setUniformTexture(int texId)
	{
		GlHelper::pushParameterToGPU(m_uniformTexture, texId);
	}
	void setUniformCameraRight(const glm::vec3& camRight)
	{
		GlHelper::pushParameterToGPU(m_uniformCameraRight, camRight);
	}
	void setUniformCameraUp(const glm::vec3& camUp)
	{
		GlHelper::pushParameterToGPU(m_uniformCameraUp, camUp);
	}
};


class MaterialParticles : public Material
{
private:
	GLuint m_uniformVP;
	GLuint m_uniformTexture;
	GLuint m_uniformCameraRight;
	GLuint m_uniformCameraUp;

public:
	MaterialParticles()
		: Material()
	{}

	MaterialParticles(const ShaderProgram& shaderProgram)
		: Material(shaderProgram)
	{
		setExternalParameters(shaderProgram.getExternalParameters());
	}

	MaterialParticles(const std::string& glProgramName, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: Material(glProgramName, glProgramId, internalParameters, externalParameters)
	{
		setExternalParameters(externalParameters);
	}

	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		m_uniformVP = MaterialHelper::findUniform("VP", externalParameters);
		m_uniformTexture = MaterialHelper::findUniform("Texture", externalParameters);
		m_uniformCameraRight = MaterialHelper::findUniform("CameraRight", externalParameters);
		m_uniformCameraUp = MaterialHelper::findUniform("CameraUp", externalParameters);
	}

	void glUniform_VP(const glm::mat4& VP)
	{
		GlHelper::pushParameterToGPU(m_uniformVP, VP);
	}
	void setUniformTexture(int texId)
	{
		GlHelper::pushParameterToGPU(m_uniformTexture, texId);
	}
	void setUniformCameraRight(const glm::vec3& camRight)
	{
		GlHelper::pushParameterToGPU(m_uniformCameraRight, camRight);
	}
	void setUniformCameraUp(const glm::vec3& camUp)
	{
		GlHelper::pushParameterToGPU(m_uniformCameraUp, camUp);
	}
};

class MaterialParticleSimulation : public Material
{
private:
	GLuint m_uniformDeltaTime;

public:
	MaterialParticleSimulation()
		: Material()
	{}

	MaterialParticleSimulation(const ShaderProgram& shaderProgram)
		: Material(shaderProgram)
	{
		setExternalParameters(shaderProgram.getExternalParameters());
	}

	MaterialParticleSimulation(const std::string& glProgramName, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: Material(glProgramName, glProgramId, internalParameters, externalParameters)
	{
		setExternalParameters(externalParameters);
	}

	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		m_uniformDeltaTime = MaterialHelper::findUniform("DeltaTime", externalParameters);
	}

	void glUniform_deltaTime(float deltaTime)
	{
		GlHelper::pushParameterToGPU(m_uniformDeltaTime, deltaTime);
	}
};


//////////////////


//struct Material : public Resource
//{
//	std::string name;
//
//	GLuint glProgram;
//
//	Material(GLuint _glProgram = 0);
//	Material(ResourcePtr<ShaderProgram> _glProgram);
//	virtual void init(const FileHandler::CompletePath& path) override;
//	virtual Material* copy() const = 0;
//	virtual ~Material();
//	virtual void use() = 0;
//	virtual void drawUI() = 0;
//	virtual void initGL() = 0;
//
//	void save(const FileHandler::CompletePath& path) const
//	{
//		assert(false, "not implemented");
//	}
//	void load(const FileHandler::CompletePath& path)
//	{
//		assert(false, "not implemented");
//	}
//};

//struct Material3DObject : public Material
//{
//	GLuint uniform_MVP;
//	GLuint uniform_normalMatrix;
//	GLuint uniform_bonesTransform[MAX_BONE_COUNT];
//	GLuint uniform_useSkeleton;
//
//	Material3DObject(GLuint _glProgram = 0);
//	Material3DObject(ResourcePtr<ShaderProgram> _glProgram);
//
//	void setUniform_MVP(glm::mat4& mvp);
//	void setUniform_normalMatrix(glm::mat4& normalMatrix);
//	void setUniformBonesTransform(unsigned int idx, const glm::mat4& boneTransform);
//	void setUniformUseSkeleton(bool useSkeleton);
//};
//
//
//class MaterialLit : public Material3DObject, public ISerializable
//{
//private:
//	std::string diffuseTextureName;
//	ResourcePtr<Texture> textureDiffuse;
//
//	float specularPower;
//	std::string specularTextureName;
//	ResourcePtr<Texture> textureSpecular;
//
//	std::string bumpTextureName;
//	ResourcePtr<Texture> textureBump;
//
//	glm::vec2 textureRepetition;
//
//	//GLuint uniform_MVP;
//	//GLuint uniform_normalMatrix;
//
//	GLuint uniform_textureDiffuse;
//	GLuint uniform_textureSpecular;
//	GLuint uniform_textureBump;
//	GLuint uniform_specularPower;
//	GLuint uniform_textureRepetition;
//
//public:
//	MaterialLit();
//	MaterialLit(GLuint _glProgram, ResourcePtr<Texture> _textureDiffuse = ResourcePtr<Texture>(), ResourcePtr<Texture> _textureSpecular = ResourcePtr<Texture>(), ResourcePtr<Texture> _textureBump = ResourcePtr<Texture>(), float _specularPower = 50);
//	MaterialLit(ResourcePtr<ShaderProgram> _glProgram, ResourcePtr<Texture> _textureDiffuse = ResourcePtr<Texture>(), ResourcePtr<Texture> _textureSpecular = ResourcePtr<Texture>(), ResourcePtr<Texture> _textureBump = ResourcePtr<Texture>(), float _specularPower = 50);
//	void init(const FileHandler::CompletePath& path) override;
//	virtual Material* copy() const override
//	{
//		return new MaterialLit(*this);
//	}
//
//	void setDiffuse(ResourcePtr<Texture> _textureDiffuse);
//	void setSpecular(ResourcePtr<Texture> _textureSpecular);
//	void setBump(ResourcePtr<Texture> _textureBump);
//
//	ResourcePtr<Texture> getDiffuse() const;
//	ResourcePtr<Texture> getSpecular() const;
//	ResourcePtr<Texture> getBump() const;
//
//	float getSpecularPower() const;
//
//	//void setUniform_MVP(glm::mat4& mvp);
//	//void setUniform_normalMatrix(glm::mat4& normalMatrix);
//	//void setUniformBonesTransform(const std::vector<glm::mat4>& bonesTransform);
//
//	virtual void use() override;
//	virtual void drawUI() override;
//	virtual void initGL() override;
//
//	// Hérité via ISerializable
//	virtual void save(Json::Value & entityRoot) const override;
//	virtual void load(const Json::Value & entityRoot) override;
//};
//
//
//struct MaterialUnlit : public Material3DObject
//{
//	//GLuint uniform_MVP;
//	//GLuint uniform_normalMatrix;
//
//	GLuint uniform_color;
//
//	MaterialUnlit(GLuint _glProgram = 0);
//	MaterialUnlit(ResourcePtr<ShaderProgram> _glProgram)
//		: MaterialUnlit(_glProgram->id)
//	{
//
//	}
//
//	virtual Material* copy() const override
//	{
//		return new MaterialUnlit(*this);
//	}
//
//	void setUniform_color(glm::vec3 color);
//
//	//void setUniform_MVP(glm::mat4& mvp);
//	//void setUniform_normalMatrix(glm::mat4& normalMatrix);
//
//	virtual void use() override;
//
//	virtual void drawUI() override;
//
//	virtual void initGL() override;
//};

//
//struct MaterialInstancedUnlit : public Material
//{
//	GLuint uniform_color;
//	GLuint uniform_VP;
//
//	MaterialInstancedUnlit(GLuint _glProgram = 0);
//	virtual Material* copy() const override
//	{
//		return new MaterialInstancedUnlit(*this);
//	}
//
//	void setUniform_color(glm::vec3 color);
//	void setUniform_VP(const glm::mat4& VP);
//
//	virtual void use() override;
//
//	virtual void drawUI() override;
//
//	virtual void initGL() override;
//};

//struct MaterialDebugDrawer : public Material
//{
//	GLuint uniform_MVP;
//
//	MaterialDebugDrawer(GLuint _glProgram = 0);
//	virtual Material* copy() const override
//	{
//		return new MaterialDebugDrawer(*this);
//	}
//
//	void setUniform_MVP(const glm::mat4& MVP);
//
//	virtual void use() override;
//	virtual void drawUI() override;
//	virtual void initGL() override;
//};

//
//class MaterialSkybox : public Material
//{
//private:
//
//	std::string diffuseTextureName;
//	ResourcePtr<CubeTexture> textureDiffuse;
//
//	GLuint uniform_textureDiffuse;
//	GLuint uniform_VP;
//
//public:
//	MaterialSkybox();
//	MaterialSkybox(GLuint _glProgram, ResourcePtr<CubeTexture> _textureDiffuse);
//	MaterialSkybox(ResourcePtr<ShaderProgram> _glProgram, ResourcePtr<CubeTexture> _textureDiffuse)
//		: MaterialSkybox(_glProgram->id, _textureDiffuse)
//	{}
//	virtual Material* copy() const override
//	{
//		return new MaterialSkybox(*this);
//	}
//	
//	void setUniform_VP(const glm::mat4& vp);
//
//	virtual void use() override;
//	virtual void drawUI() override;
//	virtual void initGL() override;
//
//	void setDiffuseTexture(ResourcePtr<CubeTexture> texture);
//	ResourcePtr<CubeTexture> getDiffuseTexture() const;
//};
//
//class MaterialShadow : public Material
//{
//public:
//	MaterialShadow();
//	MaterialShadow(GLuint _glProgram);
//	MaterialShadow(ResourcePtr<ShaderProgram> _glProgram)
//		: MaterialShadow(_glProgram->id)
//	{}
//	virtual Material* copy() const override
//	{
//		return new MaterialShadow(*this);
//	}
//
//	virtual void use() override;
//	virtual void drawUI() override;
//	virtual void initGL() override;
//};
//
//class MaterialBillboard : public Material
//{
//private:
//	GLuint m_uniformMVP;
//	GLuint m_uniformScale;
//	GLuint m_uniformTranslation;
//	GLuint m_uniformTexture;
//	GLuint m_uniformCameraRight;
//	GLuint m_uniformCameraUp;
//	GLuint m_uniformColor;
//
//public:
//	MaterialBillboard();
//	MaterialBillboard(GLuint _glProgram);
//	MaterialBillboard(ResourcePtr<ShaderProgram> _glProgram)
//		: MaterialBillboard(_glProgram->id)
//	{}
//	virtual Material* copy() const override
//	{
//		return new MaterialBillboard(*this);
//	}
//
//	virtual void use() override;
//	virtual void drawUI() override;
//	virtual void initGL() override;
//
//	void setUniformMVP(const glm::mat4& mvp);
//	void setUniformScale(const glm::vec2& scale);
//	void setUniformTranslation(const glm::vec3& translation);
//	void setUniformTexture(int texId);
//	void setUniformCameraRight(const glm::vec3& camRight);
//	void setUniformCameraUp(const glm::vec3& camUp);
//	void setUniformColor(const glm::vec4& color);
//};
//
//
//struct MaterialTerrain : public Material3DObject
//{
//
//	std::string diffuseTextureName;
//	ResourcePtr<Texture> textureDiffuse;
//
//	
//	float specularPower;
//	std::string specularTextureName;
//	ResourcePtr<Texture> textureSpecular;
//
//	std::string bumpTextureName;
//	ResourcePtr<Texture> textureBump;
//	
//
//	glm::vec2 textureRepetition;
//
//	GLuint uniform_textureDiffuse;
//	GLuint uniform_textureSpecular;
//	GLuint uniform_textureBump;
//	GLuint uniform_specularPower;
//
//	GLuint uniform_textureRepetition;
//	GLuint uniform_textureFilter;
//	GLuint uniform_filterValues;
//
//	MaterialTerrain();
//	MaterialTerrain(GLuint _glProgram);
//	MaterialTerrain(ResourcePtr<ShaderProgram> _glProgram)
//		: MaterialTerrain(_glProgram->id)
//	{}
//	virtual Material* copy() const override
//	{
//		return new MaterialTerrain(*this);
//	}
//
//	virtual void use() override;
//	virtual void drawUI() override;
//	virtual void initGL() override;
//
//	void setUniformLayoutOffset(const glm::vec2& layoutOffset);
//	void setUniformFilterTexture(int textureId);
//	void setUniformTextureRepetition(const glm::vec2& textureRepetition);
//
//	void setUniformDiffuseTexture(int textureId);
//	void setUniformBumpTexture(int textureId);
//	void setUniformSpecularTexture(int textureId);
//
//	void setUniformSpecularPower(float specularPower);
//};
//
//
//class MaterialTerrainEdition : public Material
//{
//private:
//	//std::string diffuseTextureName;
//	ResourcePtr<Texture> textureDiffuse;
//
//	//float specularPower;
//	//std::string specularTextureName;
//	ResourcePtr<Texture> textureSpecular;
//
//	//std::string bumpTextureName;
//	ResourcePtr<Texture> textureBump;
//
//	glm::vec2 textureRepetition;
//
//	GLuint uniform_textureDiffuse;
//	GLuint uniform_textureSpecular;
//	GLuint uniform_textureBump;
//	//GLuint uniform_specularPower; 
//	GLuint uniform_textureRepetition;
//	GLuint uniform_textureFilter;
//	GLuint uniform_filterValues;
//
//public:
//	MaterialTerrainEdition();
//	MaterialTerrainEdition(GLuint _glProgram);
//	MaterialTerrainEdition(ResourcePtr<ShaderProgram> _glProgram)
//		: MaterialTerrainEdition(_glProgram->id)
//	{}
//	virtual Material* copy() const override
//	{
//		return new MaterialTerrainEdition(*this);
//	}
//
//	void setUniformFilterTexture(int textureId);
//	void setUniformDiffuseTexture(int textureId);
//	void setUniformBumpTexture(int textureId);
//	void setUniformSpecularTexture(int textureId);
//	void setUniformLayoutOffset(const glm::vec2& layoutOffset);
//	void setUniformTextureRepetition(const glm::vec2& textureRepetition);
//
//	virtual void use() override;
//	virtual void drawUI() override;
//	virtual void initGL() override;
//};
//
//class MaterialDrawOnTexture : public Material
//{
//private:
//	GLuint uniform_colorToDraw;
//	GLuint uniform_drawPosition;
//	GLuint uniform_drawRadius;
//	GLuint uniform_textureToDrawOn;
//
//public : 
//	MaterialDrawOnTexture(GLuint _glProgram = 0);
//	MaterialDrawOnTexture(ResourcePtr<ShaderProgram> _glProgram)
//		: MaterialDrawOnTexture(_glProgram->id)
//	{}
//	virtual Material* copy() const override
//	{
//		return new MaterialDrawOnTexture(*this);
//	}
//
//	void setUniformDrawPosition(const glm::vec2& position);
//	void setUniformColorToDraw(const glm::vec4& color);
//	void setUniformDrawRadius(float radius);
//	void setUniformTextureToDrawOn(int textureId);
//
//	virtual void use() override;
//	virtual void drawUI() override;
//	virtual void initGL() override;
//};
//
//class MaterialGrassField : public Material
//{
//private:
//	GLuint uniform_time;
//	GLuint uniform_Texture;
//	GLuint uniform_VP;
//
//public:
//	MaterialGrassField();
//	MaterialGrassField(GLuint _glProgram);
//	MaterialGrassField(ResourcePtr<ShaderProgram> _glProgram)
//		: MaterialGrassField(_glProgram->id)
//	{}
//	virtual Material* copy() const override
//	{
//		return new MaterialGrassField(*this);
//	}
//
//	void setUniformTime(float time);
//	void setUniformTexture(int texId);
//	void setUniformVP(const glm::mat4& VP);
//
//	virtual void use() override;
//	virtual void drawUI() override;
//	virtual void initGL() override;
//};
//
//class MaterialParticlesCPU : public Material
//{
//private:
//	GLuint m_uniformVP;
//	GLuint m_uniformTexture;
//	GLuint m_uniformCameraRight;
//	GLuint m_uniformCameraUp;
//
//public:
//	MaterialParticlesCPU();
//	MaterialParticlesCPU(GLuint _glProgram);
//	MaterialParticlesCPU(ResourcePtr<ShaderProgram> _glProgram)
//		: MaterialParticlesCPU(_glProgram->id)
//	{}
//	virtual Material* copy() const override
//	{
//		return new MaterialParticlesCPU(*this);
//	}
//
//	void glUniform_VP(const glm::mat4& VP);
//	void setUniformTexture(int texId);
//	void setUniformCameraRight(const glm::vec3& camRight);
//	void setUniformCameraUp(const glm::vec3& camUp);
//
//	virtual void use() override;
//	virtual void drawUI() override;
//	virtual void initGL() override;
//};
//
//
//class MaterialParticles : public Material
//{
//private:
//	GLuint m_uniformVP;
//	GLuint m_uniformTexture;
//	GLuint m_uniformCameraRight;
//	GLuint m_uniformCameraUp;
//
//public :
//	MaterialParticles();
//	MaterialParticles(GLuint _glProgram);
//	MaterialParticles(ResourcePtr<ShaderProgram> _glProgram)
//		: MaterialParticles(_glProgram->id)
//	{}
//	virtual Material* copy() const override
//	{
//		return new MaterialParticles(*this);
//	}
//
//	void glUniform_VP(const glm::mat4& VP);
//	void setUniformTexture(int texId);
//	void setUniformCameraRight(const glm::vec3& camRight);
//	void setUniformCameraUp(const glm::vec3& camUp);
//
//	virtual void use() override;
//	virtual void drawUI() override;
//	virtual void initGL() override;
//};
//
//class MaterialParticleSimulation : public Material
//{
//private :
//	GLuint m_uniformDeltaTime;
//
//public:
//	MaterialParticleSimulation();
//	MaterialParticleSimulation(GLuint _glProgram);
//	MaterialParticleSimulation(ResourcePtr<ShaderProgram> _glProgram)
//		: MaterialParticleSimulation(_glProgram->id)
//	{}
//	virtual Material* copy() const override
//	{
//		return new MaterialParticleSimulation(*this);
//	}
//
//	void glUniform_deltaTime(float deltaTime);
//
//	virtual void use() override;
//	virtual void drawUI() override;
//	virtual void initGL() override;
//};

/*
//helpers : 


class MaterialModelsFactory : public ISingleton<MaterialModelsFactory>
{
private:
	std::map<std::string, const Material*> m_materials;

public:
	MaterialModelsFactory()
	{}

	template<typename T>
	bool add(const std::string& name)
	{
		m_materials[name] = new T;
		return true;
	}

	const Material* getPtr(const std::string& name)
	{
		return m_materials[name];
	}

	Material* getInstance(const std::string& name)
	{
		return m_materials[name]->copy();
	}

	std::map<std::string, const Material*>::iterator begin()
	{
		return m_materials.begin();
	}

	std::map<std::string, const Material*>::iterator end()
	{
		return m_materials.end();
	}


	SINGLETON_IMPL(MaterialModelsFactory)
};

#define REGISTER_MATERIAL(name, type) static bool isRegister = MaterialModelsFactory::instance().add<type>(name);

namespace ResourceHandling
{
	enum BaseMaterialType
	{
		Lit,
		Unlit,
		InstancedUnlit,
		DebugDrawer,
		Skybox,
		Shadow,
		Billboard,
		Terrain,
		TerrainEdition,
		DrawOnTexture,
		GrassField,
		ParticlesCPU,
		Particles,
		ParticleSimulation,
	};
}

class BaseMaterialHelper : public ISingleton<BaseMaterialHelper>
{
private:

public:
	BaseMaterialHelper()
	{}

	SINGLETON_IMPL(BaseMaterialHelper)

		//Used in material loading to retrieve polymorphisme
	Material* instantiateMaterialFromType(int materialType)
	{
		switch (materialType)
		{
		case ResourceHandling::BaseMaterialType::Lit:
			return new MaterialLit();
			break;
		case ResourceHandling::BaseMaterialType::Unlit:
			return new MaterialUnlit();
			break;
		case ResourceHandling::BaseMaterialType::InstancedUnlit:
			return new MaterialInstancedUnlit();
			break;
		case ResourceHandling::BaseMaterialType::DebugDrawer:
			return new MaterialDebugDrawer();
			break;
		case ResourceHandling::BaseMaterialType::Skybox:
			return new MaterialSkybox();
			break;
		case ResourceHandling::BaseMaterialType::Shadow:
			return new MaterialShadow();
			break;
		case ResourceHandling::BaseMaterialType::Billboard:
			return new MaterialBillboard();
			break;
		case ResourceHandling::BaseMaterialType::Terrain:
			return new MaterialTerrain();
			break;
		case ResourceHandling::BaseMaterialType::TerrainEdition:
			return new MaterialTerrainEdition();
			break;
		case ResourceHandling::BaseMaterialType::DrawOnTexture:
			return new MaterialDrawOnTexture();
			break;
		case ResourceHandling::BaseMaterialType::GrassField:
			return new MaterialGrassField();
			break;
		case ResourceHandling::BaseMaterialType::ParticlesCPU:
			return new MaterialParticlesCPU();
			break;
		case ResourceHandling::BaseMaterialType::Particles:
			return new MaterialParticles();
			break;
		case ResourceHandling::BaseMaterialType::ParticleSimulation:
			return new MaterialParticleSimulation();
			break;
		default:
			return nullptr;
			break;

		}
	}

	Material* loadMaterialFromPath(const FileHandler::CompletePath& path)
	{
		//pseudo code :
		std::ifstream stream;

		stream.open(path.toString());
		if (stream.is_open())
		{
			Json::Value root;
			stream >> root;

			int materialType = root.get("type", -1).asInt();

			Material* material = instantiateMaterialFromType(materialType);
			return material;
		}
		else
		{
			std::cout << "Can't load Material at path : " << path.toString() << std::endl;
			return nullptr;
		}
	}

	void getMaterialList()
	{

	}

};
*/