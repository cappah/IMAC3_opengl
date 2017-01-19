#include "AnimationStateMachine.h"
#include "imgui/imgui.h"
#include "Project.h"
#include "Editor.h"
#include "EditorFrames.h"


AnimationStateMachine::AnimationStateMachine(const AnimationStateMachine& other)
	: m_entryNodeIdx(other.m_entryNodeIdx)
	, m_currentNodeIdx(other.m_currentNodeIdx)
	, m_boolParameters(other.m_boolParameters)
	, m_floatParameters(other.m_floatParameters)
	, m_triggerParameters(other.m_triggerParameters)
{
	for (auto state : other.m_states)
	{
		m_states.push_back(std::make_shared<AnimationState>(*state));
	}
	for (auto transition : other.m_transitions)
	{
		m_transitions.push_back(std::make_shared<AnimationTransition>(*transition));
	}
}
AnimationStateMachine& AnimationStateMachine::operator=(const AnimationStateMachine& other)
{
	m_entryNodeIdx = other.m_entryNodeIdx;
	m_currentNodeIdx = other.m_currentNodeIdx;
	m_boolParameters = other.m_boolParameters;
	m_floatParameters = other.m_floatParameters;
	m_triggerParameters = other.m_triggerParameters;

	for (auto state : other.m_states)
	{
		m_states.push_back(std::make_shared<AnimationState>(*state));
	}
	for (auto transition : other.m_transitions)
	{
		m_transitions.push_back(std::make_shared<AnimationTransition>(*transition));
	}

	return *this;
}

void AnimationStateMachine::init(const FileHandler::CompletePath & completePath, const ID & id)
{
	Resource::init(completePath, id);

	assert(!Project::isPathPointingInsideProjectFolder(completePath)); //path should be relative
	FileHandler::CompletePath absolutePath = Project::getAbsolutePathFromRelativePath(completePath);

	std::ifstream stream;
	stream.open(absolutePath.toString());
	if (!stream.is_open())
	{
		std::cout << "error, can't load material at path : " << absolutePath.toString() << std::endl;
		return;
	}
	Json::Value root;
	stream >> root;

	load(root);
}

void AnimationStateMachine::resolvePointersLoading()
{
	assert(!Project::isPathPointingInsideProjectFolder(m_completePath)); //path should be relative
	FileHandler::CompletePath absolutePath = Project::getAbsolutePathFromRelativePath(m_completePath);

	std::ifstream stream;
	stream.open(absolutePath.toString());
	if (!stream.is_open())
	{
		std::cout << "error, can't load material at path : " << absolutePath.toString() << std::endl;
		return;
	}
	Json::Value root;
	stream >> root;

	/////////////////

	int idx = 0;
	for (auto state : m_states)
	{
		// States have SkeletalAnimation resource pointers to resolve.
		state->resolvePointerLoading(root["states"][idx]);
		idx++;
	}
}

void AnimationStateMachine::save()
{
	assert(!Project::isPathPointingInsideProjectFolder(m_completePath)); //path should be relative
	FileHandler::CompletePath absolutePath = Project::getAbsolutePathFromRelativePath(m_completePath);

	std::ofstream stream;
	stream.open(absolutePath.toString());
	if (!stream.is_open())
	{
		std::cout << "error, can't load material at path : " << absolutePath.toString() << std::endl;
		return;
	}
	Json::Value root;

	save(root);

	stream << root;
}

void AnimationStateMachine::drawInInspector(Scene & scene)
{
	if (ImGui::Button("Edit"))
	{
		Editor::instance().getWindowManager()->addWindowAsynchrone(std::make_shared<ASMEditorFrame>(this));
	}
}

void AnimationStateMachine::save(Json::Value & entityRoot) const
{
	// Save states :
	entityRoot["stateCount"] = m_states.size();

	int stateIdx = 0;
	for (auto state : m_states)
	{
		state->save(entityRoot["states"][stateIdx]);
		stateIdx++;
	}

	// Save transitions :
	entityRoot["transitionCount"] = m_transitions.size();

	int transitionIdx = 0;
	for (auto transition : m_transitions)
	{
		transition->save(entityRoot["transitions"][transitionIdx]);
		transitionIdx++;
	}

	// Save entry node :
	entityRoot["entryNodeIdx"] = m_entryNodeIdx;

	// Save parameters :
	entityRoot["boolParameterCount"] = m_boolParameters.size();

	int boolParameterIdx = 0;
	for (auto parameter : m_boolParameters)
	{
		entityRoot["boolParameters"][boolParameterIdx]["name"] = parameter.first;
		entityRoot["boolParameters"][boolParameterIdx]["value"] = parameter.second;
		boolParameterIdx++;
	}

	entityRoot["floatParameterCount"] = m_floatParameters.size();

	int floatParameterIdx = 0;
	for (auto parameter : m_floatParameters)
	{
		entityRoot["floatParameters"][floatParameterIdx]["name"] = parameter.first;
		entityRoot["floatParameters"][floatParameterIdx]["value"] = parameter.second;
		floatParameterIdx++;
	}

	entityRoot["triggerParameterCount"] = m_triggerParameters.size();

	int triggerParameterIdx = 0;
	for (auto parameter : m_triggerParameters)
	{
		entityRoot["triggerParameters"][triggerParameterIdx]["name"] = parameter.first;
		entityRoot["triggerParameters"][triggerParameterIdx]["value"] = parameter.second.getValue();
		triggerParameterIdx++;
	}

}

void AnimationStateMachine::load(const Json::Value & entityRoot)
{
	// Load states :
	int stateCount = entityRoot["stateCount"].asInt();
	m_states.clear();
	for (int i = 0; i < stateCount; i++)
	{
		auto newState = std::make_shared<AnimationState>();
		newState->load(entityRoot["states"][i]);
		m_states.push_back(newState);
	}

	// Save transitions :
	int transitionCount = entityRoot["transitionCount"].asInt();
	m_transitions.clear();
	for (int i = 0; i < transitionCount; i++)
	{
		auto newTransition = std::make_shared<AnimationTransition>();
		newTransition->load(entityRoot["transitions"][i]);
		m_transitions.push_back(newTransition);
	}

	// Save entry node :
	m_entryNodeIdx = entityRoot["entryNodeIdx"].asInt();

	// Save parameters :
	int boolParameterCount = entityRoot["boolParameterCount"].asInt();
	m_boolParameters.clear();
	for (int i = 0; i < boolParameterCount; i++)
	{
		std::string parameterName = entityRoot["boolParameters"][i]["name"].asString();
		bool parameterValue = entityRoot["boolParameters"][i]["value"].asBool();

		m_boolParameters[parameterName] = parameterValue;
	}

	int floatParameterCount = entityRoot["floatParameterCount"].asInt();
	m_floatParameters.clear();
	for (int i = 0; i < floatParameterCount; i++)
	{
		std::string parameterName = entityRoot["floatParameters"][i]["name"].asString();
		float parameterValue = entityRoot["floatParameters"][i]["value"].asFloat();

		m_floatParameters[parameterName] = parameterValue;
	}

	int triggerParameterCount = entityRoot["triggerParameterCount"].asInt();
	m_triggerParameters.clear();
	for (int i = 0; i < triggerParameterCount; i++)
	{
		std::string parameterName = entityRoot["triggerParameters"][i]["name"].asString();
		Trigger newTrigger;
		bool triggerValue = entityRoot["triggerParameters"][i]["value"].asBool();
		newTrigger.setValue(triggerValue);

		m_triggerParameters[parameterName] = newTrigger;
	}
}

//////////////
// Setter to parameters
void AnimationStateMachine::setBool(const std::string& parameterName, bool value)
{
	auto foundIt = m_boolParameters.find(parameterName);
	if (foundIt != m_boolParameters.end())
		foundIt->second = value;
}

void AnimationStateMachine::trigger(const std::string& parameterName)
{
	auto foundIt = m_triggerParameters.find(parameterName);
	if (foundIt != m_triggerParameters.end())
		foundIt->second.setValue(true);
}
void AnimationStateMachine::setFloat(const std::string& parameterName, float value)
{
	auto foundIt = m_floatParameters.find(parameterName);
	if (foundIt != m_floatParameters.end())
		foundIt->second = value;
}

// getter to parameters
bool AnimationStateMachine::getBool(const std::string& parameterName, bool& outValue) const
{
	auto foundIt = m_boolParameters.find(parameterName);
	if (foundIt != m_boolParameters.end())
	{
		outValue = foundIt->second;
		return true;
	}
	else
		return false;
}
bool AnimationStateMachine::getFloat(const std::string& parameterName, float& outValue) const
{
	auto foundIt = m_floatParameters.find(parameterName);
	if (foundIt != m_floatParameters.end())
	{
		outValue = foundIt->second;
		return true;
	}
	else
		return false;
}

// Safe way to get animation state from idx
AnimationState* AnimationStateMachine::getState(int index)
{
	if (index >= 0 && index < m_states.size())
		return m_states[index].get();
	else
		return nullptr;
}

// Safe way to get animation transition from idx
AnimationTransition* AnimationStateMachine::getTransition(int index)
{
	if (index >= 0 && index < m_transitions.size())
		return m_transitions[index].get();
	else
		return nullptr;
}

////////////////////////////////
// Update animations
void AnimationStateMachine::updateAnimations()
{
	// No states : Nothing to do
	if (m_states.size() == 0)
		return;

	// Check current node idx validity
	if (m_currentNodeIdx < 0 || m_currentNodeIdx >= m_states.size())
	{
		if (m_entryNodeIdx < 0 || m_entryNodeIdx >= m_states.size())
			m_entryNodeIdx = 0;

		m_currentNodeIdx = m_entryNodeIdx;
	}

	// Transition to next node
	AnimationTransition* transition = m_states[m_currentNodeIdx]->evaluateTransitions(*this);
	AnimationState* currentAnimationState = m_states[m_currentNodeIdx].get();
	if (transition != nullptr)
	{
		int idx = transition->getNextAnimationIdx();
		if (idx >= 0 && idx < m_states.size())
		{
			// Apply transition : 

			// Finish previous animation
			currentAnimationState->finish();
			AnimationState* nextAnimationState = m_states[idx].get();
			assert(nextAnimationState != nullptr);
			// Start next animation
			nextAnimationState->start();
			// Set the idx to current played animation
			m_currentNodeIdx = idx;
		}
	}
}

// For UI :
void AnimationStateMachine::setDragAnchorPos(const glm::vec2& pos)
{
	m_dragAnchorPos = pos;
}

bool AnimationStateMachine::isSelectingNodes() const
{
	return m_isSelectingNodes;
}

void AnimationStateMachine::startSelectingNodes()
{
	m_isSelectingNodes = true;
}

void AnimationStateMachine::stopSelectingNodes()
{
	m_isSelectingNodes = false;
}

bool AnimationStateMachine::getSelectingNodeCount() const
{
	return m_selectedNodeIdx.size();
}

void AnimationStateMachine::selectSingleNode(int idx)
{
	m_selectedNodeIdx.clear();
	m_selectedNodeIdx.push_back(idx);
}

void AnimationStateMachine::selectNode(int idx)
{
	m_selectedNodeIdx.push_back(idx);
}

void AnimationStateMachine::deselectNode(int idx)
{
	m_selectedNodeIdx.erase(std::remove(m_selectedNodeIdx.begin(), m_selectedNodeIdx.end(), idx), m_selectedNodeIdx.end());
}

bool AnimationStateMachine::isNodeSelected(int idx)
{
	return std::find(m_selectedNodeIdx.begin(), m_selectedNodeIdx.end(), idx) != m_selectedNodeIdx.end();
}

void AnimationStateMachine::deselectAllNodes()
{
	m_selectedNodeIdx.clear();
}

bool AnimationStateMachine::isDraggingLink() const
{
	return m_draggedLink != nullptr && m_draggedLink.use_count() > 0;
}

void AnimationStateMachine::startDraggingLink(int currentAnimationIdx)
{
	if (isDraggingLink())
		stopDraggingLink();

	m_draggedLink = std::make_shared<AnimationTransition>(currentAnimationIdx, -1);
}

void AnimationStateMachine::stopDraggingLink()
{
	m_draggedLink.reset();
}

void AnimationStateMachine::makeLinkFromDragginfLink(int nextAnimationStateIdx)
{
	auto newTransition = std::make_shared<AnimationTransition>(m_draggedLink->getCurrentAnimationIdx(), nextAnimationStateIdx);
	m_transitions.push_back(newTransition);
}

void AnimationStateMachine::startDraggingNode(int stateIdx)
{
	m_draggedNodeIdx = stateIdx;
}

void AnimationStateMachine::stopDraggingNode()
{
	m_draggedNodeIdx = -1;
}

bool AnimationStateMachine::isDraggingNode()
{
	return m_draggedNodeIdx >= 0 && m_draggedNodeIdx < m_states.size();
}

void AnimationStateMachine::removeState(int stateIdx)
{
	if (stateIdx >= 0 && stateIdx < m_states.size())
	{
		m_states.erase(m_states.begin() + stateIdx);
	}

	for (int i = 0; i < m_transitions.size(); i++)
	{
		if (m_transitions[i]->getCurrentAnimationIdx() == stateIdx)
			removeTransition(m_transitions[i]->getCurrentAnimationIdx());
		if (m_transitions[i]->getNextAnimationIdx() == stateIdx)
			removeTransition(m_transitions[i]->getNextAnimationIdx());
	}

	m_selectedNodeIdx.clear();
}

void AnimationStateMachine::removeTransition(int transitionIdx)
{
	if (transitionIdx >= 0 && transitionIdx < m_transitions.size())
	{
		m_transitions.erase(m_transitions.begin() + transitionIdx);
	}

	for (int i = 0; i < m_states.size(); i++)
	{
		for (int j = 0; j < m_states[i]->m_transitionIdx.size(); j++)
		{
			if (m_states[i]->m_transitionIdx[i] == transitionIdx)
				m_states[i]->m_transitionIdx.erase(m_states[i]->m_transitionIdx.begin() + transitionIdx);
		}
	}

	m_selectedLinkIdx = -1;
}

void AnimationStateMachine::drawUI()
{
	const float leftWindowWidth = ImGui::GetWindowWidth() * 0.2;
	const float rightWindowWidth = ImGui::GetWindowWidth() * 0.8;
	const float middleWindowWidth = ImGui::GetWindowWidth() * 0.2;

	ImGui::BeginChild("##AnimationParameters", ImVec2(leftWindowWidth, 0));

	ImGui::Combo("##SelectParamType", &m_currentParamType, "trigger/0bool/0float");
	ImGui::SameLine();
	ImGui::Button("add parameter");

	for (auto it = m_boolParameters.begin(); it != m_boolParameters.end();)
	{
		ImGui::PushID(it->first.c_str());
		ImGui::Text(it->first.c_str());
		ImGui::SameLine();
		ImGui::Checkbox("##param", &it->second);
		ImGui::SameLine();
		if (ImGui::Button("remove"))
		{
			it = m_boolParameters.erase(it);
		}
		else
			it++;
		ImGui::PopID();

	}
	for (auto it = m_floatParameters.begin(); it != m_floatParameters.end();)
	{
		ImGui::PushID(it->first.c_str());
		ImGui::Text(it->first.c_str());
		ImGui::SameLine();
		ImGui::InputFloat("##param", &it->second);
		ImGui::SameLine();
		if (ImGui::Button("remove"))
		{
			it = m_floatParameters.erase(it);
		}
		else
			it++;
		ImGui::PopID();
	}
	for (auto it = m_triggerParameters.begin(); it != m_triggerParameters.end();)
	{
		ImGui::PushID(it->first.c_str());
		ImGui::Text(it->first.c_str());
		ImGui::SameLine();
		ImGui::RadioButton("##param", it->second.getValue());
		ImGui::SameLine();
		if (ImGui::Button("remove"))
		{
			it = m_triggerParameters.erase(it);
		}
		else
			it++;
		ImGui::PopID();
	}

	ImGui::EndChild();

	//////////////////////////////////////

	ImGui::SameLine();
	ImGui::BeginChild("##nodeContent", ImVec2(middleWindowWidth, 0));
	ImVec2 middleWindowCursor = ImGui::GetCursorPos();

	// Draw background
	const ImVec2 windowPos = ImGui::GetWindowPos();
	const float lineTickness = 1.f;
	const float spaceBetweenLines = 20;
	float decalX = glm::mod(m_backgroundDecal.x, spaceBetweenLines) - spaceBetweenLines;
	float decalY = glm::mod(m_backgroundDecal.y, spaceBetweenLines) - spaceBetweenLines;
	const float numLinesX = (ImGui::GetWindowWidth() / spaceBetweenLines) + 1;
	const float numLinesY = (ImGui::GetWindowHeight() / spaceBetweenLines) + 1;
	const ImColor lineColor(0.5f, 0.5f, 0.5f, 0.7f);
	for (int i = 0; i < numLinesX + 1; i++)
	{
		decalX += (spaceBetweenLines + lineTickness);

		ImVec2 a(windowPos.x + ImGui::GetWindowContentRegionMin().x + decalX, windowPos.y + ImGui::GetWindowContentRegionMin().y - spaceBetweenLines);
		ImVec2 b(windowPos.x + ImGui::GetWindowContentRegionMin().x + decalX, windowPos.y + ImGui::GetWindowContentRegionMax().y + spaceBetweenLines);
		ImGui::GetWindowDrawList()->AddLine(a, b, lineColor, lineTickness);
	}
	for (int i = 0; i < numLinesY + 1; i++)
	{
		decalY += (spaceBetweenLines + lineTickness);

		ImVec2 a(windowPos.x + ImGui::GetWindowContentRegionMin().x, windowPos.y + ImGui::GetWindowContentRegionMin().y + decalY);
		ImVec2 b(windowPos.x + ImGui::GetWindowContentRegionMax().x, windowPos.y + ImGui::GetWindowContentRegionMin().y + decalY);
		ImGui::GetWindowDrawList()->AddLine(a, b, lineColor, lineTickness);
	}

	// Draw nodes
	m_isHoveringANode = false;
	int nodeIdx = 0;
	for (auto state : m_states)
	{
		state->drawUI(*this, middleWindowCursor, nodeIdx);

		if (state->m_mouseState == AnimationState::HOVERED)
			m_isHoveringANode = true;

		if (!isSelectingNodes() && !isDraggingLink() && !isDraggingNode())
		{
			if (state->m_mouseState == AnimationState::LEFT_SELECTED)
				selectSingleNode(nodeIdx);
			else if (state->m_mouseState == AnimationState::RIGHT_SELECTED)
			{
				selectSingleNode(nodeIdx);
				ImGui::OpenPopup("rightClicOnNodePopUp");
			}
				
			else if (state->m_mouseState == AnimationState::LEFT_CLICKED_DOWN)
			{
				startDraggingNode(nodeIdx);
				setDragAnchorPos(glm::vec2(ImGui::GetMousePos().x - state->m_position.x, ImGui::GetMousePos().y - state->m_position.y));
			}
		}
		else if (isDraggingLink())
		{
			makeLinkFromDragginfLink(nodeIdx);
			stopDraggingLink();
		}
		nodeIdx++;
	}
	if (ImGui::BeginPopup("rightClicOnNodePopUp"))
	{
		assert(m_selectedNodeIdx.size() == 1);

		if (ImGui::Button("Add transition"))
		{
			startDraggingLink(m_selectedNodeIdx[0]);
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	// Draw links
	int linkIdx = 0;
	for (auto transition : m_transitions)
	{
		transition->drawUI(*this, middleWindowCursor);

		if (!isSelectingNodes() && !isDraggingLink() && !isDraggingNode())
		{
			if (transition->m_mouseState == AnimationState::LEFT_SELECTED)
				m_selectedLinkIdx = linkIdx;
			else if (transition->m_mouseState == AnimationState::RIGHT_SELECTED)
			{
				m_selectedLinkIdx = linkIdx;
				ImGui::OpenPopup("rightClicOnLinkPopUp");
			}
		}
		linkIdx++;
	}
	if (ImGui::BeginPopup("rightClicOnLinkPopUp"))
	{
		if (ImGui::Button("Remove transition"))
		{
			removeTransition(m_selectedLinkIdx);
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	// Move all nodes
	if (ImGui::IsMouseHoveringWindow() && ImGui::IsKeyDown(GLFW_KEY_SPACE) && ImGui::IsMouseDown(0) && !isDraggingLink() && !isDraggingNode() && !DragAndDropManager::isDragAndDropping())
	{
		ImVec2 deltaDrag = ImGui::GetMouseDragDelta(0);
		for (auto state : m_states)
		{
			state->m_position.x += deltaDrag.x;
			state->m_position.y += deltaDrag.y;
		}
		m_backgroundDecal.x += deltaDrag.x;
		m_backgroundDecal.y += deltaDrag.y;
		ImGui::ResetMouseDragDelta(0);
	}

	// Select multiple nodes
	else if (ImGui::IsMouseHoveringWindow() && ImGui::IsMouseDown(0) && !isDraggingLink() && !isDraggingNode() && !DragAndDropManager::isDragAndDropping())
	{
		ImVec2 deltaDrag = ImGui::GetMouseDragDelta(0);
		ImVec2 mousePos = ImGui::GetMousePos();
		if (!isSelectingNodes())
		{
			m_dragAnchorPos = glm::vec2(mousePos.x, mousePos.y);
			startSelectingNodes();
		}
		ImVec2 a(std::min(m_dragAnchorPos.x, mousePos.x), std::min(mousePos.y, m_dragAnchorPos.y));
		ImVec2 b(std::max(m_dragAnchorPos.x, mousePos.x), std::max(mousePos.y, m_dragAnchorPos.y));
		ImGui::GetWindowDrawList()->AddRect(a, b, ImColor(1.f, 1.f, 0.f, 1.f));

		int nodeIdx = 0;
		for (auto node : m_states)
		{
			bool selected = false;
			if ((node->m_position.x >= a.x && node->m_position.x <= b.x)
				&& (node->m_position.y >= a.y && node->m_position.y <= b.y))
				selected = true;
			else if ((node->m_position.x + node->m_size.x >= a.x && node->m_position.x + node->m_size.x <= b.x)
				&& (node->m_position.y + node->m_size.y >= a.y && node->m_position.y + node->m_size.y <= b.y))
				selected = true;

			if (selected)
			{
				if (!isNodeSelected(nodeIdx))
				{
					selectNode(nodeIdx);
				}
			}
			else
			{
				if (isNodeSelected(nodeIdx))
					deselectNode(nodeIdx);
			}

			nodeIdx++;
		}
	}

	// Add a new node
	if (ImGui::IsMouseReleased(1) && !m_isHoveringANode && ImGui::IsWindowHovered())
	{
		m_newNodePos = glm::vec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
		ImGui::OpenPopup("rightClicPopUp");
	}
	if (ImGui::BeginPopup("rightClicPopUp"))
	{
		if (ImGui::Button("addAnimationState"))
		{
			std::shared_ptr<AnimationState> newNode = std::make_shared<AnimationState>();
			newNode->m_position = m_newNodePos;
			m_states.push_back(newNode);
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	// Delete nodes
	if (!isSelectingNodes() && ImGui::IsKeyPressed(GLFW_KEY_DELETE) && !isDraggingLink())
	{
		for (int i = 0; i < m_selectedNodeIdx.size(); i++)
		{
			removeState(m_selectedNodeIdx[i]);
			
			stopDraggingLink();
			stopDraggingNode();
		}
		deselectAllNodes();
	}

	// Drag nodes
	if (isDraggingNode())
	{
		ImVec2 deltaDrag = ImGui::GetMouseDragDelta(0);
		//glm::vec2 dragDelta(ImGui::GetMousePos().x - m_dragAnchorPos.x, ImGui::GetMousePos().y - m_dragAnchorPos.y);
		auto draggedNode = getState(m_draggedNodeIdx);
		if (draggedNode != nullptr)
		{
			draggedNode->m_position.x += deltaDrag.x;
			draggedNode->m_position.y += deltaDrag.y;
		}

		bool canDragSelection = false;
		for (auto nodeIdx : m_selectedNodeIdx)
		{
			if (m_draggedNodeIdx == nodeIdx)
				canDragSelection = true;
		}
		if (canDragSelection)
		{
			for (auto nodeIdx : m_selectedNodeIdx)
			{
				if (nodeIdx == m_draggedNodeIdx)
					continue;

				auto node = getState(nodeIdx);
				if (node != nullptr)
				{
					node->m_position.x += deltaDrag.x;
					node->m_position.y += deltaDrag.y;
				}
			}
		}

		ImGui::ResetMouseDragDelta(0);
	}
	// Drag links
	else if (isDraggingLink())
	{
		auto currentAnimation = m_draggedLink->getCurrentAnimation(*this);
		if (currentAnimation != nullptr)
		{
			ImVec2 A(currentAnimation->m_position.x, currentAnimation->m_position.y);
			ImVec2 B = ImGui::GetMousePos();

			ImGui::GetWindowDrawList()->AddLine(A, B, ImColor(1,1,0,1), 1.f);
		}
	}

	if (ImGui::IsMouseReleased(0))
	{
		if (isDraggingNode())
			stopDraggingNode();

		if (isSelectingNodes())
			stopSelectingNodes();
	}

	if (ImGui::IsMouseClickedAnyButton() && isDraggingLink() && !m_isHoveringANode)
	{
		stopDraggingLink();
	}

	if (ImGui::IsMouseHoveringWindow() && ImGui::IsMouseClickedAnyButton() && !m_isHoveringANode)
	{
		stopSelectingNodes();
	}

	ImGui::EndChild();

	//////////////////////////////////////

	ImGui::SameLine();
	ImGui::BeginChild("##AnimationInspector", ImVec2(rightWindowWidth, 0));

	if (m_selectedNodeIdx.size() == 1)
	{
		auto currentState = getState(m_selectedNodeIdx[0]);

		if (currentState != nullptr)
		{
			currentState->drawInInspector();
		}
	}
	else if (m_selectedLinkIdx != -1)
	{
		auto currentLink = getTransition(m_selectedLinkIdx);

		if (currentLink != nullptr)
		{
			currentLink->drawInInspector(*this);
		}
	}

	ImGui::EndChild();

}