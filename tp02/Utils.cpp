#include "Utils.h"

//forward
#include "Application.h"

glm::vec3 screenToWorld(float mouse_x, float mouse_y, int width, int height, Camera& camera)
{
	glm::mat4 projectionMatrix = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.f);
	glm::mat4 viewMatrix = glm::lookAt(camera.eye, camera.o, camera.up);

	float x = (2.0f * mouse_x) / (float)width - 1.0f;
	float y = 1.0f - (2.0f * mouse_y) / (float)height;
	float z = 1.0f;
	glm::vec3 ray_nds = glm::vec3(x, y, z);

	glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);

	glm::vec4 ray_eye = inverse(projectionMatrix) * ray_clip;
	ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
	ray_eye = (inverse(viewMatrix) * ray_eye);

	glm::vec3 ray_wor(ray_eye.x, ray_eye.y, ray_eye.z);
	// don't forget to normalise the vector at some point
	ray_wor = glm::normalize(ray_wor);

	return ray_wor;
}

// No windows implementation of strsep
char * strsep_custom(char **stringp, const char *delim)
{
	register char *s;
	register const char *spanp;
	register int c, sc;
	char *tok;
	if ((s = *stringp) == NULL)
		return (NULL);
	for (tok = s; ; ) {
		c = *s++;
		spanp = delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;
				*stringp = s;
				return (tok);
			}
		} while (sc != 0);
	}
	return 0;
}

int check_compile_error(GLuint shader, const char ** sourceBuffer)
{
	// Get error log size and print it eventually
	int logLength;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 1)
	{
		char * log = new char[logLength];
		glGetShaderInfoLog(shader, logLength, &logLength, log);
		char *token, *string;
		string = strdup(sourceBuffer[0]);
		int lc = 0;
		while ((token = strsep_custom(&string, "\n")) != NULL) {
			printf("%3d : %s\n", lc, token);
			++lc;
		}
		fprintf(stderr, "Compile : %s", log);
		delete[] log;
	}
	// If an error happend quit
	int status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
		return -1;
	return 0;
}

int check_link_error(GLuint program)
{
	// Get link error log size and print it eventually
	int logLength;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 1)
	{
		char * log = new char[logLength];
		glGetProgramInfoLog(program, logLength, &logLength, log);
		fprintf(stderr, "Link : %s \n", log);
		delete[] log;
	}
	int status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
		return -1;
	return 0;
}


GLuint compile_shader(GLenum shaderType, const char * sourceBuffer, int bufferSize)
{
	GLuint shaderObject = glCreateShader(shaderType);
	const char * sc[1] = { sourceBuffer };
	glShaderSource(shaderObject,
		1,
		sc,
		NULL);
	glCompileShader(shaderObject);
	check_compile_error(shaderObject, sc);
	return shaderObject;
}

GLuint compile_shader_from_file(GLenum shaderType, const char * path)
{
	FILE * shaderFileDesc = fopen(path, "rb");
	if (!shaderFileDesc)
		return 0;
	fseek(shaderFileDesc, 0, SEEK_END);
	long fileSize = ftell(shaderFileDesc);
	rewind(shaderFileDesc);
	char * buffer = new char[fileSize + 1];
	fread(buffer, 1, fileSize, shaderFileDesc);
	buffer[fileSize] = '\0';
	GLuint shaderObject = compile_shader(shaderType, buffer, fileSize);
	delete[] buffer;
	return shaderObject;
}


bool checkError(const char* title)
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


double interpolation_cos2D(double a, double b, double c, double d, double x, double y) 
{
	double y1 = interpolation_cos1D(a, b, x);
	double y2 = interpolation_cos1D(c, d, x);
	return  interpolation_cos1D(y1, y2, y);
}

double interpolation_cos1D(double a, double b, double x) 
{
	double k = (1 - cos(x * glm::pi<double>())) / 2;
	return a * (1 - k) + b * k;
}

glm::vec3 vertexFrom3Floats(const std::vector<float>& vertices, int indice)
{
	indice *= 3;
	return glm::vec3(vertices[indice], vertices[indice + 1], vertices[indice + 2]);
}

void mapViewportToScreenRegion(glm::vec4& viewport, const glm::vec4& screenRegion)
{
	int W = Application::get().getWindowWidth();
	int H = Application::get().getWindowHeight();

	int X = screenRegion.x;
	int Y = screenRegion.y;

	int w = screenRegion.z;
	int h = screenRegion.w;

	float xv = viewport.x;
	float yv = viewport.w;

	float wv = viewport.z;
	float hv = viewport.w;

	viewport.x = ((xv + 1) / 2.f) * w + X;
	viewport.y = ((yv + 1) / 2.f) * h + Y;

	viewport.z = (wv / 2.f) * w;
	viewport.w = (hv / 2.f) * h;
}
