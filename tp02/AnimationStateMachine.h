#pragma once

#include <vector>
#include <functional>
#include <memory>
#include <unordered_map>
#include "glm/common.hpp"
#include "Resource.h"
#include "ResourcePointer.h"
#include "SkeletalAnimation.h"
#include "EditorGUI.h"
#include "ISerializable.h"

class AnimationState;
class AnimationTransition;
class Trigger;

class AnimationStateMachine final : public Resource, public ISerializable
{
private:
	int m_entryNodeIdx;
	int m_currentNodeIdx;
	// Warning : order matters !
	std::vector<std::shared_ptr<AnimationState>> m_states;
	// Warning : order matters !
	std::vector<std::shared_ptr<AnimationTransition>> m_transitions;
	std::unordered_map<std::string, bool> m_boolParameters;
	std::unordered_map<std::string, float> m_floatParameters;
	std::unordered_map<std::string, Trigger> m_triggerParameters;

	//For UI :
	glm::vec2 m_backgroundDecal;
	std::vector<int> m_selectedNodeIdx;
	glm::vec2 m_dragAnchorPos;
	glm::vec2 m_newNodePos;
	std::shared_ptr<AnimationTransition> m_draggedLink;
	int m_draggedNodeIdx;
	bool m_isSelectingNodes;
	int m_currentParamType;
	int m_selectedLinkIdx;
	bool m_isHoveringANode;

public:

	//Deep copy
	AnimationStateMachine(const AnimationStateMachine& other);
	AnimationStateMachine& operator=(const AnimationStateMachine& other);

	AnimationStateMachine()
		: m_entryNodeIdx(-1)
		,m_currentNodeIdx(-1)
	{}

	AnimationStateMachine(const FileHandler::CompletePath& completePath, bool isDefaultResource = false)
		: Resource(completePath, isDefaultResource)
		, m_entryNodeIdx(-1)
		, m_currentNodeIdx(-1)
	{}

	/////////////
	// Inherit from Resource
	void init(const FileHandler::CompletePath& completePath, const ID& id) override;
	void resolvePointersLoading() override;
	void save() override;

	virtual void drawInInspector(Scene & scene) override;

	/////////////
	// Inherit from ISerializable
	void save(Json::Value& entityRoot) const override;
	void load(const Json::Value& entityRoot) override;

	//////////////
	// Setter to parameters
	void setBool(const std::string& parameterName, bool value);
	void trigger(const std::string& parameterName);
	void setFloat(const std::string& parameterName, float value);
	// getter to parameters
	bool getBool(const std::string& parameterName, bool& outValue) const;
	bool getFloat(const std::string& parameterName, float& outValue) const;

	// Safe way to get animation state from idx
	AnimationState* getState(int index);

	// Safe way to get animation transition from idx
	AnimationTransition* getTransition(int index);

	////////////////////////////////
	// Update animations
	void updateAnimations();

	// For UI :
	void setDragAnchorPos(const glm::vec2& pos);
	bool isSelectingNodes() const;
	void startSelectingNodes();
	void stopSelectingNodes();
	bool getSelectingNodeCount() const;
	void selectSingleNode(int idx);
	void selectNode(int idx);
	void deselectNode(int idx);
	bool isNodeSelected(int idx);
	void deselectAllNodes();
	bool isDraggingLink() const;
	void startDraggingLink(int currentAnimationIdx);
	void stopDraggingLink();
	void makeLinkFromDragginfLink(int nextAnimationStateIdx);
	void startDraggingNode(int stateIdx);
	void stopDraggingNode();
	bool isDraggingNode();
	void removeState(int stateIdx);
	void removeTransition(int transitionIdx);
	void drawUI();
	template<typename T>
	bool drawParametersPopup(std::string& outName) { assert(false && "wrong type."); }
	template<typename T>
	bool getParameterValue(const std::string& parameterName, T* outParameterValue) { assert(false && "wrong type."); }

};

template<>
bool AnimationStateMachine::drawParametersPopup<bool>(std::string& outName)
{
	for (auto parameter : m_boolParameters)
	{
		if (ImGui::Button(parameter.first.c_str()))
		{
			outName = parameter.first;
			return true;
		}
	}
	return false;
}
template<>
bool AnimationStateMachine::drawParametersPopup<float>(std::string& outName)
{
	for (auto parameter : m_floatParameters)
	{
		if (ImGui::Button(parameter.first.c_str()))
		{
			outName = parameter.first;
			return true;
		}
	}
	return false;
}
template<>
bool AnimationStateMachine::drawParametersPopup<Trigger>(std::string& outName)
{
	for (auto parameter : m_triggerParameters)
	{
		if (ImGui::Button(parameter.first.c_str()))
		{
			outName = parameter.first;
			return true;
		}
	}
	return false;
}

template<>
bool AnimationStateMachine::getParameterValue<bool>(const std::string& parameterName, bool* outParameterValue)
{
	auto foundIt = m_boolParameters.find(parameterName);
	if (foundIt != m_boolParameters.end())
	{
		outParameterValue = &foundIt->second;
		return true;
	}
	return false;
}
template<>
bool AnimationStateMachine::getParameterValue<float>(const std::string& parameterName, float* outParameterValue)
{
	auto foundIt = m_floatParameters.find(parameterName);
	if (foundIt != m_floatParameters.end())
	{
		outParameterValue = &foundIt->second;
		return true;
	}
	return false;
}
template<>
bool AnimationStateMachine::getParameterValue<Trigger>(const std::string& parameterName, Trigger* outParameterValue)
{
	auto foundIt = m_triggerParameters.find(parameterName);
	if (foundIt != m_triggerParameters.end())
	{
		outParameterValue = &foundIt->second;
		return true;
	}
	return false;
}


class AnimationState : public ISerializable
{
	friend AnimationStateMachine;

	// For UI :
	enum MouseState {NONE, HOVERED, RIGHT_CLICKED_DOWN, RIGHT_CLICKED_UP, LEFT_CLICKED_DOWN, LEFT_CLICKED_UP, RIGHT_SELECTED, LEFT_SELECTED};

private:
	ResourcePtr<SkeletalAnimation> m_animation;
	std::shared_ptr<SkeletalAnimation> m_localAnimation;
	float m_animationTime;
	bool m_loop;
	std::vector<int> m_transitionIdx;

	// For UI :
	glm::vec2 m_position;
	glm::vec2 m_size;
	MouseState m_mouseState;

public:
	AnimationState()
		: AnimationState(nullptr, false)
	{}

	AnimationState(SkeletalAnimation* animation, bool loop = false)
		: m_animation(animation)
		, m_loop(loop)
		, m_animationTime(0)
	{}

	AnimationState(const AnimationState& other)
		: m_animation(other.m_animation)
		, m_animationTime(other.m_animationTime)
		, m_loop(other.m_loop)
	{
		m_transitionIdx = other.m_transitionIdx;

		if (m_animation.isValid())
			m_localAnimation = std::make_shared<SkeletalAnimation>(*m_animation);
	}

	AnimationState& operator=(const AnimationState& other)
	{
		m_animation = other.m_animation;
		m_animationTime = other.m_animationTime;
		m_loop = other.m_loop;

		m_transitionIdx = other.m_transitionIdx;

		if (m_animation.isValid())
			m_localAnimation = std::make_shared<SkeletalAnimation>(*m_animation);

		return *this;
	}

	// We consider that an animation is finished if we are at the end of the animation (if it remains timeLeft second to finish the animation)
	bool isFinished(float timeLeft = 0.5)
	{
		if (m_localAnimation != nullptr)
		{
			return m_localAnimation->getRemainingTime() <= timeLeft;
		}
		return false;
	}

	AnimationTransition* evaluateTransitions(AnimationStateMachine& manager)
	{
		for (auto transitionIdx : m_transitionIdx)
		{
			AnimationTransition* transition = manager.getTransition(transitionIdx);
			if (transition != nullptr)
			{
				if (transition->evaluate(manager))
					return transition;
			}
		}
		return nullptr;
	}

	// Call when we just have transitionning to this animation state
	void start()
	{
		if(m_localAnimation != nullptr)
			m_localAnimation->play();
	}

	// Call before transitionning to an other animation
	void finish()
	{
		if (m_localAnimation != nullptr)
			m_localAnimation->stop();
	}

	// Inherited from ISerializable
	virtual void save(Json::Value & entityRoot) const override
	{
		m_animation.save(entityRoot["animation"]);

		entityRoot["loop"] = m_loop;

		entityRoot["transitionCount"] = m_transitionIdx.size();
		int transitionIdx = 0;
		for (auto transition : m_transitionIdx)
		{
			entityRoot["transitionIdx"][transitionIdx] = transition;
			transitionIdx++;
		}

		// For UI :
		entityRoot["position"][0] = m_position.x;
		entityRoot["position"][1] = m_position.y;
		entityRoot["size"][0] = m_size.x;
		entityRoot["size"][1] = m_size.y;
	}

	virtual void load(const Json::Value & entityRoot) override
	{
		//Load animation in resolvePointerLoading()

		m_loop = entityRoot["loop"].asBool();

		int transitionCount = entityRoot["transitionCount"].asInt();
		m_transitionIdx.clear();
		for (int i = 0; i < transitionCount; i++)
		{
			m_transitionIdx.push_back(entityRoot["transitionIdx"][i].asInt());
		}

		// For UI :
		m_position.x = entityRoot["position"][0].asFloat();
		m_position.y = entityRoot["position"][1].asFloat();
		m_size.x = entityRoot["size"][0].asFloat();
		m_size.y = entityRoot["size"][1].asFloat();
	}

	void resolvePointerLoading(const Json::Value & entityRoot)
	{
		m_animation.load(entityRoot["animation"]);
		if (m_animation.isValid())
			m_localAnimation = std::make_shared<SkeletalAnimation>(*m_animation);
	}

	// For UI :
	void drawUI(AnimationStateMachine& manager, const ImVec2& windowCursor, int stateIdx)
	{

		ImGui::PushID(this);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImColor(0.4, 0.4, 0.4, 1));

		ImVec2 recMin(windowCursor.x + m_position.x, windowCursor.y + m_position.y);
		ImVec2 recMax(windowCursor.x + m_position.x + m_size.x, windowCursor.y + m_position.y + m_size.y);

		ImGui::SetCursorScreenPos(recMin);
		const bool showBorders = (manager.isNodeSelected(stateIdx));

		ImGui::BeginChild("##Node", ImVec2(m_size.x, m_size.y), showBorders);

		if (ImGui::IsMouseHoveringWindow() && ImGui::IsMouseHoveringRect(recMin, recMax))
		{
			m_mouseState = HOVERED;

			if (ImGui::IsMouseClicked(0))
				m_mouseState = LEFT_CLICKED_DOWN;
			else if (ImGui::IsMouseReleased(0))
			{
				if (m_mouseState == LEFT_CLICKED_DOWN)
					m_mouseState = LEFT_SELECTED;
				else
					m_mouseState = LEFT_CLICKED_UP;
			}
			else if (ImGui::IsMouseClicked(0))
				m_mouseState = RIGHT_CLICKED_DOWN;
			else if (ImGui::IsMouseReleased(0))
			{
				if (m_mouseState == RIGHT_CLICKED_DOWN)
					m_mouseState = RIGHT_SELECTED;
				else
					m_mouseState = RIGHT_CLICKED_UP;
			}
		}
		else
			m_mouseState = NONE;

		ImGui::EndChild();
		ImGui::SetCursorScreenPos(windowCursor);

		ImGui::PopStyleColor();
		ImGui::PopID();
	}

	glm::vec2 getCenter() const
	{
		return glm::vec2(m_position.x + m_size.x * 0.5f, m_position.y + m_size.y * 0.5f);
	}

	void drawInInspector()
	{
		if (EditorGUI::ResourceField<SkeletalAnimation>("animation", m_animation))
		{
			if(m_animation.isValid())
				m_localAnimation = std::make_shared<SkeletalAnimation>(m_animation);
		}
	}
};


class AnimationTransition : public ISerializable
{
	friend AnimationStateMachine;

	enum TransitionType{IMMEDIATE, WAIT_END};

	// For UI :
	enum MouseState { NONE, HOVERED, RIGHT_CLICKED_DOWN, RIGHT_CLICKED_UP, LEFT_CLICKED_DOWN, LEFT_CLICKED_UP, RIGHT_SELECTED, LEFT_SELECTED };

private:
	std::vector<std::shared_ptr<ICondition>> m_conditions;
	TransitionType m_transitionType;
	int m_currentAnimationStateIdx;
	int m_nextAnimationStateIdx;

	// For UI :
	MouseState m_mouseState;
	int m_currentConditionType;

public:
	AnimationTransition(int currentAnimationStateIdx = -1, int nextAnimationStateIdx = -1)
		: m_currentAnimationStateIdx(currentAnimationStateIdx)
		, m_nextAnimationStateIdx(nextAnimationStateIdx)
		, m_currentConditionType(0)
	{

	}

	bool evaluate(AnimationStateMachine& manager)
	{
		AnimationState* currentAnimation = manager.getState(m_currentAnimationStateIdx);
		assert(currentAnimation != nullptr);
		bool passTest = false;
		for (auto condition : m_conditions)
		{
			passTest &= condition->evaluate(manager);
			passTest &= (m_transitionType == AnimationTransition::TransitionType::IMMEDIATE || (AnimationTransition::TransitionType::IMMEDIATE && currentAnimation->isFinished()));
		}
		return passTest;
	}

	AnimationState* getCurrentAnimation(AnimationStateMachine& manager)
	{
		return manager.getState(m_currentAnimationStateIdx);
	}

	AnimationState* getNextAnimation(AnimationStateMachine& manager)
	{
		return manager.getState(m_nextAnimationStateIdx);
	}

	int getCurrentAnimationIdx()
	{
		return m_currentAnimationStateIdx;
	}

	int getNextAnimationIdx()
	{
		return m_nextAnimationStateIdx;
	}

	// Inherited from ISerializable
	virtual void save(Json::Value & entityRoot) const override
	{
		entityRoot["conditionCount"] = m_conditions.size();
		int conditionIdx = 0;
		for (auto condition : m_conditions)
		{
			entityRoot["conditionType"] = (int)condition->getConditionType();
			condition->save(entityRoot["conditions"][conditionIdx]);
			conditionIdx++;
		}

		entityRoot["transitionType"] = (int)m_transitionType;

		entityRoot["currentAnimationStateIdx"] = m_currentAnimationStateIdx;
		entityRoot["nextAnimationStateIdx"] = m_nextAnimationStateIdx;
	}

	virtual void load(const Json::Value & entityRoot) override
	{
		int conditionCount = entityRoot["conditionCount"].asInt();
		m_conditions.clear();
		for (int i = 0; i < conditionCount; i++)
		{
			std::shared_ptr<ICondition> newCondition;
			ICondition::ConditionType conditionType = (ICondition::ConditionType)entityRoot["conditionType"].asInt();
			switch (conditionType)
			{
			case ICondition::NONE:
				assert(false && "you are loading a animation transition condition with an invalid type.");
				break;
			case ICondition::BOOL:
				newCondition = std::make_shared<Condition<bool>>();
				break;
			case ICondition::FLOAT:
				newCondition = std::make_shared<Condition<float>>();
				break;
			case ICondition::TRIGGER:
				newCondition = std::make_shared<Condition<Trigger>>();
				break;
			default:
				break;
			}
			assert(newCondition);

			newCondition->load(entityRoot["conditions"][i]);
		}

		m_transitionType = (TransitionType)entityRoot["transitionType"].asInt();

		m_currentAnimationStateIdx = entityRoot["currentAnimationStateIdx"].asInt();
		m_nextAnimationStateIdx = entityRoot["nextAnimationStateIdx"].asInt();
	}

	// For UI :
	void drawUI(AnimationStateMachine& manager, const ImVec2& windowCursor)
	{
		AnimationState* currentAnimation = manager.getState(m_currentAnimationStateIdx);
		AnimationState* nextAnimation = manager.getState(m_nextAnimationStateIdx);
		assert(currentAnimation != nullptr && nextAnimation != nullptr);
		const glm::vec2 A = windowCursor.x + currentAnimation->getCenter();
		const glm::vec2 B = windowCursor.y + nextAnimation->getCenter();
		ImGui::GetWindowDrawList()->AddLine(ImVec2(B.x, B.y), ImVec2(A.x, A.y), ImColor(1, 1, 0, 1), 1.f);
		const glm::vec2 AB = (B - A);
		const glm::vec2 normal = glm::normalize(glm::vec2(-AB.y, AB.x));
		const glm::vec2 mousePos(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
		const float projectionToAB = glm::dot((mousePos - A), AB);
		const bool projectedInsideAB = projectionToAB >= 0 && projectionToAB <= glm::length(AB);
		const float distanceToAB = glm::dot((mousePos - A), normal);
		if (projectedInsideAB && distanceToAB < 0.1f)
		{
			m_mouseState = HOVERED;

			if (ImGui::IsMouseClicked(0))
				m_mouseState = LEFT_CLICKED_DOWN;
			else if (ImGui::IsMouseReleased(0))
			{
				if (m_mouseState == LEFT_CLICKED_DOWN)
					m_mouseState = LEFT_SELECTED;
				else
					m_mouseState = LEFT_CLICKED_UP;
			}
			else if (ImGui::IsMouseClicked(0))
				m_mouseState = RIGHT_CLICKED_DOWN;
			else if (ImGui::IsMouseReleased(0))
			{
				if (m_mouseState == RIGHT_CLICKED_DOWN)
					m_mouseState = RIGHT_SELECTED;
				else
					m_mouseState = RIGHT_CLICKED_UP;
			}
		}
		else
			m_mouseState = NONE;
	}

	void drawInInspector(AnimationStateMachine& manager)
	{
		ImGui::Combo("##conditionType", &m_currentConditionType, "Bool/0Float/0Trigger");
		ImGui::SameLine();
		if (ImGui::Button("Add condition"))
		{
			switch (m_currentConditionType)
			{
			case 0:
				m_conditions.push_back(std::shared_ptr<Condition<bool>>());
				break;
			case 1:
				m_conditions.push_back(std::shared_ptr<Condition<float>>());
				break;
			case 2:
				m_conditions.push_back(std::shared_ptr<Condition<Trigger>>());
				break;
			default:
				break;
			}
		}
		for (auto it = m_conditions.begin(); it != m_conditions.end();)
		{
			ImGui::PushID(&(*it));
			(*it)->drawUI(manager);
			ImGui::SameLine();
			if (ImGui::Button("remove"))
			{
				it = m_conditions.erase(it);
			}
			else
				it++;
			ImGui::PopID();
		}
	}
};

//////////////////////////////////////////////////////////////////////////////////////
//// BEGIN : Parameters and conditions

class Trigger
{
private:
	bool m_value;
public:
	void setValue(bool value) { m_value = value; };
	bool getValue() const { return m_value; }
};

class ICondition : public ISerializable
{
public:
	enum ConditionType{ NONE, BOOL, FLOAT, TRIGGER };

protected : 
	ConditionType m_conditionType;

public:
	ICondition(ConditionType type) 
		: m_conditionType(type) 
	{}
	ConditionType getConditionType() { return m_conditionType; }
	virtual bool evaluate(AnimationStateMachine& manager) const = 0;
	virtual void use(AnimationStateMachine& manager) {}

	// For UI :
	virtual void drawUI(AnimationStateMachine& manager) = 0;
};

template<typename T>
class Condition final : public ICondition
{
protected:
	std::function<bool(T, T)> m_operator;
	std::string m_parameterName;
	T m_value;

	// For UI :
	int m_currentOperatorIdx;

public:
	Condition()
		: ICondition(ConditionType::NONE)
	{}

	bool evaluate(AnimationStateMachine& manager) const override
	{
		T* parameter;
		if (manager.getParameterValue<T>(m_parameterName, parameter))
		{
			return m_operator && m_operator(*parameter, m_value);
		}
		else
			return false;
	}

	std::function<bool(T,T)> getOperatorFromIdx()
	{
		assert(false && "wrong type");
	}

	// Inherited fromISerializable
	virtual void save(Json::Value & entityRoot) const override
	{
		assert(false && "wrong type.");
	}

	virtual void load(const Json::Value & entityRoot) override
	{
		assert(false && "wrong type.");
	}

	// For UI :
	void drawUI(AnimationStateMachine& manager) override
	{
		assert(false && "wrong type.");
	}
};

template<>
Condition<bool>::Condition()
	: ICondition(ConditionType::BOOL)
{}

template<>
Condition<float>::Condition()
	: ICondition(ConditionType::FLOAT)
{}

template<>
std::function<bool(bool, bool)> Condition<bool>::getOperatorFromIdx()
{
	if (m_currentOperatorIdx == 0)
	{
		return [](bool a, bool b) { return a == b; };
	}
	else if (m_currentOperatorIdx == 1)
	{
		return [](bool a, bool b) { return a != b; };
	}
	else
	{
		return [](bool a, bool b) { return a == b; };
	}
}

template<>
std::function<bool(float, float)> Condition<float>::getOperatorFromIdx()
{
	if (m_currentOperatorIdx == 0)
	{
		return [](float a, float b) { return a == b; };
	}
	else if (m_currentOperatorIdx == 1)
	{
		return [](float a, float b) { return a != b; };
	}
	else if (m_currentOperatorIdx == 2)
	{
		return [](float a, float b) { return a > b; };
	}
	else if (m_currentOperatorIdx == 3)
	{
		return [](float a, float b) { return a < b; };
	}
	else
	{
		return [](float a, float b) { return a == b; };
	}
}

template<>
void Condition<bool>::save(Json::Value & entityRoot) const
{
	entityRoot["parameterName"] = m_parameterName;
	entityRoot["value"] = m_value;
	entityRoot["operatorIdx"] = m_currentOperatorIdx;
}

template<>
void Condition<float>::save(Json::Value & entityRoot) const
{
	entityRoot["parameterName"] = m_parameterName;
	entityRoot["value"] = m_value;
	entityRoot["operatorIdx"] = m_currentOperatorIdx;
}

template<>
void Condition<bool>::load(const Json::Value & entityRoot)
{
	m_parameterName = entityRoot["parameterName"].asString();
	m_value = entityRoot["value"].asBool();
	m_currentOperatorIdx = entityRoot["operatorIdx"].asInt();

	m_operator = getOperatorFromIdx();
}

template<>
void Condition<float>::load(const Json::Value & entityRoot)
{
	m_parameterName = entityRoot["parameterName"].asString();
	m_value = entityRoot["value"].asFloat();
	m_currentOperatorIdx = entityRoot["operatorIdx"].asInt();

	m_operator = getOperatorFromIdx();
}

template<>
void Condition<bool>::drawUI(AnimationStateMachine& manager)
{
	// parameter name
	ImVec2 popupPos;
	if (ImGui::Button(m_parameterName.c_str()))
	{
		popupPos = ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y + ImGui::GetItemRectSize().y);
		ImGui::OpenPopup("chooseParameterPopup");
	}
	if (ImGui::BeginPopup("chooseParameterPopup"))
	{
		std::string tmpParameterName;
		if (manager.drawParametersPopup<bool>(tmpParameterName))
		{
			m_parameterName = tmpParameterName;
		}
		ImGui::EndPopup();
	}
	ImGui::SameLine();
	// operator
	if (ImGui::Combo("##operator", &m_currentOperatorIdx, "equal/0different"))
	{
		m_operator = getOperatorFromIdx();
	}
	ImGui::SameLine();
	// Value
	ImGui::Checkbox("##value", &m_value);
}

template<>
void Condition<float>::drawUI(AnimationStateMachine& manager)
{
	// parameter name
	ImVec2 popupPos;
	if (ImGui::Button(m_parameterName.c_str()))
	{
		popupPos = ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y + ImGui::GetItemRectSize().y);
		ImGui::OpenPopup("chooseParameterPopup");
	}
	if (ImGui::BeginPopup("chooseParameterPopup"))
	{
		std::string tmpParameterName;
		if (manager.drawParametersPopup<float>(tmpParameterName))
		{
			m_parameterName = tmpParameterName;
		}
		ImGui::EndPopup();
	}
	ImGui::SameLine();
	// operator
	if (ImGui::Combo("##operator", &m_currentOperatorIdx, "equal/0different/0greater/0less"))
	{
		m_operator = getOperatorFromIdx();
	}
	ImGui::SameLine();
	// Value
	ImGui::InputFloat("##value", &m_value);
}


template<>
class Condition<Trigger> final : public ICondition, public ISerializable
{
protected:
	std::string m_parameterName;

public:

	Condition()
		: ICondition(ConditionType::TRIGGER)
	{}

	bool evaluate(AnimationStateMachine& manager) const override
	{
		Trigger* parameter;
		if (manager.getParameterValue(m_parameterName, parameter))
			return parameter->getValue();
		else
			return false;
	}

	void use(AnimationStateMachine& manager) override
	{
		Trigger* parameter;
		if (manager.getParameterValue(m_parameterName, parameter))
			return parameter->setValue(false);
	}

	void drawUI(AnimationStateMachine& manager)
	{
		// parameter name
		ImVec2 popupPos;
		if (ImGui::Button(m_parameterName.c_str()))
		{
			popupPos = ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y + ImGui::GetItemRectSize().y);
			ImGui::OpenPopup("chooseParameterPopup");
		}
		if (ImGui::BeginPopup("chooseParameterPopup"))
		{
			std::string tmpParameterName;
			if (manager.drawParametersPopup<float>(tmpParameterName))
			{
				m_parameterName = tmpParameterName;
			}
			ImGui::EndPopup();
		}
	}

	// Hérité via ISerializable
	virtual void save(Json::Value & entityRoot) const override
	{
		entityRoot["parameterName"] = m_parameterName;
	}

	virtual void load(const Json::Value & entityRoot) override
	{
		m_parameterName = entityRoot["parameterName"].asString();
	}

};


//// END : Parameters and conditions
//////////////////////////////////////////////////////////////////////////////////////