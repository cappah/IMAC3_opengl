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

struct ShaderProgram : public Resource
{
	GLuint id;

private:
	std::vector<std::shared_ptr<InternalShaderParameterBase>> m_internalShaderParameters;
	std::vector<std::shared_ptr<ExternalShaderParameterBase>> m_externalShaderParameters;

	std::vector<Material*> m_materialRefs;

public:
	ShaderProgram();
	ShaderProgram(const FileHandler::CompletePath& path);
	ShaderProgram(const FileHandler::CompletePath& vertexShaderPath, const FileHandler::CompletePath& fragmentShaderPath);
	ShaderProgram(const FileHandler::CompletePath& vertexShaderPath, const FileHandler::CompletePath& fragmentShaderPath, const FileHandler::CompletePath& geometryShaderPath);

	void init(const FileHandler::CompletePath& path) override;

	void load(const FileHandler::CompletePath& path);
	void load(const FileHandler::CompletePath& vertexShaderPath, const FileHandler::CompletePath& fragmentShaderPath);
	void load(const FileHandler::CompletePath& vertexShaderPath, const FileHandler::CompletePath& fragmentShaderPath, const FileHandler::CompletePath& geometryShaderPath);

	//make a new material from this shaderProgram
	Material* makeNewMaterialInstance();
	//fill material datas from this shaderProgram
	void LoadMaterialInstance(Material* material);

	void addMaterialRef(Material* ref);
	void removeMaterialRef(Material* ref);

	const std::vector<std::shared_ptr<InternalShaderParameterBase>>& getInternalParameters() const;
	const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& getExternalParameters() const;

	//Not copyable
	ShaderProgram(const ShaderProgram& other) = delete;
	const ShaderProgram& operator=(const ShaderProgram& other) = delete;
};