#include "Materials.h"
#include "Factories.h"//forward

Material::Material(GLuint _glProgram) : glProgram(_glProgram)
{

}


///////////////////////////////////////////


Material3DObject::Material3DObject(GLuint _glProgram) : Material(_glProgram)
{
	uniform_MVP = glGetUniformLocation(glProgram, "MVP");
	uniform_normalMatrix = glGetUniformLocation(glProgram, "NormalMatrix");
}


void Material3DObject::setUniform_MVP(glm::mat4& mvp)
{
	glUniformMatrix4fv(uniform_MVP, 1, false, glm::value_ptr(mvp));
}

void Material3DObject::setUniform_normalMatrix(glm::mat4& normalMatrix)
{
	glUniformMatrix4fv(uniform_normalMatrix, 1, false, glm::value_ptr(normalMatrix));
}


///////////////////////////////////////////

MaterialLit::MaterialLit() : Material3DObject(ProgramFactory::get().get("defaultLit")), textureDiffuse(TextureFactory::get().get("default")), specularPower(10), textureSpecular(TextureFactory::get().get("default")), textureBump(TextureFactory::get().get("default")), textureRepetition(1, 1)
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

MaterialLit::MaterialLit(GLuint _glProgram, Texture* _textureDiffuse, Texture* _textureSpecular, Texture* _textureBump, float _specularPower) :
	Material3DObject(_glProgram), textureDiffuse(_textureDiffuse), specularPower(_specularPower), textureSpecular(_textureSpecular), textureBump(_textureBump) , textureRepetition(1, 1)
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

void MaterialLit::setDiffuse(Texture * _textureDiffuse)
{
	diffuseTextureName = _textureDiffuse->name;
	textureDiffuse = _textureDiffuse;
}

void MaterialLit::setSpecular(Texture * _textureSpecular)
{
	specularTextureName = _textureSpecular->name;
	textureSpecular = _textureSpecular;
}

void MaterialLit::setBump(Texture * _textureBump)
{
	bumpTextureName = _textureBump->name;
	textureBump = _textureBump;
}

Texture * MaterialLit::getDiffuse() const
{
	return textureDiffuse;
}

Texture * MaterialLit::getSpecular() const
{
	return textureSpecular;
}

Texture * MaterialLit::getBump() const
{
	return textureBump;
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

	char tmpTxt01[30];
	diffuseTextureName.copy(tmpTxt01, glm::min(30, (int)diffuseTextureName.size()), 0);
	tmpTxt01[diffuseTextureName.size()] = '\0';
	if (ImGui::InputText("diffuse texture name", tmpTxt01, 20))
	{
		diffuseTextureName = tmpTxt01;

		if (TextureFactory::get().contains(tmpTxt01))
		{
			textureDiffuse->freeGL();
			textureDiffuse = TextureFactory::get().get(diffuseTextureName);
			textureDiffuse->initGL();
		}
	}

	char tmpTxt02[30];
	specularTextureName.copy(tmpTxt02, glm::min(30, (int)specularTextureName.size()), 0);
	tmpTxt02[specularTextureName.size()] = '\0';
	if (ImGui::InputText("specular texture name", tmpTxt02, 20))
	{
		specularTextureName = tmpTxt02;

		if (TextureFactory::get().contains(specularTextureName))
		{
			textureSpecular->freeGL();
			textureSpecular = TextureFactory::get().get(specularTextureName);
			textureSpecular->initGL();
		}
	}

	char tmpTxt03[30];
	bumpTextureName.copy(tmpTxt03, glm::min(30, (int)bumpTextureName.size()), 0);
	tmpTxt03[bumpTextureName.size()] = '\0';
	if (ImGui::InputText("bump texture name", tmpTxt03, 20))
	{
		bumpTextureName = tmpTxt03;

		if (TextureFactory::get().contains(bumpTextureName))
		{
			textureBump->freeGL();
			textureBump = TextureFactory::get().get(bumpTextureName);
			textureBump->initGL();
		}
	}
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

///////////////////////////////////////////////

MaterialSkybox::MaterialSkybox() : Material(ProgramFactory::get().get("defaultSkybox")), textureDiffuse(CubeTextureFactory::get().get("default"))
{
	diffuseTextureName = textureDiffuse->name;

	uniform_textureDiffuse = glGetUniformLocation(glProgram, "Diffuse");

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);
}

MaterialSkybox::MaterialSkybox(GLuint _glProgram, CubeTexture * _textureDiffuse) : Material(_glProgram), textureDiffuse(_textureDiffuse)
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
	char tmpTxt[30];
	diffuseTextureName.copy(tmpTxt, glm::min(30, (int)diffuseTextureName.size()), 0);
	tmpTxt[diffuseTextureName.size()] = '\0';

	if (ImGui::InputText("diffuse texture name", tmpTxt, 20))
	{
		diffuseTextureName = tmpTxt;

		if (CubeTextureFactory::get().contains(tmpTxt))
		{
			textureDiffuse->freeGL();
			textureDiffuse = CubeTextureFactory::get().get(diffuseTextureName);
			textureDiffuse->initGL();
		}
	}
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


////////////////////////////////////////////////////////


MaterialTerrain::MaterialTerrain() : Material3DObject(ProgramFactory::get().get("defaultLit")), textureDiffuse(TextureFactory::get().get("default")), specularPower(10), textureSpecular(TextureFactory::get().get("default")), textureBump(TextureFactory::get().get("default")), textureRepetition(1, 1)
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

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);
}

void MaterialTerrain::use()
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

void MaterialTerrain::drawUI()
{
	ImGui::InputFloat("specular power", &specularPower);

	ImGui::InputFloat2("texture repetition", &textureRepetition[0]);
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

	/*

	char tmpTxt01[30];
	diffuseTextureName.copy(tmpTxt01, glm::min(30, (int)diffuseTextureName.size()), 0);
	tmpTxt01[diffuseTextureName.size()] = '\0';
	if (ImGui::InputText("diffuse texture name", tmpTxt01, 20))
	{
		diffuseTextureName = tmpTxt01;

		if (TextureFactory::get().contains(tmpTxt01))
		{
			textureDiffuse->freeGL();
			textureDiffuse = TextureFactory::get().get(diffuseTextureName);
			textureDiffuse->initGL();
		}
	}

	
	char tmpTxt02[30];
	specularTextureName.copy(tmpTxt02, glm::min(30, (int)specularTextureName.size()), 0);
	tmpTxt02[specularTextureName.size()] = '\0';
	if (ImGui::InputText("specular texture name", tmpTxt02, 20))
	{
		specularTextureName = tmpTxt02;

		if (TextureFactory::get().contains(specularTextureName))
		{
			textureSpecular->freeGL();
			textureSpecular = TextureFactory::get().get(specularTextureName);
			textureSpecular->initGL();
		}
	}

	char tmpTxt03[30];
	bumpTextureName.copy(tmpTxt03, glm::min(30, (int)bumpTextureName.size()), 0);
	tmpTxt03[bumpTextureName.size()] = '\0';
	if (ImGui::InputText("bump texture name", tmpTxt03, 20))
	{
		bumpTextureName = tmpTxt03;

		if (TextureFactory::get().contains(bumpTextureName))
		{
			textureBump->freeGL();
			textureBump = TextureFactory::get().get(bumpTextureName);
			textureBump->initGL();
		}
	}
	*/
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

////////////////////////// GRASS FIELD //////////////////

MaterialGrassField::MaterialGrassField(): Material(ProgramFactory::get().get("defaultGrassField"))
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
