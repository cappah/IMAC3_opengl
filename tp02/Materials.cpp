#include "Materials.h"
#include "Factories.h"//forward

Material::Material(GLuint _glProgram) : glProgram(_glProgram)
{
	uniform_MVP = glGetUniformLocation(glProgram, "MVP");
	uniform_normalMatrix = glGetUniformLocation(glProgram, "NormalMatrix");
}

void Material::setUniform_MVP(glm::mat4& mvp)
{
	glUniformMatrix4fv(uniform_MVP, 1, false, glm::value_ptr(mvp));
}

void Material::setUniform_normalMatrix(glm::mat4& normalMatrix)
{
	glUniformMatrix4fv(uniform_normalMatrix, 1, false, glm::value_ptr(normalMatrix));
}


///////////////////////////////////////////

MaterialLit::MaterialLit() : Material(ProgramFactory::get().get("defaultLit")), textureDiffuse(TextureFactory::get().get("default")), specularPower(10), textureSpecular(TextureFactory::get().get("default")), textureBump(TextureFactory::get().get("default")), textureRepetition(1, 1)
{
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
	Material(_glProgram), textureDiffuse(_textureDiffuse), specularPower(_specularPower), textureSpecular(_textureSpecular), textureBump(_textureBump) , textureRepetition(1, 1)
{
	uniform_MVP = glGetUniformLocation(glProgram, "MVP");
	uniform_normalMatrix = glGetUniformLocation(glProgram, "NormalMatrix");

	uniform_textureDiffuse = glGetUniformLocation(glProgram, "Diffuse");
	uniform_textureSpecular = glGetUniformLocation(glProgram, "Specular");
	uniform_textureBump = glGetUniformLocation(glProgram, "Bump");
	uniform_specularPower = glGetUniformLocation(glProgram, "specularPower");
	uniform_textureRepetition = glGetUniformLocation(glProgram, "TextureRepetition");

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);

	textureDiffuse->initGL(); // we consider that each texture on a material will be used on the sceen and should be send to the GPU.
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

	char tmpTxt[30];
	diffuseTextureName.copy(tmpTxt, glm::min(30, (int)diffuseTextureName.size()), 0);
	tmpTxt[diffuseTextureName.size()] = '\0';

	if (ImGui::InputText("diffuse texture name", tmpTxt, 20))
	{
		diffuseTextureName = tmpTxt;

		if (TextureFactory::get().contains(tmpTxt))
		{
			textureDiffuse->freeGL();
			textureDiffuse = TextureFactory::get().get(diffuseTextureName);
			textureDiffuse->initGL();
		}
	}

	specularTextureName.copy(tmpTxt, glm::min(30, (int)specularTextureName.size()), 0);
	tmpTxt[specularTextureName.size()] = '\0';
	if (ImGui::InputText("specular texture name", tmpTxt, 20))
	{
		specularTextureName = tmpTxt;

		if (TextureFactory::get().contains(specularTextureName))
		{
			textureSpecular->freeGL();
			textureSpecular = TextureFactory::get().get(specularTextureName);
			textureSpecular->initGL();
		}
	}

	bumpTextureName.copy(tmpTxt, glm::min(30, (int)bumpTextureName.size()), 0);
	tmpTxt[bumpTextureName.size()] = '\0';
	if (ImGui::InputText("bump texture name", tmpTxt, 20))
	{
		bumpTextureName = tmpTxt;

		if (TextureFactory::get().contains(bumpTextureName))
		{
			textureBump->freeGL();
			textureBump = TextureFactory::get().get(bumpTextureName);
			textureBump->initGL();
		}
	}
}


//////////////////////////////////////////////////

MaterialUnlit::MaterialUnlit(GLuint _glProgram) : Material(_glProgram)
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
	uniform_textureDiffuse = glGetUniformLocation(glProgram, "Diffuse");

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);
}

MaterialSkybox::MaterialSkybox(GLuint _glProgram, CubeTexture * _textureDiffuse) : Material(_glProgram), textureDiffuse(_textureDiffuse)
{
	uniform_textureDiffuse = glGetUniformLocation(glProgram, "Diffuse");

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);
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
