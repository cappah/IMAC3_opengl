#include <memory>
#include "ShaderParameters.h"
#include "Factories.h" //forward


namespace GlHelper {

template<>
void pushParameterToGPU<int>(GLuint uniformId, const int& value)
{
	glUniform1i(uniformId, value);
}

template<>
void pushParameterToGPU<float>(GLuint uniformId, const float& value)
{
	glUniform1f(uniformId, value);
}

template<>
void pushParameterToGPU<glm::vec2>(GLuint uniformId, const glm::vec2& value)
{
	glUniform2f(uniformId, value.x, value.y);
}

template<>
void pushParameterToGPU<glm::ivec2>(GLuint uniformId, const glm::ivec2& value)
{
	glUniform2i(uniformId, value.x, value.y);
}

template<>
void pushParameterToGPU<glm::vec3>(GLuint uniformId, const glm::vec3& value)
{
	glUniform3f(uniformId, value.x, value.y, value.z);
}

template<>
void pushParameterToGPU<glm::ivec3>(GLuint uniformId, const glm::ivec3& value)
{
	glUniform3i(uniformId, value.x, value.y, value.z);
}

template<>
void pushParameterToGPU<glm::mat4>(GLuint uniformId, const glm::mat4& value)
{
	glUniformMatrix4fv(uniformId, 1, false, glm::value_ptr(value));
}

//array version (experimental) : 

template<>
void pushParametersToGPU<float>(GLuint uniformId, int count, const std::vector<float>& values)
{
	glUniform1fv(uniformId, count, &values[0]);
}

//TODO : complete

}


//utility function to make a shader from its type
std::shared_ptr<InternalShaderParameterBase> MakeNewInternalShaderParameter(const std::string& literaltype, std::string& name)
{
	ShaderParameter::ShaderParameterType parameterType = ShaderParameter::ShaderParameterType::TYPE_COUNT;
	auto& foundTypeIt = std::find(ShaderParameter::LiteralShaderParameterType.begin(), ShaderParameter::LiteralShaderParameterType.end(), literaltype);
	if (foundTypeIt != ShaderParameter::LiteralShaderParameterType.end())
		parameterType = (ShaderParameter::ShaderParameterType)std::distance(ShaderParameter::LiteralShaderParameterType.begin(), foundTypeIt);
	else
		return nullptr;


	switch (parameterType)
	{
	case ShaderParameter::ShaderParameterType::INT:
		return std::make_shared<InternalShaderParameter<int, ShaderParameter::IsNotArray>>(name);
		break;
	case ShaderParameter::ShaderParameterType::INT2:
		return std::make_shared<InternalShaderParameter<glm::ivec2, ShaderParameter::IsNotArray>>(name);
		break;
	case ShaderParameter::ShaderParameterType::INT3:
		return std::make_shared<InternalShaderParameter<glm::ivec3, ShaderParameter::IsNotArray>>(name);
		break;
	case ShaderParameter::ShaderParameterType::FLOAT:
		return std::make_shared<InternalShaderParameter<float, ShaderParameter::IsNotArray>>(name);
		break;
	case ShaderParameter::ShaderParameterType::FLOAT2:
		return std::make_shared<InternalShaderParameter<glm::vec2, ShaderParameter::IsNotArray>>(name);
		break;
	case ShaderParameter::ShaderParameterType::FLOAT3:
		return std::make_shared<InternalShaderParameter<glm::vec3, ShaderParameter::IsNotArray>>(name);
		break;
	case ShaderParameter::ShaderParameterType::TEXTURE:
		return std::make_shared<InternalShaderParameter<Texture, ShaderParameter::IsNotArray>>(name);
		break;
	case ShaderParameter::ShaderParameterType::CUBE_TEXTURE:
		return std::make_shared<InternalShaderParameter<CubeTexture, ShaderParameter::IsNotArray>>(name);
		break;
	default:
		return nullptr;
		break;
	}
}


//utility function to make a shader from its type
std::shared_ptr<ExternalShaderParameterBase> MakeNewExternalShaderParameter(const std::string& literaltype, std::string& name)
{
	ShaderParameter::ShaderParameterType parameterType = ShaderParameter::ShaderParameterType::TYPE_COUNT;
	auto& foundTypeIt = std::find(ShaderParameter::LiteralShaderParameterType.begin(), ShaderParameter::LiteralShaderParameterType.end(), literaltype);
	if (foundTypeIt != ShaderParameter::LiteralShaderParameterType.end())
		parameterType = (ShaderParameter::ShaderParameterType)std::distance(ShaderParameter::LiteralShaderParameterType.begin(), foundTypeIt);
	else
		return nullptr;


	switch (parameterType)
	{
	case ShaderParameter::ShaderParameterType::INT:
		return std::make_shared<ExternalShaderParameter<int, ShaderParameter::IsNotArray>>(name);
		break;
	case ShaderParameter::ShaderParameterType::INT2:
		return std::make_shared<ExternalShaderParameter<glm::ivec2, ShaderParameter::IsNotArray>>(name);
		break;
	case ShaderParameter::ShaderParameterType::INT3:
		return std::make_shared<ExternalShaderParameter<glm::ivec3, ShaderParameter::IsNotArray>>(name);
		break;
	case ShaderParameter::ShaderParameterType::FLOAT:
		return std::make_shared<ExternalShaderParameter<float, ShaderParameter::IsNotArray>>(name);
		break;
	case ShaderParameter::ShaderParameterType::FLOAT2:
		return std::make_shared<ExternalShaderParameter<glm::vec2, ShaderParameter::IsNotArray>>(name);
		break;
	case ShaderParameter::ShaderParameterType::FLOAT3:
		return std::make_shared<ExternalShaderParameter<glm::vec3, ShaderParameter::IsNotArray>>(name);
		break;
	case ShaderParameter::ShaderParameterType::TEXTURE:
		return std::make_shared<ExternalShaderParameter<Texture, ShaderParameter::IsNotArray>>(name);
		break;
	case ShaderParameter::ShaderParameterType::CUBE_TEXTURE:
		return std::make_shared<ExternalShaderParameter<CubeTexture, ShaderParameter::IsNotArray>>(name);
		break;
		//array versions :
	case ShaderParameter::ShaderParameterType::ARRAY_INT:
		return std::make_shared<ExternalShaderParameter<int, ShaderParameter::IsNotArray>>(name);
		break;
	case ShaderParameter::ShaderParameterType::ARRAY_INT2:
		return std::make_shared<ExternalShaderParameter<glm::ivec2, ShaderParameter::IsNotArray>>(name);
		break;
	case ShaderParameter::ShaderParameterType::ARRAY_INT3:
		return std::make_shared<ExternalShaderParameter<glm::ivec3, ShaderParameter::IsNotArray>>(name);
		break;
	case ShaderParameter::ShaderParameterType::ARRAY_FLOAT:
		return std::make_shared<ExternalShaderParameter<float, ShaderParameter::IsNotArray>>(name);
		break;
	case ShaderParameter::ShaderParameterType::ARRAY_FLOAT2:
		return std::make_shared<ExternalShaderParameter<glm::vec2, ShaderParameter::IsNotArray>>(name);
		break;
	case ShaderParameter::ShaderParameterType::ARRAY_FLOAT3:
		return std::make_shared<ExternalShaderParameter<glm::vec3, ShaderParameter::IsNotArray>>(name);
		break;
	default:
		return nullptr;
		break;
	}
}

InternalShaderParameter<Texture, ShaderParameter::IsNotArray>::InternalShaderParameter(const std::string& name)
	: InternalShaderParameterBase(name)
	, m_uniformId(0)
{}

//init unifom id
void InternalShaderParameter<Texture, ShaderParameter::IsNotArray>::init(GLuint glProgramId)
{
	m_uniformId = glGetUniformLocation(glProgramId, m_name.data());
}

void InternalShaderParameter<Texture, ShaderParameter::IsNotArray>::drawUI()
{
	EditorGUI::ResourceField<Texture>(m_name, m_data); //TODO 10
}

void InternalShaderParameter<Texture, ShaderParameter::IsNotArray>::pushToGPU(int& boundTextureCount)
{
	glActiveTexture(GL_TEXTURE0 + boundTextureCount);
	glBindTexture(GL_TEXTURE_2D, m_data->glId);
	glUniform1i(m_uniformId, boundTextureCount);
	boundTextureCount++;
}

void InternalShaderParameter<Texture, ShaderParameter::IsNotArray>::save(Json::Value & entityRoot) const
{
	m_data.save(entityRoot);
}

void InternalShaderParameter<Texture, ShaderParameter::IsNotArray>::load(const Json::Value & entityRoot)
{
	m_data.load(entityRoot);
}

void InternalShaderParameter<Texture, ShaderParameter::IsNotArray>::setData(const void* data)
{
	m_data = *(static_cast< const ResourcePtr<Texture> *>(data));
}

void InternalShaderParameter<Texture, ShaderParameter::IsNotArray>::getData(void* outData)
{
	outData = (void*)(m_data.get());
}