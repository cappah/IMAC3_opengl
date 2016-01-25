#include "Factories.h"

void ProgramFactory::add(const std::string& name, GLuint programId)
{
	m_programs[name] = programId;
}

GLuint ProgramFactory::get(const std::string& name)
{
	return m_programs[name];
}

bool ProgramFactory::contains(const std::string& name)
{
	return m_programs.find(name) != m_programs.end();
}

///////////////////////////////////////

void TextureFactory::add(const std::string& name, const std::string& path)
{
	int x;
	int y;
	int comp;

	unsigned char * texture = stbi_load("textures/spnza_bricks_a_diff.tga", &x, &y, &comp, 3);
	GLuint newTexture;
	glGenTextures(1, &newTexture);

	glBindTexture(GL_TEXTURE_2D, newTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	m_textures[name] = newTexture;
}

void TextureFactory::add(const std::string& name, GLuint textureId)
{
	m_textures[name] = textureId;
}

GLuint TextureFactory::get(const std::string& name)
{
	return m_textures[name];
}

bool TextureFactory::contains(const std::string& name)
{
	return (m_textures.find(name) != m_textures.end()); 
}

/////////////////////////////////////////

void MeshFactory::add(const std::string& name, Mesh* mesh)
{
	m_meshes[name] = mesh;
}

Mesh* MeshFactory::get(const std::string& name)
{
	return m_meshes[name];
}

bool MeshFactory::contains(const std::string& name)
{
	return m_meshes.find(name) != m_meshes.end();
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

bool MaterialFactory::contains(const std::string& name)
{
	return m_materials.find(name) != m_materials.end();
}