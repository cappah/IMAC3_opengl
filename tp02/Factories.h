#pragma once

#include <map>

#include "stb/stb_image.h"

#include "Mesh.h"
#include "Materials.h"
#include "Texture.h"
#include "SkeletalAnimation.h"
#include "ShaderProgram.h"

#include "FileHandler.h"
#include "ISerializable.h"
#include "ISingleton.h"
#include "ResourcePointer.h"


template<typename T>
class ResourceFactory : public ISingleton<ResourceFactory<T>>, public ISerializable
{
private:
	std::map<FileHandler::CompletePath, unsigned int> m_resourceMapping;
	std::map<std::string, unsigned int> m_defaultResourceMapping;

	std::map<unsigned int, T*> m_resourcesFromHashKey;
	std::map<unsigned int, T*> m_defaultResourcesFromHashKey;

	std::map<FileHandler::CompletePath, T*> m_resources;
	std::map<std::string, T*> m_defaultResources;

public:
	ResourceFactory();

	void add(const FileHandler::CompletePath& path);
	template<typename U>
	void add(const FileHandler::CompletePath& path);
	void add(const FileHandler::CompletePath& path, T* value);
	void erase(const FileHandler::CompletePath& path);
	ResourcePtr<T> get(const FileHandler::CompletePath& path);
	bool contains(const FileHandler::CompletePath& path);
	T* getRaw(unsigned int hashKey);
	bool contains(unsigned int hashKey);
	unsigned int getHashKeyForResource(const FileHandler::CompletePath& path) const;


	void initDefaults();
	void addDefault(const std::string& name, T* resource);
	ResourcePtr<T> getDefault(const std::string& name);
	bool containsDefault(const std::string& name);
	T* getRawDefault(unsigned int hashKey);
	bool containsDefault(unsigned int hashKey);
	unsigned int getHashKeyForDefaultResource(const std::string& name) const;

	void clear();

	virtual void save(Json::Value & entityRoot) const override;
	virtual void load(Json::Value & entityRoot) override;

private:
	void add(const FileHandler::CompletePath& path, unsigned int hashKey);

};


template<typename T>
ResourceFactory<T>::ResourceFactory()
{

}

//NOT DEFAULTS
template<typename T>
void ResourceFactory<T>::add(const FileHandler::CompletePath& path, T* value)
{
	m_resources[path] = value;
	m_resourceMapping[path] = ++s_resourceCount;
	m_resourcesFromHashKey[s_resourceCount] = value;
}


template<typename T>
void ResourceFactory<T>::add(const FileHandler::CompletePath& path)
{
	T* newResource = new T(path);

	m_resources[path] = newResource;
	m_resourceMapping[path] = ++s_resourceCount;
	m_resourcesFromHashKey[s_resourceCount] = newResource;
}

template<typename T>
template<typename U>
void ResourceFactory<T>::add(const FileHandler::CompletePath& path)
{
	T* newResource = static_cast<T>(new U(path));

	m_resources[path] = newResource;
	m_resourceMapping[path] = ++s_resourceCount;
	m_resourcesFromHashKey[s_resourceCount] = newResource;
}

template<typename T>
void ResourceFactory<T>::erase(const FileHandler::CompletePath& path)
{
	if (m_resources.find(path) == m_resources.end())
		return;

	unsigned int resourceHashKey = m_resourceMapping[path];
	delete m_resources[path];

	m_resources.erase(path);
	m_resourceMapping.erase(path);
	m_resourcesFromHashKey.erase(resourceHashKey);
}

template<typename T>
void ResourceFactory<T>::add(const FileHandler::CompletePath& path, unsigned int hashKey)
{
	T* newResource = new T();
	newResource->init(path);

	m_resources[path] = newResource;
	m_resourceMapping[path] = hashKey;
	m_resourcesFromHashKey[hashKey] = newResource;
}

template<typename T>
unsigned int ResourceFactory<T>::getHashKeyForResource(const FileHandler::CompletePath& path) const
{
	return m_resourceMapping.at(path);
}

template<typename T>
ResourcePtr<T> ResourceFactory<T>::get(const FileHandler::CompletePath& path)
{
	return ResourcePtr<T>(m_resources[path], m_resourceMapping[path], false);
}

template<typename T>
T* ResourceFactory<T>::getRaw(unsigned int hashKey)
{
	return m_resourcesFromHashKey[hashKey];
}

template<typename T>
bool ResourceFactory<T>::contains(const FileHandler::CompletePath& path)
{
	return m_resources.find(path) != m_resources.end();
}

template<typename T>
bool ResourceFactory<T>::contains(unsigned int hashKey)
{
	return m_resourceFromHashKey.find(hashKey) != m_resources.end();
}

//DEFAULTS
template<typename T>
void ResourceFactory<T>::initDefaults()
{
	//nothing
}

template<typename T>
void ResourceFactory<T>::addDefault(const std::string& name, T* resource)
{
	m_defaultResources[name] = resource;
	m_defaultResourceMapping[name] = ++s_resourceCount;
	m_defaultResourcesFromHashKey[s_resourceCount] = resource;
}


template<typename T>
unsigned int ResourceFactory<T>::getHashKeyForDefaultResource(const std::string& name) const
{
	return m_defaultResourceMapping[name];
}

template<typename T>
ResourcePtr<T> ResourceFactory<T>::getDefault(const std::string& name)
{
	return ResourcePtr<T>(m_defaultResources[name], m_defaultResourceMapping[name], true);
}

template<typename T>
T* ResourceFactory<T>::getRawDefault(unsigned int hashKey)
{
	return m_defaultResourcesFromHashKey[hashKey];
}

template<typename T>
bool ResourceFactory<T>::containsDefault(const std::string& name)
{
	return m_defaultResources.find(name) != m_defaultResources.end();
}

template<typename T>
bool ResourceFactory<T>::containsDefault(unsigned int hashKey)
{
	return m_defaultResourcesFromHashKey.find(hashKey) != m_resources.end();
}

//ALL
template<typename T>
void ResourceFactory<T>::clear()
{
	for (auto& it = m_defaultResources.begin(); it != m_defaultResources.end(); it++)
	{
		T* resource = it->second;
		delete resource;
	}
	for (auto& it = m_resources.begin(); it != m_resources.end(); it++)
	{
		T* resource = it->second;
		delete resource;
	}

	m_resources.clear();
	m_defaultResources.clear();
}

template<typename T>
void ResourceFactory<T>::save(Json::Value & entityRoot) const
{
	entityRoot["size"] = m_resources.size();
	int i = 0;
	for (auto it = m_resources.begin(); it != m_resources.end(); it++)
	{
		entityRoot[i]["path"] = it->first.toString();
		entityRoot[i]["hashKey"] = getHashKeyForResource(it->first.toString());
		i++;
	}
}

template<typename T>
void ResourceFactory<T>::load(Json::Value & entityRoot)
{
	unsigned int resourceCount = s_resourceCount;

	int size = entityRoot.get("size", 0).asInt();
	for (int i = 0; i < size; i++)
	{
		std::string resourcePath = entityRoot[i]["path"].asString();
		unsigned int resourceHashKey = entityRoot[i]["hashKey"].asUInt();
		if (resourcePath != "")
			add(FileHandler::CompletePath(resourcePath), resourceHashKey);

		if (resourceCount < resourceHashKey )
			s_resourceCount = resourceHashKey;
	}
}

//Specialisations : 

template<>
void ResourceFactory<Material>::add(const FileHandler::CompletePath& path, unsigned int hashKey);

//Initialisations : 

//Shader Programes
template<>
void ResourceFactory<ShaderProgram>::initDefaults();

//Cube Texture
template<>
void ResourceFactory<CubeTexture>::initDefaults();

//Textures
template<>
void ResourceFactory<Texture>::initDefaults();

//Materials
template<>
void ResourceFactory<Material>::initDefaults();

//Mesh
template<>
void ResourceFactory<Mesh>::initDefaults();

//Animations : nothing by default

//Init helper
void initAllResourceFactories();
void clearAllResourceFactories();

//Access helper
ResourceFactory<ShaderProgram>& getProgramFactory();
ResourceFactory<Mesh>& getMeshFactory();

ResourceFactory<Texture>& getTextureFactory();

ResourceFactory<CubeTexture>& getCubeTextureFactory();
ResourceFactory<Material>& getMaterialFactory();
ResourceFactory<SkeletalAnimation>& getSkeletalAnimationFactory();

template<typename T>
ResourceFactory<T>& getResourceFactory()
{
	return ResourceFactory<T>::instance();
}

template<typename T>
ResourceType getResourceType()
{
	assert(false, "resource not implementaed");
	return ResourceType::NONE;
}

template<>
ResourceType getResourceType<Texture>();
template<>
ResourceType getResourceType<CubeTexture>();

template<>
ResourceType getResourceType<SkeletalAnimation>();
template<>
ResourceType getResourceType<Material>();
template<>
ResourceType getResourceType<ShaderProgram>();


//
//
//class ProgramFactory : public ISerializable
//{
//
//private:
//	std::map<std::string, GLuint> m_programs;
//	std::map<std::string, GLuint> m_defaults;
//
//public:
//	void add(const std::string& name, GLuint programId);
//	GLuint get(const std::string& name);
//	bool contains(const std::string& name);
//	void drawUI();
//	void clear();
//	//std::map<std::string, GLuint>::iterator begin() { return m_programs.begin(); }
//	//std::map<std::string, GLuint>::iterator end() { return m_programs.end(); };
//
//	virtual void save(Json::Value & entityRoot) const override;
//	virtual void load(Json::Value & entityRoot) override;
//
//
//	// singleton implementation :
//private:
//	ProgramFactory();
//
//public:
//	inline static ProgramFactory& get()
//	{
//		static ProgramFactory instance;
//
//		return instance;
//	}
//
//
//	ProgramFactory(const ProgramFactory& other) = delete;
//	void operator=(const ProgramFactory& other) = delete;
//
//};
//
//
////////////////////////////////////////
//
//class TextureFactory : public ISerializable
//{
//
//private:
//	std::map<std::string, Texture*> m_textures;
//
//	//for UI : 
//	char name[20];
//	char path[50];
//
//public:
//	void add(const std::string& name, const std::string& path);
//	void add(const std::string& name, Texture* texture);
//	Texture* get(const std::string& name);
//	bool contains(const std::string& name);
//	void drawUI();
//	void clear();
//	//std::map<std::string, Texture*>::iterator begin() { return m_textures.begin(); }
//	//std::map<std::string, Texture*>::iterator end() { return m_textures.end(); };
//
//	virtual void save(Json::Value & entityRoot) const override;
//	virtual void load(Json::Value & entityRoot) override;
//
//	// singleton implementation :
//private:
//	TextureFactory();
//
//public:
//	inline static TextureFactory& get()
//	{
//		static TextureFactory instance;
//
//		return instance;
//	}
//
//
//	TextureFactory(const TextureFactory& other) = delete;
//	void operator=(const TextureFactory& other) = delete;
//};
//
/////////////////////////////////
//
//
//class CubeTextureFactory : public ISerializable
//{
//
//private:
//	std::map<std::string, CubeTexture*> m_textures;
//
//	//for UI : 
//	char name[20];
//	char paths[6][50];
//
//public:
//	void add(const std::string& name, const std::vector<std::string>& paths);
//	void add(const std::string& name, CubeTexture* textureId);
//	CubeTexture* get(const std::string& name);
//	bool contains(const std::string& name);
//	void drawUI();
//	void clear();
//	//std::map<std::string, CubeTexture*>::iterator begin() { return m_textures.begin(); }
//	//std::map<std::string, CubeTexture*>::iterator end() { return m_textures.end(); };
//
//	virtual void save(Json::Value & entityRoot) const override;
//	virtual void load(Json::Value & entityRoot) override;
//
//	// singleton implementation :
//private:
//	CubeTextureFactory();
//
//public:
//	inline static CubeTextureFactory& get()
//	{
//		static CubeTextureFactory instance;
//
//		return instance;
//	}
//
//
//	CubeTextureFactory(const CubeTextureFactory& other) = delete;
//	void operator=(const CubeTextureFactory& other) = delete;
//};
//
//
//
/////////////////////////////////
//
//class MeshFactory : public ISerializable
//{
//
//private:
//	std::map<std::string, Mesh*> m_meshes;
//
//	//for UI : 
//	char name[20];
//	char path[50];
//
//	std::vector<std::string> m_defaults;
//
//public:
//	void add(const std::string& name, Mesh* mesh);
//	void add(const std::string& name, const std::string& path);
//	Mesh* get(const std::string& name);
//	bool contains(const std::string& name);
//	void drawUI();
//	void clear();
//	//std::map<std::string, Mesh*>::iterator begin() { return m_meshes.begin(); }
//	//std::map<std::string, Mesh*>::iterator end() { return m_meshes.end(); };
//
//	virtual void save(Json::Value & entityRoot) const override;
//	virtual void load(Json::Value & entityRoot) override;
//
//	// singleton implementation :
//private:
//	MeshFactory();
//
//public:
//	inline static MeshFactory& get()
//	{
//		static MeshFactory instance;
//
//		return instance;
//	}
//
//
//	MeshFactory(const MeshFactory& other) = delete;
//	void operator=(const MeshFactory& other) = delete;
//};
//
//
//
//
/////////////////////////////////
//
//class SkeletalAnimationFactory : public ISerializable
//{
//
//private:
//	std::map<std::string, std::map<std::string, SkeletalAnimation*>> m_animations;
//
//	//for UI : 
//	char name[20];
//	char path[50];
//
//public:
//	void add(const std::string& meshName, const std::string& animationName, SkeletalAnimation* animation);
//	SkeletalAnimation* get(const std::string& meshName, const std::string& animationName);
//	bool contains(const std::string& meshName, const std::string& animationName);
//	void drawUI();
//	std::map<std::string, std::map<std::string, SkeletalAnimation*>>::iterator clear(const std::string& meshName);
//	void clear();
//	//TODO : begin() et end()
//
//	virtual void save(Json::Value & entityRoot) const override;
//	virtual void load(Json::Value & entityRoot) override;
//
//	// singleton implementation :
//private:
//	SkeletalAnimationFactory();
//
//public:
//	inline static SkeletalAnimationFactory& get()
//	{
//		static SkeletalAnimationFactory instance;
//
//		return instance;
//	}
//
//	SkeletalAnimationFactory(const SkeletalAnimationFactory& other) = delete;
//	void operator=(const SkeletalAnimationFactory& other) = delete;
//};
//
//////////////////////////////////
//
//class MaterialFactory : public ISerializable
//{
//
//private:
//	std::map<std::string, Material*> m_materials;
//
//	//for UI : 
//	char name[20];
//
//	std::vector<std::string> m_defaults;
//
//public:
//	void add(const std::string& name, Material* material);
//	template<typename T>
//	T* get(const std::string& name);
//	template<typename T>
//	bool contains(const std::string& name);
//	void drawUI();
//	void clear();
//	//std::map<std::string, Material*>::iterator begin() { return m_materials.begin(); }
//	//std::map<std::string, Material*>::iterator end() { return m_materials.end(); };
//
//	virtual void save(Json::Value & entityRoot) const override;
//	virtual void load(Json::Value & entityRoot) override;
//
//	// singleton implementation :
//private:
//	MaterialFactory();
//
//public:
//	inline static MaterialFactory& get()
//	{
//		static MaterialFactory instance;
//
//		return instance;
//	}
//
//
//	MaterialFactory(const MaterialFactory& other) = delete;
//	void operator=(const MaterialFactory& other) = delete;
//};
//
//template<typename T>
//T* MaterialFactory::get(const std::string& name)
//{
//	assert(m_materials.find(name) != m_materials.end());
//
//	assert(dynamic_cast<T*>(m_materials[name]) == m_materials[name]);
//	return static_cast<T*>(m_materials[name]);
//}
//
//template<typename T>
//bool MaterialFactory::contains(const std::string& name)
//{
//	auto findResult = m_materials.find(name);
//	return ( findResult != m_materials.end() && dynamic_cast<T*>(findResult->second));
//}
//
