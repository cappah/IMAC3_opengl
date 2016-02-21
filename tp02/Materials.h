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
	std::string name;
	GLuint glProgram;


	Material(GLuint _glProgram = 0);
	virtual void use() = 0;
	virtual void drawUI() = 0;
};

struct Material3DObject : public Material
{
	GLuint uniform_MVP;
	GLuint uniform_normalMatrix;

	Material3DObject(GLuint _glProgram = 0);
	void setUniform_MVP(glm::mat4& mvp);
	void setUniform_normalMatrix(glm::mat4& normalMatrix);
};


class MaterialLit : public Material3DObject
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

	//GLuint uniform_MVP;
	//GLuint uniform_normalMatrix;

	GLuint uniform_textureDiffuse;
	GLuint uniform_textureSpecular;
	GLuint uniform_textureBump;
	GLuint uniform_specularPower;
	GLuint uniform_textureRepetition;

public:
	MaterialLit();
	MaterialLit(GLuint _glProgram, Texture* _textureDiffuse = nullptr, Texture* _textureSpecular = nullptr, Texture* _textureBump = nullptr, float _specularPower = 50);

	void setDiffuse(Texture* _textureDiffuse);
	void setSpecular(Texture* _textureSpecular);
	void setBump(Texture* _textureBump);

	Texture* getDiffuse() const;
	Texture* getSpecular() const;
	Texture* getBump() const;

	//void setUniform_MVP(glm::mat4& mvp);
	//void setUniform_normalMatrix(glm::mat4& normalMatrix);

	virtual void use() override;

	virtual void drawUI() override;
};


struct MaterialUnlit : public Material3DObject
{
	//GLuint uniform_MVP;
	//GLuint uniform_normalMatrix;

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
	GLuint uniform_VP;

public:
	MaterialSkybox();
	MaterialSkybox(GLuint _glProgram, CubeTexture * _textureDiffuse);
	
	void setUniform_VP(const glm::mat4& vp);

	virtual void use() override;

	virtual void drawUI() override;
};

class MaterialShadow : public Material
{
public:
	MaterialShadow();
	MaterialShadow(GLuint _glProgram);

	virtual void use() override;
	virtual void drawUI() override;
};



struct MaterialTerrain : public Material3DObject
{

	std::string diffuseTextureName;
	Texture* textureDiffuse;

	
	float specularPower;
	std::string specularTextureName;
	Texture* textureSpecular;

	std::string bumpTextureName;
	Texture* textureBump;
	

	glm::vec2 textureRepetition;

	GLuint uniform_textureDiffuse;
	GLuint uniform_textureSpecular;
	GLuint uniform_textureBump;
	GLuint uniform_specularPower;
	GLuint uniform_textureRepetition;


	MaterialTerrain();
	MaterialTerrain(GLuint _glProgram);

	virtual void use() override;

	virtual void drawUI() override;
};


class MaterialTerrainEdition : public Material
{
private:
	//std::string diffuseTextureName;
	Texture* textureDiffuse;

	//float specularPower;
	//std::string specularTextureName;
	Texture* textureSpecular;

	//std::string bumpTextureName;
	Texture* textureBump;

	glm::vec2 textureRepetition;

	GLuint uniform_textureDiffuse;
	GLuint uniform_textureSpecular;
	GLuint uniform_textureBump;
	//GLuint uniform_specularPower; 
	GLuint uniform_textureRepetition;
	GLuint uniform_textureFilter;
	GLuint uniform_filterValues;

public:
	MaterialTerrainEdition();
	MaterialTerrainEdition(GLuint _glProgram);

	void setUniformFilterTexture(int textureId);
	void setUniformDiffuseTexture(int textureId);
	void setUniformBumpTexture(int textureId);
	void setUniformSpecularTexture(int textureId);
	void setUniformLayoutOffset(const glm::vec2& layoutOffset);
	void setUniformTextureRepetition(const glm::vec2& textureRepetition);

	virtual void use() override;

	virtual void drawUI() override;
};

class MaterialDrawOnTexture : public Material
{
private:
	GLuint uniform_colorToDraw;
	GLuint uniform_drawPosition;
	GLuint uniform_drawRadius;
	GLuint uniform_textureToDrawOn;

public : 
	MaterialDrawOnTexture(GLuint _glProgram);

	void setUniformDrawPosition(const glm::vec2& position);
	void setUniformColorToDraw(const glm::vec4& color);
	void setUniformDrawRadius(float radius);
	void setUniformTextureToDrawOn(int textureId);

	virtual void use() override;
	virtual void drawUI() override;
};

class MaterialGrassField : public Material
{
private:
	GLuint uniform_time;
	GLuint uniform_Texture;
	GLuint uniform_VP;

public:
	MaterialGrassField();
	MaterialGrassField(GLuint _glProgram);

	void setUniformTime(float time);
	void setUniformTexture(int texId);
	void setUniformVP(const glm::mat4& VP);

	virtual void use() override;
	virtual void drawUI() override;
};

