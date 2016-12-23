#pragma once

#include <map>

#include "stb/stb_image.h"

#include "Mesh.h"
#include "Materials.h"
#include "RenderBatch.h"
#include "Texture.h"
#include "SkeletalAnimation.h"
#include "ShaderProgram.h"

#include "FileHandler.h"
#include "ISerializable.h"
#include "ISingleton.h"
#include "ResourcePointer.h"
#include "Project.h"

#include "IDGenerator.h"

class ResourceFactoryBase
{
protected:
	void setResourceName(Resource* resource, const std::string& name)
	{
		resource->setName(name);
	}
	void setResourceID(Resource* resource, ID id)
	{
		resource->setResourceID(id);
	}
};

template<typename T>
class ResourceFactory : public ISingleton<ResourceFactory<T>>, public ISerializable, public ResourceFactoryBase
{
private:
	std::map<FileHandler::CompletePath, ID> m_resourceMapping;
	std::map<std::string, ID> m_defaultResourceMapping;

	std::map<ID, T*> m_resourcesFromHashKey;
	std::map<ID, T*> m_defaultResourcesFromHashKey;

	std::map<FileHandler::CompletePath, T*> m_resources;
	std::map<std::string, T*> m_defaultResources;

public:
	ResourceFactory();

	//create the nex resource at given path. tell the resource tree to effectivly add a new file
	//void createNewResource(const FileHandler::CompletePath& path);
	//add a resource which have already been created/loaded by the resource tree
	void addResourceSoft(const FileHandler::CompletePath& path);
	template<typename U>
	void addResourceSoft(const FileHandler::CompletePath& path);
	void addResourceForce(const FileHandler::CompletePath& path, T* value);
	void erase(const FileHandler::CompletePath& path);
	T* get(const FileHandler::CompletePath& path);
	bool contains(const FileHandler::CompletePath& path);
	T* getRaw(const ID& hashKey);
	bool contains(const ID& hashKey);
	ID getHashKeyForResource(const FileHandler::CompletePath& path) const;
	void changeResourceKey(const FileHandler::CompletePath& oldKey, const FileHandler::CompletePath& newKey);


	void initDefaults();
	void addDefaultResource(const std::string& name, T* resource);
	T* getDefault(const std::string& name);
	bool containsDefault(const std::string& name);
	T* getRawDefault(const ID& hashKey);
	bool containsDefault(const ID& hashKey);
	ID getHashKeyForDefaultResource(const std::string& name) const;

	void clear();

	virtual void save(Json::Value & entityRoot) const override;
	virtual void load(const Json::Value & entityRoot) override;

	typename std::map<FileHandler::CompletePath, T*>::iterator resourceBegin();
	typename std::map<std::string, T*>::iterator defaultResourceBegin();
	typename std::map<FileHandler::CompletePath, T*>::iterator resourceEnd();
	typename std::map<std::string, T*>::iterator defaultResourceEnd();

	SINGLETON_IMPL(ResourceFactory);

private:
	void addResourceForce(const FileHandler::CompletePath& path, const ID& hashKey);

};


//Specialization for ShaderProgram
template<>
class ResourceFactory<ShaderProgram> : public ISingleton<ResourceFactory<ShaderProgram>>, public ISerializable
{
private:
	std::map<std::string, ID> m_resourceMapping;
	std::map<ID, ShaderProgram*> m_resourcesFromHashKey;
	std::map<std::string, ShaderProgram*> m_resources;

public:

	ResourceFactory()
	{}

	//create the nex resource at given path. tell the resource tree to effectivly add a new file
	//void createNewResource(const FileHandler::CompletePath& path);
	//add a resource which have already been created/loaded by the resource tree
	void addResourceSoft(const FileHandler::CompletePath& path)
	{
		std::string name = path.getFilename();

		if (m_resources.find(name) != m_resources.end())
			return;

		ID newID = IDGenerator<Resource>::instance().lockID();
		ShaderProgram* newResource = new ShaderProgram(path);
		newResource->init(path, newID);

		m_resources[name] = newResource;
		m_resourceMapping[name] = newID;
		m_resourcesFromHashKey[newID] = newResource;
		//m_resourceMapping[name] = ++s_resourceCount;
		//m_resourcesFromHashKey[s_resourceCount] = newResource;
		
	}
	void addResourceForce(const std::string& name, ShaderProgram* value)
	{
		m_resources[name] = value;
		ID newID = IDGenerator<Resource>::instance().lockID();
		m_resourceMapping[name] = newID;
		m_resourcesFromHashKey[newID] = value;
	}
	void erase(const std::string& name)
	{
		if (m_resources.find(name) == m_resources.end())
			return;

		ID resourceID = m_resourceMapping[name];
		delete m_resources[name];

		m_resources.erase(name);
		m_resourceMapping.erase(name);
		m_resourcesFromHashKey.erase(resourceID);
	}
	ShaderProgram* get(const std::string& name)
	{
		return m_resources[name];
	}
	bool contains(const std::string& name)
	{
		return m_resources.find(name) != m_resources.end();
	}
	ShaderProgram* getRaw(const ID& hashKey)
	{
		return m_resourcesFromHashKey[hashKey];
	}
	bool contains(const ID& hashKey)
	{
		return m_resourcesFromHashKey.find(hashKey) != m_resourcesFromHashKey.end();
	}
	ID getHashKeyForResource(const std::string& name) const
	{
		return m_resourceMapping.at(name);
	}

	//load all programs which are in "[projectPath]/shaders/"
	void initDefaults()
	{
		FileHandler::Path shadersPath = FileHandler::Path(Project::getShaderFolderPath());
		loadAllPrograms(shadersPath, FileHandler::Path(shadersPath.back()));
	}

	//add all programs recursivly to the ShaderProgram factory 
	//void loadAllPrograms(const FileHandler::Path& shaderFolderPath)
	//{
	//	std::vector<std::string> dirNames;
	//	FileHandler::getAllDirNames(shaderFolderPath, dirNames);

	//	for (auto& dirName : dirNames)
	//	{
	//		loadAllPrograms(FileHandler::Path(shaderFolderPath.toString() + "/" + dirName), FileHandler::Path(dirName));
	//	}
	//}
	void loadAllPrograms(const FileHandler::Path& absolutePath, const FileHandler::Path& relativePath)
	{
		std::vector<std::string> dirNames;
		FileHandler::getAllDirNames(absolutePath, dirNames);

		for (auto& dirName : dirNames)
		{
			loadAllPrograms(FileHandler::Path(absolutePath.toString() + "/" + dirName), FileHandler::Path(relativePath.toString() + "/" + dirName) );
		}

		std::vector<std::string> fileNames;
		FileHandler::getAllFileNames(absolutePath, fileNames);
		std::string outExtention;

		for (auto& fileNameAndExtention : fileNames)
		{
			//We only add files that engine understand
			FileHandler::getExtentionFromExtendedFilename(fileNameAndExtention, outExtention);
			if (FileHandler::getFileTypeFromExtention(outExtention) == FileHandler::FileType::SHADER_PROGRAM)
			{
				FileHandler::CompletePath shaderPath(relativePath.toString() + "/" + fileNameAndExtention);
				addResourceSoft(shaderPath);
			}
		}
	}
	void clear()
	{
		for (auto& it = m_resources.begin(); it != m_resources.end(); it++)
		{
			ShaderProgram* resource = it->second;
			delete resource;
		}

		m_resources.clear();
	}

	virtual void save(Json::Value & entityRoot) const override
	{
		//no need to save these resources
	}
	virtual void load(const Json::Value & entityRoot) override
	{
		//no need to load these resources
	}

	std::map<std::string, ShaderProgram*>::iterator resourceBegin()
	{
		return m_resources.begin();
	}
	std::map<std::string, ShaderProgram*>::iterator resourceEnd()
	{
		return m_resources.end();
	}

	SINGLETON_IMPL(ResourceFactory);

private:
	void addResourceForce(const std::string& name, const ID& hashKey)
	{
		ShaderProgram* newResource = new ShaderProgram();
		newResource->init(name, hashKey);

		m_resources[name] = newResource;
		m_resourceMapping[name] = hashKey;
		m_resourcesFromHashKey[hashKey] = newResource;
	}

};



template<typename T>
ResourceFactory<T>::ResourceFactory()
{

}

//NOT DEFAULTS
//
//template<typename T>
//void ResourceFactory<T>::createNewResource(const FileHandler::CompletePath& path)
//{
//	assert(0 && "error : createNewResource() function hasn't been specialized for the given resource. The resource can't be created");
//}

template<typename T>
void ResourceFactory<T>::addResourceForce(const FileHandler::CompletePath& path, T* value)
{
	m_resources[path] = value;
	ID newID = IDGenerator<Resource>::instance().lockID();
	m_resourceMapping[path] = newID;
	m_resourcesFromHashKey[newID] = value;
}


template<typename T>
void ResourceFactory<T>::addResourceSoft(const FileHandler::CompletePath& path)
{
	if (m_resources.find(path) != m_resources.end())
		return;

	ID newID = IDGenerator<Resource>::instance().lockID();

	T* newResource = new T();
	newResource->init(path, newID);

	m_resources[path] = newResource;
	m_resourceMapping[path] = newID;
	m_resourcesFromHashKey[newID] = newResource;
}

template<typename T>
void ResourceFactory<T>::addResourceForce(const FileHandler::CompletePath& path, const ID& hashKey)
{
	T* newResource = new T();
	newResource->init(path, hashKey);

	m_resources[path] = newResource;
	m_resourceMapping[path] = hashKey;
	m_resourcesFromHashKey[hashKey] = newResource;
}

//template<typename T>
//template<typename U>
//void ResourceFactory<T>::add(const FileHandler::CompletePath& path)
//{
//	T* newResource = static_cast<T>(new U(path));
//
//	m_resources[path] = newResource;
//	m_resourceMapping[path] = ++s_resourceCount;
//	m_resourcesFromHashKey[s_resourceCount] = newResource;
//}

template<typename T>
void ResourceFactory<T>::erase(const FileHandler::CompletePath& path)
{
	if (m_resources.find(path) == m_resources.end())
		return;

	ID resourceHashKey = m_resourceMapping[path];
	delete m_resources[path];

	m_resources.erase(path);
	m_resourceMapping.erase(path);
	m_resourcesFromHashKey.erase(resourceHashKey);
}

template<typename T>
ID ResourceFactory<T>::getHashKeyForResource(const FileHandler::CompletePath& path) const
{
	return m_resourceMapping.at(path);
}

template<typename T>
T* ResourceFactory<T>::get(const FileHandler::CompletePath& path)
{
	return m_resources[path];
}

template<typename T>
T* ResourceFactory<T>::getRaw(const ID& hashKey)
{
	return m_resourcesFromHashKey[hashKey];
}

template<typename T>
bool ResourceFactory<T>::contains(const FileHandler::CompletePath& path)
{
	return m_resources.find(path) != m_resources.end();
}

template<typename T>
bool ResourceFactory<T>::contains(const ID& hashKey)
{
	return m_resourcesFromHashKey.find(hashKey) != m_resourcesFromHashKey.end();
}


template<typename T>
void ResourceFactory<T>::changeResourceKey(const FileHandler::CompletePath& oldKey, const FileHandler::CompletePath& newKey)
{
	assert(m_resources.find(oldKey) != m_resources.end());
	assert(m_resources.find(newKey) == m_resources.end());

	T* movingResource = m_resources[oldKey];

	//remove resource without deleting it
	ID resourceHashKey = m_resourceMapping[oldKey];
	m_resources.erase(oldKey);
	m_resourceMapping.erase(oldKey);

	//reAdd the resource
	m_resources[newKey] = movingResource;
	m_resourceMapping[newKey] = resourceHashKey;
}

//DEFAULTS
template<typename T>
void ResourceFactory<T>::initDefaults()
{
	//nothing
}

template<typename T>
void ResourceFactory<T>::addDefaultResource(const std::string& name, T* resource)
{
	ID newID = IDGenerator<Resource>::instance().lockID();

	setResourceName(resource, name);
	setResourceID(resource, newID);

	m_defaultResources[name] = resource;
	m_defaultResourceMapping[name] = newID;
	m_defaultResourcesFromHashKey[newID] = resource;
}

template<typename T>
ID ResourceFactory<T>::getHashKeyForDefaultResource(const std::string& name) const
{
	return m_defaultResourceMapping[name];
}

template<typename T>
T* ResourceFactory<T>::getDefault(const std::string& name)
{
	return m_defaultResources[name];
}

template<typename T>
T* ResourceFactory<T>::getRawDefault(const ID& hashKey)
{
	return m_defaultResourcesFromHashKey[hashKey];
}

template<typename T>
bool ResourceFactory<T>::containsDefault(const std::string& name)
{
	return m_defaultResources.find(name) != m_defaultResources.end();
}

template<typename T>
bool ResourceFactory<T>::containsDefault(const ID& hashKey)
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
	entityRoot = Json::Value(Json::arrayValue);
	int i = 0;
	for (auto it = m_resources.begin(); it != m_resources.end(); it++)
	{
		entityRoot[i]["path"] = it->first.toString();
		getHashKeyForResource(it->first.toString()).save(entityRoot[i]["hashKey"]);
		i++;
	}
}

template<typename T>
void ResourceFactory<T>::load(const Json::Value & entityRoot)
{
	int size = entityRoot.size();
	for (int i = 0; i < size; i++)
	{
		std::string resourcePath = entityRoot[i]["path"].asString();

		ID resourceHashKey;
		resourceHashKey.load(entityRoot[i]["hashKey"]);

		bool error = true;
		if (resourcePath != "")
		{
			FileHandler::CompletePath resourceCompletePath(resourcePath);
			if (FileHandler::fileExists(resourceCompletePath) && resourceHashKey.isValid())
			{
				addResourceForce(FileHandler::CompletePath(resourcePath), resourceHashKey);
				error = false;
			}
		}
		if (error)
		{
			PRINT_ERROR("Error in resourceloading !");
		}
	}
}

template<typename T>
typename std::map<FileHandler::CompletePath, T*>::iterator ResourceFactory<T>::resourceBegin()
{
	return m_resources.begin();
}

template<typename T>
typename std::map<std::string, T*>::iterator ResourceFactory<T>::defaultResourceBegin()
{
	return m_defaultResources.begin();
}

template<typename T>
typename std::map<FileHandler::CompletePath, T*>::iterator ResourceFactory<T>::resourceEnd()
{
	return m_resources.end();
}

template<typename T>
typename std::map<std::string, T*>::iterator ResourceFactory<T>::defaultResourceEnd()
{
	return m_defaultResources.end();
}

//Specialisations : 

template<>
void ResourceFactory<Material>::addResourceForce(const FileHandler::CompletePath& path, const ID& hashKey);
template<>
void ResourceFactory<Material>::addResourceSoft(const FileHandler::CompletePath& path);
//
//template<>
//void ResourceFactory<Material>::add(const FileHandler::CompletePath& path, unsigned int hashKey);
// Creation : 

//template<>
//void ResourceFactory<Material>::createNewResource(const FileHandler::CompletePath& path);

//Initialisations : 

//Shader Programes
//template<>
//void ResourceFactory<ShaderProgram>::initDefaults();

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
	assert(false && "resource not implementaed");
	return ResourceType::NONE;
}

template<>
ResourceType getResourceType<Texture>();
template<>
ResourceType getResourceType<CubeTexture>();
template<>
ResourceType getResourceType<Mesh>();
template<>
ResourceType getResourceType<SkeletalAnimation>();
template<>
ResourceType getResourceType<Material>();
template<>
ResourceType getResourceType<ShaderProgram>();

ResourceType getResourceTypeFromFileType(FileHandler::FileType fileType);

Resource* getResourceFromTypeAndCompletePath(ResourceType resourceType, const FileHandler::CompletePath& completePath);

void addResourceToFactory(const FileHandler::CompletePath& completePath);
void renameResourceInFactory(const FileHandler::CompletePath& oldResourcePath, const FileHandler::CompletePath& newResourcePath);
void removeResourceFromFactory(const FileHandler::CompletePath& resourcePath);
void removeAllResourcesFromFactories();

template<typename T>
const std::string& getResourceExtention()
{
	assert(getResourceExtention not "implemented with the given resource type");
}

template<>
const std::string& getResourceExtention<Material>();

////////////////////////////////////////////////
//// BEGIN : Forwards

///////////////// RESOURCE PTR /////////////////

template<typename T>
void ResourcePtr<T>::load(const Json::Value & entityRoot)
{
	m_isDefaultResource = entityRoot["isDefaultResource"].asBool();
	m_resourceHashKey.load(entityRoot.get("resourceHashKey", Json::nullValue));
	m_rawPtr = m_isDefaultResource ? getResourceFactory<T>().getRawDefault(m_resourceHashKey) : getResourceFactory<T>().getRaw(m_resourceHashKey);

	assert(m_rawPtr != nullptr);
	m_rawPtr->addReferenceToThis(this);
}

template<>
inline void ResourcePtr<ShaderProgram>::load(const Json::Value & entityRoot)
{
	m_isDefaultResource = entityRoot["isDefaultResource"].asBool();
	m_resourceHashKey.load(entityRoot.get("resourceHashKey", Json::nullValue));
	m_rawPtr = getResourceFactory<ShaderProgram>().getRaw(m_resourceHashKey);

	assert(m_rawPtr != nullptr);
	m_rawPtr->addReferenceToThis(this);
}

///////////////// RESOURCE FIELD /////////////////

namespace EditorGUI {

template<typename T>
bool ResourceField(const std::string& label, ResourcePtr<T>& resourcePtr)
{
	const int bufSize = 100;
	std::string currentResourceName(resourcePtr.isValid() ? resourcePtr->getName() : "INVALID");
	currentResourceName.reserve(bufSize);

	ResourceType resourceType = getResourceType<T>();
	bool canDropIntoField = DragAndDropManager::canDropInto(&resourceType, EditorDropContext::DropIntoResourceField);
	bool isTextEdited = false;
	bool needClearPtr = false;

	int colStyleCount = 0;
	if (canDropIntoField)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 255, 0, 255));
		colStyleCount++;
	}

	if (resourcePtr.isValid())
	{
		resourcePtr->drawIconeInResourceField();
		const bool iconHovered = ImGui::IsItemHovered();
		ImGui::SameLine();
		if (iconHovered)
			resourcePtr->drawUIOnHovered();
	}
	ImGui::InputText(label.c_str(), &currentResourceName[0], bufSize, ImGuiInputTextFlags_ReadOnly);
	//isTextEdited = (enterPressed || ImGui::IsKeyPressed(GLFW_KEY_TAB) || (!ImGui::IsItemHovered() && ImGui::IsMouseClickedAnyButton()));
	ImVec2 dropRectMin = ImGui::GetItemRectMin();
	ImVec2 dropRectMax = ImGui::GetItemRectMax();

	//borders if can drop here : 
	if (ImGui::IsMouseHoveringRect(dropRectMin, dropRectMax) && canDropIntoField)
	{
		ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), 2.f);
	}

	//drop resource : 
	FileHandler::CompletePath droppedResourcePath;
	if (ImGui::IsMouseHoveringRect(dropRectMin, dropRectMax) && ImGui::IsMouseReleased(0) && canDropIntoField)
	{
		DragAndDropManager::dropDraggedItem(&droppedResourcePath, (EditorDropContext::DropIntoResourceField));
		isTextEdited = true;
	}

	ImGui::SameLine();
	needClearPtr = ImGui::SmallButton(std::string("<##" + label).data());

	ImGui::PopStyleColor(colStyleCount);

	if (needClearPtr)
		resourcePtr.reset();
	else if (isTextEdited)
	{
		if (getResourceFactory<T>().contains(droppedResourcePath))
			resourcePtr = getResourceFactory<T>().get(droppedResourcePath);
		else
			resourcePtr.reset();
	}

	return isTextEdited;
}
}


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
//	virtual void load(const Json::Value & entityRoot) override;
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
//	virtual void load(const Json::Value & entityRoot) override;
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
//	virtual void load(const Json::Value & entityRoot) override;
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
//	virtual void load(const Json::Value & entityRoot) override;
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
//	virtual void load(const Json::Value & entityRoot) override;
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
//	virtual void load(const Json::Value & entityRoot) override;
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
