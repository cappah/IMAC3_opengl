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


EditorFrameDragAndDropOperation::EditorFrameDragAndDropOperation(int currentDraggedWindowId)
	: DragAndDropOperation(EditorDragAndDropType::EditorFrameDragAndDrop, EditorDropContext::DropIntoEditorWindow)
	, m_currentDraggedWindowId(currentDraggedWindowId)
{
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
	m_currentDraggedWindowId = -1;
}

void EditorFrameDragAndDropOperation::cancelOperation()
{
	m_currentDraggedWindowId = -1;
}

void EditorFrameDragAndDropOperation::updateOperation()
{
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
{
}

EditorNode::EditorNode(std::shared_ptr<EditorNodeDisplayLogic> displayLogic, std::shared_ptr<EditorNode> firstNode)
	: m_displayLogic(displayLogic)
	, m_parent(nullptr)
{
	addChild(firstNode);
}

EditorNode::EditorNode(std::shared_ptr<EditorFrame> frame)
	: m_displayLogic(std::make_shared<EditorNodeFrameDisplay>())
	, m_parent(nullptr)
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

	m_childNodes[index]->m_parent = nullptr;
	m_childNodes.erase(m_childNodes.begin() + index);
}

void EditorNode::addChild(std::shared_ptr<EditorNode> childNode)
{
	childNode->m_parent = this;
	m_childNodes.push_back(childNode);
}

EditorNode* EditorNode::getParentNode() const
{
	return m_parent;
}

std::shared_ptr<EditorNode> EditorNode::getChild(int index) const
{
	assert(index >= 0 && index < m_childNodes.size());

	return m_childNodes[index];
}

void EditorNode::insertChild(std::shared_ptr<EditorNode> childNode, int index)
{
	m_childNodes.insert(m_childNodes.begin() + index, childNode);
}

void EditorNode::drawContent(Project& project, Editor& editor, RemovedNodeDatas* removeNodeDatas)
{
	assert(m_displayLogic);
	m_displayLogic->drawContent(*this, project, editor, removeNodeDatas);
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
int EditorNodeDisplayLogic::drawDropZone(ImVec2 pos, ImVec2 size, bool isActive)
{
	int draggedWindowId = -1;
	bool isHoveringDropZone = false;
	const ImVec2 a = pos;
	const ImVec2 b = ImVec2(pos.x + size.x, pos.y + size.y);

	if (ImGui::IsMouseHoveringRect(a, b)
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


	ImGui::GetWindowDrawList()->AddRectFilled(a, b, ImColor(0, 255, 255, dropZoneAlpha));

	return draggedWindowId;
}

void EditorNodeHorizontalDisplay::drawContent(EditorNode& node, Project& project, Editor& editor, RemovedNodeDatas* removeNodeDatas)
{
	const ImVec2 recMin = ImGui::GetWindowContentRegionMin();
	const ImVec2 recMax = ImGui::GetWindowContentRegionMin();
	const ImVec2 recSize = ImVec2(ImGui::GetWindowContentRegionWidth(), ImGui::GetWindowHeight());
	const int childCount = node.m_childNodes.size();
	const float separatorWidth = 5.f;
	const float padding = 2.f; //small top bottom right and left padding
	const float childWidth = (recSize.x - (childCount + 1)*separatorWidth - padding*2.f) / childCount;
	const float childHeight = recSize.y - padding*2.f;
	const float separatorHeight = childHeight;

	drawSeparator(0, separatorWidth, separatorHeight, node, editor);

	bool removeChildNode = false;
	int removeChildNodeId = -1;

	for (int i = 0; i < node.m_childNodes.size(); i++)
	{
		ImGui::PushID(i);

		ImGui::SameLine();
		ImGui::PushID("Child");
		ImGui::BeginChild(i, ImVec2(childWidth, childHeight), true);

		//We began to drag the node, we have to detach it from the parent window
		if (ImGui::IsWindowHovered() && ImGui::IsMouseDragging()
			&& node.m_childNodes[i]->getDisplayLogicType() == EditorNodeDisplayLogicType::FrameDisplay)
		{
			removeChildNode = true;
			removeChildNodeId = i;
		}
		else
			node.m_childNodes[i]->drawContent(project, editor, removeNodeDatas);

		ImGui::EndChild();
		ImGui::PopID();

		ImGui::SameLine();
		drawSeparator(i + 1, separatorWidth, separatorHeight, node, editor);

		ImGui::PopID();
	}

	if (removeChildNode)
	{
		removeNodeDatas->index = removeChildNodeId;
		removeNodeDatas->parentNode = &node;
	}
}

void EditorNodeHorizontalDisplay::drawSeparator(int index, float width, float height, EditorNode& node, Editor& editor)
{
	//firstseparator
	ImGui::InvisibleButton("##button", ImVec2(width, height));
	int droppedWindowId = drawDropZone(ImGui::GetItemRectMin(), ImGui::GetItemRectSize(), ImGui::IsWindowHovered());
	if (droppedWindowId != -1)
	{
		//we get the dragged node from its parent window id
		std::shared_ptr<EditorNode> droppedNode = editor.getWindowManager()->getWindow(droppedWindowId)->getNode()->getChild(0);

		//assure we insert vertical node in an horizonatl list
		std::shared_ptr<EditorNode> newNode = droppedNode;
		if (droppedNode->getDisplayLogicType() != EditorNodeDisplayLogicType::VerticalDisplay)
			newNode = std::make_shared<EditorNode>(std::make_shared<EditorNodeVerticalDisplay>(), droppedNode);

		//we insert the new node
		node.insertChild(newNode, 0);

		//we delete the dragged window
		editor.getWindowManager()->removeWindow(droppedWindowId);
	}
}

void EditorNodeVerticalDisplay::drawContent(EditorNode& node, Project& project, Editor& editor, RemovedNodeDatas* removeNodeDatas)
{
	const ImVec2 recMin = ImGui::GetWindowContentRegionMin();
	const ImVec2 recMax = ImGui::GetWindowContentRegionMin();
	const ImVec2 recSize = ImVec2(ImGui::GetWindowContentRegionWidth(), ImGui::GetWindowHeight());
	const int childCount = node.m_childNodes.size();
	const float padding = 2.f; //small top bottom right and left padding
	const float separatorHeight = 5.f;
	const float childWidth = recSize.x - padding*2.f;
	const float childHeight = (recSize.y - (childCount + 1)*separatorHeight - padding*2.f) / childCount;
	const float separatorWidth = childWidth;

	drawSeparator(0, separatorWidth, separatorHeight, node, editor);

	bool removeChildNode = false;
	int removeChildNodeId = -1;

	for (int i = 0; i < node.m_childNodes.size(); i++)
	{
		ImGui::PushID(i);

		ImGui::PushID("Child");
		ImGui::BeginChild(i, ImVec2(childWidth, childHeight), true);

		//We began to drag the node, we have to detach it from the parent window
		if (ImGui::IsWindowHovered() && ImGui::IsMouseDragging()
			&& node.m_childNodes[i]->getDisplayLogicType() == EditorNodeDisplayLogicType::FrameDisplay)
		{
			removeChildNode = true;
			removeChildNodeId = i;
		}
		else
			node.m_childNodes[i]->drawContent(project, editor, removeNodeDatas);

		ImGui::EndChild();
		ImGui::PopID();

		drawSeparator(i + 1, separatorWidth, separatorHeight, node, editor);

		ImGui::PopID();
	}

	if (removeChildNode)
	{
		removeNodeDatas->index = removeChildNodeId;
		removeNodeDatas->parentNode = &node;
	}
}

void EditorNodeVerticalDisplay::drawSeparator(int index, float width, float height, EditorNode& node, Editor& editor)
{
	//firstseparator
	ImGui::InvisibleButton("##button", ImVec2(width, height));
	int droppedWindowId = drawDropZone(ImGui::GetItemRectMin(), ImGui::GetItemRectSize(), ImGui::IsWindowHovered());
	if (droppedWindowId != -1)
	{
		//we get the dragged node from its parent window id
		std::shared_ptr<EditorNode> droppedNode = editor.getWindowManager()->getWindow(droppedWindowId)->getNode()->getChild(0);

		//assure we insert horizontal node in a vertical list
		std::shared_ptr<EditorNode> newNode = droppedNode;
		if (droppedNode->getDisplayLogicType() != EditorNodeDisplayLogicType::HorizontalDisplay)
			newNode = std::make_shared<EditorNode>(std::make_shared<EditorNodeHorizontalDisplay>(), droppedNode);

		//we insert the new node
		node.insertChild(newNode, 0);

		//we delete the dragged window
		editor.getWindowManager()->removeWindow(droppedWindowId);
	}
}

void EditorNodeUniqueDisplay::drawContent(EditorNode& node, Project& project, Editor& editor, RemovedNodeDatas* removeNodeDatas)
{
	const ImVec2 recMin = ImGui::GetItemRectMin();
	const ImVec2 recMax = ImGui::GetItemRectMax();
	const ImVec2 recSize = ImGui::GetItemRectSize();
	const int childCount = node.m_childNodes.size();
	const float horizontalSeparatorHeight = 5.f;
	const float verticalSeparatorWidth = 5.f;
	const float padding = 2.f; //small top bottom right and left padding
	const float childWidth = (recSize.x - 2*verticalSeparatorWidth - padding*2.f);
	const float childHeight = (recSize.y - 2*horizontalSeparatorHeight - padding*2.f);
	const float verticalSeparatorHeight = childHeight;
	const float horizontalSeparatorWidth = childHeight;

	assert(childCount == 1);

	ImGui::PushID(this);

	//top separator
	//ImGui::BeginChild("##separator01", ImVec2(horizontalSeparatorWidth, horizontalSeparatorHeight), false);
	ImGui::InvisibleButton("##button01", ImVec2(horizontalSeparatorWidth, horizontalSeparatorHeight));
	//ImGui::EndChild();

	int droppedWindowId = drawDropZone(ImGui::GetItemRectMin(), ImGui::GetItemRectSize(), ImGui::IsWindowHovered());
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

	droppedWindowId = drawDropZone(ImGui::GetItemRectMin(), ImGui::GetItemRectSize(), ImGui::IsWindowHovered());
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
	ImGui::BeginChild("##child", ImVec2(childWidth, childHeight), true);
	node.m_childNodes[0]->drawContent(project, editor, removeNodeDatas);
	ImGui::EndChild();

	//right separator
	ImGui::SameLine();
	//ImGui::BeginChild("##separator03", ImVec2(verticalSeparatorWidth, verticalSeparatorHeight), false);
	ImGui::InvisibleButton("##button03", ImVec2(verticalSeparatorWidth, verticalSeparatorHeight));
	//ImGui::EndChild();

	droppedWindowId = drawDropZone(ImGui::GetItemRectMin(), ImGui::GetItemRectSize(), ImGui::IsWindowHovered());
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

	droppedWindowId = drawDropZone(ImGui::GetItemRectMin(), ImGui::GetItemRectSize(), ImGui::IsWindowHovered());
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
}

void EditorNodeEmptyDisplay::drawContent(EditorNode& node, Project& project, Editor& editor, RemovedNodeDatas* removeNodeDatas)
{
	//TODO
}

void EditorNodeFrameDisplay::drawContent(EditorNode & node, Project & project, Editor& editor, RemovedNodeDatas* removeNodeDatas)
{
	ImGui::PushID(this);
	node.getFrame()->drawContent(project, nullptr);
	ImGui::PopID();
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
{
	m_node = std::make_shared<EditorNode>(std::make_shared<EditorNodeUniqueDisplay>(), std::make_shared<EditorNode>(frame));
	m_windowStrId = "editorWindow_" + std::to_string(m_windowId);
}

EditorWindow::EditorWindow(int windowId, std::shared_ptr<EditorNode> node)
	: m_windowId(windowId)
{
	m_node = std::make_shared<EditorNode>(std::make_shared<EditorNodeUniqueDisplay>(), node);
	m_windowStrId = "editorWindow_" + std::to_string(m_windowId);
}

void EditorWindow::draw(Project& project, Editor& editor)
{
	RemovedNodeDatas removeNodeDatas;

	////TODO : move this code
	if (m_node->getDisplayLogicType() == EditorNodeDisplayLogicType::UniqueDisplay
		&& m_node->getChildCount() == 1 && m_node->getChild(0)->getDisplayLogicType() == EditorNodeDisplayLogicType::FrameDisplay)
	{
		m_windowLabel = m_node->getFrameName();
		ImGui::Begin(m_windowLabel.data(), nullptr);
	}
	else
		ImGui::Begin( ("##"+m_windowStrId).c_str(), nullptr);

	//launch drag and drop of the window
	if (ImGui::IsWindowHovered() && ImGui::IsMouseDragging())
	{
		DragAndDropManager::beginDragAndDrop(std::make_shared<EditorFrameDragAndDropOperation>(m_windowId));
	}

	ImGui::PushID(m_windowStrId.c_str());

	m_node->drawContent(project, editor, &removeNodeDatas);

	ImGui::PopID();
	ImGui::End();

	//we want to remove a node
	if (removeNodeDatas.parentNode != nullptr && removeNodeDatas.index != -1)
	{
		std::shared_ptr<EditorNode> childNode = removeNodeDatas.parentNode->getChild(removeNodeDatas.index);
		removeNodeDatas.parentNode->removeChild(removeNodeDatas.index);
		editor.getWindowManager()->addWindowAsynchrone(childNode);
		removeNodeDatas.parentNode->onChildRemoved();
	}

}

void EditorWindow::setNode(std::shared_ptr<EditorNode> newNode)
{
	m_node = newNode;
}

std::shared_ptr<EditorNode> EditorWindow::getNode() const
{
	return m_node;
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
}


//// END : Editor Window manager
///////////////////////////////////////////////////////////