#include  <fstream>

#include "ResourceTree.h"
#include "imgui/imgui.h"
#include "Factories.h"


//
//void ResourceTreeWindow::save() const
//{
//	//save this window : 
//	Json::Value root;
//	
//	saveRecursivly(root);
//
//	std::ofstream streamResources;
//	streamResources.open(resourcesPath);
//	if (!streamResources.is_open())
//	{
//		std::cout << "error, can't save tree window at path : " << resourcesPath << std::endl;
//		return;
//	}
//	streamResources << root;
//}
//
//void ResourceTreeWindow::saveRecursivly(Json::Value& root, std::vector<const std::string&>& path) const
//{
//	for (auto& itFolder = m_subFoldersContainer.begin(); itFolder != m_subFoldersContainer.end(); itFolder++)
//	{
//		path.push_back(itFolder->getName());
//		saveRecursivly(root, path);
//		path.pop_back();
//	}
//
//	for (auto& itFile = m_filesContainer.begin(); itFile != m_filesContainer.end(); itFile++)
//	{
//		for (int i = 0; i < path.size(); i++)
//		{
//			root[itFile->getPath()]["path"][i] = toJsonValue<std::string>(path[i]);
//		}
//		root[itFile->getPath()]["name"] = toJsonValue<std::string>(path[i]);
//	}
//}
//
//void ResourceTreeWindow::load()
//{
//	//load this window : 
//	std::string resourcePath = m_path + "/resources.txt";
//
//	std::ifstream streamResources;
//	streamResources.open(resourcePath);
//	if (!streamResources.is_open())
//	{
//		std::cout << "error, can't load ResourceTree window at path : " << resourcePath << std::endl;
//		return;
//	}
//	Json::Value root;
//	streamResources >> root.get(;
//
//	loadRecursivly(root);
//}

ResourceTreeWindow::ResourceTreeWindow()
	: m_folderWeRightClicOn(nullptr)
{
	
	//std::vector<const std::string&> resourcePath;
	////Meshes :
	//for (auto& it = MeshFactory::get().begin(); it != MeshFactory::get().end(); it++)
	//{
	//	const std::string& resourceName = it->first;
	//	ResourceFile resource(resourceName, ResourceType::MESH);
	//	if (containsFileRecursivly(resourceName, &resourcePath))
	//	{

	//	}
	//}

	addSubFolder("toto");
	getSubFolder("toto").addFile(ResourceFile("FileToto01", ResourceType::MESH));
	getSubFolder("toto").addFile(ResourceFile("FileToto02", ResourceType::MESH));
	getSubFolder("toto").addFile(ResourceFile("FileToto03", ResourceType::MESH));

	addSubFolder("tutu");
	getSubFolder("tutu").addFile(ResourceFile("FileTutu01", ResourceType::MESH));
	getSubFolder("tutu").addFile(ResourceFile("FileTutu02", ResourceType::MESH));

	addFile(ResourceFile("File01", ResourceType::MESH));
}

ResourceTreeWindow::~ResourceTreeWindow()
{

}

void ResourceTreeWindow::displayFoldersRecusivly(ResourceFolder* parentFolder, std::vector<ResourceFolder>& foldersToDisplay, std::vector<ResourceFile>& filesToDisplay)
{

	int colorStyleModifierCount = 0;
	if (DragAndDropManager::isDragAndDropping() && (DragAndDropManager::getOperationType() | (EditorDragAndDropType::ResourceDragAndDrop | EditorDragAndDropType::ResourceFolderDragAndDrop)))
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 255, 0, 255));
		colorStyleModifierCount++;
	}

	for (int subFolderIdx = 0; subFolderIdx < foldersToDisplay.size(); subFolderIdx++)
	{
		ResourceFolder& itFolder = foldersToDisplay[subFolderIdx];

		bool nodeOpened = ImGui::TreeNode(itFolder.getName().c_str());
		auto rectMax = ImGui::GetItemRectMax();
		auto rectMin = ImGui::GetItemRectMin();
		if (nodeOpened)
		{
			//current folder drag and drop
			if (ImGui::IsItemHovered() && ImGui::IsMouseDragging(0) && ImGui::IsMouseDown(0))
			{
				DragAndDropManager::beginDragAndDrop(std::make_shared<ResourceFolderDragAndDropOperation>(&itFolder, parentFolder, this));
			}

			displayFoldersRecusivly(&itFolder, itFolder.getSubFolders(), itFolder.getFiles());

			ImGui::TreePop();
		}
		if (ImGui::IsMouseHoveringBox(rectMin, rectMax) && DragAndDropManager::isDragAndDropping())
		{
			ImGui::GetWindowDrawList()->AddRect(ImVec2(rectMin.x - 2, rectMin.y - 2), ImVec2(rectMax.x + 2, rectMax.y + 2), ImColor(255, 255, 0, 255), 5.f);
		}
		if (ImGui::IsMouseHoveringBox(rectMin, rectMax) && ImGui::IsMouseReleased(0))
		{
			DragAndDropManager::dropDraggedItem(&itFolder, EditorDropContext::DropIntoFileOrFolder);
		}
		//right clic menu open ? 
		bool shouldOpenPopup = false;
		if (ImGui::IsMouseHoveringBox(rectMin, rectMax) && ImGui::IsMouseClicked(1))
		{
			ImGui::OpenPopupEx("resourceFolderContextMenu", false);
			m_folderWeRightClicOn = &itFolder;
		}
	}

	//right clic menu display :
	if (ImGui::BeginPopup("resourceFolderContextMenu"))
	{
		if (ImGui::Button("Add folder."))
		{
			ImGui::EndPopup();
			ImGui::OpenPopupEx("AddFolderModale", true);
		}
		else
			ImGui::EndPopup();
	}

	//Modale to add new folder :
	if (ImGui::BeginPopup("AddFolderModale"))
	{
		m_uiString.resize(100);
		ImGui::InputText("##folderName", &m_uiString[0], 100);
		assert(m_folderWeRightClicOn != nullptr);
		if (!m_folderWeRightClicOn->hasSubFolder(m_uiString))
		{
			ImGui::SameLine();
			if (ImGui::Button("Validate##AddFolder") || ImGui::IsKeyPressed(GLFW_KEY_ENTER))
			{
				if(m_folderWeRightClicOn != nullptr)
					m_folderWeRightClicOn->addSubFolder(m_uiString);
				m_folderWeRightClicOn = nullptr;
				ImGui::CloseCurrentPopup();
			}
		}
		else
		{
			ImGui::TextColored(ImVec4(255, 0, 0, 255), "A folder with the same name already exists.");
		}
		ImGui::EndPopup();
	}


	for (int fileIdx = 0; fileIdx < filesToDisplay.size(); fileIdx++)
	{
		ResourceFile& currentFile = filesToDisplay[fileIdx];

		//TODO : coulaur à changer en fonction du type de resource.
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255, 0, 0, 255));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
		if(!currentFile.isBeingRenamed())
			ImGui::Button(currentFile.getName().c_str());
		else
		{
			if (currentFile.drawRenamingInputText())
				currentFile.endRenamingResource();
		}
		ImGui::PopStyleColor(4);

		//rename
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
		{
			currentFile.begingRenamingResource();
		}

		//files drag and drop
		if (!currentFile.isBeingRenamed() && ImGui::IsItemHovered() && ImGui::IsMouseDragging(0) && ImGui::IsMouseDown(0))
		{
			DragAndDropManager::beginDragAndDrop(std::make_shared<ResourceDragAndDropOperation>(&currentFile, parentFolder));
		}
	}

	ImGui::PopStyleColor(colorStyleModifierCount);
}

void ResourceTreeWindow::drawUI()
{
	ImGui::Begin("ResourceTree", nullptr);
	displayFoldersRecusivly(this, m_subFoldersContainer, m_filesContainer);
	ImGui::End();
}
//
//void ResourceTreeWindow::addFolder(const std::string& folderName)
//{
//	m_resourceFolders.push_back(ResourceFolder(folderName));
//}
//
//void ResourceTreeWindow::removeFolder(const std::string& folderName)
//{
//	for (auto& it = m_resourceFolders.begin(); it != m_resourceFolders.end(); it++)
//	{
//		if (it->getName() == folderName)
//		{
//			m_resourceFolders.erase(it);
//			return;
//		}
//	}
//}
//
//ResourceFolder& ResourceTreeWindow::getFolder(const std::string& folderName)
//{
//	for (auto& folder : m_resourceFolders)
//	{
//		if (folder.getName() == folderName)
//			return folder;
//	}
//}
//
//void ResourceTreeWindow::addFileToFolder(ResourceFile file, const std::string& folderName)
//{
//	for (auto& folder : m_resourceFolders)
//	{
//		if (folder.getName() == folderName)
//			folder.addFile(file);
//	}
//}
//
//void ResourceTreeWindow::addFileToFolder(ResourceFile file, size_t folderIdx)
//{
//	assert(folderIdx >= 0 && folderIdx < m_resourceFolders.size());
//
//	m_resourceFolders[folderIdx].addFile(file);
//}

//TODO
//Save and load ...