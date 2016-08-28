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
void ResourceFactory<T>::add(const FileHandler::CompletePath& path)
{
	T* newResource = new T(path);

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
	newResource->load(path);

	m_resources[path] = newResource;
	m_resourceMapping[path] = hashKey;
	m_resourcesFromHashKey[hashKey] = newResource;
}

template<typename T>
unsigned int ResourceFactory<T>::getHashKeyForResource(const FileHandler::CompletePath& path) const
{
	return m_resourceMapping[path];
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
		entityRoot[i]["hashKey"] = getHashKeyForResource(it->first);
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

		if (resourceHashKey < resourceCount)
			resourceHashKey = resourceCount;
	}
}

//Specialisations : 

//Shader Programes
template<>
void ResourceFactory<ShaderProgram>::initDefaults()
{
	//////////////////// CPU PARTICLE shaders ////////////////////////
	m_defaultResources["defaultParticlesCPU"] = new ShaderProgram(FileHandler::CompletePath("particleCPU.vert"), FileHandler::CompletePath("particleCPU.frag"));
	
	//////////////////// PARTICLE shaders ////////////////////////
	m_defaultResources["defaultParticles"] = new ShaderProgram(FileHandler::CompletePath("particle.vert"), FileHandler::CompletePath("particle.frag"), FileHandler::CompletePath("particle.geom"));
	
	//////////////////// PARTICLE SIMULATION shaders ////////////////////////
	m_defaultResources["particleSimulation"] = new ShaderProgram(FileHandler::CompletePath("particleSimulation.vert"), FileHandler::CompletePath(), FileHandler::CompletePath("particleSimulation.geom"));

	//////////////////// BILLBOARD shaders ////////////////////////
	m_defaultResources["defaultBillboard"] = new ShaderProgram(FileHandler::CompletePath("billboard.vert"), FileHandler::CompletePath("billboard.frag"));
	
	//////////////////// SKYBOX shaders ////////////////////////
	m_defaultResources["defaultSkybox"] = new ShaderProgram(FileHandler::CompletePath("skybox.vert"), FileHandler::CompletePath("skybox.frag"));

	//////////////////// 3D Gpass shaders ////////////////////////
	m_defaultResources["defaultLit"] = new ShaderProgram(FileHandler::CompletePath("aogl.vert"), FileHandler::CompletePath("aogl_gPass.frag"));
	
	//////////////////// WIREFRAME shaders ////////////////////////
	m_defaultResources["defaultUnlit"] = new ShaderProgram(FileHandler::CompletePath("wireframe.vert"), FileHandler::CompletePath("wireframe.frag"));

	//////////////////// TERRAIN shaders ////////////////////////
	m_defaultResources["defaultTerrain"] = new ShaderProgram(FileHandler::CompletePath("terrain.vert"), FileHandler::CompletePath("terrain.frag"));

	//////////////////// TERRAIN EDITION shaders ////////////////////////
	m_defaultResources["defaultTerrainEdition"] = new ShaderProgram(FileHandler::CompletePath("terrainEdition.vert"), FileHandler::CompletePath("terrainEdition.frag"));

	//////////////////// DRAW ON TEXTURE shaders ////////////////////////
	m_defaultResources["defaultDrawOnTexture"] = new ShaderProgram(FileHandler::CompletePath("drawOnTexture.vert"), FileHandler::CompletePath("drawOnTexture.frag"));

	//////////////////// GRASS FIELD shaders ////////////////////////
	m_defaultResources["defaultGrassField"] = new ShaderProgram(FileHandler::CompletePath("grassField.vert"), FileHandler::CompletePath("grassField.frag"));

	//////////////////// GRASS FIELD shaders ////////////////////////
	m_defaultResources["wireframeInstanced"] = new ShaderProgram(FileHandler::CompletePath("wireframeInstanced.vert"), FileHandler::CompletePath("wireframeInstanced.frag"));

	//////////////////// DEBUG DRAWER shaders ////////////////////////
	m_defaultResources["debugDrawer"] = new ShaderProgram(FileHandler::CompletePath("debugDrawer.vert"), FileHandler::CompletePath("debugDrawer.frag"));
}

//Textures
template<>
void ResourceFactory<Texture>::initDefaults()
{
	//default diffuse
	auto newTex = new Texture(255, 255, 255);
	newTex->initGL();
	newTex->name = "default";
	m_defaultResources["default"] = newTex;

	//default normal
	newTex = new Texture(0, 0, 125);
	newTex->initGL();
	newTex->name = "defaultNormal";
	m_defaultResources["defaultNormal"] = newTex;
}

//Cube Texture
template<>
void ResourceFactory<CubeTexture>::initDefaults()
{
	auto newTex = new CubeTexture(255, 255, 255);
	newTex->initGL();
	newTex->name = "default";
	m_defaultResources["default"] = newTex;
}

//Mesh
template<>
void ResourceFactory<Mesh>::initDefaults()
{
	Mesh* cube = new Mesh(GL_TRIANGLES, (Mesh::USE_INDEX | Mesh::USE_VERTICES | Mesh::USE_NORMALS | Mesh::USE_UVS | Mesh::USE_TANGENTS));
	cube->vertices = { 0.5,0.5,-0.5,  0.5,0.5,0.5,  0.5,-0.5,0.5,  0.5,-0.5,-0.5,
		-0.5,0.5,-0.5,  -0.5,0.5,0.5,  -0.5,-0.5,0.5,  -0.5,-0.5,-0.5,
		-0.5,0.5,0.5,  0.5,0.5,0.5,  0.5,-0.5,0.5,  -0.5,-0.5,0.5,
		-0.5,0.5,-0.5,  0.5,0.5,-0.5,  0.5,-0.5,-0.5,  -0.5,-0.5,-0.5,
		0.5,0.5,0.5, -0.5,0.5,0.5, -0.5,0.5,-0.5, 0.5,0.5,-0.5,
		-0.5,-0.5,-0.5,  0.5,-0.5,-0.5,  0.5,-0.5,0.5,  -0.5,-0.5,0.5 };

	cube->normals = { 1,0,0,  1,0,0,  1,0,0,  1,0,0,
		-1,0,0,  -1,0,0,  -1,0,0,  -1,0,0,
		0,0,1,  0,0,1,  0,0,1,  0,0,1,
		0,0,-1,  0,0,-1,  0,0,-1,  0,0,-1,
		0,1,0,  0,1,0,  0,1,0,  0,1,0,
		0,-1,0,  0,-1,0,  0,-1,0,  0,-1,0 };

	cube->tangents = { 0,0,1,  0,0,1,  0,0,1,  0,0,1,
		0,0,1,  0,0,1,  0,0,1,  0,0,1,
		-1,0,0,  -1,0,0,  -1,0,0,  -1,0,0,
		1,0,0,  1,0,0,  1,0,0,  1,0,0,
		1,0,0,  1,0,0,  1,0,0,  1,0,0,
		-1,0,0,  -1,0,0,  -1,0,0,  -1,0,0 };

	cube->uvs = { 0.0,0.0,  0.0,1.0,  1.0,1.0,  1.0,0.0,
		0.0,0.0,  0.0,1.0,  1.0,1.0,  1.0,0.0,
		0.0,0.0,  0.0,1.0,  1.0,1.0,  1.0,0.0,
		0.0,0.0,  0.0,1.0,  1.0,1.0,  1.0,0.0,
		0.0,0.0,  0.0,1.0,  1.0,1.0,  1.0,0.0,
		0.0,0.0,  0.0,1.0,  1.0,1.0,  1.0,0.0 };

	cube->triangleIndex = { 0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 8, 9, 10, 10, 11, 8, 12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20 };

	cube->initGl();
	cube->name = "cube";
	cube->path = FileHandler::CompletePath();
	cube->computeBoundingBox();


	Mesh* cubeWireFrame = new Mesh(GL_LINE_STRIP, (Mesh::USE_INDEX | Mesh::USE_VERTICES));
	cubeWireFrame->triangleIndex = { 0, 1, 2, 2, 1, 3, 4, 5, 6, 6, 5, 7, 8, 9, 10, 10, 9, 11, 12, 13, 14, 14, 13, 15, 16, 17, 18, 19, 17, 20, 21, 22, 23, 24, 25, 26, };
	cubeWireFrame->uvs = { 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f,  1.f, 0.f,  1.f, 1.f,  0.f, 1.f,  1.f, 1.f,  0.f, 0.f, 0.f, 0.f, 1.f, 1.f,  1.f, 0.f, };
	cubeWireFrame->vertices = { -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5 };
	cubeWireFrame->normals = { 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, };
	cubeWireFrame->initGl();
	cubeWireFrame->name = "cubeWireframe";
	cubeWireFrame->path = FileHandler::CompletePath();
	cubeWireFrame->computeBoundingBox();

	std::vector<float> tmpVertices;
	Mesh* capsuleWireFrame = new Mesh(GL_LINES, (Mesh::USE_VERTICES));
	for (int i = 0; i < 10; i++) {
		tmpVertices.push_back(0.5f*std::cos((2.f*glm::pi<float>()*i) / 10.f)); //x
		float y = 0.5f*std::sin((2.f*glm::pi<float>()*i) / 10.f);
		y = y > 0 ? y + 1.f : y - 1.f;
		tmpVertices.push_back(y); //y
		tmpVertices.push_back(0); //z
	}
	for (int i = 0; i < 10; i++) {
		tmpVertices.push_back(0); //x
		float y = 0.5f*std::cos((2.f*glm::pi<float>()*i) / 10.f);
		y = y > 0 ? y + 1.f : y - 1.f;
		tmpVertices.push_back(y); //y
		tmpVertices.push_back(0.5f*std::sin((2.f*glm::pi<float>()*i) / 10.f)); //z
	}
	for (int i = 0; i < 10; i++) {
		tmpVertices.push_back(0.5f*std::cos((2.f*glm::pi<float>()*i) / 10.f)); //x
		tmpVertices.push_back(1.f); //y
		tmpVertices.push_back(0.5f*std::sin((2.f*glm::pi<float>()*i) / 10.f)); //z
	}
	for (int i = 0; i < 10; i++) {
		tmpVertices.push_back(0.5f*std::cos((2.f*glm::pi<float>()*i) / 10.f)); //x
		tmpVertices.push_back(-1.f); //y
		tmpVertices.push_back(0.5f*std::sin((2.f*glm::pi<float>()*i) / 10.f)); //z
	}
	for (int i = 0; i < 10; i++) {
		tmpVertices.push_back(0); //x
		tmpVertices.push_back(-1.f + (i / 9.f)*2.f); //y
		tmpVertices.push_back(-0.5f); //z
	}
	for (int i = 0; i < 10; i++) {
		tmpVertices.push_back(0); //x
		tmpVertices.push_back(-1.f + (i / 9.f)*2.f); //y
		tmpVertices.push_back(0.5f); //z
	}
	for (int i = 0; i < 10; i++) {
		tmpVertices.push_back(-0.5); //x
		tmpVertices.push_back(-1.f + (i / 9.f)*2.f); //y
		tmpVertices.push_back(0); //z
	}
	for (int i = 0; i < 10; i++) {
		tmpVertices.push_back(0.5); //x
		tmpVertices.push_back(-1.f + (i / 9.f)*2.f); //y
		tmpVertices.push_back(0); //z
	}
	//CapsuleWireFrame->triangleIndex = { };
	//CapsuleWireFrame->uvs = { 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f,  1.f, 0.f,  1.f, 1.f,  0.f, 1.f,  1.f, 1.f,  0.f, 0.f, 0.f, 0.f, 1.f, 1.f,  1.f, 0.f, };
	capsuleWireFrame->vertices = tmpVertices;
	//CapsuleWireFrame->normals = { 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, };
	capsuleWireFrame->initGl();
	capsuleWireFrame->name = "capsuleWireframe";
	capsuleWireFrame->path = FileHandler::CompletePath();
	capsuleWireFrame->computeBoundingBox();

	tmpVertices.clear();

	for (int i = 0; i < 10; i++) {
		tmpVertices.push_back(0.5f*std::cos((2.f*glm::pi<float>()*i) / 10.f)); //x
		tmpVertices.push_back(0.5f*std::sin((2.f*glm::pi<float>()*i) / 10.f)); //y
		tmpVertices.push_back(0); //z
	}
	for (int i = 0; i < 10; i++) {
		tmpVertices.push_back(0.5f*std::cos((2.f*glm::pi<float>()*i) / 10.f)); //x
		tmpVertices.push_back(0); //y
		tmpVertices.push_back(0.5f*std::sin((2.f*glm::pi<float>()*i) / 10.f)); //z
	}
	for (int i = 0; i < 10; i++) {
		tmpVertices.push_back(0); //x
		tmpVertices.push_back(0.5f*std::cos((2.f*glm::pi<float>()*i) / 10.f)); //y
		tmpVertices.push_back(0.5f*std::sin((2.f*glm::pi<float>()*i) / 10.f)); //z
	}

	Mesh* sphereWireFrame = new Mesh(GL_LINES, (Mesh::USE_VERTICES));
	sphereWireFrame->vertices = tmpVertices;
	sphereWireFrame->initGl();
	sphereWireFrame->name = "sphereWireframe";
	sphereWireFrame->path = FileHandler::CompletePath();
	sphereWireFrame->computeBoundingBox();
	tmpVertices.clear();

	Mesh* plane = new Mesh();
	plane->triangleIndex = { 0, 1, 2, 2, 3, 0 };
	plane->uvs = { 0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f };
	plane->vertices = { -0.5, 0.0, -0.5, 0.5, 0.0, -0.5, 0.5, 0.0, 0.5, -0.5, 0.0, 0.5 };
	plane->normals = { 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0 };
	plane->initGl();
	plane->name = "plane";
	plane->path = FileHandler::CompletePath();
	plane->computeBoundingBox();

	Mesh* quad = new Mesh(GL_TRIANGLES, (Mesh::USE_INDEX | Mesh::USE_NORMALS | Mesh::USE_UVS | Mesh::USE_VERTICES), 2);
	quad->triangleIndex = { 0, 1, 2, 0, 2, 3 };
	quad->uvs = { 0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f };
	quad->vertices = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
	quad->normals = { 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0 };
	quad->initGl();
	quad->name = "quad";
	quad->path = FileHandler::CompletePath();
	//quad->computeBoundingBox();

	m_defaultResources["default"] = cube;
	m_defaultResources["cube"] = cube;
	m_defaultResources["cubeWireframe"] = cubeWireFrame;
	m_defaultResources["capsuleWireframe"] = capsuleWireFrame;
	m_defaultResources["sphereWireframe"] = sphereWireFrame;
	m_defaultResources["plane"] = plane;
	m_defaultResources["quad"] = plane;
}

//Animations : nothing by default


//Materials
template<>
void ResourceFactory<Material>::initDefaults()
{

	Material* newMat = new MaterialLit(ResourceFactory<ShaderProgram>::instance().getDefault("defaultLit")->id, ResourceFactory<Texture>::instance().getDefault("default"), ResourceFactory<Texture>().getDefault("default"), ResourceFactory<Texture>::instance().getDefault("default"), 50);
	newMat->name = "default";
	m_defaultResources["default"] = newMat;

	newMat = new MaterialUnlit(ResourceFactory<ShaderProgram>::instance().getDefault("defaultUnlit")->id);
	newMat->name = "wireframe";
	m_defaultResources["wireframe"] = newMat;

	newMat = new MaterialInstancedUnlit(ResourceFactory<ShaderProgram>::instance().getDefault("wireframeInstanced")->id);
	newMat->name = "wireframeInstanced";
	m_defaultResources["wireframeInstanced"] = newMat;

	newMat = new MaterialGrassField(ResourceFactory<ShaderProgram>::instance().getDefault("defaultGrassField")->id);
	newMat->name = "grassfield";
	m_defaultResources["grassfield"] = newMat;

	newMat = new MaterialBillboard(ResourceFactory<ShaderProgram>::instance().getDefault("defaultBillboard")->id);
	newMat->name = "billboard";
	m_defaultResources["billboard"] = newMat;

	newMat = new MaterialParticles(ResourceFactory<ShaderProgram>::instance().getDefault("defaultParticles")->id);
	newMat->name = "particles";
	m_defaultResources["particles"] = newMat;

	newMat = new MaterialParticlesCPU(ResourceFactory<ShaderProgram>::instance().getDefault("defaultParticlesCPU")->id);
	newMat->name = "particlesCPU";
	m_defaultResources["particlesCPU"] = newMat;

	newMat = new MaterialParticleSimulation(ResourceFactory<ShaderProgram>::instance().getDefault("particleSimulation")->id);
	newMat->name = "particleSimulation";
	m_defaultResources["particleSimulation"] = newMat;

	newMat = new MaterialDebugDrawer(ResourceFactory<ShaderProgram>::instance().getDefault("debugDrawer")->id);
	newMat->name = "debugDrawer";
	m_defaultResources["debugDrawer"] = newMat;
}

//access helper
ResourceFactory<ShaderProgram>& getProgramFactory()
{
	return ResourceFactory<ShaderProgram>::instance();
}

ResourceFactory<Mesh>& getMeshFactory()
{
	return ResourceFactory<Mesh>::instance();
}

ResourceFactory<Texture>& getTextureFactory()
{
	return ResourceFactory<Texture>::instance();
}

ResourceFactory<CubeTexture>& getCubeTextureFactory()
{
	return ResourceFactory<CubeTexture>::instance();
}

ResourceFactory<Material>& getMaterialFactory()
{
	return ResourceFactory<Material>::instance();
}

ResourceFactory<MeshAnimations>& getSkeletalAnimationFactory()
{
	return ResourceFactory<MeshAnimations>::instance();
}

template<typename T>
ResourceFactory<T>& getResourceFactory()
{
	return ResourceFactory<T>::instance();
}

template<typename T>
ResourceType getResourceType()
{ 
	assert(false, "resource not implementaed");
}

template<>
ResourceType getResourceType<Texture>()
{
	return ResourceType::TEXTURE;
}

template<>
ResourceType getResourceType<CubeTexture>()
{
	return ResourceType::CUBE_TEXTURE;
}

template<>
ResourceType getResourceType<MeshAnimations>()
{
	return ResourceType::SKELETAL_ANIMATION;
}

template<>
ResourceType getResourceType<Material>()
{
	return ResourceType::MATERIAL;
}

template<>
ResourceType getResourceType<ShaderProgram>()
{
	return ResourceType::PROGRAME;
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
