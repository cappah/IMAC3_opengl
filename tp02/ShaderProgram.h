#pragma once

#include <istream>
#include <glew/glew.h>
#include "FileHandler.h"
#include "Resource.h"
//TODO

struct ShaderProgram : public Resource
{
	GLuint id;

	ShaderProgram()
		:id(0)
	{
	}

	ShaderProgram(const FileHandler::CompletePath& path)
		: Resource(path)
		, id(0)
	{
	}

	void init(const FileHandler::CompletePath& path) override
	{
		load(path);
	}

	void load(const FileHandler::CompletePath& path)
	{
		std::ifstream stream;
		stream.open(path.toString());
		if (!stream.is_open())
		{
			std::cout << "error, can't load shader program at path : " << path.toString() << std::endl;
			return;
		}
		Json::Value root;
		stream >> root;

		const FileHandler::CompletePath vertexPath = root.get("vertex", "").asString();
		const FileHandler::CompletePath fragmentPath = root.get("fragment", "").asString();
		const FileHandler::CompletePath geometryPath = root.get("geometry", "").asString();

		load(vertexPath, fragmentPath, geometryPath);
	}

	void load(const FileHandler::CompletePath& vertexShaderPath, const FileHandler::CompletePath& fragmentShaderPath)
	{
		bool hasVertShader = !vertexShaderPath.empty();
		bool hasFragShader = !fragmentShaderPath.empty();

		GLuint vertShaderId = 0;
		GLuint fragShaderId = 0;

		if (hasVertShader)
			vertShaderId = compile_shader_from_file(GL_VERTEX_SHADER, vertexShaderPath.c_str());
		if (hasFragShader)
			fragShaderId = compile_shader_from_file(GL_FRAGMENT_SHADER, fragmentShaderPath.c_str());

		GLuint programObject = glCreateProgram();
		if (hasVertShader)
			glAttachShader(programObject, vertShaderId);
		if (hasFragShader)
			glAttachShader(programObject, fragShaderId);

		glLinkProgram(programObject);
		if (check_link_error(programObject) < 0)
			exit(1);

		//check uniform errors : 
		if (!checkError("Uniforms"))
			exit(1);
	}


	void load(const FileHandler::CompletePath& vertexShaderPath, const FileHandler::CompletePath& fragmentShaderPath, const FileHandler::CompletePath& geometryShaderPath)
	{
		bool hasVertShader = !vertexShaderPath.empty();
		bool hasFragShader = !fragmentShaderPath.empty();
		bool hasGeomShader = !geometryShaderPath.empty();

		GLuint vertShaderId = 0;
		GLuint fragShaderId = 0;
		GLuint geomShaderId = 0;

		if (hasVertShader)
			vertShaderId = compile_shader_from_file(GL_VERTEX_SHADER, vertexShaderPath.c_str());
		if (hasFragShader)
			fragShaderId = compile_shader_from_file(GL_FRAGMENT_SHADER, fragmentShaderPath.c_str());
		if (hasGeomShader)
			geomShaderId = compile_shader_from_file(GL_GEOMETRY_SHADER, geometryShaderPath.c_str());

		GLuint programObject = glCreateProgram();
		if (hasVertShader)
			glAttachShader(programObject, vertShaderId);
		if (hasFragShader)
			glAttachShader(programObject, fragShaderId);
		if (hasGeomShader)
			glAttachShader(programObject, geomShaderId);

		glLinkProgram(programObject);
		if (check_link_error(programObject) < 0)
			exit(1);

		//check uniform errors : 
		if (!checkError("Uniforms"))
			exit(1);
	}


	ShaderProgram(const FileHandler::CompletePath& vertexShaderPath, const FileHandler::CompletePath& fragmentShaderPath)
		: id(0)
	{
		load(vertexShaderPath, fragmentShaderPath);
	}


	ShaderProgram(const FileHandler::CompletePath& vertexShaderPath, const FileHandler::CompletePath& fragmentShaderPath, const FileHandler::CompletePath& geometryShaderPath)
		: id(0)
	{
		load(vertexShaderPath, fragmentShaderPath, geometryShaderPath);
	}

	//Not copyable
	ShaderProgram(const ShaderProgram& other) = delete;
	const ShaderProgram& operator=(const ShaderProgram& other) = delete;
};