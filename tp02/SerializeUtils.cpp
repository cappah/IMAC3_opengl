#include "SerializeUtils.h"

// TO JSON : 

template<>
Json::Value& toJsonValue<float>(const float& value)
{
	return Json::Value(value);
}

template<>
Json::Value& toJsonValue<int>(const int& value)
{
	return Json::Value(value);
}

template<>
Json::Value& toJsonValue<std::string>(const std::string& value)
{
	return Json::Value(value);
}

template<>
Json::Value& toJsonValue<bool>(const bool& value)
{
	return Json::Value(value);
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

// FROM JSON

template<>
float fromJsonValue<float>(Json::Value& value, const float& default)
{
	return value.asFloat();
}

template<>
int fromJsonValue<int>(Json::Value& value, const int& default)
{
	return value.asInt();
}

template<>
bool fromJsonValue<bool>(Json::Value& value, const bool& default)
{
	return value.asBool();
}

template<>
std::string fromJsonValue<std::string>(Json::Value& value, const std::string& default)
{
	return value.asString();
}

template<>
glm::quat fromJsonValue(Json::Value& value, const glm::quat& default)
{
	if (value.empty())
		return default;
	else
		return glm::quat(value[3].asFloat(), value[0].asFloat(), value[1].asFloat(), value[2].asFloat());
}

template<>
glm::vec2 fromJsonValue(Json::Value& value, const glm::vec2& default)
{
	if (value.empty())
		return default;
	else
		return glm::vec2(value[0].asFloat(), value[1].asFloat());
}

template<>
glm::vec3 fromJsonValue(Json::Value& value, const glm::vec3& default)
{
	if (value.empty())
		return default;
	else
		return glm::vec3(value[0].asFloat(), value[1].asFloat(), value[2].asFloat());
}

template<>
glm::vec4 fromJsonValue(Json::Value& value, const glm::vec4& default)
{
	if (value.empty())
		return default;
	else
		return glm::vec4(value[0].asFloat(), value[1].asFloat(), value[2].asFloat(), value[3].asFloat());
}

template<>
glm::mat3 fromJsonValue(Json::Value& value, const glm::mat3& default)
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
				matrix[i][j] = value[k].asFloat();
			}
		}
		return matrix;
	}
}

template<>
glm::mat4 fromJsonValue(Json::Value& value, const glm::mat4& default)
{
	if (value.empty())
		return default;
	else
	{
		glm::mat4 matrix;
		for (int i = 0, k = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++, k++)
			{
				matrix[i][j] = value[k].asFloat();
			}
		}
		return matrix;
	}
}

