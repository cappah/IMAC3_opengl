#pragma once

#include "jsoncpp/json/json.h"
#include "SerializeUtils.h"

class ISerializable {

	virtual void save(Json::Value& entityRoot) const = 0;
	virtual void load(Json::Value& entityRoot) = 0;

};