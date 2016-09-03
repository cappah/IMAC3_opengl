#pragma once

#include <string>
#include <vector>

#include "stb/stb_image.h"
#include "glm/common.hpp"

#include "glew/glew.h"
#include "FileHandler.h"
#include "Resource.h"

struct Texture : public Resource
{
	std::string name;

	FileHandler::CompletePath path;

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

	Texture();
	Texture(int width, int height);
	Texture(unsigned char* _pixels, int width, int height, int _comp);
	Texture(char r, char g, char b);
	Texture(const FileHandler::CompletePath& _path, bool alphaChannel = true);
	Texture(int width, int height, const glm::vec4& color);
	Texture(int width, int height, const glm::vec3 & color);
	~Texture();

	void setTextureParameters(GLint _internalFormat = GL_RGB, GLenum _format = GL_RGB, GLenum _type = GL_UNSIGNED_BYTE, bool _generateMipMap = true);

	void initGL();
	void freeGL();

	void init(const FileHandler::CompletePath& path) override;
};

struct CubeTexture : public Resource
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

	int m_textureUseCounts;

	CubeTexture();
	CubeTexture(char r, char g, char b);
	CubeTexture(const std::vector<FileHandler::CompletePath>& _paths);
	~CubeTexture();

	void setTextureParameters(GLint _internalFormat = GL_RGB, GLenum _format = GL_RGB, GLenum _type = GL_UNSIGNED_BYTE, bool _generateMipMap = false);

	void initGL();
	void freeGL();

	//For saving internal resource
	void init(const FileHandler::CompletePath& path) override;
	void load(const FileHandler::CompletePath& path);
	void save(const FileHandler::CompletePath& path) const;
};

