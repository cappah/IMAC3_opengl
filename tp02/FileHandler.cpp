#include <sstream>

#include "dirent.h"
#include "FileHandler.h"

namespace FileHandler {

Path::Path()
	:m_data("")
{
	format();
}

Path::Path(const std::string& path)
	: m_data(path)
{
	format();
}

Path::Path(const CompletePath& completePath)
	:Path(completePath.getPath())
{
	format();
}

void Path::push_back(const std::string& folderName)
{
	size_t offset = 0;
	if (folderName[0] == '/' || folderName[0] == '\\')
	{
		offset = 1;
	}

	m_data.append(folderName, offset, folderName.size() - offset);

	char data_last = *m_data.end();
	if (data_last != '/')
	{
		if (data_last == '\\')
		{
			m_data.pop_back();
		}
		m_data.push_back('/');
	}
}

void Path::push_back(const Path& subPath)
{
	push_back(subPath.m_data);
}

void Path::pop_back()
{
	size_t cutIdx = m_data.find_last_of('/', m_data.size() - 1);
	m_data = m_data.substr(0, cutIdx + 1);
}

const std::string& Path::operator[](size_t idx)
{
	size_t cutPos01 = m_data.find_first_of("/\\");
	size_t cutPos02 = m_data.find_first_of("/\\", cutPos01 + 1);

	for (int i = 1; i < idx; i++)
	{
		cutPos01 = m_data.find_first_of("/\\", cutPos02 + 1);
		cutPos02 = m_data.find_first_of("/\\", cutPos01 + 1);
	}

	return m_data.substr(cutPos01 + 1, (cutPos02 - cutPos01) - 1);
}

size_t Path::size()
{
	size_t size = 0;
	size_t cutPos = m_data.find_first_of("/\\");

	for (int i = 1; i < m_data.size(); i++)
	{
		cutPos = m_data.find_first_of("/\\", cutPos + 1);
		size++;
	}
	return size;
}

size_t Path::getPathFolderNames(std::vector<std::string>& folderNames) const
{
	size_t size = 0;
	size_t cutPos01 = m_data.find_first_of("/\\");
	size_t cutPos02 = m_data.find_first_of("/\\", cutPos01 + 1);

	for (int i = 1; i < m_data.size(); i++)
	{
		cutPos01 = m_data.find_first_of("/\\", cutPos02 + 1);
		cutPos02 = m_data.find_first_of("/\\", cutPos01 + 1);

		folderNames.push_back(m_data.substr(cutPos01 + 1, (cutPos02 - cutPos01) - 1));

		size++;
	}

	return size;
}

void Path::format()
{
	if (m_data.empty())
		return;

	size_t foundPos = m_data.find_first_of('\\');
	while (foundPos != std::string::npos)
	{
		m_data[foundPos] = '/';
		foundPos = m_data.find_first_of('\\', foundPos + 1);
	}

	foundPos = m_data.find_last_of('/');
	if (foundPos == std::string::npos)
	{
		m_data.push_back('/');
	}
}

const std::string& Path::toString() const
{
	return m_data;
}

std::ofstream& operator<<(std::ofstream& os, const Path& path)
{
	os << path.c_str();
	return os;
}

//////////////////////////////


CompletePath::CompletePath()
	: m_fileName("")
	, m_extention("")
	, m_complete(false)
{
	format();
}

CompletePath::CompletePath(Path path, const std::string& fileName, const std::string& extention)
	: m_path(path)
	, m_fileName(fileName)
	, m_extention(extention)
	, m_complete(true)
{
	if (m_extention.size() > 0 && m_extention[0] != '.')
		m_extention.insert(m_extention.begin(), '.');

	assert(hasValidExtention() && hasValidFileName());
}

CompletePath::CompletePath(Path path, const std::string& fileNameAndExtention)
	: m_path(path)
	, m_fileName("")
	, m_extention("")
{
	if (!getFileNameAndExtentionFromExtendedFilename(fileNameAndExtention, m_fileName, m_extention) != std::string::npos)
	{
		m_complete = false;
	}
	assert(hasValidExtention() && hasValidFileName());
}

const std::string& CompletePath::toString() const
{
	return m_path.toString() + m_fileName + m_extention;
}

const std::string& CompletePath::operator[](size_t idx)
{
	const std::string& folderPath = m_path.toString();

	size_t cutPos01 = folderPath.find_first_of("/\\");
	size_t cutPos02 = folderPath.find_first_of("/\\", cutPos01 + 1);

	for (int i = 1; i < idx; i++)
	{
		cutPos01 = folderPath.find_first_of("/\\", cutPos02 + 1);
		cutPos02 = folderPath.find_first_of("/\\", cutPos01 + 1);
	}

	return folderPath.substr(cutPos01 + 1, (cutPos02 - cutPos01) - 1);
}

const Path& CompletePath::getPath() const
{
	return m_path;
}

const Path& CompletePath::getFilename() const
{
	return m_fileName;
}

const std::string& CompletePath::getFilenameWithExtention() const
{
	return m_fileName + m_extention;
}

const std::string& CompletePath::getExtention() const
{
	return m_extention;
}

FileType CompletePath::getFileType() const
{
	return getFileTypeFromExtention(getExtention());
}

bool CompletePath::hasValidFileName() const
{
	return (m_fileName != ""
		&& m_fileName.find_first_of("/\\.-'&") == std::string::npos
		);
}

void CompletePath::format()
{
	//format path
	m_path.format();

	if (!m_fileName.empty())
	{
		//remove /filename patern
		if (m_fileName.find_first_of('/\\') == 0)
		{
			m_fileName.erase(0);
		}
		//remove filename. patern
		if (m_fileName.find_last_of('.') != std::string::npos)
		{
			m_fileName.pop_back();
		}
	}
	if (!m_extention.empty())
	{
		//remove extension patern (-> .extention)
		if (m_extention.find_first_of('.') != 0)
		{
			m_extention.insert(m_extention.begin(), '.');
		}
	}
}

bool CompletePath::hasValidExtention() const 
{
	return (m_extention != ""
		&& m_extention.find_first_of("/\\-'&") == std::string::npos
		&& m_extention.find_first_of('.') == 0
		);
};

std::ofstream& operator<<(std::ofstream& os, const CompletePath& path)
{
	os << path.c_str();
	return os;
}

//////////////////////////////

size_t getExtentionFromExtendedFilename(const std::string& filename, std::string& outExtention)
{
	size_t cutPos = filename.find_last_of('.');
	if (cutPos == std::string::npos)
		return cutPos;
	else
	{
		outExtention = filename.substr(cutPos);
		return cutPos;
	}
}

size_t getFileNameAndExtentionFromExtendedFilename(const std::string& extendedFileName, std::string& filename, std::string& outExtention)
{
	size_t cutPos = extendedFileName.find_last_of('.');
	if (cutPos == std::string::npos)
		return cutPos;
	else
	{
		filename = extendedFileName.substr(0, cutPos);
		outExtention = extendedFileName.substr(cutPos);
		return cutPos;
	}
}

FileType getFileTypeFromExtention(const std::string& extention)
{
	assert(extention.find_first_of("/\\_,;") == std::string::npos);

	if (extention == ".jpg" || extention == ".jpeg" || extention == ".png" || extention == ".bmp")
	{
		return FileType::IMAGE;
	}
	else if (extention == ".obj" || extention == ".fbx")
	{
		return FileType::MESH;
	}
	else if (extention == ".mp3" || extention == ".ogg")
	{
		return FileType::SOUND;
	}
	else
	{
		std::cerr << "L'extention : " << extention << " n'est pas prix en charge." << std::endl;
		return FileType::NONE;
	}
}

std::vector<std::string> getAllDirNames(const Path& path)
{
	std::vector<std::string> dirNames;

	DIR *dir;
	struct dirent *ent;

	if ((dir = opendir(path.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			if (ent->d_type == DT_DIR)
				dirNames.push_back(ent->d_name);
		}
		closedir(dir);
	}
	else {
		//could not open directory
		std::cout << "error, can't open directory at path : " << path.toString() << std::endl;
	}

	return dirNames;
}

std::vector<std::string> getAllFileAndDirNames(const Path& path)
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
		std::cout << "error, can't open directory at path : " << path.toString() << std::endl;
	}

	return fileAndDirNames;
}


bool directoryExists(const Path& path, const std::string& name)
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
		std::cout << "error, can't open directory at path : " << path.toString() << std::endl;
	}

	return false;
}

bool directoryExists(const Path& path)
{
	return (opendir(path.c_str()) != NULL);
}

bool fileExists(const CompletePath& completePath)
{
	if (!completePath.hasValidFileName())
		return false;

	const std::string& path = completePath.getPath().toString();
	const std::string& filename = completePath.getFilenameWithExtention();

	DIR *dir;
	struct dirent *ent;

	if ((dir = opendir(path.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			if (ent->d_type != DT_DIR && std::string(ent->d_name) == filename)
				return true;
		}
		closedir(dir);
	}
	return false;
}

void addDirectories(const Path& path)
{
	if (directoryExists(path))
		return;

	std::vector<std::string> dirNames;
	if(path.getPathFolderNames(dirNames) == 0)
		return;

	Path currentPath;

	for (int i = 0; i < dirNames.size(); i++)
	{
		if (!directoryExists(currentPath, dirNames[i]))
		{
			addDirectory(dirNames[i], currentPath);
		}
		currentPath.push_back(dirNames[i]);
	}
}

void addDirectory(const std::string& name, const Path& path)
{
#ifdef _WIN32
		CreateDirectory((path.toString() + name).c_str(), NULL);
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
	while (std::getline(ss, item, delim)) {
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

void copyFilePastToNewFile(const CompletePath& from, const Path& to)
{
	assert(!directoryExists(to));
	assert(fileExists(from));

	CompletePath toFile(to, from.getFilenameWithExtention());

	std::ifstream src(from.toString(), std::ios::binary);
	std::ofstream dst(toFile.toString(), std::ios::binary);

	dst << src.rdbuf();
}

void deleteFile(const CompletePath& completePath)
{
	assert(fileExists(completePath));

	if(fileExists(completePath))
		std::remove(completePath.c_str());
}

}
