#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <istream>
#include <glew/glew.h>
#include <glm/glm.hpp>
#include "FileHandler.h"
#include "MaterialAggregation.h"

//forwards :
class InternalShaderParameterBase;
class ExternalShaderParameterBase;
class Material;
class ResourceTreeView;
namespace MVS {
	class NodeManager;
}

namespace Rendering
{
	enum class MaterialType
	{
		INTERNAL,
		DEFAULT,
		COUNT
	};

	static std::vector<std::string> MaterialTypeToString = {
		"internal",
		"default",
	};

	enum class MaterialUsage
	{
		MESH,
		BILLBOARD,
		PARTICLES,
		COUNT
	};

	static std::vector<std::string> MaterialUsageToString = {
		"mesh",
		"billboard",
		"particles",
	};

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

private:
	GLuint m_programId;

	// Material parameters and aggregates.
	std::vector<std::shared_ptr<InternalShaderParameterBase>> m_internalShaderParameters;
	std::unordered_map<std::string, std::shared_ptr<MaterialAggregation>> m_aggregations;
	std::unordered_map<std::string, std::shared_ptr<PerInstanceMaterialAggregation>> m_perInstanceAggregations;
	//std::vector<std::shared_ptr<ExternalShaderParameterBase>> m_externalShaderParameters;

	// How the material is used.
	Rendering::MaterialType m_materialType;
	Rendering::MaterialUsage m_usage;
	Rendering::PipelineType m_pipeline;
	bool m_usedWithReflections;
	bool m_usedWithSkeleton;
	/*std::vector<Rendering::PipelineType> m_pipelineHandlingTypes;
	Rendering::BaseMaterialType m_baseMaterialType;*/

	// Pointers to materials using this program.
	std::vector<Material*> m_materialRefs;

	std::string m_vertexShaderName;
	std::string m_fragmentShaderName;
	std::string m_geometryShaderName;

	// A pointer to the NodeManager (i.e : the material editing window) associated with this material.
	std::shared_ptr<MVS::NodeManager> m_nodeManagerRef;

public:
	ShaderProgram();
	ShaderProgram(const FileHandler::CompletePath& path);
	//ShaderProgram(const FileHandler::CompletePath& vertexShaderPath, const FileHandler::CompletePath& fragmentShaderPath);
	//ShaderProgram(const FileHandler::CompletePath& vertexShaderPath, const FileHandler::CompletePath& fragmentShaderPath, const FileHandler::CompletePath& geometryShaderPath);

	const std::unordered_map<std::string, std::shared_ptr<MaterialAggregation>>& getAggregations() const;
	const std::unordered_map<std::string, std::shared_ptr<PerInstanceMaterialAggregation>>& getPerInstanceAggregation() const;
	GLuint getProgramId() const;
	void compile();

	void init(const FileHandler::CompletePath& path, const ID& id) override;

	void load(const FileHandler::CompletePath& path);
	void save(const FileHandler::CompletePath& path);

	GLuint makeGLProgramForInternal(const FileHandler::CompletePath& shaderFolderPath, const std::string& vertexShaderName, const std::string& fragmentShaderName, const std::string& geometryShaderName);
	//GLuint makeGLProgram(const FileHandler::CompletePath& shaderFolderPath, const std::string& vertexShaderName, const std::string& fragmentShaderName, const std::string& geometryShaderName);
	GLuint makeGLProgramForDefault(const FileHandler::CompletePath& shaderFolderPath, const std::string& fragmentShaderName);
	GLuint makeGLProgram();
	GLuint makeGLProgram(const std::string& vertexShaderContent, const std::string& fragmentShaderContent);
	//void load(const FileHandler::CompletePath& vertexShaderPath, const FileHandler::CompletePath& fragmentShaderPath);
	//void load(const FileHandler::CompletePath& vertexShaderPath, const FileHandler::CompletePath& fragmentShaderPath, const FileHandler::CompletePath& geometryShaderPath);

	//bool implementPipeline(Rendering::PipelineType pipelineType) const;
	//int getImplementedPipelineCount() const;
	//Rendering::PipelineType getImplementedPipeline(int idx) const;
	//GLuint getProgramId(Rendering::PipelineType pipelineType) const;


	//Material* makeNewMaterialInstance(const FileHandler::CompletePath& completePath);
	//std::shared_ptr<Material> makeSharedMaterialInstance(const FileHandler::CompletePath& completePath);
	////make a new material from this shaderProgram (warning : internaly use "new", you have to deal with deletion)
	//Material* makeNewMaterialInstance();
	////make a new material from this shaderProgram (use shared ptr for automatic deletion)
	//std::shared_ptr<Material> makeSharedMaterialInstance();
	//fill material datas from this shaderProgram
	void LoadMaterialInstance(Material* material) const;

	void addMaterialRef(Material* ref);
	void removeMaterialRef(Material* ref);

	const std::vector<std::shared_ptr<InternalShaderParameterBase>>& getInternalParameters() const;
	//const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& getExternalParameters() const;

	Rendering::MaterialType getMaterialType() const;

	virtual void drawInInspector(Scene & scene) override;

	// We have to display a option to create material instance based on this shaderProgram.
	virtual void drawRightClicContextMenu() override;

	//Not copyable
	ShaderProgram(const ShaderProgram& other) = delete;
	const ShaderProgram& operator=(const ShaderProgram& other) = delete;

};

//Material* makeNewMaterialInstance(const FileHandler::CompletePath& path);