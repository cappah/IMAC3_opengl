#include "stdafx.h"

#include  <fstream>

#include "ResourceTree.h"
#include "imgui/imgui.h"
#include "Factories.h"
#include "project.h"


/////////// RESSOURCE FOLDER /////////////////

void ResourceFolder::fillDatasFromExplorerFolder(const FileHandler::Path& folderPath)
{
	std::vector<std::string> dirNames;
	FileHandler::getAllDirNames(folderPath, dirNames);

	for (auto& dirName : dirNames)
	{
		int subFolderIdx = 0;
		if (addSubFolder(dirName, &subFolderIdx))
		{
			m_subFoldersContainer[subFolderIdx].fillDatasFromExplorerFolder(FileHandler::Path(folderPath, dirName));
		}
	}

	std::vector<std::string> fileNames;
	FileHandler::getAllFileNames(folderPath, fileNames);

	for (auto& fileNameAndExtention : fileNames)
	{
		addFile( fileNameAndExtention );
	}
}

bool ResourceFolder::moveSubFolderToNewLocation(const std::string& subFolderName, ResourceFolder& newLocation)
{
	ResourceFolder* subFolder = getSubFolder(subFolderName);

	if (subFolder != nullptr)
	{
		subFolder->moveTo(newLocation);
		removeSubFolder(subFolderName);
	}

	return true;
}

bool ResourceFolder::copySubFolderToNewLocation(const std::string& subFolderName, ResourceFolder& newLocation)
{
	ResourceFolder* subFolder = getSubFolder(subFolderName);

	if (subFolder != nullptr)
	{
		subFolder->copyTo(newLocation);
	}

	return true;
}

bool ResourceFolder::moveTo(ResourceFolder& newLocation)
{
	copyTo(newLocation); //it's like a copy / paste, ...

	// ... but we delete all files and folder from the old folder (we delete it because it's a "move to")
	removeAllFiles();
	removeAllSubFolders(false);

	return true;
}

bool ResourceFolder::copyTo(ResourceFolder& newLocation)
{
	int newLocationIdx = 0;
	//add a new empty sub folder
	newLocation.addSubFolder(m_name, &newLocationIdx);
	ResourceFolder* newFolder = newLocation.getSubFolder(newLocationIdx);

	//transfers folder's files in the new empty folder
	for (auto& file : getFiles())
	{
		newFolder->addFile(file);
	}

	//transfers sub folders (use same finction recursivly)
	for (auto& subFolder : getSubFolders())
	{
		subFolder.moveTo(*newLocation.getSubFolder(newLocationIdx));
	}

	return true;
}

/////////// RESSOURCE TREE /////////////////

ResourceTree::ResourceTree(const FileHandler::Path& assetResourcePath)
	: ResourceFolder("assets", FileHandler::Path("assets"), nullptr)
{
	fillDatasFromExplorerFolder(assetResourcePath);
}

void ResourceTree::moveResourceTo(const ResourceFile& resourceFileToMove, ResourceFolder& folderFrom, ResourceFolder& folderTo)
{
	const FileHandler::CompletePath newResourcePath(folderTo.getPath(), resourceFileToMove.getPath().getFilenameWithExtention());
	
	//change paths to make them begin from root application folder instead of project folder
	const FileHandler::CompletePath from_(Project::getPath().toString() + "/" + resourceFileToMove.getPath().toString());
	const FileHandler::Path to_(Project::getPath().toString() + "/" + newResourcePath.getPath().toString());

	folderTo.addFile(ResourceFile(newResourcePath));
	folderFrom.removeFile(resourceFileToMove.getName());

	FileHandler::copyPastFile(from_, to_); //NOT_SAFE
	FileHandler::deleteFile(from_);
}

void ResourceTree::copyResourceTo(const ResourceFile& resourceFileToMove, ResourceFolder& folderFrom, ResourceFolder& folderTo)
{
	const FileHandler::CompletePath newResourcePath(folderTo.getPath(), resourceFileToMove.getPath().getFilenameWithExtention());

	//change paths to make them begin from root application folder instead of project folder
	const FileHandler::CompletePath from_(Project::getPath().toString() + "/" + resourceFileToMove.getPath().toString());
	const FileHandler::Path to_(Project::getPath().toString() + "/" + newResourcePath.getPath().toString());

	folderTo.addFile(ResourceFile(newResourcePath));
	folderFrom.removeFile(resourceFileToMove.getName());

	FileHandler::copyPastFile(from_, to_); //NOT_SAFE
}

void ResourceTree::addNewMaterialTo(const std::string& materialName, const std::string& ShaderProgramName, ResourceFolder& folderTo)
{
	assert(!getProgramFactory().contains(ShaderProgramName)); //we can't add a second resource with the same name

																  //We create and save the new resource
	const FileHandler::CompletePath resourceCompletePath(folderTo.getPath(), materialName, ".mat");

	//TODO 01
	//create new instance
	//Material* newMaterial = getMaterialFactory().createNewResource(getProgramFactory().get(materialName));
	//newMaterial->save(resourceCompletePath);

	////we store the resource in its factory
	//getMaterialFactory().add(resourceCompletePath, newMaterial);

	//TODO : Add resources
}

void ResourceTree::addSubFolderTo(const std::string& folderName, ResourceFolder& folderTo)
{
	assert(folderTo.getSubFolder(folderName) == nullptr);

	folderTo.addSubFolder(folderName);

	const FileHandler::Path folderParentPath(Project::getPath().toString() + "/" + folderTo.getPath().toString() + "/" + folderName);
	FileHandler::addDirectory(folderName, folderParentPath);
}

/////////// RESOURCE TREE VIEW /////////////

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

ResourceTreeView::ResourceTreeView(ResourceTree* model)
	: m_model(model)
	, m_folderWeRightClicOn(nullptr)
{
	
	//std::vector<const std::string&> resourcePath;
	////Meshes :
	//for (auto& it = getMeshFactory().begin(); it != getMeshFactory().end(); it++)
	//{
	//	const std::string& resourceName = it->first;
	//	ResourceFile resource(resourceName, ResourceType::MESH);
	//	if (containsFileRecursivly(resourceName, &resourcePath))
	//	{

	//	}
	//}

	assert(model != nullptr);

	/*m_model->addSubFolder("toto");
	m_model->getSubFolder("toto").addFile(ResourceFile("FileToto01", ResourceType::MESH));
	m_model->getSubFolder("toto").addFile(ResourceFile("FileToto02", ResourceType::MESH));
	m_model->getSubFolder("toto").addFile(ResourceFile("FileToto03", ResourceType::MESH));

	m_model->addSubFolder("tutu");
	m_model->getSubFolder("tutu").addFile(ResourceFile("FileTutu01", ResourceType::MESH));
	m_model->getSubFolder("tutu").addFile(ResourceFile("FileTutu02", ResourceType::MESH));

	m_model->addFile(ResourceFile("File01", ResourceType::MESH));*/
}

ResourceTreeView::~ResourceTreeView()
{

}

void ResourceTreeView::displayFiles(ResourceFolder* parentFolder, ResourceFolder& currentFolder)
{

	for (int fileIdx = 0; fileIdx < currentFolder.fileCount(); fileIdx++)
	{
		ResourceFile& currentFile = currentFolder.getFile(fileIdx);

		//TODO : couleur à changer en fonction du type de resource.
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255, 0, 0, 255));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
		if (!currentFile.isBeingRenamed())
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
			DragAndDropManager::beginDragAndDrop(std::make_shared<ResourceDragAndDropOperation>(&currentFile, &currentFolder));
		}
	}
}



void ResourceTreeView::displayFoldersRecusivly(ResourceFolder* parentFolder, ResourceFolder& currentFolder, OpenModaleCallback* outOpenModaleCallback, DropCallback* outDropCallback)
{

	int colorStyleModifierCount = 0;
	if (DragAndDropManager::isDragAndDropping() && (DragAndDropManager::getOperationType() | (EditorDragAndDropType::ResourceDragAndDrop | EditorDragAndDropType::ResourceFolderDragAndDrop)))
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 255, 0, 255));
		colorStyleModifierCount++;
	}

	bool nodeOpened = ImGui::TreeNode(currentFolder.getName().c_str());
	auto rectMax = ImGui::GetItemRectMax();
	auto rectMin = ImGui::GetItemRectMin();
	if (nodeOpened)
	{

		//current folder drag and drop
		if (ImGui::IsItemHovered() && ImGui::IsMouseDragging(0) && ImGui::IsMouseDown(0))
		{
			DragAndDropManager::beginDragAndDrop(std::make_shared<ResourceFolderDragAndDropOperation>(&currentFolder, parentFolder, m_model));
		}

		//recursivity
		for (int subFolderIdx = 0; subFolderIdx < currentFolder.subFolderCount(); subFolderIdx++)
		{
			ResourceFolder* itFolder = currentFolder.getSubFolder(subFolderIdx);

			displayFoldersRecusivly(&currentFolder, *itFolder, outOpenModaleCallback, outDropCallback);

		}
		displayFiles(parentFolder, currentFolder);

		ImGui::TreePop();
	}
	if (ImGui::IsMouseHoveringBox(rectMin, rectMax) && DragAndDropManager::isDragAndDropping())
	{
		ImGui::GetWindowDrawList()->AddRect(ImVec2(rectMin.x - 2, rectMin.y - 2), ImVec2(rectMax.x + 2, rectMax.y + 2), ImColor(255, 255, 0, 255), 5.f);
	}
	if (ImGui::IsMouseHoveringBox(rectMin, rectMax) && ImGui::IsMouseReleased(0))
	{
		if (outDropCallback != nullptr)
		{
			outDropCallback->currentFolder = &currentFolder;
			outDropCallback->dropContext = EditorDropContext::DropIntoFileOrFolder;
		}
		//DragAndDropManager::dropDraggedItem(&currentFolder, EditorDropContext::DropIntoFileOrFolder);
	}

	//right clic menu open ? 
	bool shouldOpenPopup = false;
	if (ImGui::IsMouseHoveringBox(rectMin, rectMax) && ImGui::IsMouseClicked(1))
	{
		if (outOpenModaleCallback != nullptr)
		{
			outOpenModaleCallback->shouldOpen = true;
			outOpenModaleCallback->modaleName = "resourceFolderContextMenu";
		}
		//ImGui::OpenPopupEx("resourceFolderContextMenu", true);
		m_folderWeRightClicOn = &currentFolder;
	}

	ImGui::PopStyleColor(colorStyleModifierCount);
}

void ResourceTreeView::displayModales()
{
	//right clic menu display :
	if (ImGui::BeginPopup("resourceFolderContextMenu"))
	{
		if (ImGui::Button("Add folder."))
		{
			ImGui::EndPopup();
			ImGui::OpenPopupEx("AddFolderModale", true);
		}
		else if (ImGui::Button("Add resource."))
		{
			ImGui::EndPopup();
			ImGui::OpenPopupEx("AddResourcePopUp", true);
		}
		else
			ImGui::EndPopup();
	}

	//pop up to add resource :
	if (ImGui::BeginPopup("AddResourcePopUp"))
	{
		if (ImGui::Button("Material."))
		{
			ImGui::EndPopup();
			ImGui::OpenPopupEx("ChooseMaterialPopUp", true);
		}
		else if (ImGui::Button("CubeTexture."))
		{
			ImGui::EndPopup();
			ImGui::OpenPopupEx("AddCubeTexturePopUp", true);
		}
		else
			ImGui::EndPopup();
	}

	//PopUp to choose a material :
	if (ImGui::BeginPopup("ChooseMaterialPopUp"))
	{
		popUpToChooseMaterial();
	}

	if (ImGui::BeginPopup("AddMaterialPopUp"))
	{
		popUpToAddMaterial();
	}

	//PopUp to add new cubeTexture :
	if (ImGui::BeginPopup("AddCubeTexturePopUp"))
	{
		popUpToAddCubeTexture();
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
				if (m_folderWeRightClicOn != nullptr)
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
}

void ResourceTreeView::popUpToChooseMaterial()
{
	bool shouldEndPopup = true;
	int tmpProgramIdx = 0;
	for (auto& it = getProgramFactory().resourceBegin(); it != getProgramFactory().resourceEnd(); it++)
	{
		const std::string matName = it->first.getFilename();
		const std::string btnLabel = matName + "##" +std::to_string(tmpProgramIdx++);

		if (ImGui::Button(btnLabel.data()))
		{
			m_chooseMaterialName = matName;
			ImGui::EndPopup();
			ImGui::OpenPopupEx("AddMaterialPopUp", true);
			shouldEndPopup = false;
		}
		//else
		//	ImGui::EndPopup();
	}

	for (auto& it = getProgramFactory().defaultResourceBegin(); it != getProgramFactory().defaultResourceEnd(); it++)
	{
		const std::string matName = it->first;
		const std::string btnLabel = matName + "##" + std::to_string(tmpProgramIdx++);

		if (ImGui::Button(btnLabel.data()))
		{
			m_chooseMaterialName = matName;
			ImGui::EndPopup();
			ImGui::OpenPopupEx("AddMaterialPopUp", true);
			shouldEndPopup = false;
		}
		//else
		//	ImGui::EndPopup();
	}

	if(shouldEndPopup)
		ImGui::EndPopup();
}


void ResourceTreeView::popUpToAddMaterial()
{
	assert(!m_chooseMaterialName.empty());

	m_uiString.resize(100);
	ImGui::InputText("##materialName", &m_uiString[0], 100);
	assert(m_folderWeRightClicOn != nullptr);
	if (!m_folderWeRightClicOn->hasFile(m_uiString + ".mat"))
	{
		ImGui::SameLine();
		if (ImGui::Button("Validate##AddMaterial") || ImGui::IsKeyPressed(GLFW_KEY_ENTER))
		{
			if (m_folderWeRightClicOn != nullptr)
			{
				//We create and save the new resource
				const FileHandler::CompletePath resourceCompletePath(m_folderWeRightClicOn->getPath(), m_uiString, ".mat");
				Material* newMaterial = MaterialFactory::instance().getInstance(m_chooseMaterialName);
				newMaterial->save(resourceCompletePath);
				//we store the resource in its factory
				getMaterialFactory().add(resourceCompletePath, newMaterial);
			}
			m_folderWeRightClicOn = nullptr;
			ImGui::CloseCurrentPopup();
		}
	}
	else
	{
		ImGui::TextColored(ImVec4(255, 0, 0, 255), "A file with the same name already exists.");
	}
	ImGui::EndPopup();
}


void ResourceTreeView::popUpToAddCubeTexture()
{
	m_uiString.resize(100);
	ImGui::InputText("##fileName", &m_uiString[0], 100);
	assert(m_folderWeRightClicOn != nullptr);
	if (!m_folderWeRightClicOn->hasFile(m_uiString + ".ctx"))
	{
		ImGui::SameLine();
		if (ImGui::Button("Validate##AddFile") || ImGui::IsKeyPressed(GLFW_KEY_ENTER))
		{
			if (m_folderWeRightClicOn != nullptr)
			{
				//We create and save the new resource
				const FileHandler::CompletePath resourceCompletePath(m_folderWeRightClicOn->getPath(), m_uiString, ".ctx");
				CubeTexture* newCubeTexture = new CubeTexture();
				newCubeTexture->save(resourceCompletePath);
				//we store the resource in its factory
				getCubeTextureFactory().add(resourceCompletePath, newCubeTexture);
			}
			m_folderWeRightClicOn = nullptr;
			ImGui::CloseCurrentPopup();
		}
	}
	else
	{
		ImGui::TextColored(ImVec4(255, 0, 0, 255), "A file with the same name already exists.");
	}
	ImGui::EndPopup();
}

void ResourceTreeView::drawUI()
{
	ImGui::Begin("ResourceTree", nullptr);
	DropCallback dropCallback(nullptr, EditorDropContext::DropIntoFileOrFolder);
	OpenModaleCallback openModaleCallback;
	displayFoldersRecusivly(nullptr, *m_model, &openModaleCallback, &dropCallback);

	openModaleCallback.openCallbackIfNeeded();
	displayModales();

	//asynchronous drop
	if (dropCallback.currentFolder != nullptr)
	{
		DragAndDropManager::dropDraggedItem(dropCallback.currentFolder, EditorDropContext::DropIntoFileOrFolder);
	}
	//displayFoldersRecusivly(m_model, m_model->getSubFolders(), m_model->getFiles());
	ImGui::End();
}


/*
void ResourceTreeView::displayFoldersRecusivly(ResourceFolder* parentFolder, std::vector<ResourceFolder>& foldersToDisplay, std::vector<ResourceFile>& filesToDisplay)
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
DragAndDropManager::beginDragAndDrop(std::make_shared<ResourceFolderDragAndDropOperation>(&itFolder, parentFolder, m_model));
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
else if (ImGui::Button("Add resource."))
{
ImGui::EndPopup();
ImGui::OpenPopupEx("AddResourcePopUp", true);
}
else
ImGui::EndPopup();
}

//pop up to add resource :
if (ImGui::BeginPopup("AddResourcePopUp"))
{
if (ImGui::Button("Material."))
{
ImGui::EndPopup();
ImGui::OpenPopupEx("ChooseMaterialPopUp", true);
}
else if (ImGui::Button("CubeTexture."))
{
ImGui::EndPopup();
ImGui::OpenPopupEx("AddCubeTexturePopUp", true);
}
else
ImGui::EndPopup();
}

//PopUp to choose a material :
if (ImGui::BeginPopup("ChooseMaterialPopUp"))
{
popUpToChooseMaterial();
}

//PopUp to add new cubeTexture :
if (ImGui::BeginPopup("AddCubeTexturePopUp"))
{
popUpToAddCubeTexture();
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

//TODO : couleur à changer en fonction du type de resource.
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
*/

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