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
class ResourceTreeWindow;

enum EditorDragAndDropType
{
	ResourceDragAndDrop = 1<<0,
	ResourceFolderDragAndDrop = 1<<1,
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
public:
	virtual void drawUI() = 0;
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
	virtual bool canDropInto(void* data) override;
};

class ResourceFolderDragAndDropOperation : public DragAndDropOperation
{
private:
	ResourceTreeWindow* m_resourceTree;
	ResourceFolder* m_parentFolder;
	const ResourceFolder* m_folderDragged;
public:
	ResourceFolderDragAndDropOperation(const ResourceFolder* folder, ResourceFolder* parentFolder, ResourceTreeWindow* resourceTree);
	virtual void dragOperation() override;
	virtual void dropOperation(void* customData, int dropContext) override;
	virtual void cancelOperation() override;
	virtual void updateOperation() override;
};


namespace EditorGUI {

	template<typename T>
	bool ResourceField(ResourcePtr<T> resourcePtr, const std::string& label, char* buf, int bufSize)
	{
		ResourceType resourceType = getResourceType<T>();
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