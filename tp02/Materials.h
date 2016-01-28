#pragma once

#include "glew/glew.h"

#include "glm/glm.hpp"
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4, glm::ivec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/type_ptr.hpp" // glm::value_ptr

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "Utils.h"
#include "Texture.h"

struct Material
{
	GLuint glProgram;

	GLuint uniform_MVP;
	GLuint uniform_normalMatrix;

	Material(GLuint _glProgram = 0);

	void setUniform_MVP(glm::mat4& mvp);
	void setUniform_normalMatrix(glm::mat4& normalMatrix);

	virtual void use() = 0;

	virtual void drawUI() = 0;
};

class MaterialLit : public Material
{
private:
	std::string diffuseTextureName;
	Texture* textureDiffuse;

	float specularPower;
	std::string specularTextureName;
	Texture* textureSpecular;

	std::string bumpTextureName;
	Texture* textureBump;

	glm::vec2 textureRepetition;

	GLuint uniform_MVP;
	GLuint uniform_normalMatrix;

	GLuint uniform_textureDiffuse;
	GLuint uniform_textureSpecular;
	GLuint uniform_textureBump;
	GLuint uniform_specularPower;
	GLuint uniform_textureRepetition;

public:
	MaterialLit();
	MaterialLit(GLuint _glProgram, Texture* _textureDiffuse = nullptr, Texture* _textureSpecular = nullptr, Texture* _textureBump = nullptr, float _specularPower = 50);

	//void setUniform_MVP(glm::mat4& mvp);
	//void setUniform_normalMatrix(glm::mat4& normalMatrix);

	virtual void use() override;

	virtual void drawUI() override;
};


struct MaterialUnlit : public Material
{
	GLuint uniform_MVP;
	GLuint uniform_normalMatrix;

	GLuint uniform_color;

	MaterialUnlit(GLuint _glProgram);

	void setUniform_color(glm::vec3 color);

	//void setUniform_MVP(glm::mat4& mvp);
	//void setUniform_normalMatrix(glm::mat4& normalMatrix);

	virtual void use() override;

	virtual void drawUI() override;
};

class MaterialSkybox : public Material
{
private:

	std::string diffuseTextureName;
	CubeTexture* textureDiffuse;

	GLuint uniform_textureDiffuse;

public:
	MaterialSkybox();
	MaterialSkybox(GLuint _glProgram, CubeTexture * _textureDiffuse);
	
	virtual void use() override;

	virtual void drawUI() override;
};