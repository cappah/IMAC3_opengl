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
//#include "ResourcePointer.h"
#include "ShaderParameters.h"
#include "ShaderProgram.h"
#include "FileHandler.h"
#include "Resource.h"

//Statics :

static const unsigned int MAX_BONE_COUNT = 100;

//Helpers :
namespace MaterialHelper {

	GLuint getUniform(GLuint programId, const std::string& uniformName);
	std::vector<GLuint> getUniforms(GLuint programId, const std::string& uniformName, int count);
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
	Material(const FileHandler::CompletePath& glProgramPath, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters);
	Material(const FileHandler::CompletePath& glProgramPath, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters);
	Material(const ShaderProgram& shaderProgram);
	virtual ~Material();
	//init internal params. Should be called in constructor, or just after construction.
	void initInternalParameters();
	virtual void init(const FileHandler::CompletePath& path) override;
	void init(const ShaderProgram& shaderProgram);
	void drawUI();
	void pushInternalsToGPU(int& boundTextureCount);
	virtual void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters);
	void use();
	GLuint getGLId() const;
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
	//template<typename T, typename U = ShaderParameter::IsNotArray>
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

	virtual void drawInInspector(Scene & scene) override;
};

//engine materials : 

//Lightning materials : 

class MaterialLight : public Material
{
private:
	GLuint uniform_ColorTexture;
	GLuint uniform_NormalTexture;
	GLuint uniform_DepthTexture;
	GLuint uniform_ScreenToWorld;
	GLuint uniform_CameraPosition;
	GLuint uniform_ShadowTexture;

public:
	MaterialLight()
		: Material()
	{}

	MaterialLight(const ShaderProgram& shaderProgram)
		: Material(shaderProgram)
	{
		setExternalParameters(shaderProgram.getExternalParameters());
	}

	virtual ~MaterialLight()
	{}

	MaterialLight(const FileHandler::CompletePath& glProgramPath, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: Material(glProgramPath, glProgramId, internalParameters)
	{
		setExternalParameters(externalParameters);
	}

	virtual void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		uniform_ColorTexture = MaterialHelper::getUniform(m_glProgramId, "ColorBuffer");
		uniform_NormalTexture = MaterialHelper::getUniform(m_glProgramId, "NormalBuffer");
		uniform_DepthTexture = MaterialHelper::getUniform(m_glProgramId, "DepthBuffer");
		uniform_ScreenToWorld = MaterialHelper::getUniform(m_glProgramId, "ScreenToWorld");
		uniform_CameraPosition = MaterialHelper::getUniform(m_glProgramId, "CameraPosition");
		uniform_ShadowTexture = MaterialHelper::getUniform(m_glProgramId, "Shadow");

		if (!checkError("Uniforms"))
			PRINT_ERROR("error in texture initialization.")
	}

	void setUniformColorTexture(int texUnitId)
	{
		GlHelper::pushParameterToGPU(uniform_ColorTexture, texUnitId);
	}
	void setUniformNormalTexture(int texUnitId)
	{
		GlHelper::pushParameterToGPU(uniform_NormalTexture, texUnitId);
	}
	void setUniformDepthTexture(int texUnitId)
	{
		GlHelper::pushParameterToGPU(uniform_DepthTexture, texUnitId);
	}
	void setUniformScreenToWorld(const glm::mat4& screenToWorldMat)
	{
		GlHelper::pushParameterToGPU(uniform_ScreenToWorld, screenToWorldMat);
	}
	void setUniformCameraPosition(const glm::vec3& cameraPosition)
	{
		GlHelper::pushParameterToGPU(uniform_CameraPosition, cameraPosition);
	}
	void setUniformShadowTexture(int texUnitId)
	{
		GlHelper::pushParameterToGPU(uniform_ShadowTexture, texUnitId);
	}
};

class MaterialPointLight final : public MaterialLight
{
private:
	GLuint uniform_FarPlane;
	GLuint uniform_lightPosition;
	GLuint uniform_lightColor;
	GLuint uniform_lightIntensity;

public:
	MaterialPointLight()
		: MaterialLight()
	{}

	MaterialPointLight(const ShaderProgram& shaderProgram)
		: MaterialLight(shaderProgram)
	{
		setExternalParameters(shaderProgram.getExternalParameters());
	}

	virtual ~MaterialPointLight()
	{}

	MaterialPointLight(const FileHandler::CompletePath& glProgramPath, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: MaterialLight(glProgramPath, glProgramId, internalParameters, externalParameters)
	{
		setExternalParameters(externalParameters);
	}

	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		uniform_FarPlane = MaterialHelper::getUniform(m_glProgramId, "FarPlane");
		uniform_lightPosition = MaterialHelper::getUniform(m_glProgramId, "pointLight.position");
		uniform_lightColor = MaterialHelper::getUniform(m_glProgramId, "pointLight.color");
		uniform_lightIntensity = MaterialHelper::getUniform(m_glProgramId, "pointLight.intensity");

		if (!checkError("Uniforms"))
			PRINT_ERROR("error in texture initialization.")
	}

	void setUniformFarPlane(float farPlane)
	{
		GlHelper::pushParameterToGPU(uniform_FarPlane, farPlane);
	}

	void setUniformLightPosition(const glm::vec3& lightPos)
	{
		GlHelper::pushParameterToGPU(uniform_lightPosition, lightPos);
	}
	void setUniformLightColor(const glm::vec3& lightCol)
	{
		GlHelper::pushParameterToGPU(uniform_lightColor, lightCol);
	}
	void setUniformLightIntensity(float lightIntensity)
	{
		GlHelper::pushParameterToGPU(uniform_lightIntensity, lightIntensity);
	}
};

class MaterialDirectionalLight final : public MaterialLight
{
private:

	GLuint uniform_WorldToLight;
	GLuint uniform_lightDirection;
	GLuint uniform_lightColor;
	GLuint uniform_lightIntensity;

public:
	MaterialDirectionalLight()
		: MaterialLight()
	{}

	MaterialDirectionalLight(const ShaderProgram& shaderProgram)
		: MaterialLight(shaderProgram)
	{
		setExternalParameters(shaderProgram.getExternalParameters());
	}

	virtual ~MaterialDirectionalLight()
	{}

	MaterialDirectionalLight(const FileHandler::CompletePath& glProgramPath, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: MaterialLight(glProgramPath, glProgramId, internalParameters, externalParameters)
	{
		setExternalParameters(externalParameters);
	}

	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		uniform_WorldToLight = MaterialHelper::getUniform(m_glProgramId, "WorldToLightScreen");
		uniform_lightDirection = MaterialHelper::getUniform(m_glProgramId, "directionalLight.direction");
		uniform_lightColor = MaterialHelper::getUniform(m_glProgramId, "directionalLight.color");
		uniform_lightIntensity = MaterialHelper::getUniform(m_glProgramId, "directionalLight.intensity");

		if (!checkError("Uniforms"))
			PRINT_ERROR("error in texture initialization.")
	}

	void setUniformWorldToLight(const glm::mat4& worldToLightMat)
	{
		GlHelper::pushParameterToGPU(uniform_WorldToLight, worldToLightMat);
	}

	void setUniformLightDirection(const glm::vec3& lightDir)
	{
		GlHelper::pushParameterToGPU(uniform_lightDirection, lightDir);
	}
	void setUniformLightColor(const glm::vec3& lightCol)
	{
		GlHelper::pushParameterToGPU(uniform_lightColor, lightCol);
	}
	void setUniformLightIntensity(float lightIntensity)
	{
		GlHelper::pushParameterToGPU(uniform_lightIntensity, lightIntensity);
	}
};

class MaterialSpotLight final : public MaterialLight
{
private:

	GLuint uniform_WorldToLight;
	GLuint uniform_lightDirection;
	GLuint uniform_lightPosition;
	GLuint uniform_lightAngle;
	GLuint uniform_lightColor;
	GLuint uniform_lightIntensity;

public:
	MaterialSpotLight()
		: MaterialLight()
	{}

	MaterialSpotLight(const ShaderProgram& shaderProgram)
		: MaterialLight(shaderProgram)
	{
		setExternalParameters(shaderProgram.getExternalParameters());
	}

	virtual ~MaterialSpotLight()
	{}

	MaterialSpotLight(const FileHandler::CompletePath& glProgramPath, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: MaterialLight(glProgramPath, glProgramId, internalParameters, externalParameters)
	{
		setExternalParameters(externalParameters);
	}

	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		uniform_WorldToLight = MaterialHelper::getUniform(m_glProgramId, "WorldToLightScreen");
		uniform_lightDirection = MaterialHelper::getUniform(m_glProgramId, "spotLight.direction");
		uniform_lightAngle = MaterialHelper::getUniform(m_glProgramId, "spotLight.angle");
		uniform_lightPosition = MaterialHelper::getUniform(m_glProgramId, "spotLight.position");
		uniform_lightColor = MaterialHelper::getUniform(m_glProgramId, "spotLight.color");
		uniform_lightIntensity = MaterialHelper::getUniform(m_glProgramId, "spotLight.intensity");

		if (!checkError("Uniforms"))
			PRINT_ERROR("error in texture initialization.")
	}

	void setUniformWorldToLight(const glm::mat4& worldToLightMat)
	{
		GlHelper::pushParameterToGPU(uniform_WorldToLight, worldToLightMat);
	}

	void setUniformLightPosition(const glm::vec3& lightPos)
	{
		GlHelper::pushParameterToGPU(uniform_lightPosition, lightPos);
	}
	void setUniformLightDirection(const glm::vec3& lightDir)
	{
		GlHelper::pushParameterToGPU(uniform_lightDirection, lightDir);
	}
	void setUniformLightAngle(float lightAngle)
	{
		GlHelper::pushParameterToGPU(uniform_lightAngle, lightAngle);
	}
	void setUniformLightColor(const glm::vec3& lightCol)
	{
		GlHelper::pushParameterToGPU(uniform_lightColor, lightCol);
	}
	void setUniformLightIntensity(float lightIntensity)
	{
		GlHelper::pushParameterToGPU(uniform_lightIntensity, lightIntensity);
	}
};

//shadowing : 
class MaterialShadowPass final : public Material
{
private:

	GLuint uniform_MVP;

public:
	MaterialShadowPass()
		: Material()
	{}

	MaterialShadowPass(const ShaderProgram& shaderProgram)
		: Material(shaderProgram)
	{
		setExternalParameters(shaderProgram.getExternalParameters());
	}

	virtual ~MaterialShadowPass()
	{}

	MaterialShadowPass(const FileHandler::CompletePath& glProgramPath, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: Material(glProgramPath, glProgramId, internalParameters, externalParameters)
	{
		setExternalParameters(externalParameters);
	}

	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		uniform_MVP = MaterialHelper::getUniform(m_glProgramId, "MVP");

		if (!checkError("Uniforms"))
			PRINT_ERROR("error in texture initialization.")
	}

	void setUniformMVP(const glm::mat4& mvp)
	{
		GlHelper::pushParameterToGPU(uniform_MVP, mvp);
	}
};


class MaterialShadowPassOmni final : public Material
{
private:

	GLuint uniform_ModelMatrix;
	GLuint uniform_VPLight[6];
	GLuint uniform_LightPos;
	GLuint uniform_FarPlane;

public:
	MaterialShadowPassOmni()
		: Material()
	{}

	MaterialShadowPassOmni(const ShaderProgram& shaderProgram)
		: Material(shaderProgram)
	{
		setExternalParameters(shaderProgram.getExternalParameters());
	}

	virtual ~MaterialShadowPassOmni()
	{}

	MaterialShadowPassOmni(const FileHandler::CompletePath& glProgramPath, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: Material(glProgramPath, glProgramId, internalParameters, externalParameters)
	{
		setExternalParameters(externalParameters);
	}

	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		uniform_ModelMatrix = MaterialHelper::getUniform(m_glProgramId, "ModelMatrix");
		for(int i = 0; i < 6; i++)
			uniform_VPLight[i] = MaterialHelper::getUniform(m_glProgramId, "VPLight[" + std::to_string(i) + "]");
		uniform_LightPos = MaterialHelper::getUniform(m_glProgramId, "LightPos");
		uniform_FarPlane = MaterialHelper::getUniform(m_glProgramId, "FarPlane");

		if (!checkError("Uniforms"))
			PRINT_ERROR("error in texture initialization.")
	}

	void setUniformModelMatrix(const glm::mat4& modelMatrix)
	{
		GlHelper::pushParameterToGPU(uniform_ModelMatrix, modelMatrix);
	}

	void setUniformVPLight(const glm::mat4& vpLight, int index)
	{
		assert(index < 6);
		GlHelper::pushParameterToGPU(uniform_VPLight[index], vpLight);
	}

	void setUniformLightPos(const glm::vec3& lightPos)
	{
		GlHelper::pushParameterToGPU(uniform_LightPos, lightPos);
	}

	void setUniformFarPlane(float farPlane)
	{
		GlHelper::pushParameterToGPU(uniform_FarPlane, farPlane);
	}
};


//blit material : 

//Default materials :
class MaterialBlit final : public Material
{
private:
	GLuint uniform_TextureBlit;

public:
	MaterialBlit()
		: Material()
	{}

	MaterialBlit(const ShaderProgram& shaderProgram)
		: Material(shaderProgram)
	{
		setExternalParameters(shaderProgram.getExternalParameters());
	}

	virtual ~MaterialBlit()
	{}

	MaterialBlit(const FileHandler::CompletePath& glProgramPath, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: Material(glProgramPath, glProgramId, internalParameters)
	{
		setExternalParameters(externalParameters);
	}

	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		uniform_TextureBlit = MaterialHelper::getUniform(m_glProgramId, "Texture");

		if (!checkError("Uniforms"))
			PRINT_ERROR("error in texture initialization.")
	}

	void setUniformBlitTexture(int texUnitId)
	{
		GlHelper::pushParameterToGPU(uniform_TextureBlit, texUnitId);
	}
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

	Material3DObject(const FileHandler::CompletePath& glProgramPath, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: Material(glProgramPath, glProgramId, internalParameters)
	{
		setExternalParameters(externalParameters);
	}

	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		uniform_MVP = MaterialHelper::getUniform(m_glProgramId, "MVP");
		uniform_normalMatrix = MaterialHelper::getUniform(m_glProgramId, "NormalMatrix");
		uniform_bonesTransform = MaterialHelper::getUniforms(m_glProgramId, "BonesTransform", MAX_BONE_COUNT);
		uniform_useSkeleton = MaterialHelper::getUniform(m_glProgramId, "UseSkeleton");

		if (!checkError("Uniforms"))
			PRINT_ERROR("error in texture initialization.")
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

class MaterialLit final : public Material3DObject
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

	MaterialLit(const FileHandler::CompletePath& glProgramPath, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: Material3DObject(glProgramPath, glProgramId, internalParameters, externalParameters)
	{

	}
};

class MaterialUnlit final : public Material3DObject
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

	MaterialUnlit(const FileHandler::CompletePath& glProgramPath, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: Material3DObject(glProgramPath, glProgramId, internalParameters, externalParameters)
	{
		setExternalParameters(externalParameters);
	}

	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		uniform_color = MaterialHelper::getUniform(m_glProgramId, "Color");

		if (!checkError("Uniforms"))
			PRINT_ERROR("error in texture initialization.")
	}

	void setUniform_color(glm::vec3 color)
	{
		GlHelper::pushParameterToGPU<glm::vec3>(uniform_color, color);
	}
};

class MaterialInstancedUnlit final : public Material
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

	MaterialInstancedUnlit(const FileHandler::CompletePath& glProgramPath, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: Material(glProgramPath, glProgramId, internalParameters, externalParameters)
	{
		setExternalParameters(externalParameters);
	}

	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		uniform_VP = MaterialHelper::getUniform(m_glProgramId, "VP");
		uniform_color = MaterialHelper::getUniform(m_glProgramId, "Color");

		if (!checkError("Uniforms"))
			PRINT_ERROR("error in texture initialization.")
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

struct MaterialDebugDrawer final : public Material
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

	MaterialDebugDrawer(const FileHandler::CompletePath& glProgramPath, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: Material(glProgramPath, glProgramId, internalParameters, externalParameters)
	{
		setExternalParameters(externalParameters);
	}

	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		uniform_MVP = MaterialHelper::getUniform(m_glProgramId, "MVP");

		if (!checkError("Uniforms"))
			PRINT_ERROR("error in texture initialization.")
	}

	void setUniform_MVP(const glm::mat4& MVP)
	{
		GlHelper::pushParameterToGPU<glm::mat4>(uniform_MVP, MVP);
	}
};


class MaterialSkybox final : public Material
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

	MaterialSkybox(const FileHandler::CompletePath& glProgramPath, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: Material(glProgramPath, glProgramId, internalParameters, externalParameters)
	{
		setExternalParameters(externalParameters);
	}

	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		uniform_VP = MaterialHelper::getUniform(m_glProgramId, "VP");

		if (!checkError("Uniforms"))
			PRINT_ERROR("error in texture initialization.")
	}

	void setUniform_VP(const glm::mat4& VP)
	{
		GlHelper::pushParameterToGPU<glm::mat4>(uniform_VP, VP);
	}
};


class MaterialShadow : public Material
{

};


class MaterialBillboard final : public Material
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

	MaterialBillboard(const FileHandler::CompletePath& glProgramPath, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: Material(glProgramPath, glProgramId, internalParameters, externalParameters)
	{
		setExternalParameters(externalParameters);
	}

	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		uniform_MVP = MaterialHelper::getUniform(m_glProgramId, "MVP");
		uniform_Scale = MaterialHelper::getUniform(m_glProgramId, "Scale");
		uniform_Translation = MaterialHelper::getUniform(m_glProgramId, "Translation");
		uniform_Texture = MaterialHelper::getUniform(m_glProgramId, "Texture");
		uniform_CameraRight = MaterialHelper::getUniform(m_glProgramId, "CameraRight");
		uniform_CameraUp = MaterialHelper::getUniform(m_glProgramId, "CameraUp");
		uniform_Color = MaterialHelper::getUniform(m_glProgramId, "Color");

		if (!checkError("Uniforms"))
			PRINT_ERROR("error in texture initialization.")
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


struct MaterialTerrain final : public Material3DObject
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

	MaterialTerrain(const FileHandler::CompletePath& glProgramPath, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: Material3DObject(glProgramPath, glProgramId, internalParameters, externalParameters)
	{
		setExternalParameters(externalParameters);
	}

	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		uniform_LayoutOffset = MaterialHelper::getUniform(m_glProgramId, "LayoutOffset");
		uniform_FilterTexture = MaterialHelper::getUniform(m_glProgramId, "FilterTexture");
		uniform_TextureRepetition = MaterialHelper::getUniform(m_glProgramId, "TextureRepetition");
		uniform_SpecularPower = MaterialHelper::getUniform(m_glProgramId, "SpecularPower");

		if (!checkError("Uniforms"))
			PRINT_ERROR("error in texture initialization.")

		//uniform_DiffuseTexture = MaterialHelper::getUniform(m_glProgramId, "DiffuseTexture");
		//uniform_BumpTexture = MaterialHelper::getUniform(m_glProgramId, "BumpTexture");
		//uniform_SpecularTexture = MaterialHelper::getUniform(m_glProgramId, "SpecularTexture");
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


class MaterialTerrainEdition final : public Material
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

	MaterialTerrainEdition(const FileHandler::CompletePath& glProgramPath, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: Material(glProgramPath, glProgramId, internalParameters, externalParameters)
	{
		setExternalParameters(externalParameters);
	}

	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		uniform_filterValues = MaterialHelper::getUniform(m_glProgramId, "FilterValues");
		uniform_textureFilter = MaterialHelper::getUniform(m_glProgramId, "FilterTexture");
		uniform_textureRepetition = MaterialHelper::getUniform(m_glProgramId, "TextureRepetition");

		uniform_textureDiffuse = MaterialHelper::getUniform(m_glProgramId, "Diffuse");
		uniform_textureSpecular = MaterialHelper::getUniform(m_glProgramId, "Specular");
		uniform_textureBump = MaterialHelper::getUniform(m_glProgramId, "Bump");

		if (!checkError("Uniforms"))
			PRINT_ERROR("error in texture initialization.")
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

class MaterialDrawOnTexture final : public Material
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

	MaterialDrawOnTexture(const FileHandler::CompletePath& glProgramPath, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: Material(glProgramPath, glProgramId, internalParameters, externalParameters)
	{
		setExternalParameters(externalParameters);
	}

	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		uniform_drawPosition = MaterialHelper::getUniform(m_glProgramId, "DrawPosition");
		uniform_colorToDraw = MaterialHelper::getUniform(m_glProgramId, "DrawColor");
		uniform_drawRadius = MaterialHelper::getUniform(m_glProgramId, "DrawRadius");
		uniform_textureToDrawOn = MaterialHelper::getUniform(m_glProgramId, "Texture");

		if (!checkError("Uniforms"))
			PRINT_ERROR("error in texture initialization.")
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

class MaterialGrassField final : public Material
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

	MaterialGrassField(const FileHandler::CompletePath& glProgramPath, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: Material(glProgramPath, glProgramId, internalParameters, externalParameters)
	{
		setExternalParameters(externalParameters);
	}

	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		uniform_time = MaterialHelper::getUniform(m_glProgramId, "Time");
		uniform_Texture = MaterialHelper::getUniform(m_glProgramId, "Texture");
		uniform_VP = MaterialHelper::getUniform(m_glProgramId, "VP");

		if (!checkError("Uniforms"))
			PRINT_ERROR("error in texture initialization.")
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

class MaterialParticlesCPU final : public Material
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

	MaterialParticlesCPU(const FileHandler::CompletePath& glProgramPath, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: Material(glProgramPath, glProgramId, internalParameters, externalParameters)
	{
		setExternalParameters(externalParameters);
	}

	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		m_uniformVP = MaterialHelper::getUniform(m_glProgramId, "VP");
		m_uniformTexture = MaterialHelper::getUniform(m_glProgramId, "Texture");
		m_uniformCameraRight = MaterialHelper::getUniform(m_glProgramId, "CameraRight");
		m_uniformCameraUp = MaterialHelper::getUniform(m_glProgramId, "CameraUp");

		if (!checkError("Uniforms"))
			PRINT_ERROR("error in texture initialization.")
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


class MaterialParticles final : public Material
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

	MaterialParticles(const FileHandler::CompletePath& glProgramPath, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: Material(glProgramPath, glProgramId, internalParameters, externalParameters)
	{
		setExternalParameters(externalParameters);
	}

	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		m_uniformVP = MaterialHelper::getUniform(m_glProgramId, "VP");
		m_uniformTexture = MaterialHelper::getUniform(m_glProgramId, "Texture");
		m_uniformCameraRight = MaterialHelper::getUniform(m_glProgramId, "CameraRight");
		m_uniformCameraUp = MaterialHelper::getUniform(m_glProgramId, "CameraUp");

		if (!checkError("Uniforms"))
			PRINT_ERROR("error in texture initialization.")
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

class MaterialParticleSimulation final : public Material
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

	MaterialParticleSimulation(const FileHandler::CompletePath& glProgramPath, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
		: Material(glProgramPath, glProgramId, internalParameters, externalParameters)
	{
		setExternalParameters(externalParameters);
	}

	void setExternalParameters(const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters) override
	{
		m_uniformDeltaTime = MaterialHelper::getUniform(m_glProgramId, "DeltaTime");

		if (!checkError("Uniforms"))
			PRINT_ERROR("error in texture initialization.")
	}

	void glUniform_deltaTime(float deltaTime)
	{
		GlHelper::pushParameterToGPU(m_uniformDeltaTime, deltaTime);
	}
};