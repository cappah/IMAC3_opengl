#pragma once

#include <vector>
#include <map>
#include <sstream>

#include "glew/glew.h"

#include "glm/glm.hpp"
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4, glm::ivec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/type_ptr.hpp" // glm::value_ptr

#include "jsoncpp/json/json.h"


//for serialization : 

// TO JSON : 

template<typename T>
Json::Value& toJsonValue(const T& value)
{
	Json::Value serializedValue;
	value.save(serializedValue);
	return serializedValue;
}


template<typename T>
Json::Value& toJsonValue(const std::vector<T>& vector)
{
	Json::Value serializedValue;
	serializedValue["size"] = vector.size();
	for (int i = 0; i < vector.size(); i++)
	{
		serializedValue["data"][i] = toJsonValue<T>(vector[i]);
	}
	return serializedValue;
}

template<typename T, typename U>
Json::Value& toJsonValue(const std::map<T, U>& map)
{
	Json::Value serializedValue;
	serializedValue["size"] = map.size();
	int i = 0;
	for (auto it = map.begin(); it != map.end(); it++)
	{
		serializedValue["keys"][i] = toJsonValue<T>(it.first);
		serializedValue["values"][i] = toJsonValue<T>(it.second);
		i++;
	}
	return serializedValue;
}

// FROM JSON : 

template<typename T>
T fromJsonValue(Json::Value& value, const T& default)
{
	if (value.empty())
		return default;
	else
	{
		T loadedValue;
		loadedValue.load(value);
		return loadedValue;
	}
}


/*
template<typename T>
std::vector<T>& fromJsonValues_vector(Json::Value& value)
{
std::vector<T> loadedValues;
int size = value.get("size", 0).asInt();
for (int i = 0; i < size; i++)
{
loadedValues[i] = fromJsonValue<T>(value["data"][i]);
}
return loadedValues;
}

template<typename T, typename U>
std::map<T, U>& fromJsonValues_map(Json::Value& value)
{
std::map<T> loadedValues;
int size = value.get("size", 0).asInt();
for (int i = 0; i < size; i++)
{
std::pair<T, U> newPair;
newPair.first = fromJsonValue<T>(value["keys"][i]);
newPair.second = fromJsonValue<T>(value["values"][i]);
loadedValues.insert(newPair);
}
return loadedValues;
}
*/

