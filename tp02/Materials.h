#pragma once

#include "glew/glew.h"

#include "glm/glm.hpp"
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4, glm::ivec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/type_ptr.hpp" // glm::value_ptr

#include "Utils.h"

struct Material
{
	GLuint glProgram;

	GLuint uniform_MVP;
	GLuint uniform_normalMatrix;

	Material(GLuint _glProgram = 0);

	void setUniform_MVP(glm::mat4& mvp);

	void setUniform_normalMatrix(glm::mat4& normalMatrix);

	virtual void use() = 0;
};

struct MaterialLit : public Material
{
	GLuint textureDiffuse;

	float specularPower;
	GLuint textureSpecular;

	glm::vec2 textureRepetition;

	GLuint uniform_textureDiffuse;
	GLuint uniform_textureSpecular;
	GLuint uniform_specularPower;
	GLuint uniform_textureRepetition;

	MaterialLit();
	MaterialLit(GLuint _glProgram, GLuint _textureDiffuse = 0, GLuint _textureSpecular = 0, float _specularPower = 50);

	void setUniform_MVP(glm::mat4& mvp);

	void setUniform_normalMatrix(glm::mat4& normalMatrix);

	void use();
};


struct MaterialUnlit : public Material
{
	GLuint uniform_color;


	MaterialUnlit(GLuint _glProgram);

	void setUniform_color(glm::vec3 color);

	void setUniform_MVP(glm::mat4& mvp);

	void setUniform_normalMatrix(glm::mat4& normalMatrix);

	void use();
};