#include "stdafx.h"

#include "Materials.h"
#include "Factories.h"//forward
#include "EditorGUI.h"

Material::Material()
	: m_glProgramId(0)
{

}

Material::Material(const std::string& glProgramName, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters)
	: m_glProgramId(glProgramId)
	, m_internalParameters(internalParameters)
	, m_glProgramName(glProgramName)
{
	getProgramFactory().get(m_glProgramName)->addMaterialRef(this);
}

Material::Material(const std::string& glProgramName, GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
	: m_glProgramId(glProgramId)
	, m_internalParameters(internalParameters)
	, m_glProgramName(glProgramName)
{
	setExternalParameters(externalParameters);

	getProgramFactory().get(m_glProgramName)->addMaterialRef(this);
}

Material::~Material()
{
	getProgramFactory().get(m_glProgramName)->removeMaterialRef(this);
}

void Material::init(const FileHandler::CompletePath& path)
{
	Material::init(path);

	std::ifstream stream;
	stream.open(path.toString());
	if (!stream.is_open())
	{
		std::cout << "error, can't load material at path : " << path.toString() << std::endl;
		return;
	}
	Json::Value root;
	stream >> root;

	load(root);
}

void Material::drawUI()
{
	for (auto& parameter : m_internalParameters)
	{
		parameter->drawUI();
	}
}

void Material::pushInternalsToGPU(int& boundTextureCount)
{
	for (auto& parameter : m_internalParameters)
	{
		parameter->pushToGPU(boundTextureCount);
	}
}

void Material::setExternalParameters(std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
{
	for (int i = 0; i < externalParameters.size(); i++)
	{
		m_externalParameters[externalParameters[i]->getName()] = externalParameters[i];
	}
}

void Material::loadFromShaderProgramDatas(GLuint glProgramId, std::vector<std::shared_ptr<InternalShaderParameterBase>>& internalParameters, std::vector<std::shared_ptr<ExternalShaderParameterBase>>& externalParameters)
{
	m_internalParameters.clear();
	m_externalParameters.clear();

	m_glProgramId = glProgramId;
	m_internalParameters = internalParameters;

	setExternalParameters(externalParameters);
}

void Material::save(Json::Value & entityRoot) const
{
	entityRoot["shaderProgramName"] = m_glProgramName;

	int parameterIdx = 0;
	for (auto& parameter : m_internalParameters)
	{
		parameter->save(entityRoot["internalParameters"][parameterIdx]);
		parameterIdx++;
	}
}

void Material::load(Json::Value & entityRoot)
{
	std::string shaderProgramName = entityRoot.get("shaderProgramName", "").asString();
	assert(shaderProgramName != "");

	getProgramFactory().get(shaderProgramName)->LoadMaterialInstance(this);

	int parameterIdx = 0;
	for (auto& parameter : m_internalParameters)
	{
		parameter->load(entityRoot["internalParameters"][parameterIdx]);
		parameterIdx++;
	}

	getProgramFactory().get(m_glProgramName)->addMaterialRef(this);
}

void Material::save(const FileHandler::CompletePath& path) const
{
	std::ofstream stream;
	stream.open(path.toString());
	if (!stream.is_open())
	{
		std::cout << "error, can't save material at path : " << path.toString() << std::endl;
		return;
	}
	Json::Value root;
	save(root);

	stream << root;
}

//Material::Material(GLuint _glProgram) 
//	: glProgram(_glProgram)
//{
//	assert(glProgram > 0);
//}
//
//Material::Material(ResourcePtr<ShaderProgram> _glProgram) 
//	: Material(_glProgram->id)
//{
//
//}
//
//void Material::init(const FileHandler::CompletePath & path)
//{
//	Resource::init(path);
//}
//
//Material::~Material()
//{
//}


///////////////////////////////////////////

/*

Material3DObject::Material3DObject(GLuint _glProgram) 
	: Material(_glProgram)
{
	uniform_MVP = glGetUniformLocation(glProgram, "MVP");
	uniform_normalMatrix = glGetUniformLocation(glProgram, "NormalMatrix");
	for (int i = 0; i < MAX_BONE_COUNT; i++)
		uniform_bonesTransform[i] = glGetUniformLocation(glProgram, ("BonesTransform["+std::to_string(i)+"]").c_str() );
	uniform_useSkeleton = glGetUniformLocation(glProgram, "UseSkeleton");
}

Material3DObject::Material3DObject(ResourcePtr<ShaderProgram> _glProgram)
	: Material3DObject(_glProgram->id)
{}


void Material3DObject::setUniform_MVP(glm::mat4& mvp)
{
	glUniformMatrix4fv(uniform_MVP, 1, false, glm::value_ptr(mvp));
}

void Material3DObject::setUniform_normalMatrix(glm::mat4& normalMatrix)
{
	glUniformMatrix4fv(uniform_normalMatrix, 1, false, glm::value_ptr(normalMatrix));
}

void Material3DObject::setUniformBonesTransform(unsigned int idx, const glm::mat4& boneTransform)
{
	assert(idx < MAX_BONE_COUNT);
	glUniformMatrix4fv(uniform_bonesTransform[idx], 1, false, glm::value_ptr(boneTransform));
}

void Material3DObject::setUniformUseSkeleton(bool useSkeleton)
{
	glUniform1i(uniform_useSkeleton, useSkeleton);
}

///////////////////////////////////////////

MaterialLit::MaterialLit() 
	: Material3DObject(getProgramFactory().getDefault("defaultLit"))
	, textureDiffuse(getTextureFactory().getDefault("default"))
	, specularPower(10)
	, textureSpecular(getTextureFactory().getDefault("default"))
	, textureBump(getTextureFactory().getDefault("default"))
	, textureRepetition(1, 1)
{
	diffuseTextureName = textureDiffuse->name;
	specularTextureName = textureSpecular->name;
	bumpTextureName = textureBump->name;

	uniform_textureDiffuse = glGetUniformLocation(glProgram, "Diffuse");
	uniform_textureSpecular = glGetUniformLocation(glProgram, "Specular");
	uniform_textureBump = glGetUniformLocation(glProgram, "Bump");
	uniform_specularPower = glGetUniformLocation(glProgram, "specularPower");
	uniform_textureRepetition = glGetUniformLocation(glProgram, "TextureRepetition");

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);
}

MaterialLit::MaterialLit(GLuint _glProgram, ResourcePtr<Texture> _textureDiffuse, ResourcePtr<Texture> _textureSpecular, ResourcePtr<Texture> _textureBump, float _specularPower)
	: Material3DObject(_glProgram)
	, textureDiffuse(_textureDiffuse)
	, specularPower(_specularPower)
	, textureSpecular(_textureSpecular)
	, textureBump(_textureBump)
	, textureRepetition(1, 1)
{
	diffuseTextureName = textureDiffuse->name;
	specularTextureName = textureSpecular->name;
	bumpTextureName = textureBump->name;

	uniform_textureDiffuse = glGetUniformLocation(glProgram, "Diffuse");
	uniform_textureSpecular = glGetUniformLocation(glProgram, "Specular");
	uniform_textureBump = glGetUniformLocation(glProgram, "Bump");
	uniform_specularPower = glGetUniformLocation(glProgram, "specularPower");
	uniform_textureRepetition = glGetUniformLocation(glProgram, "TextureRepetition");

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);

	//textureDiffuse->initGL(); // we consider that each texture on a material will be used on the sceen and should be send to the GPU.
	//textureBump->initGL();
	//textureSpecular->initGL();
}

MaterialLit::MaterialLit(ResourcePtr<ShaderProgram> _glProgram, ResourcePtr<Texture> _textureDiffuse, ResourcePtr<Texture> _textureSpecular, ResourcePtr<Texture> _textureBump, float _specularPower)
	: MaterialLit(_glProgram->id, _textureDiffuse, _textureSpecular, _textureBump, _specularPower)
{

}

void MaterialLit::init(const FileHandler::CompletePath & path)
{
	Material::init(path);

	std::ifstream stream;
	stream.open(path.toString());
	if (!stream.is_open())
	{
		std::cout << "error, can't load material at path : " << path.toString() << std::endl;
		return;
	}
	Json::Value root;
	stream >> root;

	load(root);
}

void MaterialLit::setDiffuse(ResourcePtr<Texture> _textureDiffuse)
{
	diffuseTextureName = _textureDiffuse->name;
	textureDiffuse = _textureDiffuse;
}

void MaterialLit::setSpecular(ResourcePtr<Texture> _textureSpecular)
{
	specularTextureName = _textureSpecular->name;
	textureSpecular = _textureSpecular;
}

void MaterialLit::setBump(ResourcePtr<Texture> _textureBump)
{
	bumpTextureName = _textureBump->name;
	textureBump = _textureBump;
}

ResourcePtr<Texture> MaterialLit::getDiffuse() const
{
	return textureDiffuse;
}

ResourcePtr<Texture> MaterialLit::getSpecular() const
{
	return textureSpecular;
}

ResourcePtr<Texture> MaterialLit::getBump() const
{
	return textureBump;
}

float MaterialLit::getSpecularPower() const
{
	return specularPower;
}

void MaterialLit::use()
{
	//bind shaders
	glUseProgram(glProgram);

	//bind textures into texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureDiffuse->glId);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureSpecular->glId);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textureBump->glId);

	//send uniforms
	glUniform1f(uniform_specularPower, specularPower);
	glUniform1i(uniform_textureDiffuse, 0);
	glUniform1i(uniform_textureSpecular, 1);
	glUniform1i(uniform_textureBump, 2);
	glUniform2fv(uniform_textureRepetition, 1, glm::value_ptr(textureRepetition));
}

void MaterialLit::drawUI()
{
	ImGui::InputFloat("specular power", &specularPower);

	ImGui::InputFloat2("texture repetition", &textureRepetition[0]);

	char tmpTxt01[100];
	diffuseTextureName.copy(tmpTxt01, glm::min(100, (int)diffuseTextureName.size()), 0);
	tmpTxt01[diffuseTextureName.size()] = '\0';
	//%NOCOMMIT%
	//if (ImGui::InputText("diffuse texture name", tmpTxt01, 20))
	//{
	//	diffuseTextureName = tmpTxt01;

	//	if (getTextureFactory().contains(tmpTxt01))
	//	{
	//		//textureDiffuse->freeGL();
	//		textureDiffuse = getTextureFactory().get(diffuseTextureName);
	//		textureDiffuse->initGL();
	//	}
	//}

	ResourcePtr<Texture> texturePtrQuery;
	EditorGUI::ResourceField<Texture>(texturePtrQuery, "diffuse texture name", tmpTxt01, 100);
	if (texturePtrQuery.isValid())
	{
		textureDiffuse = texturePtrQuery;
		textureDiffuse->initGL();
		texturePtrQuery.reset();
	}

	char tmpTxt02[100];
	specularTextureName.copy(tmpTxt02, glm::min(100, (int)specularTextureName.size()), 0);
	tmpTxt02[specularTextureName.size()] = '\0';
	//%NOCOMMIT%
	//if (ImGui::InputText("specular texture name", tmpTxt02, 20))
	//{
	//	specularTextureName = tmpTxt02;

	//	if (getTextureFactory().contains(specularTextureName))
	//	{
	//		//textureSpecular->freeGL();
	//		textureSpecular = getTextureFactory().get(specularTextureName);
	//		textureSpecular->initGL();
	//	}
	//}

	EditorGUI::ResourceField<Texture>(texturePtrQuery, "specular texture name", tmpTxt02, 100);
	if (texturePtrQuery.isValid())
	{
		textureSpecular = texturePtrQuery;
		textureSpecular->initGL();
		texturePtrQuery.reset();
	}

	char tmpTxt03[100];
	bumpTextureName.copy(tmpTxt03, glm::min(100, (int)bumpTextureName.size()), 0);
	tmpTxt03[bumpTextureName.size()] = '\0';
	//%NOCOMMIT%
	//if (ImGui::InputText("bump texture name", tmpTxt03, 20))
	//{
	//	bumpTextureName = tmpTxt03;

	//	if (getTextureFactory().contains(bumpTextureName))
	//	{
	//		//textureBump->freeGL();
	//		textureBump = getTextureFactory().get(bumpTextureName);
	//		textureBump->initGL();
	//	}
	//}

	EditorGUI::ResourceField<Texture>(texturePtrQuery, "bump texture name", tmpTxt03, 100);
	if (texturePtrQuery.isValid())
	{
		textureBump = texturePtrQuery;
		textureBump->initGL();
		texturePtrQuery.reset();
	}
}

void MaterialLit::initGL()
{
	if(textureDiffuse.isValid())
		textureDiffuse->initGL();

	if(textureSpecular.isValid())
		textureSpecular->initGL();

	if(textureBump.isValid())
		textureBump->initGL();
}

void MaterialLit::save(Json::Value & objectRoot) const
{
	objectRoot["textureDiffuseName"] = textureDiffuse->name;
	objectRoot["textureSpecularName"] = textureSpecular->name;
	objectRoot["textureBumpName"] = textureBump->name;
	objectRoot["specularPower"] = specularPower;
	objectRoot["textureRepetition"] = toJsonValue(textureRepetition);
}

void MaterialLit::load(Json::Value & objectRoot)
{
	glProgram = getProgramFactory().getDefault("defaultLit")->id;
	std::string textureDiffuseName = objectRoot.get("textureDiffuseName", "default").asString();
	textureDiffuse = getTextureFactory().get(textureDiffuseName);
	std::string textureSpecularName = objectRoot.get("textureSpecularName", "default").asString();
	textureSpecular = getTextureFactory().get(textureSpecularName);
	std::string textureBumpName = objectRoot.get("textureBumpName", "default").asString();
	textureBump = getTextureFactory().get(textureBumpName);
	specularPower = objectRoot.get("specularPower", 10).asFloat();
	textureRepetition = fromJsonValue<glm::vec2>(objectRoot["textureRepetition"], glm::vec2(1,1));

	diffuseTextureName = textureDiffuse->name;
	specularTextureName = textureSpecular->name;
	bumpTextureName = textureBump->name;

	uniform_textureDiffuse = glGetUniformLocation(glProgram, "Diffuse");
	uniform_textureSpecular = glGetUniformLocation(glProgram, "Specular");
	uniform_textureBump = glGetUniformLocation(glProgram, "Bump");
	uniform_specularPower = glGetUniformLocation(glProgram, "specularPower");
	uniform_textureRepetition = glGetUniformLocation(glProgram, "TextureRepetition");

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);
}


//////////////////////////////////////////////////

MaterialUnlit::MaterialUnlit(GLuint _glProgram) : Material3DObject(_glProgram)
{
	uniform_color = glGetUniformLocation(glProgram, "Color");
}

void MaterialUnlit::setUniform_color(glm::vec3 color)
{
	glUniform3fv(uniform_color, 1, glm::value_ptr(color));
}

void MaterialUnlit::use()
{
	//bind shaders
	glUseProgram(glProgram);
}

void MaterialUnlit::drawUI()
{
	//nothing
}

void MaterialUnlit::initGL()
{
	//nothing
}


//////////////////////////////////////////////////

MaterialInstancedUnlit::MaterialInstancedUnlit(GLuint _glProgram) : Material(_glProgram)
{
	uniform_color = glGetUniformLocation(glProgram, "Color");
	uniform_VP = glGetUniformLocation(glProgram, "VP");
}

void MaterialInstancedUnlit::setUniform_color(glm::vec3 color)
{
	glUniform3fv(uniform_color, 1, glm::value_ptr(color));
}

void MaterialInstancedUnlit::setUniform_VP(const glm::mat4& VP)
{
	glUniformMatrix4fv(uniform_VP, 1, false, glm::value_ptr(VP));
}

void MaterialInstancedUnlit::use()
{
	//bind shaders
	glUseProgram(glProgram);
}

void MaterialInstancedUnlit::drawUI()
{
	//nothing
}

void MaterialInstancedUnlit::initGL()
{
	//nothing
}

///////////////////////////////////////////////

MaterialDebugDrawer::MaterialDebugDrawer(GLuint _glProgram) : Material(_glProgram)
{
	uniform_MVP = glGetUniformLocation(glProgram, "MVP");
}

void MaterialDebugDrawer::setUniform_MVP(const glm::mat4 & MVP)
{
	glUniformMatrix4fv(uniform_MVP, 1, false, glm::value_ptr(MVP));
}

void MaterialDebugDrawer::use()
{
	//bind shaders
	glUseProgram(glProgram);
}

void MaterialDebugDrawer::drawUI()
{
}

void MaterialDebugDrawer::initGL()
{
}


///////////////////////////////////////////////

MaterialSkybox::MaterialSkybox() : Material(getProgramFactory().getDefault("defaultSkybox")), textureDiffuse(getCubeTextureFactory().getDefault("default"))
{
	diffuseTextureName = textureDiffuse->name;

	uniform_textureDiffuse = glGetUniformLocation(glProgram, "Diffuse");

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);
}

MaterialSkybox::MaterialSkybox(GLuint _glProgram, ResourcePtr<CubeTexture> _textureDiffuse) : Material(_glProgram), textureDiffuse(_textureDiffuse)
{
	diffuseTextureName = textureDiffuse->name;

	uniform_textureDiffuse = glGetUniformLocation(glProgram, "Diffuse");
	uniform_VP = glGetUniformLocation(glProgram, "VP");

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);
}

void MaterialSkybox::setUniform_VP(const glm::mat4 & vp)
{
	glUniformMatrix4fv(uniform_VP, 1, false, glm::value_ptr(vp));
}

void MaterialSkybox::use()
{
	//bind shaders
	glUseProgram(glProgram);

	//bind textures into texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureDiffuse->glId);

	//send uniforms
	glUniform1i(uniform_textureDiffuse, 0);
}

void MaterialSkybox::drawUI()
{
	char tmpTxt[100];
	diffuseTextureName.copy(tmpTxt, glm::min(100, (int)diffuseTextureName.size()), 0);
	tmpTxt[diffuseTextureName.size()] = '\0';
	//%NOCOMMIT%
	//if (ImGui::InputText("diffuse texture name", tmpTxt, 20))
	//{
	//	diffuseTextureName = tmpTxt;

	//	if (getCubeTextureFactory().contains(tmpTxt))
	//	{
	//		textureDiffuse = getCubeTextureFactory().get(diffuseTextureName);
	//		textureDiffuse->initGL();
	//	}
	//}

	ResourcePtr<CubeTexture> texturePtrQuery;
	EditorGUI::ResourceField<CubeTexture>(texturePtrQuery, "diffuse texture name", tmpTxt, 100);
	if (texturePtrQuery.isValid())
	{
		textureDiffuse = texturePtrQuery;
		textureDiffuse->initGL();
		texturePtrQuery.reset();
	}
}

void MaterialSkybox::initGL()
{
	if (textureDiffuse.isValid())
		textureDiffuse->initGL();
}

void MaterialSkybox::setDiffuseTexture(ResourcePtr<CubeTexture> texture)
{
	diffuseTextureName = texture->name;
	textureDiffuse = texture;
}

ResourcePtr<CubeTexture> MaterialSkybox::getDiffuseTexture() const
{
	return textureDiffuse;
}

/////////////////// SHADOW MATERIAL ////////////////////

MaterialShadow::MaterialShadow()
{

}

MaterialShadow::MaterialShadow(GLuint _glProgram) : Material(_glProgram)
{

}

void MaterialShadow::use()
{
	//bind shaders
	glUseProgram(glProgram);
}

void MaterialShadow::drawUI()
{
	//nothing
}

void MaterialShadow::initGL()
{
	//nothing
}


////////////////////////////////////////////////////////


MaterialTerrain::MaterialTerrain() : Material3DObject(getProgramFactory().getDefault("defaultLit")), textureDiffuse(getTextureFactory().getDefault("default")), specularPower(10), textureSpecular(getTextureFactory().getDefault("default")), textureBump(getTextureFactory().getDefault("default")), textureRepetition(1, 1)
{
	diffuseTextureName = textureDiffuse->name;
	specularTextureName = textureSpecular->name;
	bumpTextureName = textureBump->name;

	uniform_textureDiffuse = glGetUniformLocation(glProgram, "Diffuse");
	uniform_textureSpecular = glGetUniformLocation(glProgram, "Specular");
	uniform_textureBump = glGetUniformLocation(glProgram, "Bump");
	uniform_specularPower = glGetUniformLocation(glProgram, "specularPower");
	uniform_textureRepetition = glGetUniformLocation(glProgram, "TextureRepetition");

	uniform_textureFilter = glGetUniformLocation(glProgram, "FilterTexture");
	uniform_filterValues = glGetUniformLocation(glProgram, "FilterValues");

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);
}

MaterialTerrain::MaterialTerrain(GLuint _glProgram) :
	Material3DObject(_glProgram), textureDiffuse(nullptr), specularPower(50.f), textureSpecular(nullptr), textureBump(nullptr), textureRepetition(1, 1)
{
	diffuseTextureName = "diffuse";
	specularTextureName = "specular";
	bumpTextureName = "bump";

	uniform_textureDiffuse = glGetUniformLocation(glProgram, "Diffuse");
	uniform_textureSpecular = glGetUniformLocation(glProgram, "Specular");
	uniform_textureBump = glGetUniformLocation(glProgram, "Bump");
	uniform_specularPower = glGetUniformLocation(glProgram, "specularPower");
	uniform_textureRepetition = glGetUniformLocation(glProgram, "TextureRepetition");

	uniform_textureFilter = glGetUniformLocation(glProgram, "FilterTexture");
	uniform_filterValues = glGetUniformLocation(glProgram, "FilterValues");

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);
}

void MaterialTerrain::use()
{
	//bind shaders
	glUseProgram(glProgram);

	
	////bind textures into texture units
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, textureDiffuse->glId);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, textureSpecular->glId);
	//glActiveTexture(GL_TEXTURE2);
	//glBindTexture(GL_TEXTURE_2D, textureBump->glId);

	////send uniforms
	//glUniform1f(uniform_specularPower, specularPower);
	//glUniform1i(uniform_textureDiffuse, 0);
	//glUniform1i(uniform_textureSpecular, 1);
	//glUniform1i(uniform_textureBump, 2);
	//glUniform2fv(uniform_textureRepetition, 1, glm::value_ptr(textureRepetition));
	
}

void MaterialTerrain::drawUI()
{
	ImGui::InputFloat("specular power", &specularPower);

	ImGui::InputFloat2("texture repetition", &textureRepetition[0]);
}

void MaterialTerrain::initGL()
{
	if(textureDiffuse.isValid())
		textureDiffuse->initGL();

	if(textureSpecular.isValid())
		textureSpecular->initGL();

	if(textureBump.isValid())
		textureBump->initGL();
}

void MaterialTerrain::setUniformLayoutOffset(const glm::vec2& layoutOffset)
{
	glUniform2fv(uniform_filterValues, 1, glm::value_ptr(layoutOffset));
}

void MaterialTerrain::setUniformFilterTexture(int textureId)
{
	glUniform1i(uniform_textureFilter, textureId);
}

void MaterialTerrain::setUniformTextureRepetition(const glm::vec2 & textureRepetition)
{
	glUniform2fv(uniform_textureRepetition, 1, glm::value_ptr(textureRepetition));
}

void MaterialTerrain::setUniformDiffuseTexture(int textureId)
{
	glUniform1i(uniform_textureDiffuse, textureId);
}

void MaterialTerrain::setUniformBumpTexture(int textureId)
{
	glUniform1i(uniform_textureBump, textureId);
}

void MaterialTerrain::setUniformSpecularTexture(int textureId)
{
	glUniform1i(uniform_textureSpecular, textureId);
}

void MaterialTerrain::setUniformSpecularPower(float specularPower)
{
	glUniform1f(uniform_specularPower, specularPower);
}


////////////////////////////////////////////////////////


MaterialTerrainEdition::MaterialTerrainEdition()
{
	//diffuseTextureName = textureDiffuse->name;
	//specularTextureName = textureSpecular->name;
	//bumpTextureName = textureBump->name;

	uniform_textureDiffuse = glGetUniformLocation(glProgram, "Diffuse");
	uniform_textureSpecular = glGetUniformLocation(glProgram, "Specular");
	uniform_textureBump = glGetUniformLocation(glProgram, "Bump");
	//uniform_specularPower = glGetUniformLocation(glProgram, "specularPower");
	uniform_textureRepetition = glGetUniformLocation(glProgram, "TextureRepetition");
	uniform_textureFilter = glGetUniformLocation(glProgram, "FilterTexture");
	uniform_filterValues = glGetUniformLocation(glProgram, "FilterValues");

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);

	textureDiffuse->initGL(); // we consider that each texture on a material will be used on the sceen and should be send to the GPU.
}

MaterialTerrainEdition::MaterialTerrainEdition(GLuint _glProgram) : Material(_glProgram)
{
	//diffuseTextureName = textureDiffuse->name;
	//specularTextureName = textureSpecular->name;
	//bumpTextureName = textureBump->name;

	uniform_textureDiffuse = glGetUniformLocation(glProgram, "Diffuse");
	uniform_textureSpecular = glGetUniformLocation(glProgram, "Specular");
	uniform_textureBump = glGetUniformLocation(glProgram, "Bump");
	//uniform_specularPower = glGetUniformLocation(glProgram, "specularPower");
	uniform_textureRepetition = glGetUniformLocation(glProgram, "TextureRepetition");
	uniform_textureFilter = glGetUniformLocation(glProgram, "FilterTexture");
	uniform_filterValues = glGetUniformLocation(glProgram, "FilterValues");

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);
}

void MaterialTerrainEdition::setUniformFilterTexture(int textureId)
{
	glUniform1i(uniform_textureFilter, textureId);
}

void MaterialTerrainEdition::setUniformDiffuseTexture(int textureId)
{
	glUniform1i(uniform_textureDiffuse, textureId);
}

void MaterialTerrainEdition::setUniformBumpTexture(int textureId)
{
	glUniform1i(uniform_textureBump, textureId);
}

void MaterialTerrainEdition::setUniformSpecularTexture(int textureId)
{
	glUniform1i(uniform_textureSpecular, textureId);
}

void MaterialTerrainEdition::setUniformLayoutOffset(const glm::vec2 & layoutOffset)
{
	glUniform2fv(uniform_filterValues, 1, glm::value_ptr(layoutOffset));
}

void MaterialTerrainEdition::setUniformTextureRepetition(const glm::vec2& textureRepetition)
{
	glUniform2fv(uniform_textureRepetition, 1, glm::value_ptr(textureRepetition));
}

void MaterialTerrainEdition::use()
{
	//bind shaders
	glUseProgram(glProgram);
}

void MaterialTerrainEdition::drawUI()
{
	//ImGui::InputFloat("specular power", &specularPower);

	ImGui::InputFloat2("texture repetition", &textureRepetition[0]);

	//char tmpTxt01[30];
	//diffuseTextureName.copy(tmpTxt01, glm::min(30, (int)diffuseTextureName.size()), 0);
	//tmpTxt01[diffuseTextureName.size()] = '\0';
	//if (ImGui::InputText("diffuse texture name", tmpTxt01, 20))
	//{
	//	diffuseTextureName = tmpTxt01;

	//	if (getTextureFactory().contains(tmpTxt01))
	//	{
	//		textureDiffuse->freeGL();
	//		textureDiffuse = getTextureFactory().get(diffuseTextureName);
	//		textureDiffuse->initGL();
	//	}
	//}

	//
	//char tmpTxt02[30];
	//specularTextureName.copy(tmpTxt02, glm::min(30, (int)specularTextureName.size()), 0);
	//tmpTxt02[specularTextureName.size()] = '\0';
	//if (ImGui::InputText("specular texture name", tmpTxt02, 20))
	//{
	//	specularTextureName = tmpTxt02;

	//	if (getTextureFactory().contains(specularTextureName))
	//	{
	//		textureSpecular->freeGL();
	//		textureSpecular = getTextureFactory().get(specularTextureName);
	//		textureSpecular->initGL();
	//	}
	//}

	//char tmpTxt03[30];
	//bumpTextureName.copy(tmpTxt03, glm::min(30, (int)bumpTextureName.size()), 0);
	//tmpTxt03[bumpTextureName.size()] = '\0';
	//if (ImGui::InputText("bump texture name", tmpTxt03, 20))
	//{
	//	bumpTextureName = tmpTxt03;

	//	if (getTextureFactory().contains(bumpTextureName))
	//	{
	//		textureBump->freeGL();
	//		textureBump = getTextureFactory().get(bumpTextureName);
	//		textureBump->initGL();
	//	}
	//}
	//
}

void MaterialTerrainEdition::initGL()
{
	if (textureDiffuse.isValid())
		textureDiffuse->initGL();

	if (textureSpecular.isValid())
		textureSpecular->initGL();

	if (textureBump.isValid())
		textureBump->initGL();
}

MaterialDrawOnTexture::MaterialDrawOnTexture(GLuint _glProgram) : Material(_glProgram)
{
	uniform_colorToDraw = glGetUniformLocation(_glProgram, "DrawColor");
	uniform_drawPosition = glGetUniformLocation(_glProgram, "DrawPosition");
	uniform_drawRadius = glGetUniformLocation(_glProgram, "DrawRadius");
	uniform_textureToDrawOn = glGetUniformLocation(_glProgram, "Texture");
}

void MaterialDrawOnTexture::setUniformDrawPosition(const glm::vec2& position)
{
	glUniform2fv(uniform_drawPosition, 1, glm::value_ptr(position));
}

void MaterialDrawOnTexture::setUniformColorToDraw(const glm::vec4& color)
{
	glUniform4fv(uniform_colorToDraw, 1, glm::value_ptr(color));
}

void MaterialDrawOnTexture::setUniformDrawRadius(float radius)
{
	glUniform1f(uniform_drawRadius, radius);
}

void MaterialDrawOnTexture::setUniformTextureToDrawOn(int textureId)
{
	glUniform1i(uniform_textureToDrawOn, textureId);
}

void MaterialDrawOnTexture::use()
{
	glUseProgram(glProgram);
}

void MaterialDrawOnTexture::drawUI()
{
	//not displayed in UI
}

void MaterialDrawOnTexture::initGL()
{
	//nothing
}

////////////////////////// GRASS FIELD //////////////////

MaterialGrassField::MaterialGrassField(): Material(getProgramFactory().getDefault("defaultGrassField"))
{
	if (glProgram == 0)
		std::cerr << "wrong initialization of material of type MaterialGrassField, program with name defaultGrassField doesn't exist in ProgramFactory." << std::endl;

	uniform_time = glGetUniformLocation(glProgram, "Time");
	uniform_Texture = glGetUniformLocation(glProgram, "Texture");
	uniform_VP = glGetUniformLocation(glProgram, "VP");
}

MaterialGrassField::MaterialGrassField(GLuint _glProgram) : Material(_glProgram)
{
	uniform_time = glGetUniformLocation(glProgram, "Time");
	uniform_Texture = glGetUniformLocation(glProgram, "Texture");
	uniform_VP = glGetUniformLocation(glProgram, "VP");
}

void MaterialGrassField::setUniformTime(float time)
{
	glUniform1f(uniform_time, time);
}

void MaterialGrassField::setUniformTexture(int texId)
{
	glUniform1i(uniform_Texture, texId);
}

void MaterialGrassField::setUniformVP(const glm::mat4 & VP)
{
	glUniformMatrix4fv(uniform_VP, 1, false, glm::value_ptr(VP));
}

void MaterialGrassField::use()
{
	glUseProgram(glProgram);
}

void MaterialGrassField::drawUI()
{
	//nothing
}

void MaterialGrassField::initGL()
{
	//nothing
}

//////////////////////////////// BILLBOARD //////////////////////

MaterialBillboard::MaterialBillboard()
{
	MaterialBillboard(getProgramFactory().getDefault("defaultBillboard"));
}

MaterialBillboard::MaterialBillboard(GLuint _glProgram) : Material(_glProgram)
{
	 m_uniformMVP = glGetUniformLocation(glProgram, "MVP");
	 m_uniformScale = glGetUniformLocation(glProgram, "Scale");
	 m_uniformTranslation = glGetUniformLocation(glProgram, "Translation");
	 m_uniformTexture = glGetUniformLocation(glProgram, "Texture");
	 m_uniformCameraRight = glGetUniformLocation(glProgram, "CameraRight");
	 m_uniformCameraUp = glGetUniformLocation(glProgram, "CameraUp");
	 m_uniformColor = glGetUniformLocation(glProgram, "Color");
}

void MaterialBillboard::use()
{
	glUseProgram(glProgram);
}

void MaterialBillboard::drawUI()
{
	//nothing
}

void MaterialBillboard::initGL()
{
	//nothing
}

void MaterialBillboard::setUniformMVP(const glm::mat4 & mvp)
{
	glUniformMatrix4fv(m_uniformMVP, 1, false, glm::value_ptr(mvp));
}

void MaterialBillboard::setUniformScale(const glm::vec2 & scale)
{
	glUniform2fv(m_uniformScale, 1, glm::value_ptr(scale));
}

void MaterialBillboard::setUniformTranslation(const glm::vec3 & translation)
{
	glUniform3fv(m_uniformTranslation, 1, glm::value_ptr(translation));
}

void MaterialBillboard::setUniformTexture(int texId)
{
	glUniform1i(m_uniformTexture, texId);
}

void MaterialBillboard::setUniformCameraRight(const glm::vec3& camRight)
{
	glUniform3fv(m_uniformCameraRight, 1, glm::value_ptr(camRight));
}

void MaterialBillboard::setUniformCameraUp(const glm::vec3& camUp)
{
	glUniform3fv(m_uniformCameraUp, 1, glm::value_ptr(camUp));
}

void MaterialBillboard::setUniformColor(const glm::vec4 & color)
{
	glUniform4fv(m_uniformColor, 1, glm::value_ptr(color));
}



///////////////////////////////////////////////

MaterialParticlesCPU::MaterialParticlesCPU() : Material(getProgramFactory().getDefault("defaultParticlesCPU"))
{
	m_uniformVP = glGetUniformLocation(glProgram, "VP");
	m_uniformTexture = glGetUniformLocation(glProgram, "Texture");
	m_uniformCameraRight = glGetUniformLocation(glProgram, "CameraRight");
	m_uniformCameraUp = glGetUniformLocation(glProgram, "CameraUp");
}

MaterialParticlesCPU::MaterialParticlesCPU(GLuint _glProgram) : Material(_glProgram)
{
	m_uniformVP = glGetUniformLocation(glProgram, "VP");
	m_uniformTexture = glGetUniformLocation(glProgram, "Texture");
	m_uniformCameraRight = glGetUniformLocation(glProgram, "CameraRight");
	m_uniformCameraUp = glGetUniformLocation(glProgram, "CameraUp");
}

void MaterialParticlesCPU::use()
{
	glUseProgram(glProgram);
}

void MaterialParticlesCPU::drawUI()
{
	//nothing
}

void MaterialParticlesCPU::initGL()
{
	//nothing
}

void MaterialParticlesCPU::glUniform_VP(const glm::mat4& VP)
{
	glUniformMatrix4fv(m_uniformVP, 1, false, glm::value_ptr(VP));
}

void MaterialParticlesCPU::setUniformTexture(int texId)
{
	glUniform1i(m_uniformTexture, texId);
}

void MaterialParticlesCPU::setUniformCameraRight(const glm::vec3& camRight)
{
	glUniform3fv(m_uniformCameraRight, 1, glm::value_ptr(camRight));
}

void MaterialParticlesCPU::setUniformCameraUp(const glm::vec3& camUp)
{
	glUniform3fv(m_uniformCameraUp, 1, glm::value_ptr(camUp));
}


///////////////////////////////////////////////

MaterialParticles::MaterialParticles(): Material(getProgramFactory().getDefault("defaultParticles"))
{
	m_uniformVP = glGetUniformLocation(glProgram, "VP");
	m_uniformTexture = glGetUniformLocation(glProgram, "Texture");
	m_uniformCameraRight = glGetUniformLocation(glProgram, "CameraRight");
	m_uniformCameraUp = glGetUniformLocation(glProgram, "CameraUp");
}

MaterialParticles::MaterialParticles(GLuint _glProgram) : Material(_glProgram)
{
	m_uniformVP = glGetUniformLocation(glProgram, "VP");
	m_uniformTexture = glGetUniformLocation(glProgram, "Texture");
	m_uniformCameraRight = glGetUniformLocation(glProgram, "CameraRight");
	m_uniformCameraUp = glGetUniformLocation(glProgram, "CameraUp");
}

void MaterialParticles::use()
{
	glUseProgram(glProgram);
}

void MaterialParticles::drawUI()
{
	//nothing
}

void MaterialParticles::initGL()
{
	//nothing
}

void MaterialParticles::glUniform_VP(const glm::mat4& VP)
{
	glUniformMatrix4fv(m_uniformVP, 1, false, glm::value_ptr(VP));
}

void MaterialParticles::setUniformTexture(int texId)
{
	glUniform1i(m_uniformTexture, texId);
}

void MaterialParticles::setUniformCameraRight(const glm::vec3& camRight)
{
	glUniform3fv(m_uniformCameraRight, 1, glm::value_ptr(camRight));
}

void MaterialParticles::setUniformCameraUp(const glm::vec3& camUp)
{
	glUniform3fv(m_uniformCameraUp, 1, glm::value_ptr(camUp));
}

///////////////////////////////////////////

MaterialParticleSimulation::MaterialParticleSimulation(): Material(getProgramFactory().getDefault("particleSimulation"))
{
	m_uniformDeltaTime = glGetUniformLocation(glProgram, "DeltaTime");
}

MaterialParticleSimulation::MaterialParticleSimulation(GLuint _glProgram) : Material(_glProgram)
{
	m_uniformDeltaTime = glGetUniformLocation(glProgram, "DeltaTime");
}

void MaterialParticleSimulation::glUniform_deltaTime(float deltaTime)
{
	glUniform1f(m_uniformDeltaTime, deltaTime);
}

void MaterialParticleSimulation::use()
{
	glUseProgram(glProgram);
}

void MaterialParticleSimulation::drawUI()
{
	//nothing
}

void MaterialParticleSimulation::initGL()
{
	//nothing
}

*/