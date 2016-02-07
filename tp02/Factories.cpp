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

void ProgramFactory::drawUI()
{
	ImGui::PushID("programFactory");

	for (auto& p : m_programs)
	{
		ImGui::Text(p.first.c_str());
	}

	ImGui::PopID();
}

///////////////////////////////////////

TextureFactory::TextureFactory()
{
	auto newTex = new Texture(255, 255, 255);
	newTex->initGL();
	
	newTex->name = "default";
	m_textures["default"] = newTex;
}

void TextureFactory::add(const std::string& name, const std::string& path)
{
	if (name == "default") //can't override default key
		return;

	auto newTexture = new Texture(path);

	newTexture->name = name;
	m_textures[name] = newTexture;
}

void TextureFactory::add(const std::string& name, Texture* texture)
{
	if (name == "default") //can't override default key
		return;

	texture->name = name;
	m_textures[name] = texture;
}

Texture* TextureFactory::get(const std::string& name)
{
	return m_textures[name];
}

bool TextureFactory::contains(const std::string& name)
{
	return (m_textures.find(name) != m_textures.end()); 
}

void TextureFactory::drawUI()
{
	ImGui::PushID("textureFactory");

	ImGui::PushItemWidth(70);
	ImGui::InputText("name", name, 20);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::PushItemWidth(180);
	ImGui::InputText("path", path, 50);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::SmallButton("add"))
	{
		add(name, path);
	}
	

	for (auto& t : m_textures)
	{
		ImGui::Text(t.first.c_str());
		ImGui::SameLine();
		ImGui::Text(t.second->path.c_str());
	}

	ImGui::PopID();
}

/////////////////////////////////////////


CubeTextureFactory::CubeTextureFactory()
{
	auto newTex = new CubeTexture(255, 255, 255);
	newTex->initGL();

	newTex->name = "default";
	m_textures["default"] = newTex;
}

void CubeTextureFactory::add(const std::string& name, const std::vector<std::string>& paths)
{
	if (name == "default") //can't override default key
		return;

	auto newTexture = new CubeTexture(paths);

	newTexture->name = "default";
	m_textures[name] = newTexture;
}

void CubeTextureFactory::add(const std::string& name, CubeTexture* textureId)
{
	if (name == "default") //can't override default key
		return;

	textureId->name = "default";
	m_textures[name] = textureId;
}

CubeTexture* CubeTextureFactory::get(const std::string& name)
{
	return m_textures[name];
}

bool CubeTextureFactory::contains(const std::string& name)
{
	return (m_textures.find(name) != m_textures.end());
}

void CubeTextureFactory::drawUI()
{
	ImGui::PushID("textureFactory");

	ImGui::PushItemWidth(70);
	ImGui::InputText("name", name, 20);
	ImGui::PopItemWidth();

	ImGui::InputText("path right", paths[0], 50);
	ImGui::InputText("path left", paths[1], 50);
	ImGui::InputText("path top", paths[2], 50);
	ImGui::InputText("path bottom", paths[3], 50);
	ImGui::InputText("path back", paths[4], 50);
	ImGui::InputText("path front", paths[5], 50);

	if (ImGui::SmallButton("add"))
	{
		std::vector<std::string> tmpPaths;
		for (int i = 0; i < 6; i++)
		{
			tmpPaths.push_back(paths[i]);
		}

		add(name, tmpPaths);
	}


	for (auto& t : m_textures)
	{
		ImGui::Text(t.first.c_str());
	}

	ImGui::PopID();
}

/////////////////////////////////////////

void MeshFactory::add(const std::string& name, Mesh* mesh)
{
	if (name == "default") //can't override default key
		return;

	mesh->name = name;
	m_meshes[name] = mesh;
}

void MeshFactory::add(const std::string & name, const std::string & path)
{
	if (name == "default") //can't override default key
		return;

	Mesh* newMesh = new Mesh(path);

	newMesh->name = name;
	m_meshes[name] = newMesh;
}

Mesh* MeshFactory::get(const std::string& name)
{
	return m_meshes[name];
}

bool MeshFactory::contains(const std::string& name)
{
	return m_meshes.find(name) != m_meshes.end();
}

void MeshFactory::drawUI()
{
	ImGui::PushID("meshFactory");

	ImGui::PushItemWidth(70);
	ImGui::InputText("name", name, 20);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::PushItemWidth(180);
	ImGui::InputText("path", path, 50);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::SmallButton("add"))
	{
		add(name, path); 
	}


	for (auto& m : m_meshes)
	{
		ImGui::Text(m.first.c_str());
		ImGui::SameLine();
		ImGui::Text(m.second->path.c_str());
	}

	ImGui::PopID();
}

//////////////////////////////////////////

void MaterialFactory::add(const std::string& name, Material* material)
{
	material->name = name;
	m_materials[name] = material;
}

//Material* MaterialFactory::get(const std::string& name)
//{
//	return m_materials[name];
//}

void MaterialFactory::drawUI()
{
	ImGui::PushID("materialFactory");

	ImGui::PushItemWidth(70);
	ImGui::InputText("name", name, 20);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::SmallButton("add"))
	{
		add(name, new MaterialLit() );
	}


	for (auto& m : m_materials)
	{
		ImGui::Text(m.first.c_str());
	}

	ImGui::PopID();
}
