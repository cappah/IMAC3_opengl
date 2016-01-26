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

MaterialLit::MaterialLit() : Material(ProgramFactory::get().get("defaultLit")), textureDiffuse(0), specularPower(10), textureSpecular(0), textureRepetition(1, 1)
{
	uniform_textureDiffuse = glGetUniformLocation(glProgram, "Diffuse");
	uniform_textureSpecular = glGetUniformLocation(glProgram, "Specular");
	uniform_specularPower = glGetUniformLocation(glProgram, "specularPower");
	uniform_textureRepetition = glGetUniformLocation(glProgram, "TextureRepetition");

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);
}

MaterialLit::MaterialLit(GLuint _glProgram, GLuint _textureDiffuse, GLuint _textureSpecular, float _specularPower) :
	Material(_glProgram), textureDiffuse(_textureDiffuse), specularPower(_specularPower), textureSpecular(_textureSpecular), textureRepetition(1, 1)
{
	uniform_textureDiffuse = glGetUniformLocation(glProgram, "Diffuse");
	uniform_textureSpecular = glGetUniformLocation(glProgram, "Specular");
	uniform_specularPower = glGetUniformLocation(glProgram, "specularPower");
	uniform_textureRepetition = glGetUniformLocation(glProgram, "TextureRepetition");

	//check uniform errors : 
	if (!checkError("Uniforms"))
		exit(1);
}

void MaterialLit::setUniform_MVP(glm::mat4& mvp)
{
	glUniformMatrix4fv(uniform_MVP, 1, false, glm::value_ptr(mvp));
}

void MaterialLit::setUniform_normalMatrix(glm::mat4& normalMatrix)
{
	glUniformMatrix4fv(uniform_normalMatrix, 1, false, glm::value_ptr(normalMatrix));
}

void MaterialLit::use()
{
	//bind shaders
	glUseProgram(glProgram);

	//bind textures into texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureDiffuse);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureSpecular);

	//send uniforms
	glUniform1f(uniform_specularPower, specularPower);
	glUniform1i(uniform_textureDiffuse, 0);
	glUniform1i(uniform_textureSpecular, 1);
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

		if (TextureFactory::get().contains(diffuseTextureName))
		{

			textureDiffuse = TextureFactory::get().get(diffuseTextureName);
		}
	}

	specularTextureName.copy(tmpTxt, glm::min(30, (int)specularTextureName.size()), 0);
	tmpTxt[specularTextureName.size()] = '\0';
	if (ImGui::InputText("specular texture name", tmpTxt, 20))
	{
		specularTextureName = tmpTxt;

		if (TextureFactory::get().contains(specularTextureName))
			textureSpecular = TextureFactory::get().get(specularTextureName);
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

void MaterialUnlit::setUniform_MVP(glm::mat4& mvp)
{
	glUniformMatrix4fv(uniform_MVP, 1, false, glm::value_ptr(mvp));
}

void MaterialUnlit::setUniform_normalMatrix(glm::mat4& normalMatrix)
{
	glUniformMatrix4fv(uniform_normalMatrix, 1, false, glm::value_ptr(normalMatrix));
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
