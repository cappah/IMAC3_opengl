#include "Factories.h"


void MeshFactory::add(const std::string& name, Mesh* mesh)
{
	m_meshes[name] = mesh;
}

Mesh* MeshFactory::get(const std::string& name)
{
	return m_meshes[name];
}

//////////////////////////////////////////

void MaterialFactory::add(const std::string& name, Material* material)
{
	m_materials[name] = material;
}

Material* MaterialFactory::get(const std::string& name)
{
	return m_materials[name];
}