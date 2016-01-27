#include "Texture.h"


Texture::Texture(char r, char g, char b) : glId(0), path(""), internalFormat(GL_RGB), format(GL_RGB), type(GL_UNSIGNED_BYTE), generateMipMap(true), m_textureUseCounts(0)
{
	comp = 4;
	w = 1;
	h = 1;
	pixels = new unsigned char[3];
	pixels[0] = r;
	pixels[0] = g;
	pixels[0] = b;
}


Texture::Texture(const std::string& _path) : glId(0), path(_path), internalFormat(GL_RGB), format(GL_RGB), type(GL_UNSIGNED_BYTE), generateMipMap(true), m_textureUseCounts(0)
{
	pixels = stbi_load(path.c_str(), &w, &h, &comp, 3);
}


Texture::~Texture()
{
	delete[] pixels;
}

void Texture::setTextureParameters(GLint _internalFormat, GLenum _format, GLenum _type, bool _generateMipMap)
{
	internalFormat = _internalFormat;
	format = _format;
	type = _type;
	generateMipMap = _generateMipMap;
}

void Texture::initGL()
{

	m_textureUseCounts++;

	if (m_textureUseCounts == 1)
	{
		glGenTextures(1, &glId);

		glBindTexture(GL_TEXTURE_2D, glId);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, type, pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (generateMipMap)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
}

void Texture::freeGL()
{
	m_textureUseCounts--;

	if (m_textureUseCounts <= 0)
	{
		if(glId > 0)
			glDeleteTextures(1, &glId);

		m_textureUseCounts = 0;
	}
}
