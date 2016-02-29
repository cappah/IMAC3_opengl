#pragma once

#include "glew/glew.h"

#include "glm/glm.hpp"
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4, glm::ivec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/type_ptr.hpp" // glm::value_ptr

#include <vector>
#include <map>
#include <sstream>

#include "Camera.h"

namespace patch
{
	template < typename T > std::string to_string(const T& n)
	{
		std::ostringstream stm;
		stm << n;
		return stm.str();
	}
}

// Shader utils
int check_link_error(GLuint program);
int check_compile_error(GLuint shader, const char ** sourceBuffer);
GLuint compile_shader(GLenum shaderType, const char * sourceBuffer, int bufferSize);
GLuint compile_shader_from_file(GLenum shaderType, const char * fileName);

// OpenGL utils
bool checkError(const char* title);

//return the point in world coordinate matching the given mouse position in screen space : 
glm::vec3 screenToWorld(float mouse_x, float mouse_y, int width, int height, BaseCamera& camera);

double interpolation_cos2D(double a, double b, double c, double d, double x, double y);
double interpolation_cos1D(double a, double b, double x);

glm::vec3 vertexFrom3Floats(const std::vector<float>& vertices, int indice);

//array :
int idx2DToIdx1D(int i, int j, int array2DWidth);

//physic : 
bool rayOBBoxIntersect(glm::vec3 Start, glm::vec3 Dir, glm::vec3 P, glm::vec3 H[3], glm::vec3 E, float* t);
bool raySlabIntersect(float start, float dir, float min, float max, float* tfirst, float* tlast);

//for serialization : 

template<typename T>
Json::Value& toJsonValue(const T& value)
{
	Json::Value serializedValue;
	value.save(serializedValue);
	return serializedValue;
}

template<>
Json::Value& toJsonValue(const glm::vec2& vec)
{
	return Json::Value(Json::arrayValue).append(vec.x).append(vec.y);
}

template<>
Json::Value& toJsonValue(const glm::vec3& vec)
{
	return Json::Value(Json::arrayValue).append(vec.x).append(vec.y).append(vec.z);
}

template<>
Json::Value& toJsonValue(const glm::vec4& vec)
{
	return Json::Value(Json::arrayValue).append(vec.x).append(vec.y).append(vec.z).append(vec.w);
}

template<>
Json::Value& toJsonValue(const glm::quat& quat)
{
	return Json::Value(Json::arrayValue).append(quat.x).append(quat.y).append(quat.z).append(quat.w);
}

template<>
Json::Value& toJsonValue(const glm::mat3& mat)
{
	Json::Value formatedValue(Json::arrayValue);
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			formatedValue.append(mat[i][j]);
		}
	}
	return formatedValue;
}

template<>
Json::Value& toJsonValue(const glm::mat4& mat)
{
	Json::Value formatedValue(Json::arrayValue);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			formatedValue.append(mat[i][j]);
		}
	}
	return formatedValue;
}

template<typename T>
Json::Value& toJsonValue(const std::vector<T>& vector)
{
	Json::Value serializedValue;
	for (int i = 0; i < vector.size(); i++)
	{
		serializedValue[i] = toJsonValue<T>(vector[i]);
	}
	return serializedValue;
}

template<typename T>
Json::Value& toJsonValue(const std::map<T>& map)
{
	Json::Value serializedValue;
	for (auto it = map.begin(); it != map.end(); it++)
	{
		serializedValue[it.first] = toJsonValue<T>(it.second);
	}
	return serializedValue;
}

template<typename T = glm::quat>
T& fromJsonValue(const Json::Value& value, const T& default = T())
{
	if (value.empty)
		return default;
	else
		return glm::quat(value[3].asFloat(), value[0].asFloat(), value[1].asFloat(), value[2].asFloat());
}

template<typename T = glm::vec2>
T& fromJsonValue(const Json::Value& value, const T& default = T())
{
	if (value.empty())
		return default;
	else
		return glm::vec3(value[0].asFloat(), value[1].asFloat());
}

template<typename T = glm::vec3>
T& fromJsonValue(const Json::Value& value, const T& default = T())
{
	if (value.empty())
		return default;
	else
		return glm::vec3(value[0].asFloat(), value[1].asFloat(), value[2].asFloat());
}

template<typename T = glm::vec4>
T& fromJsonValue(const Json::Value& value, const T& default = T())
{
	if (value.empty())
		return default;
	else
		return glm::vec4(value[0].asFloat(), value[1].asFloat(), value[2].asFloat(), value[3].asFloat());
}

template<typename T = glm::mat3>
T& fromJsonValue(const Json::Value& value, const T& default = T())
{
	if (value.empty())
		return default;
	else
	{
		glm::mat3 matrix;
		for (int i = 0, k = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++, k++)
			{
				matrix[i][j] = value[k].asFloat():
			}
		}
		return matrix;
	}
}

template<typename T = glm::mat4>
T& fromJsonValue(const Json::Value& value, const T& default = T())
{
	if (value.empty())
		return default;
	else
	{
		glm::mat3 matrix;
		for (int i = 0, k = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++, k++)
			{
				matrix[i][j] = value[k].asFloat() :
			}
		}
		return matrix;
	}
}

