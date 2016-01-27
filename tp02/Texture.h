#pragma once

#include <string>

#include "stb/stb_image.h"
#include "glm/common.hpp"

#include "glew/glew.h"

struct Texture
{
	std::string path;

	int w;
	int h;
	int comp;

	unsigned char * pixels;

	GLuint glId;

	GLint internalFormat = GL_RGB;
	GLenum format = GL_RGB;
	GLenum type = GL_UNSIGNED_BYTE;
	bool generateMipMap = true;

	int m_textureUseCounts;

	Texture(char r, char g, char b);
	Texture(const std::string& _path);
	~Texture();

	void setTextureParameters(GLint _internalFormat = GL_RGB, GLenum _format = GL_RGB, GLenum _type = GL_UNSIGNED_BYTE, bool _generateMipMap = true);

	void initGL();
	void freeGL();
};

