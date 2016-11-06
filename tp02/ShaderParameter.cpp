#include <memory>
#include "ShaderParameters.h"


namespace GlHelper {

template<>
inline void pushParameterToGPU<int>(GLuint uniformId, const int& value)
{
	glUniform1i(uniformId, value);
}

template<>
inline void pushParameterToGPU<float>(GLuint uniformId, const float& value)
{
	glUniform1f(uniformId, value);
}

template<>
inline void pushParameterToGPU<glm::vec2>(GLuint uniformId, const glm::vec2& value)
{
	glUniform2f(uniformId, value.x, value.y);
}

template<>
inline void pushParameterToGPU<glm::ivec2>(GLuint uniformId, const glm::ivec2& value)
{
	glUniform2i(uniformId, value.x, value.y);
}

template<>
inline void pushParameterToGPU<glm::vec3>(GLuint uniformId, const glm::vec3& value)
{
	glUniform3f(uniformId, value.x, value.y, value.z);
}

template<>
inline void pushParameterToGPU<glm::ivec3>(GLuint uniformId, const glm::ivec3& value)
{
	glUniform3i(uniformId, value.x, value.y, value.z);
}

template<>
inline void pushParameterToGPU<glm::mat4>(GLuint uniformId, const glm::mat4& value)
{
	glUniformMatrix4fv(uniformId, 1, false, glm::value_ptr(value));
}

//array version (experimental) : 

template<>
inline void pushParametersToGPU<float>(GLuint uniformId, int count, const std::vector<float>& values)
{
	glUniform1fv(uniformId, count, &values[0]);
}

//TODO : complete

}