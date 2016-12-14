#pragma once

#include "glew/glew.h"
#include <string>
#include <iostream>


inline bool checkError(const char* title)
{
	int error;
	if ((error = glGetError()) != GL_NO_ERROR)
	{
		std::string errorString;
		switch (error)
		{
		case GL_INVALID_ENUM:
			errorString = "GL_INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			errorString = "GL_INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			errorString = "GL_INVALID_OPERATION";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			errorString = "GL_INVALID_FRAMEBUFFER_OPERATION";
			break;
		case GL_OUT_OF_MEMORY:
			errorString = "GL_OUT_OF_MEMORY";
			break;
		default:
			errorString = "UNKNOWN";
			break;
		}
		fprintf(stdout, "OpenGL Error(%s): %s\n", errorString.c_str(), title);
	}
	return error == GL_NO_ERROR;
}


inline std::string GLErrorToString(int error)
{
	if (error != GL_NO_ERROR)
	{
		switch (error)
		{
		case GL_INVALID_ENUM:
			return "GL_INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			return "GL_INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			return "GL_INVALID_OPERATION";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			return "GL_INVALID_FRAMEBUFFER_OPERATION";
			break;
		case GL_OUT_OF_MEMORY:
			return "GL_OUT_OF_MEMORY";
			break;
		default:
			return "UNKNOWN";
			break;
		}
	}
	else
		return "NO_ERROR";
}

#define PRINT_GL_ERROR(error)\
{\
	std::cerr << "----------------- Error detected -----------------" << std::endl;\
	std::cerr << "- In file : " << __FILE__ << std::endl;\
	std::cerr << "- At line : " << __LINE__ << std::endl;\
	std::cerr << "- GL message : " << GLErrorToString(error) << std::endl; \
	std::cerr << "- Message : no message" << std::endl;\
}

#define PRINT_GL_ERROR(error, message)\
{\
	std::cerr << "----------------- Error detected -----------------" << std::endl;\
	std::cerr << "- In file : " << __FILE__ << std::endl;\
	std::cerr << "- At line : " << __LINE__ << std::endl;\
	std::cerr << "- GL message : " << GLErrorToString(error) << std::endl; \
	std::cerr << "- Message : "<< message << std::endl;\
}


#define CHECK_GL_ERROR()\
{\
	int error = glGetError();\
	if (error) PRINT_GL_ERROR(error);\
}\

#define CHECK_GL_ERROR(message)\
{\
	int error = glGetError();\
	if (error) PRINT_GL_ERROR(error, message);\
}\

#define PRINT_ERROR(message)\
{\
	std::cerr << "----------------- Error detected -----------------" << std::endl;\
	std::cerr << "- In file : " << __FILE__ << std::endl;\
	std::cerr << "- At line : " << __LINE__ << std::endl;\
	std::cerr << "- Message : " << message << std::endl;\
}

//#define PRINT_ERROR()\
//{\
//	std::cerr << "----------------- Error detected -----------------" << std::endl;\
//	std::cerr << "- In file : " << __FILE__ << std::endl;\
//	std::cerr << "- At line : " << __LINE__ << std::endl;\
//	std::cerr << "- Message : no message" << std::endl;\
//}

//#define PRINT_WARNING()\
//{\
//	std::cerr << "----------------- Warning -----------------" << std::endl;\
//	std::cerr << "- In file : " << __FILE__ << std::endl;\
//	std::cerr << "- At line : " << __LINE__ << std::endl;\
//	std::cerr << "- Message : no message" << std::endl;\
//}

#define PRINT_WARNING(message)\
{\
	std::cerr << "----------------- Warning -----------------" << std::endl;\
	std::cerr << "- In file : " << __FILE__ << std::endl;\
	std::cerr << "- At line : " << __LINE__ << std::endl;\
	std::cerr << "- Message : " << message << std::endl;\
}