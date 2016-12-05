#pragma once

#include <string>
#include <vector>

#include "stb/stb_image.h"
#include "glm/common.hpp"

#include "glew/glew.h"
#include "FileHandler.h"
#include "Resource.h"

struct Texture final : public Resource
{
	int w;
	int h;
	int comp;

	unsigned char * pixels;

	GLuint glId;

	GLint internalFormat;
	GLenum format;
	GLenum type;
	bool generateMipMap;
	GLint textureWrapping_u;
	GLint textureWrapping_v;
	GLint minFilter;
	GLint magFilter;

	int m_textureUseCounts;

	Texture(int width = 0, int height = 0, bool useAlpha = false);
	Texture(unsigned char* _pixels, int width, int height, bool useAlpha, int _comp);
	Texture::Texture(const glm::vec3& color);
	Texture::Texture(const glm::vec4& color);
	Texture(int width, int height, const glm::vec4& color);
	Texture(int width, int height, const glm::vec3& color);
	Texture(const FileHandler::CompletePath& _path, bool alphaChannel = true);
	~Texture();

	void setTextureParameters(GLint _internalFormat = GL_RGB, GLenum _format = GL_RGB, GLenum _type = GL_UNSIGNED_BYTE, bool _generateMipMap = true);
	void setTextureMinMaxFilters(GLint _maxFilter = GL_NEAREST, GLint _format = GL_NEAREST);
	void setTextureWrapping(GLint _uWrapping = GL_CLAMP_TO_EDGE, GLint _vWrapping = GL_CLAMP_TO_EDGE);

	void initGL();
	void freeGL();

	void init(const FileHandler::CompletePath& path) override;

	void drawInInspector(Scene & scene) override;
};

struct CubeTexture final : public Resource
{
	std::string name;

	FileHandler::CompletePath paths[6];

	int w;
	int h;
	int comp;

	unsigned char* pixels[6];

	GLuint glId;

	GLint internalFormat;
	GLenum format;
	GLenum type;
	bool generateMipMap;
	GLint textureWrapping_s;
	GLint textureWrapping_t;
	GLint textureWrapping_r;
	GLint minFilter;
	GLint magFilter;

	int m_textureUseCounts;

	CubeTexture();
	CubeTexture(char r, char g, char b);
	CubeTexture(const std::vector<FileHandler::CompletePath>& _paths);
	~CubeTexture();

	void setTextureParameters(GLint _internalFormat = GL_RGB, GLenum _format = GL_RGB, GLenum _type = GL_UNSIGNED_BYTE, bool _generateMipMap = false);
	void setTextureMinMaxFilters(GLint _maxFilter = GL_LINEAR, GLint _format = GL_LINEAR);
	void setTextureWrapping(GLint _sWrapping = GL_CLAMP_TO_EDGE, GLint _tWrapping = GL_CLAMP_TO_EDGE, GLint _rWrapping = GL_CLAMP_TO_EDGE);

	void initGL();
	void freeGL();

	//For saving internal resource
	void init(const FileHandler::CompletePath& path) override;
	void load(const FileHandler::CompletePath& path);
	void save(const FileHandler::CompletePath& path) const;
};

namespace GlHelper{

	//make a basic color texture with GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE
	Texture* makeNewColorTexture(float width, float height);
	//make a basic normal texture with GL_RGBA16, GL_RGBA, GL_FLOAT
	Texture* makeNewNormalTexture(float width, float height);
	//make a basic depth texture with GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT
	Texture* makeNewDepthTexture(float width, float height);
}