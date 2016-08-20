#include "EditorGUI.h"
#include "ResourceTree.h"

ResourceDragAndDropOperation::ResourceDragAndDropOperation(const ResourceFile* resource, ResourceFolder* resourceFolder)
	: DragAndDropOperation(EditorDragAndDropType::ResourceDragAndDrop, (EditorDropContext::DropIntoFileOrFolder | EditorDropContext::DropIntoResourceField))
	, m_folderResourceBelongsTo(resourceFolder)
	, m_resourceDragged(resource)
{ }

void ResourceDragAndDropOperation::dragOperation()
{ }

void ResourceDragAndDropOperation::dropOperation(void* customData, int dropContext)
{
	if (dropContext == EditorDropContext::DropIntoFileOrFolder)
	{
		ResourceFolder* resourceFolder = static_cast<ResourceFolder*>(customData);

		if (!resourceFolder->hasFile(m_resourceDragged->getName()))
		{
			const std::string newResourcePath(resourceFolder->getPath() + resourceFolder->getName() + "/");
			//On fait expres de copier ca c'est une nouvelle resource (copier collé)
			resourceFolder->addFile(ResourceFile(*m_resourceDragged, newResourcePath));
			m_folderResourceBelongsTo->removeFile(m_resourceDragged->getName());

			m_folderResourceBelongsTo = nullptr;
			m_resourceDragged = nullptr;
		}
		else
			cancelOperation();
	}
	else if (dropContext == EditorDropContext::DropIntoResourceField)
	{
		std::string* resourceName = static_cast<std::string*>(customData);
		*resourceName = m_resourceDragged->getName();

		m_folderResourceBelongsTo = nullptr;
		m_resourceDragged = nullptr;
	}
	else
		cancelOperation();
}

void ResourceDragAndDropOperation::cancelOperation()
{
	m_folderResourceBelongsTo = nullptr;
	m_resourceDragged = nullptr;
}

void ResourceDragAndDropOperation::updateOperation()
{
	ImVec2 mousePos = ImGui::GetMousePos();
	ImGui::SetNextWindowPos(mousePos);
	ImGui::Begin("DragAndDropWidget", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoInputs);
	ImGui::Text(m_resourceDragged->getName().c_str());
	ImGui::End();
}

bool ResourceDragAndDropOperation::canDropInto(void* data)
{
	return (m_resourceDragged->getType() & *static_cast<ResourceType*>(data)) != 0;
}

//////////////////////

ResourceFolderDragAndDropOperation::ResourceFolderDragAndDropOperation(const ResourceFolder* folder, ResourceFolder* parentFolder, ResourceTreeWindow* resourceTree)
	: DragAndDropOperation(EditorDragAndDropType::ResourceFolderDragAndDrop, EditorDropContext::DropIntoFileOrFolder)
	, m_resourceTree(resourceTree)
	, m_parentFolder(parentFolder)
	, m_folderDragged(folder)
{

}

void ResourceFolderDragAndDropOperation::dragOperation()
{

}

void ResourceFolderDragAndDropOperation::dropOperation(void* customData, int dropContext)
{
	ResourceFolder* resourceFolder = static_cast<ResourceFolder*>(customData);

	if (!resourceFolder->hasSubFolder(m_folderDragged->getName()) && resourceFolder->getName() != m_folderDragged->getName())
	{
		const std::string newFolderPath(resourceFolder->getPath() + resourceFolder->getName() + "/");
		resourceFolder->addSubFolder(ResourceFolder(*m_folderDragged, newFolderPath));
		if (m_parentFolder != nullptr)
			m_parentFolder->removeSubFolder(m_folderDragged->getName());
		else
			m_resourceTree->removeSubFolder(m_folderDragged->getName());
		m_parentFolder = nullptr;
		m_folderDragged = nullptr;
		m_resourceTree = nullptr;
	}
	else
		cancelOperation();
}

void ResourceFolderDragAndDropOperation::cancelOperation()
{
	m_parentFolder = nullptr;
	m_folderDragged = nullptr;
	m_resourceTree = nullptr;
}

void ResourceFolderDragAndDropOperation::updateOperation()
{
	ImVec2 mousePos = ImGui::GetMousePos();
	ImGui::SetNextWindowPos(mousePos);
	ImGui::Begin("DragAndDropWidget", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoInputs);
	ImGui::Text(m_folderDragged->getName().c_str());
	ImGui::End();
}

namespace EditorGUI {
bool ResourceField(ResourceType resourceType, const std::string& label, char* buf, int bufSize)
{
	bool canDropIntoField = DragAndDropManager::canDropInto(&resourceType);
	bool isTextEdited = false;

	int colStyleCount = 0;
	if (canDropIntoField)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 255, 0, 255));
		colStyleCount++;
	}

	bool enterPressed = ImGui::InputText(label.c_str(), buf, bufSize, ImGuiInputTextFlags_EnterReturnsTrue);
	isTextEdited = (enterPressed || ImGui::IsKeyPressed(GLFW_KEY_TAB) || (!ImGui::IsItemHovered() && ImGui::IsMouseClickedAnyButton()));

	//borders if can drop here : 
	if (ImGui::IsItemHovered() && canDropIntoField)
	{
		ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), 2.f);
	}

	//drop resource : 
	if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(0))
	{
		std::string droppedResourceName;
		DragAndDropManager::dropDraggedItem(&droppedResourceName, (EditorDropContext::DropIntoResourceField));
		assert(droppedResourceName.size() < bufSize);
		strcpy(buf, droppedResourceName.c_str());
	}

	ImGui::PopStyleColor(colStyleCount);

	return isTextEdited;
}
}