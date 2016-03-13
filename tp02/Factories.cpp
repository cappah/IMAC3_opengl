#include "Factories.h"
//forwards : 
#include "Utils.h"
#include "imgui_extension.h"

ProgramFactory::ProgramFactory()
{
	//////////////////// PARTICLE shaders ////////////////////////
	// Try to load and compile shaders
	GLuint vertShaderId_particle = compile_shader_from_file(GL_VERTEX_SHADER, "particle.vert");
	GLuint fragShaderId_particle = compile_shader_from_file(GL_FRAGMENT_SHADER, "particle.frag");

	GLuint programObject_particle = glCreateProgram();
	glAttachShader(programObject_particle, vertShaderId_particle);
	glAttachShader(programObject_particle, fragShaderId_particle);

	const GLchar* feedbackVaryings[] = { "outPositions" }; //TODO others parameters
	glTransformFeedbackVaryings(programObject_particle, 1, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);

	glLinkProgram(programObject_particle);
	if (check_link_error(programObject_particle) < 0)
		exit(1);

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);

	//////////////////// BILLBOARD shaders ////////////////////////
	// Try to load and compile shaders
	GLuint vertShaderId_billboard = compile_shader_from_file(GL_VERTEX_SHADER, "billboard.vert");
	GLuint fragShaderId_billboard = compile_shader_from_file(GL_FRAGMENT_SHADER, "billboard.frag");

	GLuint programObject_billboard = glCreateProgram();
	glAttachShader(programObject_billboard, vertShaderId_billboard);
	glAttachShader(programObject_billboard, fragShaderId_billboard);

	glLinkProgram(programObject_billboard);
	if (check_link_error(programObject_billboard) < 0)
		exit(1);

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);

	//////////////////// SKYBOX shaders ////////////////////////
	// Try to load and compile shaders
	GLuint vertShaderId_skybox = compile_shader_from_file(GL_VERTEX_SHADER, "skybox.vert");
	GLuint fragShaderId_skybox = compile_shader_from_file(GL_FRAGMENT_SHADER, "skybox.frag");

	GLuint programObject_skybox = glCreateProgram();
	glAttachShader(programObject_skybox, vertShaderId_skybox);
	glAttachShader(programObject_skybox, fragShaderId_skybox);

	glLinkProgram(programObject_skybox);
	if (check_link_error(programObject_skybox) < 0)
		exit(1);

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);

	//////////////////// 3D Gpass shaders ////////////////////////
	// Try to load and compile shaders
	GLuint vertShaderId_gpass = compile_shader_from_file(GL_VERTEX_SHADER, "aogl.vert");
	GLuint fragShaderId_gpass = compile_shader_from_file(GL_FRAGMENT_SHADER, "aogl_gPass.frag");

	GLuint programObject_gPass = glCreateProgram();
	glAttachShader(programObject_gPass, vertShaderId_gpass);
	glAttachShader(programObject_gPass, fragShaderId_gpass);

	glLinkProgram(programObject_gPass);
	if (check_link_error(programObject_gPass) < 0)
		exit(1);

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);

	//////////////////// WIREFRAME shaders ////////////////////////
	// Try to load and compile shaders
	GLuint vertShaderId_wireframe = compile_shader_from_file(GL_VERTEX_SHADER, "wireframe.vert");
	GLuint fragShaderId_wireframe = compile_shader_from_file(GL_FRAGMENT_SHADER, "wireframe.frag");

	GLuint programObject_wireframe = glCreateProgram();
	glAttachShader(programObject_wireframe, vertShaderId_wireframe);
	glAttachShader(programObject_wireframe, fragShaderId_wireframe);

	glLinkProgram(programObject_wireframe);
	if (check_link_error(programObject_wireframe) < 0)
		exit(1);

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);

	//////////////////// TERRAIN shaders ////////////////////////
	// Try to load and compile shaders
	GLuint vertShaderId_terrain = compile_shader_from_file(GL_VERTEX_SHADER, "terrain.vert");
	GLuint fragShaderId_terrain = compile_shader_from_file(GL_FRAGMENT_SHADER, "terrain.frag");

	GLuint programObject_terrain = glCreateProgram();
	glAttachShader(programObject_terrain, vertShaderId_terrain);
	glAttachShader(programObject_terrain, fragShaderId_terrain);

	glLinkProgram(programObject_terrain);
	if (check_link_error(programObject_terrain) < 0)
		exit(1);

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);

	//////////////////// TERRAIN EDITION shaders ////////////////////////
	// Try to load and compile shaders
	GLuint vertShaderId_terrainEdition = compile_shader_from_file(GL_VERTEX_SHADER, "terrainEdition.vert");
	GLuint fragShaderId_terrainEdition = compile_shader_from_file(GL_FRAGMENT_SHADER, "terrainEdition.frag");

	GLuint programObject_terrainEdition = glCreateProgram();
	glAttachShader(programObject_terrainEdition, vertShaderId_terrainEdition);
	glAttachShader(programObject_terrainEdition, fragShaderId_terrainEdition);

	glLinkProgram(programObject_terrainEdition);
	if (check_link_error(programObject_terrainEdition) < 0)
		exit(1);

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);

	//////////////////// DRAW ON TEXTURE shaders ////////////////////////
	// Try to load and compile shaders
	GLuint vertShaderId_drawOnTexture = compile_shader_from_file(GL_VERTEX_SHADER, "drawOnTexture.vert");
	GLuint fragShaderId_drawOnTexture = compile_shader_from_file(GL_FRAGMENT_SHADER, "drawOnTexture.frag");

	GLuint programObject_drawOnTexture = glCreateProgram();
	glAttachShader(programObject_drawOnTexture, vertShaderId_drawOnTexture);
	glAttachShader(programObject_drawOnTexture, fragShaderId_drawOnTexture);

	glLinkProgram(programObject_drawOnTexture);
	if (check_link_error(programObject_drawOnTexture) < 0)
		exit(1);

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);

	//////////////////// GRASS FIELD shaders ////////////////////////
	// Try to load and compile shaders
	GLuint vertShaderId_grassField = compile_shader_from_file(GL_VERTEX_SHADER, "grassField.vert");
	GLuint fragShaderId_grassField = compile_shader_from_file(GL_FRAGMENT_SHADER, "grassField.frag");

	GLuint programObject_grassField = glCreateProgram();
	glAttachShader(programObject_grassField, vertShaderId_grassField);
	glAttachShader(programObject_grassField, fragShaderId_grassField);

	glLinkProgram(programObject_grassField);
	if (check_link_error(programObject_grassField) < 0)
		exit(1);

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);

	//////////////////// GRASS FIELD shaders ////////////////////////
	// Try to load and compile shaders
	GLuint vertShaderId_wireframeInstanced = compile_shader_from_file(GL_VERTEX_SHADER, "wireframeInstanced.vert");
	GLuint fragShaderId_wireframeInstanced = compile_shader_from_file(GL_FRAGMENT_SHADER, "wireframeInstanced.frag");

	GLuint programObject_wireframeInstanced = glCreateProgram();
	glAttachShader(programObject_wireframeInstanced, vertShaderId_wireframeInstanced);
	glAttachShader(programObject_wireframeInstanced, fragShaderId_wireframeInstanced);

	glLinkProgram(programObject_wireframeInstanced);
	if (check_link_error(programObject_wireframeInstanced) < 0)
		exit(1);

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);

	m_programs["defaultLit"] = programObject_gPass;
	m_programs["defaultUnlit"] = programObject_wireframe;
	m_programs["defaultSkybox"] = programObject_skybox;
	m_programs["defaultTerrain"] = programObject_terrain;
	m_programs["defaultTerrainEdition"] = programObject_terrainEdition;
	m_programs["defaultDrawOnTexture"] = programObject_drawOnTexture;
	m_programs["defaultGrassField"] = programObject_grassField;
	m_programs["wireframeInstanced"] = programObject_wireframeInstanced;
	m_programs["defaultBillboard"] = programObject_billboard;

	m_defaults.push_back("defaultLit");
	m_defaults.push_back("defaultUnlit");
	m_defaults.push_back("defaultSkybox");
	m_defaults.push_back("defaultTerrain");
	m_defaults.push_back("defaultTerrainEdition");
	m_defaults.push_back("defaultDrawOnTexture");
	m_defaults.push_back("defaultGrassField");
	m_defaults.push_back("wireframeInstanced");
	m_defaults.push_back("defaultBillboard");
}

void ProgramFactory::add(const std::string& name, GLuint programId)
{
	if (std::find(m_defaults.begin(), m_defaults.end(), name) != m_defaults.end()) //can't override default key
		return;

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
	for (auto& it = m_programs.begin(); it != m_programs.end();)
	{
		if (std::find(m_defaults.begin(), m_defaults.end(), it->first) == m_defaults.end()) // we keep defaults alive
		{
			glDeleteProgram(it->second);
			it = m_programs.erase(it);
		}
		else
			it++;
	}
}

void ProgramFactory::save(Json::Value & objectRoot) const
{
	//objectRoot["size"] = m_programs.size();
	//int i = 0;
	//for (auto it = m_programs.begin(); it != m_programs.end(); it++)
	//{
	//	objectRoot["keys"][i] = it->first;
	//	objectRoot["values"][i] = it->second;
	//	i++;
	//}
}

void ProgramFactory::load(Json::Value & entityRoot)
{
	//int size = entityRoot.get("size", 0).asInt();
	//for (int i = 0; i < size; i++)
	//{
	//	std::string key = entityRoot["keys"][i].asString();
	//	GLuint glId = entityRoot["values"][i].asInt();
	//	m_programs[key] = glId;
	//}
}

///////////////////////////////////////

TextureFactory::TextureFactory()
{
	auto newTex = new Texture(255, 255, 255);
	newTex->initGL();
	
	newTex->name = "default";
	m_textures["default"] = newTex;

	newTex = new Texture(0, 0, 125);
	newTex->initGL();

	newTex->name = "defaultNormal";
	m_textures["defaultNormal"] = newTex;
}

void TextureFactory::add(const std::string& name, const std::string& path)
{
	if (name == "default" || name == "defaultNormal") //can't override default key
		return;

	auto newTexture = new Texture(path);

	newTexture->name = name;
	m_textures[name] = newTexture;
}

void TextureFactory::add(const std::string& name, Texture* texture)
{
	if (name == "default" || name == "defaultNormal") //can't override default key
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

	ImGui::InputFilePath("path", path, 50);

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

	ImGui::InputFilePath("path", paths[0], 50);
	ImGui::InputFilePath("path", paths[2], 50);
	ImGui::InputFilePath("path", paths[3], 50);
	ImGui::InputFilePath("path", paths[4], 50);
	ImGui::InputFilePath("path", paths[5], 50);

	//ImGui::InputText("path right", paths[0], 50);
	//ImGui::InputText("path left", paths[1], 50);
	//ImGui::InputText("path top", paths[2], 50);
	//ImGui::InputText("path bottom", paths[3], 50);
	//ImGui::InputText("path back", paths[4], 50);
	//ImGui::InputText("path front", paths[5], 50);

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
	cube->path = "";
	cube->computeBoundingBox();


	Mesh* cubeWireFrame = new Mesh(GL_LINE_STRIP, (Mesh::USE_INDEX | Mesh::USE_VERTICES));
	cubeWireFrame->triangleIndex = { 0, 1, 2, 2, 1, 3, 4, 5, 6, 6, 5, 7, 8, 9, 10, 10, 9, 11, 12, 13, 14, 14, 13, 15, 16, 17, 18, 19, 17, 20, 21, 22, 23, 24, 25, 26, };
	cubeWireFrame->uvs = { 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f,  1.f, 0.f,  1.f, 1.f,  0.f, 1.f,  1.f, 1.f,  0.f, 0.f, 0.f, 0.f, 1.f, 1.f,  1.f, 0.f, };
	cubeWireFrame->vertices = { -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5 };
	cubeWireFrame->normals = { 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, };
	cubeWireFrame->initGl();
	cubeWireFrame->name = "cubeWireframe";
	cubeWireFrame->path = "";
	cubeWireFrame->computeBoundingBox();

	Mesh* plane = new Mesh();
	plane->triangleIndex = { 0, 1, 2, 2, 3, 0 };
	plane->uvs = { 0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f };
	plane->vertices = { -0.5, 0.0, -0.5, 0.5, 0.0, -0.5, 0.5, 0.0, 0.5, -0.5, 0.0, 0.5};
	plane->normals = { 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0 };
	plane->initGl();
	plane->name = "plane";
	plane->path = "";
	plane->computeBoundingBox();

	Mesh* quad = new Mesh(GL_TRIANGLES, (Mesh::USE_INDEX | Mesh::USE_NORMALS | Mesh::USE_UVS | Mesh::USE_VERTICES), 2);
	quad->triangleIndex = { 0, 1, 2, 0, 2, 3 };
	quad->uvs = { 0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f };
	quad->vertices = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
	quad->normals = { 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0 };
	quad->initGl();
	quad->name = "quad";
	quad->path = "";
	//quad->computeBoundingBox();

	m_meshes["default"] = cube;
	m_meshes["cube"] = cube;
	m_meshes["cubeWireframe"] = cubeWireFrame;
	m_meshes["plane"] = plane;
	m_meshes["quad"] = plane;

	m_defaults.push_back("default");
	m_defaults.push_back("cube");
	m_defaults.push_back("cubeWireframe");
	m_defaults.push_back("plane");
	m_defaults.push_back("quad");
}

void MeshFactory::add(const std::string& name, Mesh* mesh)
{
	if (std::find(m_defaults.begin(), m_defaults.end(), name) != m_defaults.end()) //can't override default key
		return;

	mesh->name = name;
	m_meshes[name] = mesh;
}

void MeshFactory::add(const std::string & name, const std::string & path)
{
	if (std::find(m_defaults.begin(), m_defaults.end(), name) != m_defaults.end()) //can't override default key
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
	ImGui::InputFilePath("path", path, 50);
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
		if (std::find(m_defaults.begin(), m_defaults.end(), it->first) == m_defaults.end()) // we keep defaults alive
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
		entityRoot["values"][i] = it->second != nullptr ? it->second->path : "";
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

MaterialFactory::MaterialFactory()
{
	Material* newMat = new MaterialLit(ProgramFactory::get().get("defaultLit"), TextureFactory::get().get("default"), TextureFactory::get().get("default"), TextureFactory::get().get("default"), 50);
	newMat->name = "default";
	m_materials["default"] = newMat;

	newMat = new MaterialUnlit(ProgramFactory::get().get("defaultUnlit"));
	newMat->name = "wireframe";
	m_materials["wireframe"] = newMat;

	newMat = new MaterialInstancedUnlit(ProgramFactory::get().get("wireframeInstanced"));
	newMat->name = "wireframeInstanced";
	m_materials["wireframeInstanced"] = newMat;

	newMat = new MaterialGrassField(ProgramFactory::get().get("defaultGrassField"));
	newMat->name = "grassfield";
	m_materials["grassfield"] = newMat;

	newMat = new MaterialBillboard(ProgramFactory::get().get("defaultBillboard"));
	newMat->name = "billboard";
	m_materials["billboard"] = newMat;
	
	m_defaults.push_back("default");
	m_defaults.push_back("wireframe");
	m_defaults.push_back("wireframeInstanced");
	m_defaults.push_back("grassfield");
	m_defaults.push_back("billboard");
}

void MaterialFactory::add(const std::string& name, Material* material)
{
	if (std::find(m_defaults.begin(), m_defaults.end(), name) != m_defaults.end()) //can't override default key
		return;

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
		if (ImGui::CollapsingHeader(m.first.c_str()))
		{
			m.second->drawUI();
		}
	}

	ImGui::PopID();
}

void MaterialFactory::clear()
{
	for (auto& it = m_materials.begin(); it != m_materials.end();)
	{
		if (std::find(m_defaults.begin(), m_defaults.end(), it->first) == m_defaults.end()) // we keep defaults alive
		{
			delete it->second;
			it = m_materials.erase(it);
		}
		else
			it++;
	}
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
		if (std::find(m_defaults.begin(), m_defaults.end(), name) == m_defaults.end()) //we don't save default materials
		{
			entityRoot["keys"][i] = litMaterials[i]->name;
			litMaterials[i]->save(entityRoot["values"][i]);
		}
	}
}

void MaterialFactory::load(Json::Value & entityRoot)
{
	//we only load lit materials : 
	int size = entityRoot.get("size", 0).asInt();
	for (int i = 0; i < size; i++)
	{
		std::string materialName = entityRoot["keys"][i].asString();
		if (std::find(m_defaults.begin(), m_defaults.end(), name) == m_defaults.end()) //we don't load default materials
		{
			MaterialLit* newMaterial = new MaterialLit();
			newMaterial->load(entityRoot["values"][i]);
			add(materialName, newMaterial);
		}
	}
}
