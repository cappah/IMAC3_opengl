#pragma once

#include <map>

#include "stb/stb_image.h"

#include "Mesh.h"
#include "Materials.h"
#include "Texture.h"

#include "ISerializable.h"


class ProgramFactory : public ISerializable
{

private:
	std::map<std::string, GLuint> m_programs;

	std::vector<std::string> m_defaults;

public:
	void add(const std::string& name, GLuint programId);
	GLuint get(const std::string& name);
	bool contains(const std::string& name);
	void drawUI();
	void clear();

	virtual void save(Json::Value & entityRoot) const override;
	virtual void load(Json::Value & entityRoot) override;


	// singleton implementation :
private:
	ProgramFactory();

public:
	inline static ProgramFactory& get()
	{
		static ProgramFactory instance;

		return instance;
	}


	ProgramFactory(const ProgramFactory& other) = delete;
	void operator=(const ProgramFactory& other) = delete;

};


//////////////////////////////////////

class TextureFactory : public ISerializable
{

private:
	std::map<std::string, Texture*> m_textures;

	//for UI : 
	char name[20];
	char path[50];

public:
	void add(const std::string& name, const std::string& path);
	void add(const std::string& name, Texture* texture);
	Texture* get(const std::string& name);
	bool contains(const std::string& name);
	void drawUI();
	void clear();

	virtual void save(Json::Value & entityRoot) const override;
	virtual void load(Json::Value & entityRoot) override;

	// singleton implementation :
private:
	TextureFactory();

public:
	inline static TextureFactory& get()
	{
		static TextureFactory instance;

		return instance;
	}


	TextureFactory(const TextureFactory& other) = delete;
	void operator=(const TextureFactory& other) = delete;
};

///////////////////////////////


class CubeTextureFactory : public ISerializable
{

private:
	std::map<std::string, CubeTexture*> m_textures;

	//for UI : 
	char name[20];
	char paths[6][50];

public:
	void add(const std::string& name, const std::vector<std::string>& paths);
	void add(const std::string& name, CubeTexture* textureId);
	CubeTexture* get(const std::string& name);
	bool contains(const std::string& name);
	void drawUI();
	void clear();

	virtual void save(Json::Value & entityRoot) const override;
	virtual void load(Json::Value & entityRoot) override;

	// singleton implementation :
private:
	CubeTextureFactory();

public:
	inline static CubeTextureFactory& get()
	{
		static CubeTextureFactory instance;

		return instance;
	}


	CubeTextureFactory(const CubeTextureFactory& other) = delete;
	void operator=(const CubeTextureFactory& other) = delete;
};



///////////////////////////////

class MeshFactory : public ISerializable
{

private:
	std::map<std::string, Mesh*> m_meshes;

	//for UI : 
	char name[20];
	char path[50];

	std::vector<std::string> m_defaults;

public:
	void add(const std::string& name, Mesh* mesh);
	void add(const std::string& name, const std::string& path);
	Mesh* get(const std::string& name);
	bool contains(const std::string& name);
	void drawUI();
	void clear();

	virtual void save(Json::Value & entityRoot) const override;
	virtual void load(Json::Value & entityRoot) override;

	// singleton implementation :
private:
	MeshFactory();

public:
	inline static MeshFactory& get()
	{
		static MeshFactory instance;

		return instance;
	}


	MeshFactory(const MeshFactory& other) = delete;
	void operator=(const MeshFactory& other) = delete;
};

////////////////////////////////

class MaterialFactory : public ISerializable
{

private:
	std::map<std::string, Material*> m_materials;

	//for UI : 
	char name[20];

	std::vector<std::string> m_defaults;

public:
	void add(const std::string& name, Material* material);
	template<typename T>
	T* get(const std::string& name);
	template<typename T>
	bool contains(const std::string& name);
	void drawUI();
	void clear();

	virtual void save(Json::Value & entityRoot) const override;
	virtual void load(Json::Value & entityRoot) override;

	// singleton implementation :
private:
	MaterialFactory();

public:
	inline static MaterialFactory& get()
	{
		static MaterialFactory instance;

		return instance;
	}


	MaterialFactory(const MaterialFactory& other) = delete;
	void operator=(const MaterialFactory& other) = delete;
};

template<typename T>
T* MaterialFactory::get(const std::string& name)
{
	assert(m_materials.find(name) != m_materials.end());

	assert(dynamic_cast<T*>(m_materials[name]) == m_materials[name]);
	return static_cast<T*>(m_materials[name]);
}

template<typename T>
bool MaterialFactory::contains(const std::string& name)
{
	auto findResult = m_materials.find(name);
	return ( findResult != m_materials.end() && dynamic_cast<T*>(findResult->second));
}

