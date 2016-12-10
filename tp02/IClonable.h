#pragma once

#include <memory>


////////////////////////////////
// Interface for clonable object. 
// Clone methode allows things like : 
// Titi* titi = new Toto();
// Toto* toto = titi->clone();
// IClonableWithName is the same as IClonable, except that it allow the cloned object to have a constructor taking a string.
////////////////////////////////

template<typename T>
class IClonable
{
public:
	virtual T* clone() = 0;
	virtual std::shared_ptr<T> cloneShared() = 0;
};

#define CLONABLE_IMPL(BaseType, Type)\
	virtual BaseType* clone() override\
	{\
		auto cloned = new Type();\
		return cloned;\
	}\
	virtual std::shared_ptr<BaseType> cloneShared() override\
	{\
		auto cloned = std::make_shared<Type>();\
		return cloned;\
	}

template<typename T>
class IClonableWithName
{
public:
	virtual T* clone(const std::string& name) = 0;
	virtual std::shared_ptr<T> cloneShared(const std::string& name) = 0;
};

#define CLONABLE_WITH_NAME_IMPL(BaseType, Type)\
	virtual BaseType* clone(const std::string& name) override\
	{\
		return new Type(name);\
	}\
	virtual std::shared_ptr<BaseType> cloneShared(const std::string& name) override\
	{\
		return std::make_shared<Type>(name);\
	}
	