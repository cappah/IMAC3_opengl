#pragma once

#include <string>

#include "Reflexion.h"

class Object
{
public:
	virtual int getClassId() const = 0;
	virtual std::string getClassName() const = 0;
	virtual bool isA(const Object& other) const = 0;
	virtual bool isA(int otherClassId) const = 0;
	virtual bool inheritFrom(const Object& other) const = 0;
	virtual bool inheritFrom(int otherClassId) const = 0;

	template<typename T>
	bool inheritFrom() const
	{
		int otherClassId = Reflexion<T>::instance().getClassId();
		return inheritFrom(otherClassId);
	}

	template<typename T>
	bool isA(int otherClassId) const
	{
		int otherClassId = Reflexion<T>::instance().getClassId();
		return isA(otherClassId);
	}
};