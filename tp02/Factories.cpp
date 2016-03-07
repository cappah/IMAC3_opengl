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

void ProgramFactory::clear()
{
	for (auto& it = m_programs.begin(); it != m_programs.end(); it++)
	{
		glDeleteProgram(it->second);
	}
	m_programs.clear();
}

void ProgramFactory::save(Json::Value & objectRoot) const
{
	objectRoot["size"] = m_programs.size();
	int i = 0;
	for (auto it = m_programs.begin(); it != m_programs.end(); it++)
	{
		objectRoot["keys"][i] = it->first;
		objectRoot["values"][i] = it->second;
		i++;
	}
}

void ProgramFactory::load(Json::Value & entityRoot)
{
	int size = entityRoot.get("size", 0).asInt();
	for (int i = 0; i < size; i++)
	{
		std::string key = entityRoot["keys"][i].asString();
		GLuint glId = entityRoot["values"][i].asInt();
		m_programs[key] = glId;
	}
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

void TextureFactory::clear()
{
	for (auto& it = m_textures.begin(); it != m_textures.end();)
	{
		if (it->first != "default")
		{
			it->second->freeGL();
			delete it->second;
			it = m_textures.erase(it);
		}
		else
			it++;
	}
}

void TextureFactory::save(Json::Value & entityRoot) const
{
	entityRoot["size"] = m_textures.size();
	int i = 0;
	for (auto it = m_textures.begin(); it != m_textures.end(); it++)
	{
		entityRoot["keys"][i] = it->first;
		entityRoot["values"][i] = it->second->path;
		i++;
	}
}

void TextureFactory::load(Json::Value & entityRoot)
{
	int size = entityRoot.get("size", 0).asInt();
	for (int i = 0; i < size; i++)
	{
		std::string textureName = entityRoot["keys"][i].asString();
		std::string texturePath = entityRoot["values"][i].asString();
		add(textureName, texturePath);
	}
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

void CubeTextureFactory::clear()
{
	for (auto& it = m_textures.begin(); it != m_textures.end();)
	{
		if (it->first != "default")
		{
			it->second->freeGL();
			delete it->second;
			it = m_textures.erase(it);
		}
		else
			it++;
	}
}

void CubeTextureFactory::save(Json::Value & entityRoot) const
{
	entityRoot["size"] = m_textures.size();
	int i = 0;
	for (auto it = m_textures.begin(); it != m_textures.end(); it++)
	{
		entityRoot["keys"][i] = it->first;

		entityRoot["values"][i]["size"] = it->second->paths.size();
		for (int p = 0; p < it->second->paths.size(); p++)
			entityRoot["values"][i]["paths"][p] = it->second->paths[p];
		i++;
	}
}

void CubeTextureFactory::load(Json::Value & entityRoot)
{
	int size = entityRoot.get("size", 0).asInt();
	for (int i = 0; i < size; i++)
	{
		std::string cubeTextureName = entityRoot["keys"][i].asString();

		int pathSize = entityRoot["values"][i].get("size",0).asInt();
		std::vector<std::string> cubeTexturePaths;
		for (int p = 0; p < pathSize; p++)
			cubeTexturePaths.push_back(entityRoot["values"][i]["paths"].get(p,"default").asString());

		add(cubeTextureName, cubeTexturePaths);
	}
}


/////////////////////////////////////////

MeshFactory::MeshFactory()
{
	//Set the cube as a default mesh
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

	cube->name = "default";
	m_meshes["default"] = cube;
}

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
		if (&m == nullptr)
			continue;

		ImGui::Text(m.first.c_str());
		ImGui::SameLine();
		ImGui::Text(m.second->path.c_str());
	}

	ImGui::PopID();
}

void MeshFactory::clear()
{
	for (auto& it = m_meshes.begin(); it != m_meshes.end();)
	{
		if (it->first != "default")
		{
			it->second->freeGl();
			delete it->second;
			it = m_meshes.erase(it);
		}
		else
			it++;
	}
}

void MeshFactory::save(Json::Value & entityRoot) const
{
	entityRoot["size"] = m_meshes.size();
	int i = 0;
	for (auto it = m_meshes.begin(); it != m_meshes.end(); it++)
	{
		entityRoot["keys"][i] = it->first;
		entityRoot["values"][i] = it->second->path;
		i++;
	}
}

void MeshFactory::load(Json::Value & entityRoot)
{
	int size = entityRoot.get("size", 0).asInt();
	for (int i = 0; i < size; i++)
	{
		std::string meshName = entityRoot["keys"][i].asString();
		std::string meshPath = entityRoot["values"][i].asString();
		
		//don't laod mesh without mesh path these meshes are renerated with code : 
		if(meshPath != "")
			add(meshName, meshPath);
	}
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

void MaterialFactory::clear()
{
	for (auto& it = m_materials.begin(); it != m_materials.end(); it++)
	{
		delete it->second;
	}
	m_materials.clear();
}

void MaterialFactory::save(Json::Value & entityRoot) const
{

	//We only save lit materials : 
	std::vector<MaterialLit*> litMaterials;
	for (auto it = m_materials.begin(); it != m_materials.end(); it++)
	{
		MaterialLit* matLitCasted = dynamic_cast<MaterialLit*>(it->second);
		if (matLitCasted != nullptr)
		{
			litMaterials.push_back(matLitCasted);
		}
	}

	entityRoot["size"] = litMaterials.size();

	for (int i = 0; i < litMaterials.size(); i++)
	{
		entityRoot["keys"][i] = litMaterials[i]->name;
		litMaterials[i]->save(entityRoot["values"][i]);
	}
}

void MaterialFactory::load(Json::Value & entityRoot)
{
	//we only load lit materials : 
	int size = entityRoot.get("size", 0).asInt();
	for (int i = 0; i < size; i++)
	{
		std::string materialName = entityRoot["keys"][i].asString();
		MaterialLit* newMaterial = new MaterialLit();
		newMaterial->load(entityRoot["values"][i]);
		add(materialName, newMaterial);
	}
}
