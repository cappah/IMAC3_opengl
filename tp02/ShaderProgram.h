#pragma once

#include <vector>
#include <memory>
#include <istream>
#include <glew/glew.h>
#include <glm/glm.hpp>
#include "FileHandler.h"

//forwards :
class InternalShaderParameterBase;
class ExternalShaderParameterBase;
class Material;


enum PipelineTypes
{
	OPAQUE_PIPILINE,
	TRANSPARENT_PIPELINE,
	REFLEXIVE_PIPELINE,
	WATER_PIPELINE,
	COUNT
};

static std::vector<std::string> PipelineTypesToString = {
	"opaque",
	"transparent",
	"reflexive",
	"water",
};

enum ShaderProgramType {
	CUSTOM = 0,
	LIT,
	UNLIT,
};

static std::vector<std::string> ShaderProgramTypes = { 
	"custom",
	"lit",
	"unlit",
};


struct ShaderProgram : public Resource
{
	GLuint id;

private:
	ShaderProgramType m_programType;
	PipelineTypes m_pipelineType;

	std::vector<std::shared_ptr<InternalShaderParameterBase>> m_internalShaderParameters;
	std::vector<std::shared_ptr<ExternalShaderParameterBase>> m_externalShaderParameters;

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
	void load(const FileHandler::CompletePath& shaderFolderPath, const std::string& vertexShaderName, const std::string& fragmentShaderName, const std::string& geometryShaderName);


	//make a new material from this shaderProgram (warning : internaly use "new", you have to deal with deletion)
	Material* makeNewMaterialInstance();
	//make a new material from this shaderProgram (use shared ptr for automatic deletion)
	std::shared_ptr<Material> makeSharedMaterialInstance();
	//fill material datas from this shaderProgram
	void LoadMaterialInstance(Material* material);

	void addMaterialRef(Material* ref);
	void removeMaterialRef(Material* ref);

	const std::vector<std::shared_ptr<InternalShaderParameterBase>>& getInternalParameters() const;
	const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& getExternalParameters() const;

	ShaderProgramType getType() const;
	PipelineTypes getPipelineType() const;

	//Not copyable
	ShaderProgram(const ShaderProgram& other) = delete;
	const ShaderProgram& operator=(const ShaderProgram& other) = delete;

};

Material* makeNewMaterialInstance(const FileHandler::CompletePath& path);