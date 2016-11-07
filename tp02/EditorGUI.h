#pragma once

#include <string>

#include "imgui/imgui.h"

#include "imgui_extension.h"
#include "ISingleton.h"
#include "GUI.h"
#include "Utils.h"
#include "ResourcePointer.h"
#include "FileHandler.h"

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
	bool ResourceField(const std::string& label, ResourcePtr<T> resourcePtr);
	//{
	//	const int bufSize = 100;
	//	std::string currentResourceName(resourcePtr.isValid() ? resourcePtr->getCompletePath().getFilename() : "INVALID");
	//	currentResourceName.reserve(bufSize);

	//	ResourceType resourceType = getResourceType<T>();
	//	bool canDropIntoField = DragAndDropManager::canDropInto(&resourceType, EditorDropContext::DropIntoResourceField);
	//	bool isTextEdited = false;
	//	bool needClearPtr = false;

	//	int colStyleCount = 0;
	//	if (canDropIntoField)
	//	{
	//		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 255, 0, 255));
	//		colStyleCount++;
	//	}

	//	bool enterPressed = ImGui::InputText(label.c_str(), &currentResourceName[0], bufSize, ImGuiInputTextFlags_EnterReturnsTrue|ImGuiInputTextFlags_ReadOnly);
	//	isTextEdited = (enterPressed || ImGui::IsKeyPressed(GLFW_KEY_TAB) || (!ImGui::IsItemHovered() && ImGui::IsMouseClickedAnyButton()));
	//	ImGui::SameLine();
	//	needClearPtr = ImGui::SmallButton(std::string("<##" + label).data());


	//	//borders if can drop here : 
	//	if (ImGui::IsItemHovered() && canDropIntoField)
	//	{
	//		ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), 2.f);
	//	}

	//	//drop resource : 
	//	FileHandler::CompletePath droppedResourcePath;
	//	if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(0))
	//	{
	//		DragAndDropManager::dropDraggedItem(&droppedResourcePath, (EditorDropContext::DropIntoResourceField));
	//		isTextEdited = true;
	//	}

	//	ImGui::PopStyleColor(colStyleCount);

	//	if (needClearPtr)
	//		resourcePtr.reset();
	//	else if (isTextEdited)
	//	{
	//		if (getResourceFactory<T>().contains(droppedResourcePath))
	//			resourcePtr = getResourceFactory<T>().get(droppedResourcePath);
	//		else
	//			resourcePtr.reset();
	//	}

	//	return isTextEdited;
	//}
	
	//Value fields : 
	template<typename T>
	bool ValueField(const std::string& label, T& value)
	{
		assert(0 && "invalid value type form this field.");
		return false;
	}

	template<typename T>
	bool ValueField(const std::string& label, T& value, const T& minValue, const T& maxValue)
	{
		assert(0 && "invalid value type form this field.");
		return false;
	}

	//resource field 
	//template<typename U>
	//bool ValueField(const std::string& label, ResourcePtr<U> resourcePtr)
	//{
	//	return ResourceField(label, resourcePtr);
	//}

	//template<>
	//bool ValueField<Texture>(const std::string& label, ResourcePtr<Texture> resourcePtr)
	//{
	//	return ResourceField(label, resourcePtr);
	//}

	//template<>
	//bool ValueField<Texture>(const std::string& label, ResourcePtr<Texture> resourcePtr)
	//{
	//	return ResourceField(label, resourcePtr);
	//}

	//string field
	bool ValueField(const std::string& label, std::string& outString, char* buf, int bufSize);


	//drag field

	//float
	template<>
	bool ValueField<float>(const std::string& label, float& value, const float& minValue, const float& maxValue);

	//input field

	//float 
	template<>
	bool ValueField<float>(const std::string& label, float& value);

	template<>
	bool ValueField<glm::vec2>(const std::string& label, glm::vec2& value);

	template<>
	bool ValueField<glm::vec3>(const std::string& label, glm::vec3& value);


	//int 
	template<>
	bool ValueField<int>(const std::string& label, int& value);

	template<>
	bool ValueField<glm::ivec2>(const std::string& label, glm::ivec2& value);

	template<>
	bool ValueField<glm::ivec3>(const std::string& label, glm::ivec3& value);

}