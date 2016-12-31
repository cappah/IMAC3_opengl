#include "Project.h"
#include "Materials.h"
#include "RenderBatch.h"
#include "ShaderParameters.h"
#include "ShaderProgram.h"
#include "Factories.h"


ShaderProgram::ShaderProgram()
	//:id(0)
{
}

ShaderProgram::ShaderProgram(const FileHandler::CompletePath& path)
	: Resource(path)
	//, id(0)
{
}

//ShaderProgram::ShaderProgram(const FileHandler::CompletePath& vertexShaderPath, const FileHandler::CompletePath& fragmentShaderPath)
//	: id(0)
//{
//	load(vertexShaderPath, fragmentShaderPath);
//}
//
//
//ShaderProgram::ShaderProgram(const FileHandler::CompletePath& vertexShaderPath, const FileHandler::CompletePath& fragmentShaderPath, const FileHandler::CompletePath& geometryShaderPath)
//	: id(0)
//{
//	load(vertexShaderPath, fragmentShaderPath, geometryShaderPath);
//}

void ShaderProgram::init(const FileHandler::CompletePath& path, const ID& id)
{
	Resource::init(path, id);

	assert(!Project::isPathPointingInsideProjectFolder(path)); //path should be relative
	FileHandler::CompletePath absolutePath = Project::getAbsolutePathFromRelativePath(path);

	load(absolutePath);
}

void ShaderProgram::load(const FileHandler::CompletePath& path)
{
	std::ifstream stream;
	stream.open(path.toString());
	if (!stream.is_open())
	{
		std::cout << "error, can't load shader program at path : " << path.toString() << std::endl;
		return;
	}
	Json::Value root;
	stream >> root;

	// Pipeline handling type
	m_pipelineHandlingTypes.clear();
	Json::Value pipelineHandlingTypeJson = root["pipelineHandling"];
	if (pipelineHandlingTypeJson.isArray())
	{
		for (int i = 0; i < pipelineHandlingTypeJson.size(); i++)
		{
			auto foundItPipelineType = std::find(Rendering::PipelineTypesToString.begin(), Rendering::PipelineTypesToString.end(), pipelineHandlingTypeJson[i].asString());
			assert(foundItPipelineType != Rendering::PipelineTypesToString.end());
			int foundIdxPipelineType = foundItPipelineType - Rendering::PipelineTypesToString.begin();
			m_pipelineHandlingTypes.push_back((Rendering::PipelineType)foundIdxPipelineType);
		}
	}
	else
	{
		auto foundItPipelineType = std::find(Rendering::PipelineTypesToString.begin(), Rendering::PipelineTypesToString.end(), root.get("pipelineHandling", "").asString()/*pipelineHandlingTypeJson.asString()*/);
		assert(foundItPipelineType != Rendering::PipelineTypesToString.end());
		int foundIdxPipelineType = foundItPipelineType - Rendering::PipelineTypesToString.begin();
		m_pipelineHandlingTypes.push_back((Rendering::PipelineType)foundIdxPipelineType);
	}

	// Base Material
	auto foundItBaseMaterial = std::find(Rendering::BaseMaterialTypeToString.begin(), Rendering::BaseMaterialTypeToString.end(), root.get("baseMaterial", "").asString());
	assert(foundItBaseMaterial != Rendering::BaseMaterialTypeToString.end());
	int foundIdxBaseMaterial = foundItBaseMaterial - Rendering::BaseMaterialTypeToString.begin();
	m_baseMaterialType = (Rendering::BaseMaterialType)foundIdxBaseMaterial;

	if (std::find(m_pipelineHandlingTypes.begin(), m_pipelineHandlingTypes.end(), Rendering::PipelineType::DEFERRED_PIPILINE) != m_pipelineHandlingTypes.end()
		&& std::find(m_pipelineHandlingTypes.begin(), m_pipelineHandlingTypes.end(), Rendering::PipelineType::FORWARD_PIPELINE) != m_pipelineHandlingTypes.end())
	{
		Json::Value deferredJson = root.get("deferred", "");
		assert(!deferredJson.empty());
		Json::Value forwardJson = root.get("forward", "");
		assert(!forwardJson.empty());

		// Deferred : 
		std::string vertexShaderName = deferredJson.get("vertex", "").asString();
		std::string fragmentShaderName = deferredJson.get("fragment", "").asString();
		std::string geometryShaderName = deferredJson.get("geometry", "").asString();

		load(Rendering::PipelineType::DEFERRED_PIPILINE, path, vertexShaderName, fragmentShaderName, geometryShaderName);

		// Forward : 
		vertexShaderName = forwardJson.get("vertex", "").asString();
		fragmentShaderName = forwardJson.get("fragment", "").asString();
		geometryShaderName = forwardJson.get("geometry", "").asString();

		load(Rendering::PipelineType::FORWARD_PIPELINE, path, vertexShaderName, fragmentShaderName, geometryShaderName);
	}
	else
	{
		const std::string vertexShaderName = root.get("vertex", "").asString();
		const std::string fragmentShaderName = root.get("fragment", "").asString();
		const std::string geometryShaderName = root.get("geometry", "").asString();

		assert(m_pipelineHandlingTypes.size() == 1);
		Rendering::PipelineType pipelineType = (Rendering::PipelineType)m_pipelineHandlingTypes[0];
		load(pipelineType, path, vertexShaderName, fragmentShaderName, geometryShaderName);
	}



	// Internal parameters
	Json::Value internalShaderParameters = root["internalShaderParameters"];
	int inernalShaderParameterCount = internalShaderParameters.size();
	
	m_internalShaderParameters.clear();
	for (int i = 0; i < inernalShaderParameterCount; i++)
	{
		assert(internalShaderParameters.isValidIndex(i));

		auto newParameter = MakeNewInternalShaderParameter(internalShaderParameters[i]);
		//newParameter->init(id); //don't forget to init the parameter to get the uniforms
		m_internalShaderParameters.push_back(newParameter);
	}

	// External parameters
	/*Json::Value externalShaderParameters = root["externalShaderParameters"];
	int externalShaderParameterCount = externalShaderParameters.size();

	m_externalShaderParameters.clear();
	for (int i = 0; i < externalShaderParameterCount; i++)
	{
		assert(externalShaderParameters.isValidIndex(i));

		std::string parameterType = externalShaderParameters[i].get("type", "").asString();
		std::string parameterName = externalShaderParameters[i].get("name", "").asString();

		m_externalShaderParameters.push_back(MakeNewExternalShaderParameter(parameterType, parameterName));
	}*/

	PRINT_ERROR("error in shader program initialization.");
}
//
//void ShaderProgram::load(const FileHandler::CompletePath& vertexShaderPath, const FileHandler::CompletePath& fragmentShaderPath)
//{
//	bool hasVertShader = !vertexShaderPath.empty();
//	bool hasFragShader = !fragmentShaderPath.empty();
//
//	GLuint vertShaderId = 0;
//	GLuint fragShaderId = 0;
//
//	if (hasVertShader)
//		vertShaderId = compile_shader_from_file(GL_VERTEX_SHADER, vertexShaderPath.c_str());
//	if (hasFragShader)
//		fragShaderId = compile_shader_from_file(GL_FRAGMENT_SHADER, fragmentShaderPath.c_str());
//
//	GLuint programObject = glCreateProgram();
//	if (hasVertShader)
//		glAttachShader(programObject, vertShaderId);
//	if (hasFragShader)
//		glAttachShader(programObject, fragShaderId);
//
//	glLinkProgram(programObject);
//	if (check_link_error(programObject) < 0)
//		exit(1);
//
//	//check uniform errors : 
//	if (!checkError("Uniforms"))
//		exit(1);
//
//	id = programObject;
//}
//

void ShaderProgram::load(Rendering::PipelineType pipelineType, const FileHandler::CompletePath& shaderFolderPath, const std::string& vertexShaderName, const std::string& fragmentShaderName, const std::string& geometryShaderName)
{
	bool hasVertShader = !vertexShaderName.empty();
	bool hasFragShader = !fragmentShaderName.empty();
	bool hasGeomShader = !geometryShaderName.empty();

	GLuint vertShaderId = 0;
	GLuint fragShaderId = 0;
	GLuint geomShaderId = 0;

	if (hasVertShader)
	{
		const FileHandler::CompletePath& vertexAbsolutePath(shaderFolderPath.getPath().toString() + "/" + vertexShaderName);
		vertShaderId = compile_shader_from_file(GL_VERTEX_SHADER, vertexAbsolutePath.c_str());
	}
	if (hasFragShader)
	{
		const FileHandler::CompletePath& fragmentAbsolutePath(shaderFolderPath.getPath().toString() + "/" + fragmentShaderName);
		fragShaderId = compile_shader_from_file(GL_FRAGMENT_SHADER, fragmentAbsolutePath.c_str());
	}
	if (hasGeomShader)
	{
		const FileHandler::CompletePath& geometryAbsolutePath(shaderFolderPath.getPath().toString() + "/" + geometryShaderName);
		geomShaderId = compile_shader_from_file(GL_GEOMETRY_SHADER, geometryAbsolutePath.c_str());
	}

	GLuint programObject = glCreateProgram();
	if (hasVertShader)
		glAttachShader(programObject, vertShaderId);
	if (hasFragShader)
		glAttachShader(programObject, fragShaderId);
	if (hasGeomShader)
		glAttachShader(programObject, geomShaderId);

	glLinkProgram(programObject);
	if (check_link_error(programObject) < 0)
 		exit(1);

	//check uniform errors : 
	ASSERT_GL_ERROR("error in material initialization.");

	ids[pipelineType] = programObject;
}

bool ShaderProgram::implementPipeline(Rendering::PipelineType pipelineType) const
{
	return ids.find(pipelineType) != ids.end();
}

int ShaderProgram::getImplementedPipelineCount() const
{
	return m_pipelineHandlingTypes.size();
}

Rendering::PipelineType ShaderProgram::getImplementedPipeline(int idx) const
{
	assert(idx >= 0 && idx < m_pipelineHandlingTypes.size());
	return m_pipelineHandlingTypes[idx];
}

GLuint ShaderProgram::getProgramId(Rendering::PipelineType pipelineType) const
{
	assert(ids.find(pipelineType) != ids.end());
	return ids.find(pipelineType)->second;
}

void ShaderProgram::LoadMaterialInstance(Material* material) const
{
	Rendering::PipelineType materialPipelineType = material->getPipelineType();
	auto& foundProgramId = ids.find(materialPipelineType);
	assert(foundProgramId != ids.end());

	material->loadFromShaderProgramDatas(foundProgramId->second, m_internalShaderParameters/*, m_externalShaderParameters*/);
}

void ShaderProgram::addMaterialRef(Material* ref)
{
	if (std::find(m_materialRefs.begin(), m_materialRefs.end(), ref) != m_materialRefs.end())
		m_materialRefs.push_back(ref);
}

void ShaderProgram::removeMaterialRef(Material* ref)
{
	m_materialRefs.erase(std::remove(m_materialRefs.begin(), m_materialRefs.end(), ref), m_materialRefs.end());
}

const std::vector<std::shared_ptr<InternalShaderParameterBase>>& ShaderProgram::getInternalParameters() const
{
	return m_internalShaderParameters;
}

//const std::vector<std::shared_ptr<ExternalShaderParameterBase>>& ShaderProgram::getExternalParameters() const
//{
//	return m_externalShaderParameters;
//}

Rendering::BaseMaterialType ShaderProgram::getBaseMaterialType() const
{
	return m_baseMaterialType;
}

Material* ShaderProgram::makeNewMaterialInstance(const FileHandler::CompletePath& completePath)
{
	switch (m_baseMaterialType)
	{
	case Rendering::BaseMaterialType::OBJECT_3D:
		return new Material3DObject(*this, completePath);
	case Rendering::BaseMaterialType::BILLBOARD:
		return new MaterialBillboard(*this, completePath);
	case Rendering::BaseMaterialType::PARTICLE:
		return new MaterialParticlesCPU(*this, completePath);
	case Rendering::BaseMaterialType::REFLECTION:
		return new MaterialReflection(*this, completePath);
	default:
		std::cout << "warning : we are trying to build a custom material from its program !";
		return nullptr;
	}
}

std::shared_ptr<Material> ShaderProgram::makeSharedMaterialInstance(const FileHandler::CompletePath& completePath)
{
	switch (m_baseMaterialType)
	{
	case Rendering::BaseMaterialType::OBJECT_3D:
		return std::make_shared<Material3DObject>(*this, completePath);
	case Rendering::BaseMaterialType::BILLBOARD:
		return std::make_shared<MaterialBillboard>(*this, completePath);
	case Rendering::BaseMaterialType::PARTICLE:
		return std::make_shared<MaterialParticlesCPU>(*this, completePath);
	case Rendering::BaseMaterialType::REFLECTION:
		return std::make_shared<MaterialReflection>(*this, completePath);
	default:
		std::cout << "warning : we are trying to build a custom material from its program !";
		return nullptr;
	}
}


Material* ShaderProgram::makeNewMaterialInstance()
{
	switch (m_baseMaterialType)
	{
	case Rendering::BaseMaterialType::OBJECT_3D:
		return new Material3DObject(*this);
	case Rendering::BaseMaterialType::BILLBOARD:
		return new MaterialBillboard(*this);
	case Rendering::BaseMaterialType::PARTICLE:
		return new MaterialParticlesCPU(*this);
	case Rendering::BaseMaterialType::REFLECTION:
		return new MaterialReflection(*this);
	default:
		std::cout << "warning : we are trying to build a custom material from its program !";
		return nullptr;
	}
}

std::shared_ptr<Material> ShaderProgram::makeSharedMaterialInstance()
{
	switch (m_baseMaterialType)
	{
	case Rendering::BaseMaterialType::OBJECT_3D:
		return std::make_shared<Material3DObject>(*this);
	case Rendering::BaseMaterialType::BILLBOARD:
		return std::make_shared<MaterialBillboard>(*this);
	case Rendering::BaseMaterialType::PARTICLE:
		return std::make_shared<MaterialParticlesCPU>(*this);
	case Rendering::BaseMaterialType::REFLECTION:
		return std::make_shared<MaterialReflection>(*this);
	default:
		std::cout << "warning : we are trying to build a custom material from its program !";
		return nullptr;
	}
}

Material* makeNewMaterialInstance(const FileHandler::CompletePath& path)
{
	assert(!Project::isPathPointingInsideProjectFolder(path)); //path should be relative
	FileHandler::CompletePath absolutePath = Project::getAbsolutePathFromRelativePath(path);

	std::ifstream stream;
	stream.open(absolutePath.toString());
	if (!stream.is_open())
	{
		std::cout << "error, can't load material at path : " << absolutePath.toString() << std::endl;
		return nullptr;
	}
	Json::Value root;
	stream >> root;

	std::string shaderProgramName = root.get("shaderProgramName", "").asString();
	assert(shaderProgramName != "");
	ShaderProgram* program = getProgramFactory().get(shaderProgramName);
	assert(program != nullptr);
	Rendering::BaseMaterialType baseMaterialType = program->getBaseMaterialType();

	switch (baseMaterialType)
	{
	case Rendering::BaseMaterialType::OBJECT_3D:
	{
		Material* newMaterial = new Material3DObject();
		newMaterial->load(root);
		return newMaterial;
	}
	case Rendering::BaseMaterialType::BILLBOARD:
	{
		Material* newMaterial = new MaterialBillboard();
		newMaterial->load(root);
		return newMaterial;
	}
	case Rendering::BaseMaterialType::PARTICLE:
	{
		Material* newMaterial = new MaterialParticlesCPU();
		newMaterial->load(root);
		return newMaterial;
	}
	case Rendering::BaseMaterialType::REFLECTION:
	{
		Material* newMaterial = new MaterialReflection();
		newMaterial->load(root);
		return newMaterial;
	}
	default:
		std::cout << "warning : we are trying to build a custom material from its program !";
		return nullptr;
	}
}