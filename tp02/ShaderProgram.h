#pragma once

#include <vector>
#include <memory>
#include <istream>
#include <glm/glm.hpp>
#include <glew/glew.h>
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

	ShaderProgram();
	ShaderProgram(const FileHandler::CompletePath& path);
	ShaderProgram(const FileHandler::CompletePath& vertexShaderPath, const FileHandler::CompletePath& fragmentShaderPath);
	ShaderProgram(const FileHandler::CompletePath& vertexShaderPath, const FileHandler::CompletePath& fragmentShaderPath, const FileHandler::CompletePath& geometryShaderPath);

	void init(const FileHandler::CompletePath& path) override;

	void load(const FileHandler::CompletePath& path);
	void load(const FileHandler::CompletePath& vertexShaderPath, const FileHandler::CompletePath& fragmentShaderPath);
	void load(const FileHandler::CompletePath& vertexShaderPath, const FileHandler::CompletePath& fragmentShaderPath, const FileHandler::CompletePath& geometryShaderPath);

	Material* makeNewMaterialInstance();

	//Not copyable
	ShaderProgram(const ShaderProgram& other) = delete;
	const ShaderProgram& operator=(const ShaderProgram& other) = delete;
};