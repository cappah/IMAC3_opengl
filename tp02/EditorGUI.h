#pragma once

#include <string>

#include "imgui/imgui.h"

#include "imgui_extension.h"
#include "ISingleton.h"
#include "GUI.h"
#include "Utils.h"
#include "Factories.h"
#include "ResourcePointer.h"

class ResourceFolder;
class ResourceFile;
class ResourceTree;

enum EditorDragAndDropType
{
	ResourceDragAndDrop = 1<<0,
	ResourceFolderDragAndDrop = 1<<1,
	DroppedFileDragAndDrop = 1<<2
};
ENUM_MASK_IMPLEMENTATION(EditorDragAndDropType);

enum EditorDropContext
{
	DropIntoFileOrFolder = 1 << 0,
	DropIntoResourceField = 1 << 1,
};
ENUM_MASK_IMPLEMENTATION(EditorDropContext);

class EditorWindow
{
protected:
	std::string m_windowName;
	bool m_shouldCloseModale;

public:
	EditorWindow(const std::string& windowName) : m_windowName(windowName), m_shouldCloseModale(false)
	{}

	virtual void drawContent() = 0;
	virtual void drawAsWindow()
	{
		ImGui::Begin(m_windowName.data(), nullptr);
		drawContent();
		ImGui::End();

	}
	virtual void drawAsModal(bool* opened = nullptr)
	{
		ImGui::OpenPopup(m_windowName.data());

		if (ImGui::BeginPopupModal(m_windowName.data(), opened))
		{
			drawContent();

			ImGui::EndPopup();
		}
	}

	virtual bool shouldCloseModale() const
	{
		return m_shouldCloseModale;
	}
};

class DroppedFileDragAndDropOperation : public DragAndDropOperation
{
private:
	ResourceFile* m_resourceFile;

public:
	DroppedFileDragAndDropOperation(const FileHandler::CompletePath& resourcePath);
	virtual void dragOperation() override;
	virtual void dropOperation(void* customData, int dropContext) override;
	virtual void cancelOperation() override;
	virtual void updateOperation() override;
	virtual bool canDropInto(void* customData, int dropContext) override;
};

class ResourceDragAndDropOperation : public DragAndDropOperation
{
private:
	ResourceFolder* m_folderResourceBelongsTo;
	const ResourceFile* m_resourceDragged;
public:
	ResourceDragAndDropOperation(const ResourceFile* resource, ResourceFolder* resourceFolder);
	virtual void dragOperation() override;
	virtual void dropOperation(void* customData, int dropContext) override;
	virtual void cancelOperation() override;
	virtual void updateOperation() override;
	virtual bool canDropInto(void* customData, int dropContext) override;
};

class ResourceFolderDragAndDropOperation : public DragAndDropOperation
{
private:
	ResourceTree* m_resourceTree;
	ResourceFolder* m_parentFolder;
	const ResourceFolder* m_folderDragged;
public:
	ResourceFolderDragAndDropOperation(const ResourceFolder* folder, ResourceFolder* parentFolder, ResourceTree* resourceTree);
	virtual void dragOperation() override;
	virtual void dropOperation(void* customData, int dropContext) override;
	virtual void cancelOperation() override;
	virtual void updateOperation() override;
	virtual bool canDropInto(void* customData, int dropContext) override;
};


namespace EditorGUI {

	template<typename T>
	bool ResourceField(ResourcePtr<T> resourcePtr, const std::string& label, char* buf, int bufSize)
	{
		ResourceType resourceType = getResourceType<T>();
		bool canDropIntoField = DragAndDropManager::canDropInto(&resourceType, EditorDropContext::DropIntoResourceField);
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
			FileHandler::CompletePath droppedResourcePath;
			DragAndDropManager::dropDraggedItem(&droppedResourcePath, (EditorDropContext::DropIntoResourceField));
			assert(droppedResourcePath.toString().size() < bufSize);
			strcpy(buf, droppedResourcePath.c_str());
			isTextEdited = true;
		}

		ImGui::PopStyleColor(colStyleCount);

		if (isTextEdited)
		{
			FileHandler::CompletePath resourcePath(buf);
			if (getResourceFactory<T>().contains(resourcePath))
			{
				resourcePtr = getResourceFactory<T>().get(resourcePath);
			}
		}

		return isTextEdited;
	}
}