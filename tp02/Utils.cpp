#include "Utils.h"
//forward
#include "Camera.h"
#include "dirent.h"

#ifdef _WIN32 
#include <Windows.h>
#endif // _WIN32 

void AutoCompletion::addWord(std::string word)
{
	char* newWord = new char[word.size()+1];
	std::strcpy(newWord, word.c_str());
	m_words.push_back(newWord);
}

void AutoCompletion::clearWords()
{
	for (int i = 0; i < m_words.size(); i++)
		delete[] (m_words[i]);

	m_words.clear();

	//m_currentItem = 0;
}

AutoCompletion::AutoCompletion(): m_currentItem(0), m_upKey(false), m_downKey(false)
{

}


bool AutoCompletion::apply(std::string& result)
{
	//char** words = new char*[_words.size()];
	//for (int i = 0; i < _words.size(); i++)
	//{
	//	words[i] = new char[_words[i].size()+1];
	//	std::strcpy(words[i], _words[i].c_str());
	//	//_words[i].copy(words[i], _words[i].size());
	//}


	//if (ImGui::ListBox("##autocompletion", &m_currentItem, m_words.data(), (int)m_words.size()))
	//{
	//	result = m_words[m_currentItem];
	//}

	//for (int i = 0; i < _words.size(); i++)
	//	delete[] words[i];
	//delete[] words;

	if (!ImGui::GetIO().KeysDown[GLFW_KEY_DOWN]) {
		m_downKey = false;
	}
	if (!ImGui::GetIO().KeysDown[GLFW_KEY_UP]) {
		m_upKey = false;
	}

	if (ImGui::GetIO().KeysDown[GLFW_KEY_DOWN] && !m_downKey) {
		m_hasFocus = true;
		m_downKey = true;
		m_currentItem++;
		if(m_words.size())
			m_currentItem %= m_words.size();
	}
	else if (ImGui::GetIO().KeysDown[GLFW_KEY_UP] && !m_upKey) {
		m_hasFocus = true;
		m_upKey = true;
		m_currentItem = m_currentItem > 0 ? (m_currentItem - 1) : m_words.size() - 1;
	}

	if(m_words.size() > 0)
		ImGui::ListBox("##autocompletion", &m_currentItem, m_words.data(), (int)m_words.size());

	if (ImGui::GetIO().KeysDown[GLFW_KEY_TAB] || ImGui::GetIO().KeysDown[GLFW_KEY_ENTER]) {
		m_hasFocus = false;
		m_itemSelected = true;
		if(m_currentItem >=0 && m_currentItem < m_words.size())
			result = m_words[m_currentItem];
		return true;
	}

	return false;	
}


bool AutoCompletion::apply(char* result)
{

	if (!ImGui::GetIO().KeysDown[GLFW_KEY_DOWN]) {
		m_downKey = false;
	}
	if (!ImGui::GetIO().KeysDown[GLFW_KEY_UP]) {
		m_upKey = false;
	}

	if (ImGui::GetIO().KeysDown[GLFW_KEY_DOWN] && !m_downKey) {
		m_hasFocus = true;
		m_downKey = true;
		m_currentItem++;
		if (m_words.size())
			m_currentItem %= m_words.size();
	}
	else if (ImGui::GetIO().KeysDown[GLFW_KEY_UP] && !m_upKey) {
		m_hasFocus = true;
		m_upKey = true;
		m_currentItem = m_currentItem > 0 ? (m_currentItem - 1) : m_words.size() - 1;
	}

	ImGui::ListBox("##autocompletion", &m_currentItem, m_words.data(), (int)m_words.size());

	if (ImGui::GetIO().KeysDown[GLFW_KEY_TAB] || ImGui::GetIO().KeysDown[GLFW_KEY_ENTER]) {
		m_hasFocus = false;
		m_itemSelected = true;
		std::strcmp(result, m_words[m_currentItem]);
		if (m_currentItem >= 0 && m_currentItem < m_words.size())
			return true;
	}
	return false;
}

void AutoCompletion::toggleOpen()
{
	m_isOpen = !m_isOpen;
	if (m_isOpen == false)
		m_autocompletionJustClosed = true;
	else
		m_autocompletionJustClosed = false;
}

bool AutoCompletion::getIsOpen()
{
	return m_isOpen;
}

void AutoCompletion::setIsOpen(bool state)
{
	m_isOpen = state;
	if (m_isOpen == false)
		m_autocompletionJustClosed = true;
	else
		m_autocompletionJustClosed = false;
}

bool AutoCompletion::isAutocompletionJustClosed() const
{
	return m_autocompletionJustClosed;
}

bool AutoCompletion::getIsActive() const
{
	return m_isActive;
}

void AutoCompletion::setIsActive(bool state)
{
	m_isActive = state;
	m_itemSelected = false;
}

bool AutoCompletion::getHasFocus() const
{
	return m_hasFocus;
}

void AutoCompletion::setHasFocus(bool state)
{
	m_hasFocus = state;
	m_itemSelected = false;
}

bool AutoCompletion::getItemSelected() const
{
	return m_itemSelected;
}

void AutoCompletion::setItemSelected(bool state)
{
	m_itemSelected = state;
}

glm::vec3 screenToWorld(float mouse_x, float mouse_y, int width, int height, BaseCamera& camera)
{
	glm::mat4 projectionMatrix = camera.getProjectionMatrix();//glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.f);
	glm::mat4 viewMatrix = camera.getViewMatrix();//glm::lookAt(camera.eye, camera.o, camera.up);

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

glm::mat4 assimpMat4ToglmMat4(const aiMatrix4x4 & aiMat)
{
	return glm::mat4(aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1, aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2, aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3, aiMat.a4, aiMat.b4, aiMat.c4, aiMat.d4);
}

void assimpMat4ToglmMat4(const aiMatrix4x4 & aiMat, glm::mat4& glMat)
{
	glMat[0][0] = aiMat.a1; glMat[0][1] = aiMat.b1; glMat[0][2] = aiMat.c1; glMat[0][3] = aiMat.d1;
	glMat[1][0] = aiMat.a2; glMat[1][1] = aiMat.b2; glMat[1][2] = aiMat.c2; glMat[1][3] = aiMat.d2;
	glMat[2][0] = aiMat.a3; glMat[2][1] = aiMat.b3; glMat[2][2] = aiMat.c3; glMat[2][3] = aiMat.d3;
	glMat[3][0] = aiMat.a4; glMat[3][1] = aiMat.b4; glMat[3][2] = aiMat.c4; glMat[3][3] = aiMat.d4;
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

int idx2DToIdx1D(int i, int j, int array2DWidth)
{
	return j*array2DWidth + i;
}

bool rayOBBoxIntersect(glm::vec3 Start, glm::vec3 Dir, glm::vec3 P, glm::vec3 H[3], glm::vec3 E, float* t)
{
	float tfirst = 0.0f, tlast = 1.0f;

	if (!raySlabIntersect(glm::dot(Start, H[0]), glm::dot(Dir, H[0]), glm::dot(P, H[0] - E.x),glm::dot(P , H[0] + E.x), &tfirst, &tlast)) return false;
	if (!raySlabIntersect(glm::dot(Start, H[1]), glm::dot(Dir, H[1]), glm::dot(P, H[1] - E.y), glm::dot(P, H[1] + E.y), &tfirst, &tlast)) return false;
	if (!raySlabIntersect(glm::dot(Start, H[2]), glm::dot(Dir, H[2]), glm::dot(P, H[2] - E.z), glm::dot(P, H[2] + E.z), &tfirst, &tlast)) return false;

	*t = tfirst;
	return true;
}


bool raySlabIntersect(float start, float dir, float min, float max, float* tfirst, float* tlast)
{
	if (fabs(dir) < 0.00000001)
	{
		return (start < max && start > min);
	}

	float tmin = (min - start) / dir;
	float tmax = (max - start) / dir;
	if (tmin > tmax)
	{
		float w = tmin;
		tmin = tmax;
		tmax = w;
	}

	if (tmax < *tfirst || tmin > *tlast)
		return false;

	if (tmin > *tfirst) (*tfirst) = tmin;
	if (tmax < *tlast)  (*tlast) = tmax;
	return true;
}

std::vector<std::string> getAllDirNames(const std::string& path)
{
	std::vector<std::string> dirNames;

	DIR *dir;
	struct dirent *ent;

	if ((dir = opendir(path.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			if(ent->d_type == DT_DIR)
				dirNames.push_back(ent->d_name);
		}
		closedir(dir);
	}
	else {
		//could not open directory
		std::cout << "error, can't open directory at path : " << path << std::endl;
	}

	return dirNames;
}

std::vector<std::string> getAllFileAndDirNames(const std::string& path)
{
	std::vector<std::string> fileAndDirNames;

	DIR *dir;
	struct dirent *ent;

	if ((dir = opendir(path.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			fileAndDirNames.push_back(ent->d_name);
		}
		closedir(dir);
	}
	else {
		//could not open directory
		std::cout << "error, can't open directory at path : " << path << std::endl;
	}

	return fileAndDirNames;
}


bool directoryExists(const std::string& name, const std::string& path)
{
	DIR *dir;
	struct dirent *ent;

	if ((dir = opendir(path.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			if (ent->d_type == DT_DIR && std::string(ent->d_name) == name)
				return true;
		}
		closedir(dir);
	}
	else {
		//could not open directory
		std::cout << "error, can't open directory at path : " << path << std::endl;
	}

	return false;
}

bool directoryExists(const std::string& path)
{
	return (opendir(path.c_str()) != NULL);
}

void addDirectories(const std::string& path)
{
	if (directoryExists(path))
		return;

	std::vector<std::string> dirNames = splitString(path, '/', '\\');
	if (dirNames.size() == 0)
		return;

	std::string currentPath = "";

	for (int i = 0; i < dirNames.size(); i++)
	{
		if (!directoryExists(currentPath, dirNames[i]))
		{
			addDirectory(dirNames[i], currentPath);
		}
		currentPath += (dirNames[i] + "/");
	}
}

void addDirectory(const std::string& name, const std::string& path)
{
#ifdef _WIN32
	if ( path.find_last_of("/") >= path.size() || path.find_last_of("\\") >= path.size() )
		CreateDirectory( (path + "/" + name).c_str() , NULL);
	else
		CreateDirectory( (path + name).c_str(), NULL);
#endif // _WIN32

#ifdef linux
	std::cerr << "error : the addDirectory function isn't support on linux platform yet." << std::endl;
#endif // linux


	//TODO add linux support.
}

std::vector<std::string> splitString(const std::string& s, char delim)
{
	std::vector<std::string> elements;
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)){
		elements.push_back(item);
	}
	return elements;
}

std::vector<std::string> splitString(const std::string& s, char delim01, char delim02)
{
	std::vector<std::string> tmpElements;
	std::vector<std::string> elements;
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim01)) {
		tmpElements.push_back(item);
	}

	for (int i = 0; i < tmpElements.size(); i++)
	{
		std::stringstream ss2(tmpElements[i]);

		while (std::getline(ss2, item, delim02)) {
			elements.push_back(item);
		}
	}

	return elements;
}

std::size_t splitPathFileName(const std::string& pathAndFileName, std::string& path, std::string& filename)
{
	std::size_t pathLength = pathAndFileName.find_last_of("/\\");
	if (pathLength == std::string::npos) {
		pathLength = 0;
		path = "";
		filename = pathAndFileName;
	}
	else {
		path = pathAndFileName.substr(0, pathLength);
		filename = pathAndFileName.substr(pathLength + 1);
	}
	return pathLength;
}

namespace Physic{

	void computeLink(float deltaTime, Link* link)
	{
		float d = glm::distance(link->M1->position, link->M2->position);
		if (d < 0.00000001f)
			return;

		float f = link->k * (1.f - link->l / d);
		if (std::abs(f) < 0.00000001f)
			return;

		glm::vec3 M1M2 = link->M2->position - link->M1->position;
		glm::normalize(M1M2);
		if (glm::length(M1M2) < 0.00000001f)
			return;

		//frein :
		glm::vec3 frein = link->z*(link->M2->vitesse - link->M1->vitesse);

		link->M1->force += (f * M1M2 + frein);
		link->M2->force += (-f * M1M2 - frein);

	}

	void computePoints(float deltaTime, Point* point)
	{
		if (point->masse < 0.00000001f)
			return;

		//leapfrog
		point->vitesse += (deltaTime / point->masse)*point->force;
		point->position += deltaTime*point->vitesse;
		point->force = glm::vec3(0, 0, 0);
	}
}

