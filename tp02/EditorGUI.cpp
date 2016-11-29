#include "stdafx.h"

#include "Editor.h"
#include "EditorGUI.h"
#include "ResourceTree.h"
#include "Project.h" //forward

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
//// BEGIN : editor frame drag and drop


EditorFrameDragAndDropOperation::EditorFrameDragAndDropOperation(int currentDraggedWindowId, Editor* editorPtr)
	: DragAndDropOperation(EditorDragAndDropType::EditorFrameDragAndDrop, EditorDropContext::DropIntoEditorWindow)
	, m_currentDraggedWindowId(currentDraggedWindowId)
	, m_editorPtr(editorPtr)
{
	m_editorPtr->getWindowManager()->getWindow(m_currentDraggedWindowId)->setAlpha(0.2f);
}

void EditorFrameDragAndDropOperation::dragOperation()
{
}

void EditorFrameDragAndDropOperation::dropOperation(void* customData, int dropContext)
{
	//std::shared_ptr<EditorNode>* ptrToNodeWeDropOn = (std::shared_ptr<EditorNode>*)customData;
	//*ptrToNodeWeDropOn = m_currentDraggedWindow->getNode();
	//m_currentDraggedWindow = nullptr;
	int* ptrToWindowsId = (int*)customData;
	*ptrToWindowsId = m_currentDraggedWindowId;

	m_editorPtr->getWindowManager()->getWindow(m_currentDraggedWindowId)->setAlpha(1.0f);
	m_currentDraggedWindowId = -1;
	m_editorPtr = nullptr;
}

void EditorFrameDragAndDropOperation::cancelOperation()
{
	m_editorPtr->getWindowManager()->getWindow(m_currentDraggedWindowId)->setAlpha(1.0f);
	m_currentDraggedWindowId = -1;
	m_editorPtr = nullptr;
}

void EditorFrameDragAndDropOperation::updateOperation()
{
	m_editorPtr->getWindowManager()->getWindow(m_currentDraggedWindowId)->move(ImGui::GetIO().MouseDelta);
}

bool EditorFrameDragAndDropOperation::canDropInto(void * customData, int dropContext)
{
	return false;
}

//// END : editor frame drag and drop
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



///////////////////////////////////////////////////////////
//// BEGIN : Editor nodes

EditorNode::EditorNode(std::shared_ptr<EditorNodeDisplayLogic> displayLogic)
	: m_displayLogic(displayLogic)
	, m_parent(nullptr)
	, m_size(1,1)
{
}

EditorNode::EditorNode(std::shared_ptr<EditorNodeDisplayLogic> displayLogic, std::shared_ptr<EditorNode> firstNode)
	: m_displayLogic(displayLogic)
	, m_parent(nullptr)
	, m_size(1, 1)
{
	addChild(firstNode);
}

EditorNode::EditorNode(std::shared_ptr<EditorFrame> frame)
	: m_displayLogic(std::make_shared<EditorNodeFrameDisplay>())
	, m_parent(nullptr)
	, m_size(1, 1)
{
	setFrame(frame);
}

int EditorNode::getChildCount() const
{
	return m_childNodes.size();
}

void EditorNode::removeChild(int index)
{
	assert(index >= 0 && index < m_childNodes.size());

	onBeforeRemoveChild(index);

	m_childNodes[index]->m_parent = nullptr;
	m_childNodes.erase(m_childNodes.begin() + index);
}

void EditorNode::removeChild(EditorNode* nodeToRemove)
{
	auto found = std::find_if(m_childNodes.begin(), m_childNodes.end(), [nodeToRemove](std::shared_ptr<EditorNode>& item) { return nodeToRemove == item.get(); });

	if (found != m_childNodes.end())
	{
		int index = std::distance(m_childNodes.begin(), found);

		onBeforeRemoveChild(index);

		m_childNodes[index]->m_parent = nullptr;
		m_childNodes.erase(m_childNodes.begin() + index);
	}
}

void EditorNode::addChild(std::shared_ptr<EditorNode> childNode)
{
	childNode->m_parent = this;
	m_childNodes.push_back(childNode);

	onChildAdded(m_childNodes.size() - 1);
}

EditorNode* EditorNode::getParentNode() const
{
	return m_parent;
}

const ImVec2& EditorNode::getSize() const
{
	return m_size;
}

//const ImVec2 & EditorNode::getSizeOffset() const
//{
//	return m_sizeOffset;
//}
//
//const ImVec2 & EditorNode::getCorrectedSize() const
//{
//	return m_correctedSize;
//}

void EditorNode::setWidth(float newWidth)
{

	//Set size
	const float lastWidth = getSize().x;
	m_size.x = newWidth;

	////Set corrected size
	//float widthSizeOffset = 0;
	//if (m_parent != nullptr)
	//{
	//	if (m_parent->getDisplayLogicType() == EditorNodeDisplayLogicType::HorizontalDisplay)
	//		if(m_parent->getChildCount() == 1)
	//			widthSizeOffset = 10.0f;
	//		else
	//			widthSizeOffset = 7.5f;
	//	else if(m_parent->getDisplayLogicType() == EditorNodeDisplayLogicType::UniqueDisplay)
	//		widthSizeOffset = 10.0f;

	//	m_sizeOffset.x = m_parent->getSizeOffset().x + widthSizeOffset;
	//}
	//else
	//	m_sizeOffset.x = 0.f;

	//m_correctedSize.x = m_size.x - m_sizeOffset.x;

	//Recursivity
	for (auto& child : m_childNodes)
	{
		float relativeWidth = child->getSize().x / lastWidth;
		child->setWidth(relativeWidth * newWidth);
	}
	
}

void EditorNode::setHeight(float newHeight)
{
	//Set size
	const float lastHeight = getSize().y;
	m_size.y = newHeight;

	////Set corrected height
	//float heightSizeOffset = 0;
	//if (m_parent != nullptr)
	//{
	//	if (m_parent->getDisplayLogicType() == EditorNodeDisplayLogicType::VerticalDisplay)
	//		if(m_parent->getChildCount() == 1)
	//			heightSizeOffset = 10.0f;
	//		else
	//			heightSizeOffset = 7.5f;
	//	else if(m_parent->getDisplayLogicType() == EditorNodeDisplayLogicType::UniqueDisplay)
	//		heightSizeOffset = 10.0f;

	//	m_sizeOffset.y = m_parent->getSizeOffset().y + heightSizeOffset;
	//}
	//else
	//	m_sizeOffset.y = 0.f;

	//m_correctedSize.y = m_size.y - m_sizeOffset.y;


	//Recursivity
	for (auto& child : m_childNodes)
	{
		float relativeHeight = child->getSize().y / lastHeight;
		child->setHeight(relativeHeight * newHeight);
	}
}

std::shared_ptr<EditorNode> EditorNode::getChild(int index) const
{
	assert(index >= 0 && index < m_childNodes.size());

	return m_childNodes[index];
}

void EditorNode::insertChild(std::shared_ptr<EditorNode> childNode, int index)
{
	m_childNodes.insert(m_childNodes.begin() + index, childNode);
	onChildAdded(index);
}

void EditorNode::onChildAdded(int index)
{
	m_displayLogic->onChildAdded(*this, index);
}

void EditorNode::onBeforeRemoveChild(int index)
{
	m_displayLogic->onBeforeRemoveChild(*this, index);
}

bool EditorNode::drawContent(Project& project, Editor& editor, RemovedNodeDatas* removeNodeDatas, const ImVec2& parentSizeOffset)
{
	assert(m_displayLogic);
	return m_displayLogic->drawContent(*this, project, editor, removeNodeDatas, parentSizeOffset);
}

void EditorNode::setDisplayLogic(std::shared_ptr<EditorNodeDisplayLogic> displayLogic)
{
	m_displayLogic = displayLogic;
}

EditorNodeDisplayLogicType EditorNode::getDisplayLogicType() const
{
	return m_displayLogic->getLogicType();
}

const std::string& EditorNode::getFrameName() const
{
	assert(m_displayLogic->getLogicType() == EditorNodeDisplayLogicType::FrameDisplay 
		|| (m_displayLogic->getLogicType() == EditorNodeDisplayLogicType::UniqueDisplay && m_childNodes.size() == 1 && m_childNodes[0]->getDisplayLogicType() == EditorNodeDisplayLogicType::FrameDisplay));

	if (m_displayLogic->getLogicType() == EditorNodeDisplayLogicType::FrameDisplay)
	{
		return m_frame->getName();
	}
	else if (m_displayLogic->getLogicType() == EditorNodeDisplayLogicType::UniqueDisplay)
	{
		return m_childNodes[0]->getFrameName();
	}
}

void EditorNode::setFrame(std::shared_ptr<EditorFrame> frame)
{
	assert(m_displayLogic->getLogicType() == EditorNodeDisplayLogicType::FrameDisplay);
	m_frame = frame;
}

std::shared_ptr<EditorFrame> EditorNode::getFrame() const
{
	assert(m_displayLogic->getLogicType() == EditorNodeDisplayLogicType::FrameDisplay);
	return m_frame;
}

void EditorNode::onChildRemoved()
{
	for (int i = 0; i < m_childNodes.size(); i++)
	{
		if (m_childNodes[i]->getChildCount() == 0)
			removeChild(i);
	}

	if (m_parent != nullptr)
		m_parent->onChildRemoved();
	//we are in the root window node
	else if (m_childNodes[0]->getChildCount() == 1)
	{
		//root window node -> (node 01) horizontal node / vertical node -> (node 02) vertical node / horizontal node -> frame node
		std::shared_ptr<EditorNode> node01 = m_childNodes[0];
		std::shared_ptr<EditorNode> node02 = m_childNodes[0]->getChild(0);
		if (node02->getChildCount() == 1)
		{
			std::shared_ptr<EditorNode> frameNode = node02->getChild(0); //frame node
			node02->removeChild(0); //remove frame
			node01->removeChild(0); //remove node 02
			removeChild(0); //remove node 01

			addChild(frameNode); //add frame node again
		}
	}
}


///////////////////////

//TODO : use active
int EditorNodeDisplayLogic::drawDropZone(ImVec2 rectMin, ImVec2 recMax, bool isActive, Editor& editor)
{
	int draggedWindowId = -1;
	bool isHoveringDropZone = false;

	if (ImGui::IsMouseHoveringRect(rectMin, recMax)
		&& !editor.getWindowManager()->getIsResizingChild()
		&& DragAndDropManager::isDragAndDropping() 
		&& DragAndDropManager::getOperationType() == EditorDragAndDropType::EditorFrameDragAndDrop)
	{

		if (ImGui::IsMouseReleased(GLFW_MOUSE_BUTTON_LEFT))
		{
			DragAndDropManager::dropDraggedItem(&draggedWindowId, EditorDropContext::DropIntoEditorWindow);
		}
		else
			isHoveringDropZone = true;
	}

	int dropZoneAlpha = 100;
	if (isHoveringDropZone)
		dropZoneAlpha = 255;


	ImGui::GetWindowDrawList()->AddRectFilled(rectMin, recMax, ImColor(0, 255, 255, dropZoneAlpha));

	return draggedWindowId;
}

bool EditorNodeHorizontalDisplay::drawContent(EditorNode& node, Project& project, Editor& editor, RemovedNodeDatas* removeNodeDatas, const ImVec2& parentSizeOffset)
{
	const ImVec2 recMin = ImGui::GetWindowContentRegionMin();
	const ImVec2 recMax = ImGui::GetWindowContentRegionMin();
	const ImVec2 recSize = ImVec2(ImGui::GetWindowContentRegionWidth(), ImGui::GetWindowHeight());
	const int childCount = node.m_childNodes.size();
	const float separatorWidth = 5.f;
	//const float childWidth = (recSize.x - (childCount + 1)*separatorWidth - padding*2.f) / childCount;
	//const float childHeight = recSize.y - padding*2.f;
	const float padding = 2.f; //small top bottom right and left padding
	const float separatorHeight = recSize.y;

	bool removeChildNode = false;
	int removeChildNodeId = -1;
	bool isResizingChild = false;
	ImVec2 currentSizeOffset(parentSizeOffset);
	currentSizeOffset.x += 5.f * (node.getChildCount() + 1.f);
	ImVec2 perChildSizeOffset(currentSizeOffset.x / (float)node.getChildCount(), currentSizeOffset.y);

	isResizingChild |= drawSeparator(0, separatorWidth, separatorHeight, node, editor);

	for (int i = 0; i < childCount; i++)
	{
		EditorNode& currentChildNode = *(node.m_childNodes[i]);

		ImGui::PushID(i);

		ImGui::SameLine();
		ImGui::PushID("Child");
		ImGui::BeginChild(i, ImVec2(currentChildNode.getSize().x - perChildSizeOffset.x, currentChildNode.getSize().y - perChildSizeOffset.y), true/*, ImGuiWindowFlags_NoScrollbar*/);

		if(currentChildNode.drawContent(project, editor, removeNodeDatas, perChildSizeOffset))
			removeNodeDatas->nodeToRemove = node.m_childNodes[i];

		ImGui::EndChild();
		ImGui::PopID();

		ImGui::SameLine();
		isResizingChild |= drawSeparator(i + 1, separatorWidth, separatorHeight, node, editor);

		ImGui::PopID();
	}

	return false;
}

void EditorNodeHorizontalDisplay::onChildAdded(EditorNode& node, int index)
{
	node.m_childNodes[index]->setWidth(node.getSize().x / node.getChildCount());
	node.m_childNodes[index]->setHeight(node.getSize().y);

	const float availableWidth = node.getSize().x - node.m_childNodes[index]->getSize().x;

	int currentIndex = 0;
	for (auto& child : node.m_childNodes)
	{
		if (currentIndex != index)
		{
			float lastRelativeWidth = child->getSize().x / node.getSize().x;
			child->setWidth(availableWidth * lastRelativeWidth);
		}

		currentIndex++;
	}
}

void EditorNodeHorizontalDisplay::onBeforeRemoveChild(EditorNode & node, int index)
{
	const float lastAvailableWidth = node.getSize().x - node.m_childNodes[index]->getSize().x;

	for (auto& child : node.m_childNodes)
	{
		float lastRelativeWidth = child->getSize().x / lastAvailableWidth;
		child->setWidth(node.getSize().x * lastRelativeWidth);
	}
}

bool EditorNodeHorizontalDisplay::drawSeparator(int index, float width, float height, EditorNode& node, Editor& editor)
{
	bool isResizingChild = false;

	//Invisible button for behavior and sizes
	ImGui::InvisibleButton("##button", ImVec2(width, height));
	const ImVec2 size = ImGui::GetItemRectSize();
	const ImVec2 recMin = ImGui::GetItemRectMin();
	const ImVec2 recMax = ImVec2(recMin.x + size.x, recMin.y + size.y);
	const bool isButtonActive = ImGui::IsItemActive();
	//Display the drop zone
	int droppedWindowId = drawDropZone(recMin, recMax, ImGui::IsWindowHovered(), editor);

	if (droppedWindowId != -1)
	{
		//We get the dragged node from its parent window id
		std::shared_ptr<EditorNode> droppedNode = editor.getWindowManager()->getWindow(droppedWindowId)->getNode()->getChild(0);

		//Assure we insert vertical node in an horizonatl list
		std::shared_ptr<EditorNode> newNode = droppedNode;
		if (droppedNode->getDisplayLogicType() != EditorNodeDisplayLogicType::VerticalDisplay)
			newNode = std::make_shared<EditorNode>(std::make_shared<EditorNodeVerticalDisplay>(), droppedNode);

		if (node.getChildCount() == 1)
		{
			std::shared_ptr<EditorNode> child = node.getChild(0);
			node.removeChild(0);
			node.addChild(std::make_shared<EditorNode>(std::make_shared<EditorNodeVerticalDisplay>(), child));
		}

		//We insert the new node
		node.insertChild(newNode, index);

		//We delete the dragged window
		editor.getWindowManager()->removeWindow(droppedWindowId);
	}
	else
	{
		//We handle the resize
		if (isButtonActive)
		{
			if (index > 0)
			{
				const float lastWidth = node.getChild(index - 1)->getSize().x;
				node.getChild(index - 1)->setWidth(lastWidth + ImGui::GetIO().MouseDelta.x);
			}
			if (index <= node.getChildCount())
			{
				const float lastWidth = node.getChild(index)->getSize().x;
				node.getChild(index)->setWidth(lastWidth - ImGui::GetIO().MouseDelta.x);
			}

			isResizingChild = true;
			editor.getWindowManager()->setIsResizingChild(true);
		}
	}

	return isResizingChild;
}

bool EditorNodeVerticalDisplay::drawContent(EditorNode& node, Project& project, Editor& editor, RemovedNodeDatas* removeNodeDatas, const ImVec2& parentSizeOffset)
{
	const ImVec2 recMin = ImGui::GetWindowContentRegionMin();
	const ImVec2 recMax = ImGui::GetWindowContentRegionMin();
	const ImVec2 recSize = ImVec2(ImGui::GetWindowContentRegionWidth(), ImGui::GetWindowHeight());
	const int childCount = node.m_childNodes.size();
	const float padding = 2.f; //small top bottom right and left padding
	//const float childWidth = recSize.x - padding*2.f;
	//const float childHeight = (recSize.y - (childCount + 1)*separatorHeight - padding*2.f) / childCount;
	const float separatorHeight = 5.f;
	const float separatorWidth = recSize.x;

	bool removeChildNode = false;
	int removeChildNodeId = -1;
	bool isResizingChild = false;
	ImVec2 currentSizeOffset(parentSizeOffset);
	currentSizeOffset.y += 5.0f * (node.getChildCount() + 1.f);
	ImVec2 perChildSizeOffset(currentSizeOffset.x, currentSizeOffset.y / (float)node.getChildCount());

	isResizingChild |= drawSeparator(0, separatorWidth, separatorHeight, node, editor);

	for (int i = 0; i < node.m_childNodes.size(); i++)
	{
		EditorNode& currentChildNode = *node.m_childNodes[i];

		ImGui::PushID(i);

		ImGui::PushID("Child");
		ImGui::BeginChild(i, ImVec2(currentChildNode.getSize().x - perChildSizeOffset.x, currentChildNode.getSize().y - perChildSizeOffset.y), true/*, ImGuiWindowFlags_NoScrollbar*/);

		if (currentChildNode.drawContent(project, editor, removeNodeDatas, perChildSizeOffset))
			removeNodeDatas->nodeToRemove = node.m_childNodes[i];

		ImGui::EndChild();
		ImGui::PopID();

		isResizingChild |= drawSeparator(i + 1, separatorWidth, separatorHeight, node, editor);

		ImGui::PopID();
	}

	return false;
}

void EditorNodeVerticalDisplay::onChildAdded(EditorNode & node, int index)
{
	node.m_childNodes[index]->setWidth(node.getSize().x);
	node.m_childNodes[index]->setHeight(node.getSize().y / (float) node.getChildCount());

	const float availableHeight = node.getSize().y - node.m_childNodes[index]->getSize().y;

	int currentIndex = 0;
	for (auto& child : node.m_childNodes)
	{
		if (currentIndex != index)
		{
			float lastRelativeHeight = child->getSize().y / (float) node.getSize().y;
			child->setHeight(availableHeight * lastRelativeHeight);
		}

		currentIndex++;
	}
}

void EditorNodeVerticalDisplay::onBeforeRemoveChild(EditorNode & node, int index)
{
	const float lastAvailableHeight = node.getSize().y - node.m_childNodes[index]->getSize().y;

	for (auto& child : node.m_childNodes)
	{
		float lastRelativeHeight = child->getSize().y / lastAvailableHeight;
		child->setHeight(node.getSize().y * lastRelativeHeight);
	}
}

bool EditorNodeVerticalDisplay::drawSeparator(int index, float width, float height, EditorNode& node, Editor& editor)
{
	bool isResizingChild = false;

	//Invisible button for behavior and sizes
	ImGui::InvisibleButton("##button", ImVec2(width, height));
	const ImVec2 size = ImGui::GetItemRectSize();
	const ImVec2 recMin = ImGui::GetItemRectMin();
	const ImVec2 recMax = ImVec2(recMin.x + size.x, recMin.y + size.y);
	const bool isButtonActive = ImGui::IsItemActive();
	//Display the drop zone
	int droppedWindowId = drawDropZone(recMin, recMax, ImGui::IsWindowHovered(), editor);

	//We handle the window drop : 
	if (droppedWindowId != -1)
	{
		//We get the dragged node from its parent window id
		std::shared_ptr<EditorNode> droppedNode = editor.getWindowManager()->getWindow(droppedWindowId)->getNode()->getChild(0);

		//Assure we insert horizontal node in a vertical list
		std::shared_ptr<EditorNode> newNode = droppedNode;
		if (droppedNode->getDisplayLogicType() != EditorNodeDisplayLogicType::HorizontalDisplay)
			newNode = std::make_shared<EditorNode>(std::make_shared<EditorNodeHorizontalDisplay>(), droppedNode);

		if (node.getChildCount() == 1)
		{
			std::shared_ptr<EditorNode> child = node.getChild(0);
			node.removeChild(0);
			node.addChild(std::make_shared<EditorNode>(std::make_shared<EditorNodeHorizontalDisplay>(), child));
		}

		//We insert the new node
		node.insertChild(newNode, index);

		//We delete the dragged window
		editor.getWindowManager()->removeWindow(droppedWindowId);
	}
	else
	{
		//We handle the resize 
		if (isButtonActive)
		{
			if (index > 0)
			{
				const float lastHeight = node.getChild(index - 1)->getSize().y;
				node.getChild(index - 1)->setHeight(lastHeight + ImGui::GetIO().MouseDelta.y);
			}
			if (index <= node.getChildCount())
			{
				const float lastHeight = node.getChild(index)->getSize().y;
				node.getChild(index)->setHeight(lastHeight - ImGui::GetIO().MouseDelta.y);
			}

			isResizingChild = true;
			editor.getWindowManager()->setIsResizingChild(true);
		}
	}

	return isResizingChild;
}

void EditorNodeUniqueDisplay::onChildAdded(EditorNode & node, int index)
{
	assert(index == 0);
	assert(node.getChildCount() == 1);

	node.m_childNodes[index]->setWidth(node.getSize().x);
	node.m_childNodes[index]->setHeight(node.getSize().y);
}

bool EditorNodeUniqueDisplay::drawContent(EditorNode& node, Project& project, Editor& editor, RemovedNodeDatas* removeNodeDatas, const ImVec2& parentSizeOffset)
{
	const ImVec2 recMin = ImGui::GetItemRectMin();
	const ImVec2 recMax = ImGui::GetItemRectMax();
	const ImVec2 recSize = ImVec2(ImGui::GetWindowContentRegionWidth(), recMax.y - recMin.y);
	const int childCount = node.m_childNodes.size();
	const float horizontalSeparatorHeight = 5.f;
	const float verticalSeparatorWidth = 5.f;
	const float padding = 0.f; //small top bottom right and left padding
	const float childWidth = (recSize.x - 2*verticalSeparatorWidth - padding*2.f);
	const float childHeight = (recSize.y - 2*horizontalSeparatorHeight - padding*2.f);
	const float verticalSeparatorHeight = childHeight;
	const float horizontalSeparatorWidth = recSize.x;

	ImVec2 currentSizeOffset(parentSizeOffset);
	currentSizeOffset.x += 10.f;
	currentSizeOffset.y += 10.f;

	assert(childCount == 1);

	ImGui::PushID(this);

	//top separator
	//ImGui::BeginChild("##separator01", ImVec2(horizontalSeparatorWidth, horizontalSeparatorHeight), false);
	ImGui::InvisibleButton("##button01", ImVec2(horizontalSeparatorWidth, horizontalSeparatorHeight));
	//ImGui::EndChild();

	int droppedWindowId = drawDropZone(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImGui::IsWindowHovered(), editor);
	if (droppedWindowId != -1)
	{
		std::shared_ptr<EditorNode> droppedNode = editor.getWindowManager()->getWindow(droppedWindowId)->getNode()->getChild(0);
		std::shared_ptr<EditorNode> singleNode = node.m_childNodes[0];
		node.removeChild(0);

		std::shared_ptr<EditorNode> newHorizontalNode01 = std::make_shared<EditorNode>(std::make_shared<EditorNodeHorizontalDisplay>(), singleNode);
		std::shared_ptr<EditorNode> newHorizontalNode02 = std::make_shared<EditorNode>(std::make_shared<EditorNodeHorizontalDisplay>(), droppedNode);
		auto newVerticalNode = std::make_shared<EditorNode>(std::make_shared<EditorNodeVerticalDisplay>(), newHorizontalNode01);
		newVerticalNode->addChild(newHorizontalNode02);
		node.addChild(newVerticalNode);
		droppedNode = nullptr;

		editor.getWindowManager()->removeWindow(droppedWindowId);
	}

	//left separator
	//ImGui::BeginChild("##separator02", ImVec2(verticalSeparatorWidth, verticalSeparatorHeight), false);
	ImGui::InvisibleButton("##button02", ImVec2(verticalSeparatorWidth, verticalSeparatorHeight));
	//ImGui::EndChild();

	droppedWindowId = drawDropZone(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImGui::IsWindowHovered(), editor);
	if (droppedWindowId != -1)
	{
		std::shared_ptr<EditorNode> droppedNode = editor.getWindowManager()->getWindow(droppedWindowId)->getNode()->getChild(0);
		std::shared_ptr<EditorNode> singleNode = node.m_childNodes[0];
		node.removeChild(0);

		std::shared_ptr<EditorNode> newVerticalNode01 = std::make_shared<EditorNode>(std::make_shared<EditorNodeVerticalDisplay>(), singleNode);
		std::shared_ptr<EditorNode> newVerticalNode02 = std::make_shared<EditorNode>(std::make_shared<EditorNodeVerticalDisplay>(), droppedNode);
		auto newHorizontalNode = std::make_shared<EditorNode>(std::make_shared<EditorNodeHorizontalDisplay>(), newVerticalNode01);
		newHorizontalNode->addChild(newVerticalNode02);
		node.addChild(newHorizontalNode);
		droppedNode = nullptr;

		editor.getWindowManager()->removeWindow(droppedWindowId);
	}

	ImGui::SameLine();
	ImGui::BeginChild("##child", ImVec2(childWidth, childHeight), true/*, ImGuiWindowFlags_NoScrollbar*/);
	node.m_childNodes[0]->drawContent(project, editor, removeNodeDatas, currentSizeOffset);
	ImGui::EndChild();

	//right separator
	ImGui::SameLine();
	//ImGui::BeginChild("##separator03", ImVec2(verticalSeparatorWidth, verticalSeparatorHeight), false);
	ImGui::InvisibleButton("##button03", ImVec2(verticalSeparatorWidth, verticalSeparatorHeight));
	//ImGui::EndChild();

	droppedWindowId = drawDropZone(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImGui::IsWindowHovered(), editor);
	if (droppedWindowId != -1)
	{
		std::shared_ptr<EditorNode> droppedNode = editor.getWindowManager()->getWindow(droppedWindowId)->getNode()->getChild(0);
		std::shared_ptr<EditorNode> singleNode = node.m_childNodes[0];
		node.removeChild(0);

		std::shared_ptr<EditorNode> newVerticalNode01 = std::make_shared<EditorNode>(std::make_shared<EditorNodeVerticalDisplay>(), singleNode);
		std::shared_ptr<EditorNode> newVerticalNode02 = std::make_shared<EditorNode>(std::make_shared<EditorNodeVerticalDisplay>(), droppedNode);
		auto newHorizontalNode = std::make_shared<EditorNode>(std::make_shared<EditorNodeHorizontalDisplay>(), newVerticalNode01);
		newHorizontalNode->addChild(newVerticalNode02);
		node.addChild(newHorizontalNode);
		droppedNode = nullptr;

		editor.getWindowManager()->removeWindow(droppedWindowId);
	}

	//bottom separator
	//ImGui::BeginChild("##separator04", ImVec2(horizontalSeparatorWidth, horizontalSeparatorHeight), false);
	ImGui::InvisibleButton("##button04", ImVec2(horizontalSeparatorWidth, horizontalSeparatorHeight));
	//ImGui::EndChild();

	droppedWindowId = drawDropZone(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImGui::IsWindowHovered(), editor);
	if (droppedWindowId != -1)
	{
		std::shared_ptr<EditorNode> droppedNode = editor.getWindowManager()->getWindow(droppedWindowId)->getNode()->getChild(0);
		std::shared_ptr<EditorNode> singleNode = node.m_childNodes[0];
		node.removeChild(0);

		std::shared_ptr<EditorNode> newHorizontalNode01 = std::make_shared<EditorNode>(std::make_shared<EditorNodeHorizontalDisplay>(), singleNode);
		std::shared_ptr<EditorNode> newHorizontalNode02 = std::make_shared<EditorNode>(std::make_shared<EditorNodeHorizontalDisplay>(), droppedNode);
		auto newVerticalNode = std::make_shared<EditorNode>(std::make_shared<EditorNodeVerticalDisplay>(), newHorizontalNode01);
		newVerticalNode->addChild(newHorizontalNode02);
		node.addChild(newVerticalNode);
		droppedNode = nullptr;

		editor.getWindowManager()->removeWindow(droppedWindowId);
	}

	ImGui::PopID();

	return false;
}

bool EditorNodeEmptyDisplay::drawContent(EditorNode& node, Project& project, Editor& editor, RemovedNodeDatas* removeNodeDatas, const ImVec2& parentSizeOffset)
{
	//TODO
	return false;
}

bool EditorNodeFrameDisplay::drawContent(EditorNode & node, Project & project, Editor& editor, RemovedNodeDatas* removeNodeDatas, const ImVec2& parentSizeOffset)
{
	bool needRemove = false;

	EditorStyleSheet::pushFramePadding();
	ImGui::PushID(this);

	//ImGui::BeginChild("##button", ImVec2(node.getSize().x - parentSizeOffset.x, 20), false, ImGuiWindowFlags_NoScrollbar);
	////We began to drag the node, we have to detach it from the parent window
	//if (node.getParentNode()->getDisplayLogicType() != EditorNodeDisplayLogicType::UniqueDisplay)
	//{
	//	////Draw custom button : 
	//	{
	//		//const char label[] = {'>', '\0'};
	//		//const ImRect bb(ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionWidth() - 20, ImGui::GetWindowPos().y + ImGui::GetWindowHeight() - 20
	//		//	, ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionWidth(), ImGui::GetWindowPos().y + ImGui::GetWindowHeight());
	//		//const ImGuiID id = ImGui::GetCurrentWindow()->GetID(label);
	//		//bool hovered, held;
	//		//bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

	//		//// Render
	//		//const ImGuiStyle style = ImGui::GetStyle();
	//		//const ImU32 col = ImGui::GetColorU32((hovered && held) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
	//		//ImGui::RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);
	//		//ImGui::RenderTextClipped(ImVec2(bb.Min.x + style.FramePadding.x, bb.Min.y + style.FramePadding.y)
	//		//	, ImVec2(bb.Max.x - style.FramePadding.x, bb.Max.y - style.FramePadding.y)
	//		//	, label, NULL, nullptr, ImVec2(0.f, 0.f), &bb);

	//		ImGui::Text(node.getFrameName().c_str());
	//		ImGui::SameLine();
	//		ImGui::Separator();
	//		ImGui::SameLine();
	//		ImGui::PushItemWidth(-20);
	//		if (ImGui::SmallButton(">"))
	//		{
	//			needRemove = true;
	//		}
	//		ImGui::PopItemWidth();
	//	}
	//}
	//ImGui::EndChild();
	ImVec2 windowSize(node.getSize().x - parentSizeOffset.x, node.getSize().y - parentSizeOffset.y);
	ImGui::BeginChild("##child", windowSize, true, ImGuiWindowFlags_HorizontalScrollbar /*, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar*/);

	if (node.getParentNode()->getDisplayLogicType() != EditorNodeDisplayLogicType::UniqueDisplay)
	{
		ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetWindowPos(), ImVec2(ImGui::GetWindowPos().x + windowSize.x, ImGui::GetWindowPos().y + 20), ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_TitleBg]));
		if (ImGui::SmallButton(">"))
		{
			needRemove = true;
		}
		ImGui::SameLine();
		ImGui::Dummy(ImVec2(10, 0));
		ImGui::SameLine();
		ImGui::Text(node.getFrameName().c_str());
	}
	ImGui::Separator();

	node.getFrame()->drawContent(project, nullptr);
	ImGui::EndChild();

	ImGui::PopID();

	EditorStyleSheet::popFramePadding();

	return needRemove;
}


//// END : Editor nodes
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//// BEGIN : Editor Windows and modals

EditorModal::EditorModal(int windowId, std::shared_ptr<EditorFrame> frame)
	: m_modalId(windowId)
	, m_shouldCloseModale(false)
{
	m_modalName = "editorModale_" + std::to_string(m_modalId);
	m_frame = frame;
}

void EditorModal::draw(Project& project, Editor& editor, bool* opened)
{
	ImGui::OpenPopup(m_modalName.data());

	if (ImGui::BeginPopupModal(m_modalName.data(), opened))
	{
		m_frame->drawContent(project, this);

		ImGui::EndPopup();
	}
}

bool EditorModal::shouldCloseModal() const
{
	return m_shouldCloseModale;
}

void EditorModal::closeModal()
{
	ImGui::CloseCurrentPopup();
	m_shouldCloseModale = true;
}

//////////////////

EditorWindow::EditorWindow(int windowId, std::shared_ptr<EditorFrame> frame) 
	: m_windowId(windowId)
	, m_size(200, 300)
	, m_position(0, 0)
	, m_alpha(1)
{
	setNode(std::make_shared<EditorNode>(std::make_shared<EditorNodeUniqueDisplay>(), std::make_shared<EditorNode>(frame)));
	m_windowStrId = "editorWindow_" + std::to_string(m_windowId);
}

EditorWindow::EditorWindow(int windowId, std::shared_ptr<EditorNode> node)
	: m_windowId(windowId)
	, m_size(200, 300)
	, m_position(0, 0)
	, m_alpha(1)
{
	setNode(std::make_shared<EditorNode>(std::make_shared<EditorNodeUniqueDisplay>(), node));
	m_windowStrId = "editorWindow_" + std::to_string(m_windowId);
}

void EditorWindow::draw(Project& project, Editor& editor)
{
	RemovedNodeDatas removeNodeDatas;
	ImVec2 currentWindowSize = m_size;
	ImVec2 currentWindowPosition = m_position;

	////BEGIN DRAW

	//Override style
	float previousAlpha = ImGui::GetStyle().Alpha;
	ImGui::GetStyle().Alpha = m_alpha;

	////TODO : move this code
	if (m_node->getDisplayLogicType() == EditorNodeDisplayLogicType::UniqueDisplay
		&& m_node->getChildCount() == 1 && m_node->getChild(0)->getDisplayLogicType() == EditorNodeDisplayLogicType::FrameDisplay)
	{
		m_windowLabel = m_node->getFrameName();
		//ImGui::Begin(m_windowLabel.data(), nullptr/*, ImGuiWindowFlags_NoScrollbar*/);
	}
	//else
	//	ImGui::Begin( ("##"+m_windowStrId).c_str(), nullptr/*, ImGuiWindowFlags_NoScrollbar*/);

	ImGui::SetNextWindowPos(m_position);
	ImGui::Begin(m_windowStrId.c_str(), nullptr, ImGuiWindowFlags_NoTitleBar);
	ImGui::PushID(m_windowStrId.c_str());

	bool shouldClose = false, shouldMove = false;
	drawHeader(m_windowLabel, shouldClose, shouldMove);
	//launch drag and drop of the window
	if (shouldMove)
		DragAndDropManager::beginDragAndDrop(std::make_shared<EditorFrameDragAndDropOperation>(m_windowId, &editor));
	else 
	{
		currentWindowPosition = ImGui::GetWindowPos();
	}

	ImGui::BeginChild("##childs", ImVec2(0, 0), false);
	ImVec2 sizeOffset(0, 0);
	m_node->drawContent(project, editor, &removeNodeDatas, sizeOffset);
	currentWindowSize = ImVec2(ImGui::GetWindowContentRegionWidth(), ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y);
	ImGui::EndChild();

	ImGui::PopID();
	ImGui::End();

	ImGui::GetStyle().Alpha = previousAlpha;

	////END DRAW

	//We have resized the window
	if (std::abs(currentWindowSize.x - m_size.x) > 0.1f || std::abs(currentWindowSize.y - m_size.y) > 0.1f)
	{
		m_size = currentWindowSize;
		m_node->setWidth(m_size.x);
		m_node->setHeight(m_size.y);
	}
	//we have moved the window
	if (std::abs(currentWindowPosition.x - m_position.x) > 0.1f || std::abs(currentWindowPosition.y - m_position.y) > 0.1f)
	{
		m_position = currentWindowPosition;
	}

	//We want to remove a node
	if (removeNodeDatas.nodeToRemove)
	{
		auto parentNode = removeNodeDatas.nodeToRemove->getParentNode();
		parentNode->removeChild(removeNodeDatas.nodeToRemove.get());
		editor.getWindowManager()->addWindowAsynchrone(removeNodeDatas.nodeToRemove);
		parentNode->onChildRemoved();
		//std::shared_ptr<EditorNode> childNode = removeNodeDatas.parentNode->getChild(removeNodeDatas.index);
		//removeNodeDatas.parentNode->removeChild(removeNodeDatas.index);
		//editor.getWindowManager()->addWindowAsynchrone(childNode);
		//removeNodeDatas.parentNode->onChildRemoved();
	}

	//We want to close the window
	//close window
	if (!shouldMove && shouldClose)
	{
		editor.getWindowManager()->removeWindow(m_windowId);
	}
}

void EditorWindow::setNode(std::shared_ptr<EditorNode> newNode)
{
	m_node = newNode;
	m_node->setWidth(m_size.x);
	m_node->setHeight(m_size.y);
}

std::shared_ptr<EditorNode> EditorWindow::getNode() const
{
	return m_node;
}

void EditorWindow::setAlpha(float alpha)
{
	m_alpha = alpha;
}

float EditorWindow::getAlpha() const
{
	return m_alpha;
}

void EditorWindow::drawHeader(const std::string& title, bool& shouldClose, bool& shouldMove)
{
	ImGui::PushID(title.c_str());
	ImGui::BeginChild("##Header", ImVec2(0, 20), false);
	ImGui::Text(title.c_str());
	float textWidth = ImGui::GetItemRectSize().x;
	ImGui::SameLine();
	ImGui::Dummy(ImVec2(ImGui::GetWindowWidth() - 25 - textWidth, 0));
	ImGui::SameLine();
	shouldClose = ImGui::Button("><##Header", ImVec2(20, 20));
	shouldMove = ImGui::IsMouseHoveringWindow() && ImGui::IsWindowFocused() && ImGui::IsMouseDragging();
	ImGui::EndChild();
	ImGui::PopID();
}

void EditorWindow::move(const ImVec2 & delta)
{
	m_position.x += delta.x;
	m_position.y += delta.y;
}


//// END : Editor Windows and modals
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//// BEGIN : Editor Window manager

void EditorWindowManager::addModal(std::shared_ptr<EditorFrame> windowFrame)
{
	if (!m_freeModalIds.empty())
	{
		int currentId = m_freeModalIds.back();
		m_freeModalIds.pop_back();
		m_editorModals[currentId] = std::make_shared<EditorModal>(currentId, windowFrame);
	}
	else
	{
		int currentId = m_editorModals.size();
		m_editorModals.push_back(std::make_shared<EditorModal>(currentId, windowFrame));
	}
}

void EditorWindowManager::removeModal(EditorModal* modal)
{
	auto found = std::find_if(m_editorModals.begin(), m_editorModals.end(), [modal](const std::shared_ptr<EditorModal>& item) { return item.get() == modal; });
	if (found != m_editorModals.end())
	{
		found->reset();
		m_freeModalIds.push_back((int)std::distance(m_editorModals.begin(), found));
	}
}

void EditorWindowManager::removeModal(int modalId)
{
	m_editorModals[modalId].reset();
	m_freeModalIds.push_back(modalId);
}

void EditorWindowManager::setIsResizingChild(bool state)
{
	m_isResizingChild = state;
}

bool EditorWindowManager::getIsResizingChild() const
{
	return m_isResizingChild;
}

void EditorWindowManager::displayModals(Project& project, Editor& editor)
{
	auto modalIter = m_editorModals.begin();
	while (modalIter != m_editorModals.end())
	{
		auto nextModalIter = modalIter;
		nextModalIter++;

		if (*modalIter)
		{
			(*modalIter)->draw(project, editor);

			//close modal ?
			if ((*modalIter)->shouldCloseModal())
			{
				removeModal(std::distance(m_editorModals.begin(), modalIter));
			}
		}

		modalIter = nextModalIter;
	}
}

void EditorWindowManager::addWindow(std::shared_ptr<EditorFrame> windowFrame)
{
	if (!m_freeWindowIds.empty())
	{
		int currentId = m_freeWindowIds.back();
		m_freeWindowIds.pop_back();
		m_editorWindows[currentId] = std::make_shared<EditorWindow>(currentId, windowFrame);
	}
	else
	{
		int currentId = m_editorWindows.size();
		m_editorWindows.push_back(std::make_shared<EditorWindow>(currentId, windowFrame));
	}
}

void EditorWindowManager::addWindow(std::shared_ptr<EditorNode> windowNode)
{
	if (!m_freeWindowIds.empty())
	{
		int currentId = m_freeWindowIds.back();
		m_freeWindowIds.pop_back();
		m_editorWindows[currentId] = std::make_shared<EditorWindow>(currentId, windowNode);
	}
	else
	{
		int currentId = m_editorWindows.size();
		m_editorWindows.push_back(std::make_shared<EditorWindow>(currentId, windowNode));
	}
}

void EditorWindowManager::addWindowAsynchrone(std::shared_ptr<EditorNode> windowNode)
{
	m_windowsToAdd.push_back(windowNode);
}

void EditorWindowManager::removeWindow(EditorWindow* window)
{
	auto found = std::find_if(m_editorWindows.begin(), m_editorWindows.end(), [window](const std::shared_ptr<EditorWindow>& item) { return item.get() == window; });
	if (found != m_editorWindows.end())
	{
		found->reset();
		m_freeWindowIds.push_back((int)std::distance(m_editorWindows.begin(), found));
	}
}

void EditorWindowManager::removeWindow(int windowId)
{
	m_editorWindows[windowId].reset();
	m_freeWindowIds.push_back(windowId);
}

std::shared_ptr<EditorWindow> EditorWindowManager::getWindow(int windowId) const
{
	assert(windowId >= 0 && windowId < m_editorWindows.size());

	return m_editorWindows[windowId];
}

void EditorWindowManager::displayWindows(Project& project, Editor& editor)
{
	auto& windowIter = m_editorWindows.begin();
	while (windowIter != m_editorWindows.end())
	{
		if (*windowIter != nullptr)
		{
			(*windowIter)->draw(project, editor);
		}

		windowIter++;
	}
}

void EditorWindowManager::update()
{
	for (auto& windowToAdd : m_windowsToAdd)
	{
		addWindow(windowToAdd);
	}
	m_windowsToAdd.clear();

	if(ImGui::IsMouseReleased(GLFW_MOUSE_BUTTON_LEFT))
		setIsResizingChild(false);
}


//// END : Editor Window manager
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//// BEGIN : Editor style sheet

namespace EditorStyleSheet {

	void applyDefaultStyleSheet()
	{
		ImGuiStyle& style = ImGui::GetStyle();

		//Scrollbar
		style.ScrollbarRounding = 0.5f;
		style.ScrollbarSize = 10.0f;
		//Frame
		style.FrameRounding = 0.5f;
		style.FramePadding = ImVec2(1, 1);
		//Grab
		style.GrabMinSize = 3.f;
		style.GrabRounding = 0.5f;
		//Window
		style.WindowPadding = ImVec2(0.0f, 0.0f);
		style.WindowRounding = 0.5f;
		//Spacing
		style.ItemSpacing = ImVec2(0.0f, 0.0f);

		//Colors
		ImColor mainBgColor(0.4f, 0.4f, 0.4f);
		ImColor mainBgTitleColor(0.6f, 0.6f, 0.6f);
		ImColor mainBgChildColor(0.5f, 0.5f, 0.5f);
		ImColor mainBorderColor(0.5f, 0.5f, 0.0f);
		ImColor mainTextColor(1.f, 1.f, 1.f);
		ImColor mainHoveredColor(0.3f, 0.3f, 0.3f);
		ImColor mainActiveColor(0.4f, 0.4f, 0.4f);
		ImColor mainDisabledColor(0.8f, 0.8f, 0.8f);


		style.Colors[ImGuiCol_Text] = mainTextColor;
		style.Colors[ImGuiCol_TextDisabled] = mainDisabledColor;
		style.Colors[ImGuiCol_WindowBg] = mainBgColor;              // Background of normal windows
		style.Colors[ImGuiCol_ChildWindowBg] = mainBgChildColor;         // Background of child windows
		style.Colors[ImGuiCol_PopupBg] = mainBgChildColor;               // Background of popups, menus, tooltips windows
		style.Colors[ImGuiCol_Border] = mainBorderColor;
		//style.Colors[ImGuiCol_BorderShadow] 
		style.Colors[ImGuiCol_FrameBg] = mainBgColor;               // Background of checkbox, radio button, plot, slider, text input
		style.Colors[ImGuiCol_FrameBgHovered] = mainHoveredColor;
		style.Colors[ImGuiCol_FrameBgActive] = mainActiveColor;
		style.Colors[ImGuiCol_TitleBg] = mainBgTitleColor;
		style.Colors[ImGuiCol_TitleBgCollapsed] = mainBgTitleColor;
		style.Colors[ImGuiCol_TitleBgActive] = mainBgTitleColor;
		style.Colors[ImGuiCol_MenuBarBg] = mainBgTitleColor;
		//style.Colors[ImGuiCol_ScrollbarBg],
		//style.Colors[ImGuiCol_ScrollbarGrab],
		//style.Colors[ImGuiCol_ScrollbarGrabHovered],
		//style.Colors[ImGuiCol_ScrollbarGrabActive],
		//style.Colors[ImGuiCol_ComboBg],
		//style.Colors[ImGuiCol_CheckMark],
		//style.Colors[ImGuiCol_SliderGrab],
		//style.Colors[ImGuiCol_SliderGrabActive],
		//style.Colors[ImGuiCol_Button],
		//style.Colors[ImGuiCol_ButtonHovered],
		//style.Colors[ImGuiCol_ButtonActive],
		//style.Colors[ImGuiCol_Header],
		//style.Colors[ImGuiCol_HeaderHovered],
		//style.Colors[ImGuiCol_HeaderActive],
		//style.Colors[ImGuiCol_Column],
		//style.Colors[ImGuiCol_ColumnHovered],
		//style.Colors[ImGuiCol_ColumnActive],
		//style.Colors[ImGuiCol_ResizeGrip],
		//style.Colors[ImGuiCol_ResizeGripHovered],
		//style.Colors[ImGuiCol_ResizeGripActive],
		//style.Colors[ImGuiCol_CloseButton],
		//style.Colors[ImGuiCol_CloseButtonHovered],
		//style.Colors[ImGuiCol_CloseButtonActive],
		//style.Colors[ImGuiCol_PlotLines],
		//style.Colors[ImGuiCol_PlotLinesHovered],
		//style.Colors[ImGuiCol_PlotHistogram],
		//style.Colors[ImGuiCol_PlotHistogramHovered],
		//style.Colors[ImGuiCol_TextSelectedBg],
		//style.Colors[ImGuiCol_ModalWindowDarkening],  // darken entire screen when a modal window is active
	}

	void pushFramePadding()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3.f, 3.f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(3.f, 3.f));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(3.f, 3.f));
	}

	void popFramePadding()
	{
		ImGui::PopStyleVar(3);
	}

}


//// END : Editor style sheet
///////////////////////////////////////////////////////////