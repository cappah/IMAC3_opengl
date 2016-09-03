#pragma once

//#include "Factories.h"

template<typename T>
void ResourcePtr<T>::load(Json::Value & entityRoot)
{
	m_isDefaultResource = entityRoot["isDefaultResource"].asBool();
	m_resourceHashKey = entityRoot["resourceHashKey"].asUInt();
	m_rawPtr = nullptr; //m_isDefaultResource ? getResourceFactory<T>().getRawDefault(m_resourceHashKey) : getResourceFactory<T>().getRaw(m_resourceHashKey); // TODO // %NOCOMMIT%
}
