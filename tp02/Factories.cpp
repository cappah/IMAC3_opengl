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
	
	m_textures["default"] = newTex;
}

void TextureFactory::add(const std::string& name, const std::string& path)
{
	if (name == "default") //can't override default key
		return;

	auto newTexture = new Texture(path);

	m_textures[name] = newTexture;
}

void TextureFactory::add(const std::string& name, Texture* textureId)
{
	if (name == "default") //can't override default key
		return;

	m_textures[name] = textureId;
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
		// TODO
		//add(name, path); 
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
