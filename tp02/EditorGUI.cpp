#include "stdafx.h"

#include "Editor.h"
#include "EditorGUI.h"
#include "ResourceTree.h"

///////////////////////////////////////////////////////////
//// BEGIN : DroppedFileDragAndDropOperation

DroppedFileDragAndDropOperation::DroppedFileDragAndDropOperation(const FileHandler::CompletePath& resourcePath)
	: DragAndDropOperation(EditorDragAndDropType::DroppedFileDragAndDrop, (EditorDropContext::DropIntoFileOrFolder))
	, m_resourceFile(nullptr)
{ 
	m_resourceFile = new ResourceFile(resourcePath);
}

void DroppedFileDragAndDropOperation::dragOperation()
{ }

void DroppedFileDragAndDropOperation::dropOperation(void* customData, int dropContext)
{
	if (!canDropInto(customData, dropContext))
	{
		cancelOperation();
		return;
	}

	if (dropContext == EditorDropContext::DropIntoFileOrFolder)
	{
		ResourceFolder* resourceFolder = static_cast<ResourceFolder*>(customData);

		if (!resourceFolder->hasFile(m_resourceFile->getKey()))
		{
			ResourceTree::addExternalResourceTo(*m_resourceFile, *resourceFolder);
			Editor::instance().removeDroppedFile(m_resourceFile->getPath());

			delete m_resourceFile;
			m_resourceFile = nullptr;
		}
		else
			cancelOperation();
	}
	else
		cancelOperation();
}

void DroppedFileDragAndDropOperation::cancelOperation()
{
	delete m_resourceFile;
	m_resourceFile = nullptr;
}

void DroppedFileDragAndDropOperation::updateOperation()
{
	ImVec2 mousePos = ImGui::GetMousePos();
	ImGui::SetNextWindowPos(mousePos);
	ImGui::Begin("DragAndDropWidget", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoInputs);
		ImGui::Text(m_resourceFile->getName().c_str());
	ImGui::End();
}

bool DroppedFileDragAndDropOperation::canDropInto(void* customData, int dropContext)
{
	if (dropContext == EditorDropContext::DropIntoFileOrFolder)
	{
		ResourceFolder* resourceFolder = static_cast<ResourceFolder*>(customData);
		return !resourceFolder->hasFile(m_resourceFile->getKey());
	}
	else
		return false;
}

//// END : DroppedFileDragAndDropOperation
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//// BEGIN : ResourceDragAndDropOperation

ResourceDragAndDropOperation::ResourceDragAndDropOperation(const ResourceFile* resource, ResourceFolder* resourceFolder)
	: DragAndDropOperation(EditorDragAndDropType::ResourceDragAndDrop, (EditorDropContext::DropIntoFileOrFolder | EditorDropContext::DropIntoResourceField))
	, m_folderResourceBelongsTo(resourceFolder)
	, m_resourceDragged(resource)
{ }

void ResourceDragAndDropOperation::dragOperation()
{ }

void ResourceDragAndDropOperation::dropOperation(void* customData, int dropContext)
{
	if (!canDropInto(customData, dropContext))
	{
		cancelOperation();
		return;
	}

	if (dropContext == EditorDropContext::DropIntoFileOrFolder)
	{
		ResourceFolder* resourceFolder = static_cast<ResourceFolder*>(customData);

		if (!resourceFolder->hasFile(m_resourceDragged->getKey()))
		{
			//const FileHandler::CompletePath newResourcePath(resourceFolder->getPath(), m_resourceDragged->getPath().getFilenameWithExtention());
			//On fait expres de copier ca c'est une nouvelle resource (copier collé)
			ResourceTree::moveResourceTo(*m_resourceDragged, *m_folderResourceBelongsTo, *resourceFolder);
			//resourceFolder->addFile(ResourceFile(newResourcePath));
			//m_folderResourceBelongsTo->removeFile(m_resourceDragged->getName());

			//FileHandler::copyPastFile(m_resourceDragged->getPath(), newResourcePath.getPath()); //NOT_SAFE

			m_folderResourceBelongsTo = nullptr;
			m_resourceDragged = nullptr;
		}
		else
			cancelOperation();
	}
	else if (dropContext == EditorDropContext::DropIntoResourceField)
	{
		FileHandler::CompletePath* resourcePath = static_cast<FileHandler::CompletePath*>(customData);
		*resourcePath = m_resourceDragged->getPath();

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

bool ResourceDragAndDropOperation::canDropInto(void* customData, int dropContext)
{
	if (dropContext == EditorDropContext::DropIntoFileOrFolder)
	{
		ResourceFolder* resourceFolder = static_cast<ResourceFolder*>(customData);
		return !resourceFolder->hasFile(m_resourceDragged->getKey());
	}
	else if(dropContext == EditorDropContext::DropIntoResourceField)
		return (m_resourceDragged->getType() & *static_cast<ResourceType*>(customData)) != 0;
}

//// END : ResourceDragAndDropOperation
///////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////
//// BEGIN : ResourceFolderDragAndDropOperation

ResourceFolderDragAndDropOperation::ResourceFolderDragAndDropOperation(const ResourceFolder* folder, ResourceFolder* parentFolder, ResourceTree* resourceTree)
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
	if (!canDropInto(customData, dropContext))
	{
		cancelOperation();
		return;
	}

	ResourceFolder* resourceFolder = static_cast<ResourceFolder*>(customData);

	if (dropContext == EditorDropContext::DropIntoFileOrFolder
		&& !resourceFolder->hasSubFolder(m_folderDragged->getName()) && resourceFolder->getName() != m_folderDragged->getName())
	{
		if (m_parentFolder != nullptr)
			ResourceTree::moveSubFolderTo(m_folderDragged->getName(), *m_parentFolder , *resourceFolder);

		//if (m_parentFolder != nullptr)
		//	m_parentFolder->moveSubFolderToNewLocation(m_folderDragged->getName(), *resourceFolder);
		//else
		//	m_resourceTree->moveSubFolderToNewLocation(m_folderDragged->getName(), *resourceFolder);
		//%NOCOMMIT% : TODO : transvaser les fichiers et sous dossier dans le dossier cible

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

bool ResourceFolderDragAndDropOperation::canDropInto(void* customData, int dropContext)
{
	if (dropContext == EditorDropContext::DropIntoFileOrFolder)
	{
		ResourceFolder* resourceFolder = static_cast<ResourceFolder*>(customData);
		return !resourceFolder->hasSubFolder(m_folderDragged->getName()) && (resourceFolder->getName() != m_folderDragged->getName());
	}
}


//// END : ResourceFolderDragAndDropOperation
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//// BEGIN : Specialization for ValueFields

namespace EditorGUI {

//string field
bool ValueField(const std::string& label, std::string& outString, char* buf, int bufSize)
{
	bool textEdited = ImGui::InputText(label.c_str(), buf, bufSize, ImGuiInputTextFlags_EnterReturnsTrue);
	if (textEdited)
	{
		outString.clear();
		outString = buf;
	}
	return textEdited;
}


//drag field

//float
template<>
bool ValueField<float>(const std::string& label, float& value, const float& minValue, const float& maxValue)
{
	return ImGui::DragFloat(label.data(), &value, (maxValue - minValue)*0.1f, minValue, maxValue);
}

//input field

//float 
template<>
bool ValueField<float>(const std::string& label, float& value)
{
	return ImGui::InputFloat(label.data(), &value);
}

template<>
bool ValueField<glm::vec2>(const std::string& label, glm::vec2& value)
{
	return ImGui::InputFloat2(label.data(), &value[0]);
}

template<>
bool ValueField<glm::vec3>(const std::string& label, glm::vec3& value)
{
	return ImGui::InputFloat3(label.data(), &value[0]);
}


//int 
template<>
bool ValueField<int>(const std::string& label, int& value)
{
	return ImGui::InputInt(label.data(), &value);
}

template<>
bool ValueField<glm::ivec2>(const std::string& label, glm::ivec2& value)
{
	return ImGui::InputInt(label.data(), &value[0]);
}

template<>
bool ValueField<glm::ivec3>(const std::string& label, glm::ivec3& value)
{
	return ImGui::InputInt(label.data(), &value[0]);
}

}


//// END : Specialization for ValueFields
///////////////////////////////////////////////////////////
