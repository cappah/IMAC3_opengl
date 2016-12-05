#include "stdafx.h"

#include "Texture.h"
#include "Utils.h"
#include "ErrorHandler.h"
#include "Project.h"

#include "jsoncpp/json/json.h"

Texture::Texture(int width, int height, bool useAlpha)
	: glId(0)
	, type(GL_UNSIGNED_BYTE)
	, generateMipMap(true)
	, m_textureUseCounts(0)
	, comp(3)
	, pixels(0)
	, w(width)
	, h(height)
	, textureWrapping_u(GL_REPEAT)
	, textureWrapping_v(GL_REPEAT)
	, minFilter(GL_LINEAR)
	, magFilter(GL_LINEAR)
{
	if (!useAlpha)
	{
		internalFormat = GL_RGB;
		format = GL_RGB;
		comp = 3;
	}
	else
	{
		internalFormat = GL_RGBA;
		format = GL_RGBA;
		comp = 4;
	}

	//pixels = new unsigned char[3*width*height];
	//for (int i = 0; i < 3*width*height; i++) {
	//	pixels[i] = 255;
	//}
}

Texture::Texture(unsigned char * _pixels, int width, int height, bool useAlpha, int _comp) 
	: Texture(width, height, useAlpha)
{
	comp = _comp;
	pixels = _pixels;
}

Texture::Texture(const glm::vec4& color)
	: Texture(1, 1, color)
{

}

Texture::Texture(const glm::vec3& color)
	: Texture(1, 1, color)
{

}

Texture::Texture(int width, int height, const glm::vec4& color) 
	: Texture(width, height, true)
{
	comp = 4;
	pixels = new unsigned char[4*width*height];
	for (int i = 0; i < width * height * 4; i += 4)
	{
		pixels[i] = color.r;
		pixels[i + 1] = color.g;
		pixels[i + 2] = color.b;
		pixels[i + 3] = color.a;
	}
}

Texture::Texture(int width, int height, const glm::vec3& color) 
	: Texture(width, height, false)
{
	comp = 3;
	pixels = new unsigned char[3 * width*height];
	for (int i = 0; i < width * height * 3; i += 3)
	{
		pixels[i] = color.r;
		pixels[i + 1] = color.g;
		pixels[i + 2] = color.b;
	}
}

Texture::Texture(const FileHandler::CompletePath& _path, bool alphaChannel)
	: Resource(_path)
	, glId(0)
	, generateMipMap(true)
	, m_textureUseCounts(0)
	, textureWrapping_u(GL_REPEAT)
	, textureWrapping_v(GL_REPEAT)
	, minFilter(GL_LINEAR)
	, magFilter(GL_LINEAR)
{
	if (!alphaChannel)
	{
		internalFormat = GL_RGB;
		format = GL_RGB;
		type = GL_UNSIGNED_BYTE;
		pixels = stbi_load(_path.c_str(), &w, &h, &comp, 3);
	}
	else
	{
		internalFormat = GL_RGBA;
		format = GL_RGBA;
		type = GL_UNSIGNED_BYTE;
		pixels = stbi_load(_path.c_str(), &w, &h, &comp, 4);
	}
}

void Texture::init(const FileHandler::CompletePath& path)
{
	Resource::init(path);

	assert(!Project::isPathPointingInsideProjectFolder(path));
	FileHandler::CompletePath absolutePath = Project::getAbsolutePathFromRelativePath(path);

	internalFormat = GL_RGB;
	format = GL_RGB;
	type = GL_UNSIGNED_BYTE;
	pixels = stbi_load(absolutePath.c_str(), &w, &h, &comp, 3);

	initGL();
}

void Texture::drawInInspector(Scene & scene)
{
	Resource::drawInInspector(scene);

	ImGui::Image((void*)glId, ImVec2(w, h), ImVec2(0, 1), ImVec2(1, 0));
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

void Texture::setTextureMinMaxFilters(GLint _magFilter, GLint _minFilter)
{
	magFilter = _magFilter;
	minFilter = _minFilter;
}

void Texture::setTextureWrapping(GLint _uWrapping, GLint _vWrapping)
{
	textureWrapping_u = _uWrapping;
	textureWrapping_v = _vWrapping;
}

void Texture::initGL()
{

	//m_textureUseCounts++;

	//if (m_textureUseCounts == 1)
	if(glId <= 0)
	{
		glGenTextures(1, &glId);

		glBindTexture(GL_TEXTURE_2D, glId);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, type, pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrapping_u);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrapping_v);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

		if (generateMipMap)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);

		if (!checkError("Uniforms"))
			PRINT_ERROR("error in texture initialization.")
	}
}

void Texture::freeGL()
{
	//m_textureUseCounts--;

	//if (m_textureUseCounts <= 0)
	if(glId > 0){
		glDeleteTextures(1, &glId);
	}
}

//////////////////////////////////////////

CubeTexture::CubeTexture()
	: CubeTexture(0, 0, 0)
{

}

CubeTexture::CubeTexture(char r, char g, char b) 
	: glId(0)
	, internalFormat(GL_RGB)
	, format(GL_RGB)
	, type(GL_UNSIGNED_BYTE)
	, generateMipMap(false)
	, m_textureUseCounts(0)
	, magFilter(GL_LINEAR)
	, minFilter(GL_LINEAR)
	, textureWrapping_s(GL_CLAMP_TO_EDGE)
	, textureWrapping_t(GL_CLAMP_TO_EDGE)
	, textureWrapping_r(GL_CLAMP_TO_EDGE)
{
	comp = 3;
	w = 1;
	h = 1;

	for (int i = 0; i < 6; i++)
	{
		pixels[i] = new unsigned char[3];
		pixels[i][0] = r;
		pixels[i][1] = g;
		pixels[i][2] = b;
	}
}

CubeTexture::CubeTexture(const std::vector<FileHandler::CompletePath>& _paths) 
	: glId(0)
	, internalFormat(GL_RGB)
	, format(GL_RGB)
	, type(GL_UNSIGNED_BYTE)
	, generateMipMap(true)
	, m_textureUseCounts(0)
	, magFilter(GL_LINEAR)
	, minFilter(GL_LINEAR)
	, textureWrapping_s(GL_CLAMP_TO_EDGE)
	, textureWrapping_t(GL_CLAMP_TO_EDGE)
	, textureWrapping_r(GL_CLAMP_TO_EDGE)

{
	for (int i = 0; i < 6; i++)
	{
		paths[i] = _paths[i];
		pixels[i] = stbi_load(paths[i].c_str(), &w, &h, &comp, 3);
	}
}


CubeTexture::~CubeTexture()
{
	for (int i = 0; i < 6; i++)
	{
		delete[] pixels[i];
	}
}

void CubeTexture::setTextureParameters(GLint _internalFormat, GLenum _format, GLenum _type, bool _generateMipMap)
{
	internalFormat = _internalFormat;
	format = _format;
	type = _type;
	generateMipMap = _generateMipMap;
}

void CubeTexture::setTextureMinMaxFilters(GLint _magFilter, GLint _minFilter)
{
	magFilter = _magFilter;
	minFilter = _minFilter;
}

void CubeTexture::setTextureWrapping(GLint _sWrapping, GLint _tWrapping, GLint _rWrapping)
{
	textureWrapping_s = _sWrapping;
	textureWrapping_t = _tWrapping;
	textureWrapping_r = _rWrapping;
}

void CubeTexture::initGL()
{
	if (glId <= 0){

		glGenTextures(1, &glId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, glId);

		for (int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, w, h, 0, format, type, pixels[i]);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, magFilter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, textureWrapping_s);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, textureWrapping_t);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, textureWrapping_r);
		

		if (generateMipMap)
		{
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		}
		else
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		if (!checkError("Uniforms"))
			PRINT_ERROR("error in texture initialization.")
	}
}

void CubeTexture::freeGL()
{
	if (glId > 0){
			glDeleteTextures(1, &glId);
	}
}

void CubeTexture::init(const FileHandler::CompletePath & path)
{
	Resource::init(path);

	assert(!Project::isPathPointingInsideProjectFolder(path)); //path should be relative
	FileHandler::CompletePath absolutePath = Project::getAbsolutePathFromRelativePath(path);

	load(absolutePath);

	initGL();
}

void CubeTexture::load(const FileHandler::CompletePath & path)
{
	std::ifstream stream;
	stream.open(path.toString());
	if (!stream.is_open())
	{
		std::cout << "error, can't load cube texture at path : " << path.toString() << std::endl;
		return;
	}
	Json::Value root;
	stream >> root;

	for (int i = 0; i < 6; i++)
	{
		const std::string strPath = root[i]["path"].asString();
		paths[i] = FileHandler::CompletePath(strPath);
		pixels[i] = stbi_load(paths[i].c_str(), &w, &h, &comp, 3);
	}

	internalFormat = root["internalFormat"].asInt();
	format = root["format"].asInt();
	type = root["type"].asInt();
	generateMipMap = root["generateMipMap"].asBool();
}

void CubeTexture::save(const FileHandler::CompletePath & path) const
{
	Json::Value root;

	for (int i = 0; i < 6; i++)
	{
		root[i]["path"] = paths[i].toString();
	}

	root["internalFormat"] = (int)internalFormat;
	root["format"] = (int)format;
	root["type"] = (int)type;
	root["generateMipMap"] = generateMipMap;

	std::ofstream stream;
	stream.open(path.toString());
	if (!stream.is_open())
	{
		std::cout << "error, can't load cube texture at path : " << path.toString() << std::endl;
		return;
	}
	stream << root;
}

///////////////////////////////////////////
//// BEGIN : texture helpers



namespace GlHelper {

	Texture* makeNewColorTexture(float width, float height)
	{
		// Create color texture
		Texture* newTexture = new Texture(width, height, true);
		newTexture->setTextureParameters(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, true);
		newTexture->setTextureMinMaxFilters(GL_NEAREST, GL_NEAREST);
		newTexture->setTextureWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		return newTexture;
	}

	Texture* makeNewNormalTexture(float width, float height)
	{
		// Create normal texture
		Texture* newTexture = new Texture(width, height, true);
		newTexture->setTextureParameters(GL_RGBA16, GL_RGBA, GL_FLOAT, true);
		newTexture->setTextureMinMaxFilters(GL_NEAREST, GL_NEAREST);
		newTexture->setTextureWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		return newTexture;
	}

	Texture* makeNewDepthTexture(float width, float height)
	{
		// Create depth texture
		Texture* newTexture = new Texture(width, height, true);
		newTexture->setTextureParameters(GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT, true);
		newTexture->setTextureMinMaxFilters(GL_NEAREST, GL_NEAREST);
		newTexture->setTextureWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		return newTexture;
	}
}

//// END : texture helpers
///////////////////////////////////////////