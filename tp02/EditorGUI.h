#pragma once

#include <string>

#include "imgui/imgui.h"

#include "imgui_extension.h"
#include "ISingleton.h"
#include "GUI.h"
#include "Utils.h"
#include "ResourcePointer.h"
#include "FileHandler.h"

//forwards : 
class ResourceFolder;
class ResourceFile;
class ResourceTree;

class EditorNode;
class Project;
class Editor;
class EditorModal;
class EditorFrame;
class EditorWindow;
class RemovedNodeDatas;


///////////////////////////////////////////
//// BEGIN : Drag and drop

enum EditorDragAndDropType
{
	ResourceDragAndDrop = 1<<0,
	ResourceFolderDragAndDrop = 1<<1,
	DroppedFileDragAndDrop = 1<<2,
	EditorFrameDragAndDrop = 1<<3
};
ENUM_MASK_IMPLEMENTATION(EditorDragAndDropType);

enum EditorDropContext
{
	DropIntoFileOrFolder = 1 << 0,
	DropIntoResourceField = 1 << 1,
	DropIntoEditorWindow = 1 << 2
};
ENUM_MASK_IMPLEMENTATION(EditorDropContext);


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

class EditorFrameDragAndDropOperation : public DragAndDropOperation
{
private:
	//Don't use it directly ! otherwise, use a shared_from_this. We only need this to get the node of the window.
	int m_currentDraggedWindowId;
public:
	EditorFrameDragAndDropOperation(int currentDraggedWindowId);
	virtual void dragOperation() override;
	virtual void dropOperation(void* customData, int dropContext) override;
	virtual void cancelOperation() override;
	virtual void updateOperation() override;
	virtual bool canDropInto(void* customData, int dropContext) override;
};


//// END : Drag and drop
///////////////////////////////////////////

///////////////////////////////////////////
//// BEGIN : Editor windows

enum EditorNodeDisplayLogicType {
	HorizontalDisplay,
	VerticalDisplay,
	UniqueDisplay,
	EmptyDisplay,
	FrameDisplay
};

class EditorNodeDisplayLogic
{
private:
	EditorNodeDisplayLogicType m_logicType;
public:
	EditorNodeDisplayLogic(EditorNodeDisplayLogicType logicType) : m_logicType(logicType)
	{}
	virtual void drawContent(EditorNode& node, Project& project, Editor& editor, RemovedNodeDatas* removeNodeDatas = nullptr) = 0;
	EditorNodeDisplayLogicType getLogicType() const { return m_logicType; }
	int drawDropZone(ImVec2 pos, ImVec2 size, bool isActive);
};

class EditorNodeHorizontalDisplay : public EditorNodeDisplayLogic
{
public:
	EditorNodeHorizontalDisplay() : EditorNodeDisplayLogic(EditorNodeDisplayLogicType::HorizontalDisplay)
	{}
	virtual void drawContent(EditorNode& node, Project& project, Editor& editor, RemovedNodeDatas* removeNodeDatas = nullptr) override;

	void drawSeparator(int index, float width, float height, EditorNode& node, Editor& editor);
};

class EditorNodeVerticalDisplay : public EditorNodeDisplayLogic
{
public:
	EditorNodeVerticalDisplay() : EditorNodeDisplayLogic(EditorNodeDisplayLogicType::VerticalDisplay)
	{}
	virtual void drawContent(EditorNode& node, Project& project, Editor& editor, RemovedNodeDatas* removeNodeDatas = nullptr) override;

	void drawSeparator(int index, float width, float height, EditorNode& node, Editor& editor);
};

class EditorNodeUniqueDisplay : public EditorNodeDisplayLogic
{
public:
	EditorNodeUniqueDisplay() : EditorNodeDisplayLogic(EditorNodeDisplayLogicType::UniqueDisplay)
	{}
	virtual void drawContent(EditorNode& node, Project& project, Editor& editor, RemovedNodeDatas* removeNodeDatas = nullptr) override;
};

class EditorNodeEmptyDisplay : public EditorNodeDisplayLogic
{
public:
	EditorNodeEmptyDisplay() : EditorNodeDisplayLogic(EditorNodeDisplayLogicType::EmptyDisplay)
	{}
	virtual void drawContent(EditorNode& node, Project& project, Editor& editor, RemovedNodeDatas* removeNodeDatas = nullptr) override;
};

class EditorNodeFrameDisplay : public EditorNodeDisplayLogic
{
public:
	EditorNodeFrameDisplay() : EditorNodeDisplayLogic(EditorNodeDisplayLogicType::FrameDisplay)
	{}
	virtual void drawContent(EditorNode& node, Project& project, Editor& editor, RemovedNodeDatas* removeNodeDatas = nullptr) override;
};

class EditorNode
{
private:
	std::shared_ptr<EditorNodeDisplayLogic> m_displayLogic;
	std::vector<std::shared_ptr<EditorNode>> m_childNodes;
	std::shared_ptr<EditorFrame> m_frame;
	EditorNode* m_parent;

public:
	EditorNode::EditorNode(std::shared_ptr<EditorNodeDisplayLogic> displayLogic);
	EditorNode(std::shared_ptr<EditorNodeDisplayLogic> displayLogic, std::shared_ptr<EditorNode> firstNode);
	EditorNode(std::shared_ptr<EditorFrame> frame);
	EditorNode* EditorNode::getParentNode() const;

	void removeChild(int index);
	void addChild(std::shared_ptr<EditorNode> childNode);
	int getChildCount() const;
	std::shared_ptr<EditorNode> getChild(int index) const;
	void insertChild(std::shared_ptr<EditorNode> childNode, int index);

	void drawContent(Project& project, Editor& editor, RemovedNodeDatas* removeNodeDatas);
	void setDisplayLogic(std::shared_ptr<EditorNodeDisplayLogic> displayLogic);
	EditorNodeDisplayLogicType getDisplayLogicType() const;
	const std::string& getFrameName() const;
	void setFrame(std::shared_ptr<EditorFrame> frame);
	std::shared_ptr<EditorFrame> getFrame() const;
	void onChildRemoved();

	friend class EditorNodeDisplayLogic;
	friend class EditorNodeHorizontalDisplay;
	friend class EditorNodeUniqueDisplay;
	friend class EditorNodeVerticalDisplay;
};

class EditorFrame
{
private:
	std::string m_frameName;

public:
	EditorFrame(const std::string& name) : m_frameName(name)
	{}
	virtual void drawContent(Project& project, EditorModal* parentModale = nullptr) = 0;
	const std::string& getName() { return m_frameName; }

};

//A modale displayed in the editor
class EditorModal
{
protected:
	std::string m_modalName;
	int m_modalId;
	bool m_shouldCloseModale;
	std::shared_ptr<EditorFrame> m_frame;

public:
	EditorModal(int windowId, std::shared_ptr<EditorFrame> frame);
	virtual void draw(Project& project, Editor& editor, bool* opened = nullptr);
	bool shouldCloseModal() const;
	void closeModal();
};

//A small helper to allaw asynchronous node removal
struct RemovedNodeDatas {
	int index;
	EditorNode* parentNode;

	RemovedNodeDatas() : parentNode(nullptr), index(-1)
	{}
};

//A window displayed in the editor
class EditorWindow
{

protected:
	std::string m_windowLabel;
	std::string m_windowStrId;
	int m_windowId;
	std::shared_ptr<EditorNode> m_node;

public:
	EditorWindow(int windowId, std::shared_ptr<EditorFrame> frame);
	EditorWindow(int windowId, std::shared_ptr<EditorNode> node);
	virtual void draw(Project& project, Editor& editor);

	void setNode(std::shared_ptr<EditorNode> newNode);
	std::shared_ptr<EditorNode> getNode() const;
};


//Manager which handle windows and modals
class EditorWindowManager
{
private:
	//windows (views)
	std::vector<std::shared_ptr<EditorWindow>> m_editorWindows;
	std::vector<int> m_freeWindowIds;
	std::vector<std::shared_ptr<EditorNode>> m_windowsToAdd;

	//Modals (views)
	std::vector<std::shared_ptr<EditorModal>> m_editorModals;
	std::vector<int> m_freeModalIds;

public:

	void displayWindows(Project& project, Editor& editor);
	void addWindow(std::shared_ptr<EditorFrame> windowFrame);
	void addWindow(std::shared_ptr<EditorNode> windowNode);
	void removeWindow(EditorWindow* modal);
	void removeWindow(int windowId);
	std::shared_ptr<EditorWindow> getWindow(int windowId) const;
	void addWindowAsynchrone(std::shared_ptr<EditorNode> windowNode);

	void displayModals(Project& project, Editor& editor);
	void addModal(std::shared_ptr<EditorFrame> windowFrame);
	void removeModal(EditorModal* modal);
	void removeModal(int modalId);

	//deals with asynchonous commands
	void update();
};

//// END : Editor windows
///////////////////////////////////////////



///////////////////////////////////////////
//// BEGIN : Editor fields

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

//// END : Editor fields
///////////////////////////////////////////