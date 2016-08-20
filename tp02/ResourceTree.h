#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>

#include "EditorGUI.h"
#include "imgui_extension.h"

class ResourceFile
{
private:
	//TODO
	//ResourceFactory* m_factory;
	ResourceType m_type;
	std::string m_name;
	std::string m_path;
	bool m_isBeingRenamed;
public:

	ResourceFile(const std::string& name, ResourceType type)
		: m_name(name)
		, m_path("")
		, m_type(type)
		, m_isBeingRenamed(false)
	{
		//TODO
	}

	ResourceFile(const ResourceFile& other, const std::string& path)
		: ResourceFile(other)
	{
		m_path = path;
	}

	void begingRenamingResource()
	{
		m_isBeingRenamed = true;
	}

	void endRenamingResource()
	{
		m_isBeingRenamed = false;
	}

	bool isBeingRenamed() const
	{
		return m_isBeingRenamed;
	}

	bool drawRenamingInputText()
	{
		bool enterPressed = false;
		m_name.reserve(100);
		enterPressed = ImGui::InputText("##currentRenamedText", &m_name[0], 100, ImGuiInputTextFlags_EnterReturnsTrue);
		return (enterPressed || ImGui::IsKeyPressed(GLFW_KEY_TAB) || (!ImGui::IsItemHovered() && ImGui::IsMouseClickedAnyButton()));
	}

	const std::string& getName() const { return m_name; }
	const std::string& getPath() const { return m_path; }
	ResourceType getType() const { return m_type; }
};

class ResourceFolder
{
protected:

	typedef std::vector<ResourceFile> ResourceFileContainerType;
	typedef std::vector<ResourceFolder> ResourceSubFolderContainerType;
	ResourceFileContainerType m_filesContainer;
	ResourceSubFolderContainerType m_subFoldersContainer;
	std::string m_name;
	std::string m_path;

public:
	ResourceFolder()
		: m_name("")
		, m_path("")
	{ }

	ResourceFolder(const std::string& name) 
		: m_name(name)
		, m_path("")
	{ }

	ResourceFolder(const ResourceFolder& other, const std::string& path)
		: ResourceFolder(other)
	{ 
		m_path = path;
	}

	const std::string& getName() const { return m_name; }
	const std::string& getPath() const { return m_path; }

	//deals with files 
	ResourceFileContainerType::iterator filesBegin() { return m_filesContainer.begin(); };
	ResourceFileContainerType::iterator filesEnd() { return m_filesContainer.end(); };
	size_t fileCount() { return m_filesContainer.size(); }
	const ResourceFile& getFile(int idx) { return m_filesContainer[idx]; }

	void addFile(const std::string& filePath) { m_filesContainer.push_back(ResourceFile(filePath, ResourceType::NONE)); }
	void addFile(const ResourceFile& file) { m_filesContainer.push_back(file); }
	void removeFile(const std::string& filePath)
	{
		for (auto& itFile = m_filesContainer.begin(); itFile != m_filesContainer.end(); itFile++)
			if (itFile->getName() == filePath)
			{
				m_filesContainer.erase(itFile);
				return;
			}
	}
	bool hasFile(const std::string& filePath) const
	{
		for (auto& itFile = m_filesContainer.begin(); itFile != m_filesContainer.end(); itFile++)
			if (itFile->getName() == filePath)
				return true;
		return false;
	}
	bool getFile(const std::string& filePath, ResourceFile& outFile) const
	{
		for (auto& itFile = m_filesContainer.begin(); itFile != m_filesContainer.end(); itFile++)
			if (itFile->getName() == filePath)
			{
				outFile = *itFile;
				return true;
			}
		return false;
	}

	//deals with sub folders :
	ResourceSubFolderContainerType::iterator subFoldersBegin() { return m_subFoldersContainer.begin(); };
	ResourceSubFolderContainerType::iterator subFoldersEnd() { return m_subFoldersContainer.end(); };
	size_t subFolderCount() { return m_subFoldersContainer.size(); }
	ResourceFolder& getSubFolder(int idx) { return m_subFoldersContainer[idx]; }
	ResourceFolder& getSubFolder(const std::string& folderName)
	{ 
		for (auto& itFolder = m_subFoldersContainer.begin(); itFolder != m_subFoldersContainer.end(); itFolder++)
			if (itFolder->getName() == folderName)
				return *itFolder;
	}

	bool addSubFolder(const std::string& folderName)
	{
		if (hasSubFolder(folderName))
			return false;

		m_subFoldersContainer.push_back(ResourceFolder(folderName));
		return true;
	}
	void addSubFolder(const ResourceFolder& folder)
	{
		m_subFoldersContainer.push_back(folder);
	}
	void removeSubFolder(const std::string& folderName)
	{
		for (auto& itFolder = m_subFoldersContainer.begin(); itFolder != m_subFoldersContainer.end(); itFolder++)
			if (itFolder->getName() == folderName)
			{
				m_subFoldersContainer.erase(itFolder);
				return;
			}
	}
	bool hasSubFolder(const std::string& folderName)
	{
		for (auto& itFolder = m_subFoldersContainer.begin(); itFolder != m_subFoldersContainer.end(); itFolder++)
			if (itFolder->getName() == folderName)
				return true;
		return false;
	}
	std::vector<ResourceFolder>& getSubFolders()
	{
		return m_subFoldersContainer;
	}
	std::vector<ResourceFile>& getFiles()
	{
		return m_filesContainer;
	}

	bool ResourceFolder::searchFileRecursivly(const std::string& fileName, ResourceFile& outFile, std::vector<std::string>* outPathToFile)
	{
		if (getFile(fileName, outFile))
			return true;
		else
		{
			for (auto& itFolder = m_subFoldersContainer.begin(); itFolder != m_subFoldersContainer.end(); itFolder++)
			{
				if (outPathToFile != nullptr)
					outPathToFile->push_back(getName());
				//searchFileRecursivly(fileName, outFile, outPathToFile);
				if (outPathToFile != nullptr)
					outPathToFile->pop_back();
			}
		}
	}

	bool containsFileRecursivly(const std::string& fileName, std::vector<std::string>* outPathToFile)
	{
		if (hasFile(fileName))
			return true;
		else
		{
			for (auto& itFolder = m_subFoldersContainer.begin(); itFolder != m_subFoldersContainer.end(); itFolder++)
			{
				if (outPathToFile != nullptr)
					outPathToFile->push_back(getName());
				containsFileRecursivly(fileName, outPathToFile);
				if (outPathToFile != nullptr)
					outPathToFile->pop_back();
			}
		}
	}

	int getFileCountRecursivly()
	{
		int fileCount = 0;
		for (auto& itFolder = m_subFoldersContainer.begin(); itFolder != m_subFoldersContainer.end(); itFolder++)
		{
			fileCount += m_filesContainer.size() + getFileCountRecursivly();
		}
		return fileCount;
	}
};

class ResourceTreeWindow : public EditorWindow, public ResourceFolder
{
//private:
//	std::vector<ResourceFolder> m_resourceFolders;
//	std::vector<ResourceFile> m_resourceFiles;
private:
	std::string m_uiString;
	ResourceFolder* m_folderWeRightClicOn;

public:
	ResourceTreeWindow();
	~ResourceTreeWindow();
	virtual void drawUI() override;
	/*void addFolder(const std::string& folderName);
	void removeFolder(const std::string& folderName);
	ResourceFolder& getFolder(const std::string& folderName);
	void addFileToFolder(ResourceFile file, const std::string& folderName);
	void addFileToFolder(ResourceFile file, size_t folderIdx);*/
	void displayFoldersRecusivly(ResourceFolder* parentFolder, std::vector<ResourceFolder>& foldersToDisplay, std::vector<ResourceFile>& filesToDisplay);

	//save / load functions :
	//void save() const;
	//void load();
	//void saveRecursivly(Json::Value& root) const;
	//void loadRecursivly(Json::Value& root);
};