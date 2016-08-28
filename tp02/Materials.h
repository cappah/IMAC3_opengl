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

#include "ISerializable.h"
#include "ResourcePointer.h"
#include "ShaderProgram.h"
#include "FileHandler.h"
#include "Resource.h"

static const unsigned int MAX_BONE_COUNT = 100;

struct Material : public Resource
{
	std::string name;

	GLuint glProgram;

	Material(GLuint _glProgram = 0);
	Material(ResourcePtr<ShaderProgram> _glProgram);
	virtual void init(const FileHandler::CompletePath& path) override;
	virtual ~Material();
	virtual void use() = 0;
	virtual void drawUI() = 0;
	virtual void initGL() = 0;

	void save(const FileHandler::CompletePath& path) const;
	void load(const FileHandler::CompletePath& path);
};

struct Material3DObject : public Material
{
	GLuint uniform_MVP;
	GLuint uniform_normalMatrix;
	GLuint uniform_bonesTransform[MAX_BONE_COUNT];
	GLuint uniform_useSkeleton;

	Material3DObject(GLuint _glProgram = 0);
	Material3DObject(ResourcePtr<ShaderProgram> _glProgram);

	void setUniform_MVP(glm::mat4& mvp);
	void setUniform_normalMatrix(glm::mat4& normalMatrix);
	void setUniformBonesTransform(unsigned int idx, const glm::mat4& boneTransform);
	void setUniformUseSkeleton(bool useSkeleton);
};


class MaterialLit : public Material3DObject, public ISerializable
{
private:
	std::string diffuseTextureName;
	ResourcePtr<Texture> textureDiffuse;

	float specularPower;
	std::string specularTextureName;
	ResourcePtr<Texture> textureSpecular;

	std::string bumpTextureName;
	ResourcePtr<Texture> textureBump;

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
	MaterialLit(GLuint _glProgram, ResourcePtr<Texture> _textureDiffuse = ResourcePtr<Texture>(), ResourcePtr<Texture> _textureSpecular = ResourcePtr<Texture>(), ResourcePtr<Texture> _textureBump = ResourcePtr<Texture>(), float _specularPower = 50);
	MaterialLit(ResourcePtr<ShaderProgram> _glProgram, ResourcePtr<Texture> _textureDiffuse = ResourcePtr<Texture>(), ResourcePtr<Texture> _textureSpecular = ResourcePtr<Texture>(), ResourcePtr<Texture> _textureBump = ResourcePtr<Texture>(), float _specularPower = 50);
	void init(const FileHandler::CompletePath& path) override;

	void setDiffuse(ResourcePtr<Texture> _textureDiffuse);
	void setSpecular(ResourcePtr<Texture> _textureSpecular);
	void setBump(ResourcePtr<Texture> _textureBump);

	ResourcePtr<Texture> getDiffuse() const;
	ResourcePtr<Texture> getSpecular() const;
	ResourcePtr<Texture> getBump() const;

	float getSpecularPower() const;

	//void setUniform_MVP(glm::mat4& mvp);
	//void setUniform_normalMatrix(glm::mat4& normalMatrix);
	//void setUniformBonesTransform(const std::vector<glm::mat4>& bonesTransform);

	virtual void use() override;
	virtual void drawUI() override;
	virtual void initGL() override;

	// Hérité via ISerializable
	virtual void save(Json::Value & entityRoot) const override;
	virtual void load(Json::Value & entityRoot) override;
};


struct MaterialUnlit : public Material3DObject
{
	//GLuint uniform_MVP;
	//GLuint uniform_normalMatrix;

	GLuint uniform_color;

	MaterialUnlit(GLuint _glProgram);
	MaterialUnlit(ResourcePtr<ShaderProgram> _glProgram)
		: MaterialUnlit(_glProgram->id)
	{

	}

	void setUniform_color(glm::vec3 color);

	//void setUniform_MVP(glm::mat4& mvp);
	//void setUniform_normalMatrix(glm::mat4& normalMatrix);

	virtual void use() override;

	virtual void drawUI() override;

	virtual void initGL() override;
};


struct MaterialInstancedUnlit : public Material
{
	GLuint uniform_color;
	GLuint uniform_VP;

	MaterialInstancedUnlit(GLuint _glProgram);

	void setUniform_color(glm::vec3 color);
	void setUniform_VP(const glm::mat4& VP);

	virtual void use() override;

	virtual void drawUI() override;

	virtual void initGL() override;
};

struct MaterialDebugDrawer : public Material
{
	GLuint uniform_MVP;

	MaterialDebugDrawer(GLuint _glProgram);

	void setUniform_MVP(const glm::mat4& MVP);

	virtual void use() override;
	virtual void drawUI() override;
	virtual void initGL() override;
};


class MaterialSkybox : public Material
{
private:

	std::string diffuseTextureName;
	ResourcePtr<CubeTexture> textureDiffuse;

	GLuint uniform_textureDiffuse;
	GLuint uniform_VP;

public:
	MaterialSkybox();
	MaterialSkybox(GLuint _glProgram, CubeTexture * _textureDiffuse);
	MaterialSkybox(ResourcePtr<ShaderProgram> _glProgram, CubeTexture * _textureDiffuse)
		: MaterialSkybox(_glProgram->id, _textureDiffuse)
	{}
	
	void setUniform_VP(const glm::mat4& vp);

	virtual void use() override;
	virtual void drawUI() override;
	virtual void initGL() override;

	void setDiffuseTexture(ResourcePtr<CubeTexture> texture);
	ResourcePtr<CubeTexture> getDiffuseTexture() const;
};

class MaterialShadow : public Material
{
public:
	MaterialShadow();
	MaterialShadow(GLuint _glProgram);
	MaterialShadow(ResourcePtr<ShaderProgram> _glProgram)
		: MaterialShadow(_glProgram->id)
	{}

	virtual void use() override;
	virtual void drawUI() override;
	virtual void initGL() override;
};

class MaterialBillboard : public Material
{
private:
	GLuint m_uniformMVP;
	GLuint m_uniformScale;
	GLuint m_uniformTranslation;
	GLuint m_uniformTexture;
	GLuint m_uniformCameraRight;
	GLuint m_uniformCameraUp;
	GLuint m_uniformColor;

public:
	MaterialBillboard();
	MaterialBillboard(GLuint _glProgram);
	MaterialBillboard(ResourcePtr<ShaderProgram> _glProgram)
		: MaterialBillboard(_glProgram->id)
	{}

	virtual void use() override;
	virtual void drawUI() override;
	virtual void initGL() override;

	void setUniformMVP(const glm::mat4& mvp);
	void setUniformScale(const glm::vec2& scale);
	void setUniformTranslation(const glm::vec3& translation);
	void setUniformTexture(int texId);
	void setUniformCameraRight(const glm::vec3& camRight);
	void setUniformCameraUp(const glm::vec3& camUp);
	void setUniformColor(const glm::vec4& color);
};


struct MaterialTerrain : public Material3DObject
{

	std::string diffuseTextureName;
	ResourcePtr<Texture> textureDiffuse;

	
	float specularPower;
	std::string specularTextureName;
	ResourcePtr<Texture> textureSpecular;

	std::string bumpTextureName;
	ResourcePtr<Texture> textureBump;
	

	glm::vec2 textureRepetition;

	GLuint uniform_textureDiffuse;
	GLuint uniform_textureSpecular;
	GLuint uniform_textureBump;
	GLuint uniform_specularPower;

	GLuint uniform_textureRepetition;
	GLuint uniform_textureFilter;
	GLuint uniform_filterValues;

	MaterialTerrain();
	MaterialTerrain(GLuint _glProgram);
	MaterialTerrain(ResourcePtr<ShaderProgram> _glProgram)
		: MaterialTerrain(_glProgram->id)
	{}

	virtual void use() override;
	virtual void drawUI() override;
	virtual void initGL() override;

	void setUniformLayoutOffset(const glm::vec2& layoutOffset);
	void setUniformFilterTexture(int textureId);
	void setUniformTextureRepetition(const glm::vec2& textureRepetition);

	void setUniformDiffuseTexture(int textureId);
	void setUniformBumpTexture(int textureId);
	void setUniformSpecularTexture(int textureId);

	void setUniformSpecularPower(float specularPower);
};


class MaterialTerrainEdition : public Material
{
private:
	//std::string diffuseTextureName;
	ResourcePtr<Texture> textureDiffuse;

	//float specularPower;
	//std::string specularTextureName;
	ResourcePtr<Texture> textureSpecular;

	//std::string bumpTextureName;
	ResourcePtr<Texture> textureBump;

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
	MaterialTerrainEdition(ResourcePtr<ShaderProgram> _glProgram)
		: MaterialTerrainEdition(_glProgram->id)
	{}

	void setUniformFilterTexture(int textureId);
	void setUniformDiffuseTexture(int textureId);
	void setUniformBumpTexture(int textureId);
	void setUniformSpecularTexture(int textureId);
	void setUniformLayoutOffset(const glm::vec2& layoutOffset);
	void setUniformTextureRepetition(const glm::vec2& textureRepetition);

	virtual void use() override;
	virtual void drawUI() override;
	virtual void initGL() override;
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
	MaterialDrawOnTexture(ResourcePtr<ShaderProgram> _glProgram)
		: MaterialDrawOnTexture(_glProgram->id)
	{}

	void setUniformDrawPosition(const glm::vec2& position);
	void setUniformColorToDraw(const glm::vec4& color);
	void setUniformDrawRadius(float radius);
	void setUniformTextureToDrawOn(int textureId);

	virtual void use() override;
	virtual void drawUI() override;
	virtual void initGL() override;
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
	MaterialGrassField(ResourcePtr<ShaderProgram> _glProgram)
		: MaterialGrassField(_glProgram->id)
	{}

	void setUniformTime(float time);
	void setUniformTexture(int texId);
	void setUniformVP(const glm::mat4& VP);

	virtual void use() override;
	virtual void drawUI() override;
	virtual void initGL() override;
};

class MaterialParticlesCPU : public Material
{
private:
	GLuint m_uniformVP;
	GLuint m_uniformTexture;
	GLuint m_uniformCameraRight;
	GLuint m_uniformCameraUp;

public:
	MaterialParticlesCPU();
	MaterialParticlesCPU(GLuint _glProgram);
	MaterialParticlesCPU(ResourcePtr<ShaderProgram> _glProgram)
		: MaterialParticlesCPU(_glProgram->id)
	{}

	void glUniform_VP(const glm::mat4& VP);
	void setUniformTexture(int texId);
	void setUniformCameraRight(const glm::vec3& camRight);
	void setUniformCameraUp(const glm::vec3& camUp);

	virtual void use() override;
	virtual void drawUI() override;
	virtual void initGL() override;
};


class MaterialParticles : public Material
{
private:
	GLuint m_uniformVP;
	GLuint m_uniformTexture;
	GLuint m_uniformCameraRight;
	GLuint m_uniformCameraUp;

public :
	MaterialParticles();
	MaterialParticles(GLuint _glProgram);
	MaterialParticles(ResourcePtr<ShaderProgram> _glProgram)
		: MaterialParticles(_glProgram->id)
	{}

	void glUniform_VP(const glm::mat4& VP);
	void setUniformTexture(int texId);
	void setUniformCameraRight(const glm::vec3& camRight);
	void setUniformCameraUp(const glm::vec3& camUp);

	virtual void use() override;
	virtual void drawUI() override;
	virtual void initGL() override;
};

class MaterialParticleSimulation : public Material
{
private :
	GLuint m_uniformDeltaTime;

public:
	MaterialParticleSimulation();
	MaterialParticleSimulation(GLuint _glProgram);
	MaterialParticleSimulation(ResourcePtr<ShaderProgram> _glProgram)
		: MaterialParticleSimulation(_glProgram->id)
	{}

	void glUniform_deltaTime(float deltaTime);

	virtual void use() override;
	virtual void drawUI() override;
	virtual void initGL() override;
};


