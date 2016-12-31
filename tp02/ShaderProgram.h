#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <istream>
#include <glew/glew.h>
#include <glm/glm.hpp>
#include "FileHandler.h"

//forwards :
class InternalShaderParameterBase;
class ExternalShaderParameterBase;
class Material;

namespace Rendering
{

	enum class BaseMaterialType
	{
		OBJECT_3D = 0,
		BILLBOARD,
		PARTICLE,
		REFLECTION,
		CUSTOM,
		COUNT
	};

	static std::vector<std::string> BaseMaterialTypeToString = {
		"object3D",
		"billboard",
		"particle",
		"reflection",
		"custom",
	};

	enum class PipelineType
	{
		DEFERRED_PIPILINE = 0,
		FORWARD_PIPELINE,
		CUSTOM_PIPELINE,
		COUNT
	};

	static std::vector<std::string> PipelineTypesToString = {
		"deferred",
		"forward",
		"custom",
	};
}


struct ShaderProgram : public Resource
{
	std::unordered_map<Rendering::PipelineType, GLuint> ids;

private:
	//ShaderProgramType m_programType;
	//Rendering::Rendering::PipelineType m_pipelineType;
	std::vector<Rendering::PipelineType> m_pipelineHandlingTypes;
	Rendering::BaseMaterialType m_baseMaterialType;

	std::vector<std::shared_ptr<InternalShaderParameterBase>> m_internalShaderParameters;
	//std::vector<std::shared_ptr<ExternalShaderParameterBase>> m_externalShaderParameters;

	std::vector<Material*> m_materialRefs;

public:
	ShaderProgram();
	ShaderProgram(const FileHandler::CompletePath& path);
	//ShaderProgram(const FileHandler::CompletePath& vertexShaderPath, const FileHandler::CompletePath& fragmentShaderPath);
	//ShaderProgram(const FileHandler::CompletePath& vertexShaderPath, const FileHandler::CompletePath& fragmentShaderPath, const FileHandler::CompletePath& geometryShaderPath);

	void init(const FileHandler::CompletePath& path, const ID& id) override;

	void load(const FileHandler::CompletePath& path);
	//void load(const FileHandler::CompletePath& vertexShaderPath, const FileHandler::CompletePath& fragmentShaderPath);
	//void load(const FileHandler::CompletePath& vertexShaderPath, const FileHandler::CompletePath& fragmentShaderPath, const FileHandler::CompletePath& geometryShaderPath);
	void load(Rendering::PipelineType pipelineType, const FileHandler::CompletePath& shaderFolderPath, const std::string& vertexShaderName, const std::string& fragmentShaderName, const std::string& geometryShaderName);

	bool implementPipeline(Rendering::PipelineType pipelineType) const;
	int getImplementedPipelineCount() const;
	Rendering::PipelineType getImplementedPipeline(int idx) const;
	GLuint getProgramId(Rendering::PipelineType pipelineType) const;


	Material* makeNewMaterialInstance(const FileHandler::CompletePath& completePath);
	std::shared_ptr<Material> makeSharedMaterialInstance(const FileHandler::CompletePath& completePath);
	//make a new material from this shaderProgram (warning : internaly use "new", you have to deal with deletion)
	Material* makeNewMaterialInstance();
	//make a new material from this shaderProgram (use shared ptr for automatic deletion)
	std::shared_ptr<Material> makeSharedMaterialInstance();
	//fill material datas from this shaderProgram
	void LoadMaterialInstance(Material* material) const;

	void addMaterialRef(Material* ref);
	void removeMaterialRef(Material* ref);

	const std::vector<std::shared_ptr<InternalShaderParameterBase>>& getInternalParameters() const;
	//const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& getExternalParameters() const;

	Rendering::BaseMaterialType getBaseMaterialType() const;

	//Not copyable
	ShaderProgram(const ShaderProgram& other) = delete;
	const ShaderProgram& operator=(const ShaderProgram& other) = delete;

};

Material* makeNewMaterialInstance(const FileHandler::CompletePath& path);