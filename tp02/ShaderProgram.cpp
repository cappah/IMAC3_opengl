#include "Project.h"
#include "Materials.h"
#include "ShaderParameters.h"
#include "ShaderProgram.h"


ShaderProgram::ShaderProgram()
	:id(0)
{
}

ShaderProgram::ShaderProgram(const FileHandler::CompletePath& path)
	: Resource(path)
	, id(0)
{
}

ShaderProgram::ShaderProgram(const FileHandler::CompletePath& vertexShaderPath, const FileHandler::CompletePath& fragmentShaderPath)
	: id(0)
{
	load(vertexShaderPath, fragmentShaderPath);
}


ShaderProgram::ShaderProgram(const FileHandler::CompletePath& vertexShaderPath, const FileHandler::CompletePath& fragmentShaderPath, const FileHandler::CompletePath& geometryShaderPath)
	: id(0)
{
	load(vertexShaderPath, fragmentShaderPath, geometryShaderPath);
}

void ShaderProgram::init(const FileHandler::CompletePath& path)
{
	load(path);
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

	const std::string vertexShaderName = root.get("vertex", "").asString();
	const std::string fragmentShaderName = root.get("fragment", "").asString();
	const std::string geometryShaderName = root.get("geometry", "").asString();

	const FileHandler::CompletePath& vertexAbsolutePath(path.getPath().toString() + "/" + vertexShaderName);
	const FileHandler::CompletePath& fragmentAbsolutePath(path.getPath().toString() + "/" + vertexShaderName);
	const FileHandler::CompletePath& geometryAbsolutePath(path.getPath().toString() + "/" + vertexShaderName);


	load(vertexAbsolutePath, fragmentAbsolutePath, geometryAbsolutePath);

	//Internal parameters
	Json::Value internalShaderParameters = root["internalShaderParameters"];
	int inernalShaderParameterCount = internalShaderParameters.size();
	
	m_internalShaderParameters.clear();
	for (int i = 0; i < inernalShaderParameterCount; i++)
	{
		assert(internalShaderParameters.isValidIndex(i));

		std::string parameterType = internalShaderParameters[i].get("type", "").asString();
		std::string parameterName = internalShaderParameters[i].get("name", "").asString();

		m_internalShaderParameters.push_back(MakeNewInternalShaderParameter(parameterType, parameterName));
	}

	//external parameters
	Json::Value externalShaderParameters = root["externalShaderParameters"];
	int externalShaderParameterCount = externalShaderParameters.size();

	m_externalShaderParameters.clear();
	for (int i = 0; i < externalShaderParameterCount; i++)
	{
		assert(externalShaderParameters.isValidIndex(i));

		std::string parameterType = externalShaderParameters[i].get("type", "").asString();
		std::string parameterName = externalShaderParameters[i].get("name", "").asString();

		m_externalShaderParameters.push_back(MakeNewExternalShaderParameter(parameterType, parameterName));
	}
}

void ShaderProgram::load(const FileHandler::CompletePath& vertexShaderPath, const FileHandler::CompletePath& fragmentShaderPath)
{
	bool hasVertShader = !vertexShaderPath.empty();
	bool hasFragShader = !fragmentShaderPath.empty();

	GLuint vertShaderId = 0;
	GLuint fragShaderId = 0;

	if (hasVertShader)
		vertShaderId = compile_shader_from_file(GL_VERTEX_SHADER, vertexShaderPath.c_str());
	if (hasFragShader)
		fragShaderId = compile_shader_from_file(GL_FRAGMENT_SHADER, fragmentShaderPath.c_str());

	GLuint programObject = glCreateProgram();
	if (hasVertShader)
		glAttachShader(programObject, vertShaderId);
	if (hasFragShader)
		glAttachShader(programObject, fragShaderId);

	glLinkProgram(programObject);
	if (check_link_error(programObject) < 0)
		exit(1);

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);

	id = programObject;
}


void ShaderProgram::load(const FileHandler::CompletePath& vertexShaderPath, const FileHandler::CompletePath& fragmentShaderPath, const FileHandler::CompletePath& geometryShaderPath)
{
	bool hasVertShader = !vertexShaderPath.empty();
	bool hasFragShader = !fragmentShaderPath.empty();
	bool hasGeomShader = !geometryShaderPath.empty();

	GLuint vertShaderId = 0;
	GLuint fragShaderId = 0;
	GLuint geomShaderId = 0;

	if (hasVertShader)
		vertShaderId = compile_shader_from_file(GL_VERTEX_SHADER, vertexShaderPath.c_str());
	if (hasFragShader)
		fragShaderId = compile_shader_from_file(GL_FRAGMENT_SHADER, fragmentShaderPath.c_str());
	if (hasGeomShader)
		geomShaderId = compile_shader_from_file(GL_GEOMETRY_SHADER, geometryShaderPath.c_str());

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
	if (!checkError("Uniforms"))
		exit(1);

	id = programObject;
}

Material* ShaderProgram::makeNewMaterialInstance()
{
	return new Material(m_completePath.getFilename(), id, m_internalShaderParameters, m_externalShaderParameters);
}

void ShaderProgram::LoadMaterialInstance(Material* material)
{
	material->loadFromShaderProgramDatas(id, m_internalShaderParameters, m_externalShaderParameters);
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