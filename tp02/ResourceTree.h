#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>

#include "EditorGUI.h"
#include "imgui_extension.h"
#include "FileHandler.h"
#include "Factories.h"

class ResourceFile
{
private:
	//TODO
	//ResourceFactory* m_factory;
	ResourceType m_type;
	std::string m_name;
	FileHandler::CompletePath m_path;
	bool m_isBeingRenamed;
public:

	ResourceFile(const FileHandler::CompletePath& completePath)
		: m_name(completePath.getFilename())
		, m_path(completePath)
		, m_type(ResourceType::NONE)
		, m_isBeingRenamed(false)
	{
		m_type = getResourceTypeFromFileType(completePath.getFileType());
		//TODO
	}

	//ResourceFile(const ResourceFile& other, const std::string& path)
	//	: ResourceFile(other)
	//{
	//	m_path = path;
	//}

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
	const FileHandler::CompletePath& getPath() const { return m_path; }
	ResourceType getType() const { return m_type; }
};

class ResourceFolder
{
protected:

	ResourceFolder* m_parentFolder;
	std::vector<ResourceFile> m_filesContainer;
	std::vector<ResourceFolder> m_subFoldersContainer;
	std::string m_name;
	FileHandler::Path m_path;

public:
	ResourceFolder()
		: m_name("")
		, m_path("")
		, m_parentFolder(nullptr)
	{ }

	ResourceFolder(const std::string& name, const FileHandler::Path& path, ResourceFolder* parentFolder) 
		: m_name(name)
		, m_path(path)
		, m_parentFolder(parentFolder)
	{ }

	virtual ~ResourceFolder()
	{ }

	//ResourceFolder(const ResourceFolder& other, const std::string& path)
	//	: ResourceFolder(other)
	//{ 
	//	m_path = path;
	//}

	const std::string& getName() const { return m_name; }
	const FileHandler::Path& getPath() const { return m_path; }
	ResourceFolder* getParentFolder() const { return m_parentFolder; }

	//deals with files 
	std::vector<ResourceFile>::iterator filesBegin() { return m_filesContainer.begin(); };
	std::vector<ResourceFile>::iterator filesEnd() { return m_filesContainer.end(); };
	size_t fileCount() { return m_filesContainer.size(); }
	ResourceFile& getFile(int idx) { return m_filesContainer[idx]; }

	void addFile(const std::string& fileNameAndExtention) 
	{
		FileHandler::CompletePath resourcePath(m_path, fileNameAndExtention);
		m_filesContainer.push_back(ResourceFile(resourcePath));

		addResourceToFactory(resourcePath);
	}

	void addFile(const ResourceFile& file) 
	{ 
		m_filesContainer.push_back(file);
	}

	void removeFile(const std::string& filePath)
	{
		for (auto& itFile = m_filesContainer.begin(); itFile != m_filesContainer.end(); itFile++)
			if (itFile->getName() == filePath)
			{
				m_filesContainer.erase(itFile);
				return;
			}
	}

	void removeAllFiles()
	{
		m_filesContainer.clear();
	}

	bool hasFile(const std::string& fileName) const
	{
		for (auto& itFile = m_filesContainer.begin(); itFile != m_filesContainer.end(); itFile++)
			if (itFile->getName() == fileName)
				return true;
		return false;
	}

	bool getFile(const std::string& fileName, ResourceFile& outFile) const
	{
		for (auto& itFile = m_filesContainer.begin(); itFile != m_filesContainer.end(); itFile++)
			if (itFile->getName() == fileName)
			{
				outFile = *itFile;
				return true;
			}
		return false;
	}

	//deals with sub folders :
	std::vector<ResourceFolder>::iterator subFoldersBegin() { return m_subFoldersContainer.begin(); };
	std::vector<ResourceFolder>::iterator subFoldersEnd() { return m_subFoldersContainer.end(); };
	size_t subFolderCount() { return m_subFoldersContainer.size(); }
	ResourceFolder* getSubFolder(int idx) 
	{ 
		if (idx < 0 || idx >= m_subFoldersContainer.size())
			return nullptr;

		return &m_subFoldersContainer[idx];
	}
	ResourceFolder* getSubFolder(const std::string& folderName)
	{ 
		for (auto& itFolder = m_subFoldersContainer.begin(); itFolder != m_subFoldersContainer.end(); itFolder++)
			if (itFolder->getName() == folderName)
				return &(*itFolder);

		return nullptr;
	}

	bool addSubFolder(const std::string& folderName, int* outFolderIdx = nullptr)
	{
		if (hasSubFolder(folderName, outFolderIdx))
			return false;

		m_subFoldersContainer.push_back(ResourceFolder(folderName, FileHandler::Path(m_path, folderName), this));
		if (outFolderIdx != nullptr)
		{
			*outFolderIdx = (m_subFoldersContainer.size() - 1);
		}
		return true;
	}

	bool addSubFolder(ResourceFolder& folder, int* outFolderIdx = nullptr)
	{
		if (hasSubFolder(folder, outFolderIdx))
			return false;

		folder.m_parentFolder = this;
		m_subFoldersContainer.push_back(folder);
		if (outFolderIdx != nullptr)
		{
			*outFolderIdx = (m_subFoldersContainer.size() - 1);
		}
		return true;
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

	void removeAllSubFolders(bool recursive = false)
	{
		if (recursive)
		{
			for (auto& itFolder = m_subFoldersContainer.begin(); itFolder != m_subFoldersContainer.end(); itFolder++)
			{
				itFolder->removeAllSubFolders(recursive);
			}
		}
		m_subFoldersContainer.clear();
	}

	bool hasSubFolder(const std::string& folderName, int* outSubFolderIdx = nullptr)
	{
		int idx = 0;
		for (auto& itFolder = m_subFoldersContainer.begin(); itFolder != m_subFoldersContainer.end(); itFolder++)
		{
			if (itFolder->getName() == folderName)
			{
				if (outSubFolderIdx != nullptr)
				{
					*outSubFolderIdx = idx;
				}
				return true;
			}
			idx++;
		}
		return false;
	}

	bool hasSubFolder(const ResourceFolder& folder, int* outSubFolderIdx = nullptr)
	{
		int idx = 0;
		for (auto& itFolder = m_subFoldersContainer.begin(); itFolder != m_subFoldersContainer.end(); itFolder++)
		{
			if (itFolder->getName() == folder.getName())
			{
				if (outSubFolderIdx != nullptr)
				{
					*outSubFolderIdx = idx;
				}
				return true;
			}
			idx++;
		}
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

	//Recursivly get datas and construct hierachy from explorer folder hierarchy
	void fillDatasFromExplorerFolder(const FileHandler::Path& folderPath);

	//return true if the path exists in the current hierachy
	bool pathExists(const FileHandler::Path& path, ResourceFolder* outFolderAtPath = nullptr) const
	{
		if (path.size() >= 1)
		{
			const std::string& firstFolderName = path[0];
		
			for (auto& subFolder : m_subFoldersContainer)
			{
				if (subFolder.getName() == firstFolderName)
				{
					if (outFolderAtPath != nullptr)
						*outFolderAtPath = subFolder;

					FileHandler::Path subPath(path);
					subPath.pop_front();
					return subFolder.pathExists(subPath, outFolderAtPath);
				}
			}

			return false;
		}
		return true;
	}

	//move a subFolder to a new location
	bool moveSubFolderToNewLocation(const std::string& subFolderName, ResourceFolder& newLocation);
	bool copySubFolderToNewLocation(const std::string& subFolderName, ResourceFolder& newLocation);

private:
	//move the folder to a new location, used by moveSubFolderToNewLocation.
	bool moveTo(ResourceFolder& newLocation);
	bool copyTo(ResourceFolder& newLocation);
};

//utility callback to handle asynchronous file or folder removal
struct DropCallback
{
	ResourceFolder* currentFolder;
	EditorDropContext dropContext;

	DropCallback(ResourceFolder* _currentFolder, EditorDropContext _dropContext) : currentFolder(_currentFolder), dropContext(_dropContext)
	{}
};

//utility callback to open modale asynchronously
struct OpenModaleCallback
{
	std::string modaleName;
	bool shouldOpen;

	OpenModaleCallback() : shouldOpen(false)
	{}

	void openCallbackIfNeeded()
	{
		if (shouldOpen)
		{
			ImGui::OpenPopupEx("resourceFolderContextMenu", true);
		}
	}
};


class ResourceTree : public ResourceFolder
{
public :
	ResourceTree(const FileHandler::Path& assetResourcePath);
	virtual ~ResourceTree()
	{}

	static void moveResourceTo(const ResourceFile& resourceFileToMove, ResourceFolder& folderFrom, ResourceFolder& folderTo);
	static void copyResourceTo(const ResourceFile& resourceFileToMove, ResourceFolder& folderFrom, ResourceFolder& folderTo);

	static void addNewMaterialTo(const std::string& materialName, const std::string& ShaderProgramName, ResourceFolder& folderTo);
	static void addSubFolderTo(const std::string& folderName, ResourceFolder& folderTo);
};


class ResourceTreeView : public EditorWindow
{
//private:
//	std::vector<ResourceFolder> m_resourceFolders;
//	std::vector<ResourceFile> m_resourceFiles;
private:
	ResourceTree* m_model;

	std::string m_uiString;
	ResourceFolder* m_folderWeRightClicOn;
	std::string m_chooseMaterialName;

public:
	ResourceTreeView(ResourceTree* model);
	~ResourceTreeView();
	virtual void drawUI() override;
	/*void addFolder(const std::string& folderName);
	void removeFolder(const std::string& folderName);
	ResourceFolder& getFolder(const std::string& folderName);
	void addFileToFolder(ResourceFile file, const std::string& folderName);
	void addFileToFolder(ResourceFile file, size_t folderIdx);*/
	
	void displayFiles(ResourceFolder* parentFolder, ResourceFolder& currentFolder);
	void displayFoldersRecusivly(ResourceFolder* parentFolder, ResourceFolder& currentFolder, OpenModaleCallback* outOpenModaleCallback, DropCallback* outDropCallback = nullptr);
	void displayModales();
	//void displayFoldersRecusivly(ResourceFolder* parentFolder, std::vector<ResourceFolder>& foldersToDisplay, std::vector<ResourceFile>& filesToDisplay);
	void popUpToAddCubeTexture();
	void popUpToChooseMaterial();
	void popUpToAddMaterial();

	//save / load functions :
	//void save() const;
	//void load();
	//void saveRecursivly(Json::Value& root) const;
	//void loadRecursivly(Json::Value& root);
};