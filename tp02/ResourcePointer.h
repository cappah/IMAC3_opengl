#pragma once

#include "Utils.h"

#include "jsoncpp/json/json.h"
#include "ISingleton.h"

enum ResourceType
{
	NONE,
	PROGRAME,
	TEXTURE,
	CUBE_TEXTURE,
	MESH,
	SKELETAL_ANIMATION,
	MATERIAL,
};

static unsigned int s_resourceCount = 0;

class ResourceManager : public ISingleton<ResourceManager>
{ 
	template<typename T>
	void addResource()
	{

	}

	template<typename T>
	bool removeResource()
	{

	}

public:
	SINGLETON_IMPL(ResourceManager);
};

template<typename T>
class ResourcePtr : public ISerializable
{
	template<typename U>
	friend class ResourcePtr;

private:
	T* m_rawPtr;
	unsigned int m_resourceHashKey;
	bool m_isDefaultResource;
public:
	ResourcePtr(const Json::Value & entityRoot)
		: m_isDefaultResource(false)
		, m_rawPtr(nullptr)
		, m_resourceHashKey(0)
	{
		load(entityRoot);
	}

	ResourcePtr(T* ptr = nullptr, unsigned int hashKey = 0, bool isDefault = false)
		: m_isDefaultResource(isDefault)
		, m_resourceHashKey(hashKey)
		, m_rawPtr(ptr)
	{
	}

	template<typename U>
	ResourcePtr(const ResourcePtr<U>& other)
	{
		m_rawPtr = static_cast<T*>(other.m_rawPtr);
		m_resourceHashKey = other.m_resourceHashKey;
		m_isDefaultResource = other.m_isDefaultResource;
	}

	T* operator->() const
	{
		return m_rawPtr;
	}

	T& operator*()
	{
		return *m_rawPtr;
	}

	T* get()
	{
		return m_rawPtr;
	}

	const T* get() const
	{
		return m_rawPtr;
	}

	bool isValid() const
	{
		return m_rawPtr != nullptr && m_resourceHashKey != 0;
	}

	void reset()
	{
		m_isDefaultResource = true;
		m_resourceHashKey = 0;
		m_rawPtr = nullptr;
	}

	virtual void save(Json::Value & entityRoot) const override
	{
		entityRoot["isDefaultResource"] = m_isDefaultResource;
		entityRoot["resourceHashKey"] = m_resourceHashKey;
	}

	virtual void load(const Json::Value & entityRoot) override;

};

//#include "ResourcePtr.inl"