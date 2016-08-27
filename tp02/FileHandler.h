#pragma once

#include <string>
#include <cassert>
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>


/**
* FileHandler system :
* A Path is a path toward a directory, with format : "toto/tutu/"
* A CompletePath is a path toward a file, composed by a Path, a fileName and a file extention
* A CompletePath toward : "toto/tutu/titi.txt" will give us :
* CompletePath::Path = "toto/tutu/",
* CompletePath::FileName = "titi",
* CompletePath::Extention = ".txt",
* The toString() methode will return : "toto/tutu/titi.txt"
*/
namespace FileHandler {

enum FileType
{
	NONE,
	IMAGE,
	MESH,
	SOUND,
};

class CompletePath;

class Path
{
private:
	std::string m_data;

public:
	Path();
	Path(const std::string& path);
	Path(const CompletePath& completePath);

	const std::string& toString() const;
	const char* c_str() const { return toString().c_str(); }
	//operator std::string() const { return toString(); }

	const std::string& operator[](size_t idx);
	size_t size();
	size_t getPathFolderNames(std::vector<std::string>& folderNames) const;

	void push_back(const std::string& folderName);
	void push_back(const Path& subPath);
	void pop_back();
	void format();
};

std::ofstream& operator<<(std::ofstream& os, const Path& path);

///////////////////

class CompletePath
{
private:
	Path m_path;
	std::string m_fileName;
	std::string m_extention;
	bool m_complete;

public:
	CompletePath();
	CompletePath(Path path, const std::string& fileName, const std::string& extention);
	CompletePath(Path path, const std::string& fileNameAndExtention);

	const std::string& toString() const;
	const char* c_str() const { return toString().c_str(); }
	//operator std::string() const { return toString(); }

	const std::string& operator[](size_t idx);

	const Path& getPath() const;
	const Path& getFilename() const;
	const std::string& getFilenameWithExtention() const;
	const std::string& getExtention() const;
	bool hasValidFileName() const;
	bool hasValidExtention() const;
	void format();

	FileType getFileType() const;
};

std::ofstream& operator<<(std::ofstream& os, const CompletePath& path);


///////////////////

size_t getExtentionFromExtendedFilename(const std::string& filename, std::string& outExtention);
size_t getFileNameAndExtentionFromExtendedFilename(const std::string& extendedFileName, std::string& filename, std::string& outExtention);

FileType getFileTypeFromExtention(const std::string& extention);

//fill a vector with all directories's names in the directory at given path.
std::vector<std::string> getAllDirNames(const Path& path);
//fill a vector with all files' and directories' names in the directory at given path.
std::vector<std::string> getAllFileAndDirNames(const Path& path);

bool directoryExists(const Path& path);
bool directoryExists(const Path& path, const std::string& name);
bool fileExists(const CompletePath& completePath);

void addDirectories(const Path& path);
void addDirectory(const std::string& name, const Path& path);

//string helper
std::vector<std::string> splitString(const std::string& s, char delim);
std::vector<std::string> splitString(const std::string& s, char delim01, char delim02);
std::size_t splitPathFileName(const std::string& pathAndFileName, std::string& path, std::string& filename);

void copyFilePastToNewFile(const CompletePath &from, const Path &to);
void deleteFile(const CompletePath& completePath);

}