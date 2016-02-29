#pragma once

#include "jsoncpp/json/json.h"

class ISerializable {

	virtual void save(Json::Value& entityRoot) = 0;
	virtual void load(Json::Value& entityRoot) = 0;

};