#pragma once

#include <map>

#include "Mesh.h"
#include "Materials.h"

class MeshFactory
{

private:
	std::map<std::string, Mesh*> m_meshes;

public:
	void add(const std::string& name, Mesh* mesh);
	Mesh* get(const std::string& name);

	// singleton implementation :
private:
	MeshFactory() {

	}

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

class MaterialFactory
{

private:
	std::map<std::string, Material*> m_materials;

public:
	void add(const std::string& name, Material* material);
	Material* get(const std::string& name);

	// singleton implementation :
private:
	MaterialFactory() {

	}

public:
	inline static MaterialFactory& get()
	{
		static MaterialFactory instance;

		return instance;
	}


	MaterialFactory(const MaterialFactory& other) = delete;
	void operator=(const MaterialFactory& other) = delete;
};

