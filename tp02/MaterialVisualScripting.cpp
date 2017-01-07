#include "MaterialVisualScripting.h"
#include "ShaderProgram.h"

namespace MVS {


void formatAndOutputResult(std::stringstream& stream, const Output& output, FlowType desiredType, CompilationErrorCheck& errorCheck)
{
	if (desiredType == output.outputType)
	{
		stream << output.valueStr;
	}
	else if (desiredType == FlowType::FLOAT)
	{
		if (output.outputType == FlowType::FLOAT2)
		{
			stream << "vec2(" << output.valueStr << ',' << output.valueStr << ')';
		}
		else if (output.outputType == FlowType::FLOAT3)
		{
			stream << "vec3(" << output.valueStr << ',' << output.valueStr << ',' << output.valueStr << ')';
		}
		else if (output.outputType == FlowType::FLOAT3)
		{
			stream << "vec3(" << output.valueStr << ',' << output.valueStr << ',' << output.valueStr << ',' << output.valueStr << ')';
		}
	}
	else if (desiredType == FlowType::FLOAT2)
	{
		if (output.outputType == FlowType::FLOAT)
		{
			stream << output.valueStr << '.r';
		}
		else
		{
			errorCheck.errorMsg = "incompatible types.";
			errorCheck.compilationFailed = true;
			assert(false && "incompatible types.");
		}
	}
	else if (desiredType == FlowType::FLOAT3)
	{
		if (output.outputType == FlowType::FLOAT)
		{
			stream << output.valueStr << '.r';
		}
		else if (output.outputType == FlowType::FLOAT2)
		{
			stream << output.valueStr << '.rg';
		}
		else
		{
			errorCheck.errorMsg = "incompatible types.";
			errorCheck.compilationFailed = true;
			assert(false && "incompatible types.");
		}
	}
	else if (desiredType == FlowType::FLOAT4)
	{
		if (output.outputType == FlowType::FLOAT)
		{
			stream << output.valueStr << '.r';
		}
		else if (output.outputType == FlowType::FLOAT2)
		{
			stream << output.valueStr << '.rg';
		}
		else if (output.outputType == FlowType::FLOAT3)
		{
			stream << output.valueStr << '.rgb';
		}
		else
		{
			errorCheck.errorMsg = "incompatible types.";
			errorCheck.compilationFailed = true;
			assert(false && "incompatible types.");
		}
	}
}


///////////////////////////////////////////
//// BEGIN : Input

Input::Input(Node* _parentNode, const std::string& _name, FlowType _desiredType)
	: parentNode(_parentNode)
	, name(_name)
	, desiredType(_desiredType)
	, link(nullptr)
{}

void Input::resolveUndeterminedTypes(FlowType _desiredType)
{
	assert(_desiredType != FlowType::UNDEFINED);
	if (desiredType == FlowType::UNDEFINED)
		desiredType = _desiredType;

	if (nodePtr != nullptr)
	{
		if (nodePtr->outputs[outputIdx]->outputType == FlowType::UNDEFINED)
			nodePtr->outputs[outputIdx]->outputType = _desiredType;

		nodePtr->resolveUndeterminedTypes();
	}
}

void Input::compile(std::stringstream& stream, CompilationErrorCheck& errorCheck)
{
	nodePtr->compile(errorCheck);
	formatAndOutputResult(stream, *nodePtr->outputs[outputIdx], desiredType, errorCheck);
}

void Input::linkToOutput(Node* _nodePtr, int _outputIdx)
{
	nodePtr = _nodePtr;
	outputIdx = _outputIdx;
}

//// END : Input
///////////////////////////////////////////

///////////////////////////////////////////
//// BEGIN : Output

Output::Output(Node* _parentNode, const std::string& _name, FlowType _outputType)
	: parentNode(_parentNode)
	, name(_name)
	, outputType(_outputType)
	, link(nullptr)
{}

//// END : Output
///////////////////////////////////////////

///////////////////////////////////////////
//// BEGIN : Link

Link::Link(Output* _input, Input* _output)
	: input(_input)
	, output(_output)
{
}

void Link::save(Json::Value& root) const
{
	inputNodeID.save(root["inputNodeID"]);
	root["inputIdx"] = inputIdx;
	outputNodeID.save(root["outputNodeID"]);
	root["outputIdx"] = outputIdx;
}
void Link::load(const Json::Value& root)
{
	inputNodeID.load(root["inputNodeID"]);
	inputIdx = root["inputIdx"].asInt();
	outputNodeID.load(root["outputNodeID"]);
	outputIdx = root["outputIdx"].asInt();
}

void Link::drawUI(NodeManager& manager)
{
	if (input == nullptr || output == nullptr)
		return;

	ImVec2 A(input->position.x, input->position.y);
	ImVec2 A_(A.x + 100.f, A.y);
	ImVec2 B(output->position.x, output->position.y);
	ImVec2 B_(B.x - 100.f, B.y);

	ImGui::GetWindowDrawList()->AddBezierCurve(A, A_, B_, B, ImColor(0.5f, 0.5f, 0.5f, 1.0f), 2, 0);
}

//// END : Link
///////////////////////////////////////////

///////////////////////////////////////////
//// BEGIN : Node

Node::Node()
	: type(NodeType::FUNCTION)
	, name("default")
{
	nodeID = IDGenerator<MVS::Node>::instance().lockID();
}

Node::Node(NodeType _type, const std::string& _name)
	: type(_type)
	, name(_name)
{
	nodeID = IDGenerator<MVS::Node>::instance().lockID();
}

void Node::resolveUndeterminedTypes()
{
	for (auto input : inputs)
	{
		if (input->desiredType == FlowType::UNDEFINED && outputs.size() > 0)
			input->resolveUndeterminedTypes(outputs[0]->outputType);
		else if (input->desiredType != FlowType::UNDEFINED)
			input->resolveUndeterminedTypes(input->desiredType);
	}
}

void Node::save(Json::Value& root) const
{
	root["name"] = name;
	root["type"] = (int)type;
	nodeID.save(root["id"]);
}
void Node::load(const Json::Value& root)
{
	//name = root["name"].asString();
	nodeID.load(root["id"]);
}

void Node::drawUI(NodeManager& manager)
{
	ImGui::PushID(this);
	//ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(20, 5));

	ImVec2 cursorPos = ImGui::GetCursorScreenPos();
	ImVec2 size(200, 100);
	ImVec2 recMin(cursorPos.x + position.x, cursorPos.y + position.y);
	ImVec2 recMax(cursorPos.x + position.x + size.x, cursorPos.y + position.y + size.y);

	ImGui::SetCursorScreenPos(recMin);
	ImGui::BeginChild(name.c_str(), size, true);

	ImGui::GetWindowDrawList()->AddRectFilled(recMin, recMax, ImColor(0.4f, 0.4f, 0.4f, 1.f), 0.4f);
	if (ImGui::IsMouseHoveringRect(recMin, recMax) && ImGui::IsMouseDragging(0))
	{
		if (!manager.isDraggingNode() && !manager.getIsDraggingLink())
		{
			manager.setIsDraggingNode(true);
			manager.setDragAnchorPos(glm::vec2(ImGui::GetMousePos().x - position.x, ImGui::GetMousePos().y - position.y));
			manager.setDraggedNode(this);
		}
	}

	ImVec2 inputsPosition(recMin.x + 12.f, recMin.y + 50.f);
	for (auto input : inputs)
	{
		ImGui::PushID(input->name.c_str());

		ImVec2 textSize = ImGui::CalcTextSize(input->name.c_str());
		ImGui::SetCursorScreenPos(ImVec2(inputsPosition.x, inputsPosition.y));
		if (ImGui::RadioButton("##input", true))
		{
			if (!manager.isDraggingNode())
			{
				if (!manager.getIsDraggingLink())
				{
					manager.setIsDraggingLink(true);

					Link* link = input->link;
					if (link != nullptr)
					{
						manager.setDraggedLink(link);
					}
					else
					{
						auto newLink = std::make_shared<Link>(nullptr, input.get());
						manager.addLink(newLink);
						manager.setDraggedLink(newLink.get());
					}
				}
				else
				{
					Link* link = manager.getDraggedLink();
					link->output = input.get();
					manager.setIsDraggingLink(false);
					manager.setDraggedLink(nullptr);
				}
			}
		}
		if (ImGui::IsItemHovered())
			manager.setCanResetDragLink(false);
		ImGui::SameLine();
		ImGui::Text(input->name.c_str());

		//ImGui::GetWindowDrawList()->AddCircleFilled(outputsPosition, 10.f, ImColor(0.3f, 0.3f, 0.3f, 1.0f), 8);
		//ImGui::GetWindowDrawList()->AddText(ImVec2(outputsPosition.x - 12.f, outputsPosition.y), ImColor(0.8f, 0.8f, 0.8f, 1.0f), output->name.c_str());
		input->position.x = inputsPosition.x; // ImGui::GetCursorScreenPos().x; // outputsPosition.x + 10;
		input->position.y = inputsPosition.y; // ImGui::GetCursorScreenPos().y; // outputsPosition.y + 10;

		inputsPosition.y += 20.f;

		ImGui::PopID();
	}

	ImVec2 outputsPosition(recMax.x - 20.f, recMin.y + 50.f);
	for (auto output : outputs)
	{
		ImGui::PushID(output->name.c_str());

		ImVec2 textSize = ImGui::CalcTextSize(output->name.c_str());
		outputsPosition.x = recMax.x - textSize.x - 20.f;
		ImGui::SetCursorScreenPos(ImVec2(outputsPosition.x - textSize.x, outputsPosition.y));
		ImGui::Text(output->name.c_str());
		ImGui::SameLine();
		if (ImGui::RadioButton("##output", true))
		{
			if (!manager.isDraggingNode())
			{
				if (!manager.getIsDraggingLink())
				{
					manager.setIsDraggingLink(true);

					Link* link = output->link;
					if (link != nullptr)
					{
						manager.setDraggedLink(link);
					}
					else
					{
						auto newLink = std::make_shared<Link>(output.get(), nullptr);
						manager.addLink(newLink);
						manager.setDraggedLink(newLink.get());
					}
				}
				else
				{
					Link* link = manager.getDraggedLink();
					link->input = output.get();
					manager.setIsDraggingLink(false);
					manager.setDraggedLink(nullptr);
				}
			}
		}
		if (ImGui::IsItemHovered())
			manager.setCanResetDragLink(false);
		//ImGui::GetWindowDrawList()->AddCircleFilled(outputsPosition, 10.f, ImColor(0.3f, 0.3f, 0.3f, 1.0f), 8);
		//ImGui::GetWindowDrawList()->AddText(ImVec2(outputsPosition.x - 12.f, outputsPosition.y), ImColor(0.8f, 0.8f, 0.8f, 1.0f), output->name.c_str());
		output->position.x = outputsPosition.x; //ImGui::GetCursorScreenPos().x; // outputsPosition.x + 10;
		output->position.y = outputsPosition.y; //ImGui::GetCursorScreenPos().y; // outputsPosition.y + 10;

		outputsPosition.y += 20.f;

		ImGui::PopID();
	}

	ImGui::SetCursorScreenPos(recMin);
	ImGui::Text(name.c_str());
	ImGui::Separator();

	ImGui::EndChild();
	ImGui::SetCursorScreenPos(cursorPos);

	//ImGui::PopStyleVar();
	ImGui::PopID();
}

void Node::setPosition(const glm::vec2& pos)
{
	position = pos;
}

const glm::vec2& Node::getPosition() const
{
	return position;
}

//// END : Node
///////////////////////////////////////////

///////////////////////////////////////////
//// BEGIN : NodeManager

NodeManager::NodeManager(ShaderProgram* programPtr)
	: EditorFrame("MaterialVisualScripting")
	, m_programPtr(programPtr)
	, m_draggedNode(nullptr)
	, m_isDraggingNode(false)
	, m_dragAnchorPos(0, 0)
	, m_draggedLink(nullptr)
	, m_isDraggingLink(false)
{}

NodeManager::~NodeManager()
{
	if (m_programPtr != nullptr)
	{
		m_programPtr->resetNodeManagerPtr();
	}
}

void NodeManager::compile()
{
	CompilationErrorCheck errorCheck;
	internalResolveUndeterminedTypes();
	internalDefineParameters(m_compileStream, errorCheck);
	internalCompile(errorCheck);
	if (errorCheck.compilationFailed)
		m_lastCompilationSucceeded = false;
	else
	{
		m_compileStream << m_finalOutput->outputs[0]->valueStr;
		m_compileResult = m_compileStream.str();

		m_programPtr->compile(m_compileResult);
	}
}

void NodeManager::internalResolveUndeterminedTypes()
{
	m_finalOutput->resolveUndeterminedTypes();
}

void NodeManager::internalDefineParameters(std::stringstream& compileStream, CompilationErrorCheck& errorCheck)
{
	for (auto node : m_parameterNodes)
	{
		node->defineParameter(compileStream, errorCheck);
	}
}

void NodeManager::internalCompile(CompilationErrorCheck& errorCheck)
{
	m_finalOutput->compile(errorCheck);
}

Node* NodeManager::getNode(const ID& id) const
{
	auto foundIt = std::find_if(m_allNodes.begin(), m_allNodes.end(), [&id](const std::shared_ptr<Node>& item) { return item->nodeID == id; });
	if (foundIt != m_allNodes.end())
		return foundIt->get();
	else
		return nullptr;
}

void NodeManager::save(Json::Value& root) const
{
	root.clear();
	for (auto node : m_allNodes)
	{
		root["nodes"] = Json::Value(Json::objectValue);
		node->save(root["nodes"]);
	}

	for (auto link : m_allLinks)
	{
		root["links"] = Json::Value(Json::objectValue);
		link->save(root["links"]);
	}
}

void NodeManager::load(const Json::Value& root)
{
	// Clear
	m_allNodes.clear();
	m_allLinks.clear();
	m_finalOutput = nullptr;
	m_parameterNodes.clear();

	// Load nodes
	int nodeCount = root["nodes"].size();
	for (int i = 0; i < nodeCount; i++)
	{
		// Create
		std::string nodeName = root["nodes"][i]["name"].asString();
		NodeType nodeType = (NodeType)root["nodes"][i]["type"].asInt();
		// Load
		std::shared_ptr<Node> newNode = NodeFactory::instance().getSharedNodeInstance(nodeName);
		newNode->load(root["nodes"][i]);
		// Add
		m_allNodes.push_back(newNode);

		// Store refs to special nodes
		if (nodeType == NodeType::PARAMETER)
		{
			assert(dynamic_cast<BaseParameterNode*>(newNode.get()) != nullptr);
			m_parameterNodes.push_back(static_cast<BaseParameterNode*>(newNode.get()));
		}
		else if (nodeType == NodeType::FINAL)
		{
			m_finalOutput = newNode.get();
		}

	}

	// Load links
	int linkCount = root["links"].size();
	for (int i = 0; i < linkCount; i++)
	{
		// Create
		std::shared_ptr<Link> newLink = std::make_shared<Link>(nullptr, nullptr);
		// Load
		newLink->load(root["links"][i]);
		Node* leftNode = getNode(newLink->inputNodeID);
		Node* rightNode = getNode(newLink->outputNodeID);
		assert(newLink->inputIdx >= 0 && newLink->inputIdx < leftNode->outputs.size());
		newLink->input = leftNode->outputs[newLink->inputIdx].get();
		assert(newLink->outputIdx >= 0 && newLink->outputIdx  < leftNode->inputs.size());
		newLink->output = rightNode->inputs[newLink->outputIdx].get();
		// Add
		m_allLinks.push_back(newLink);
	}
}

void NodeManager::drawContent(Project& project, EditorModal* parentWindow)
{
	for (auto node : m_allNodes)
	{
		node->drawUI(*this);
	}

	for (auto link : m_allLinks)
	{
		link->drawUI(*this);
	}

	if (ImGui::IsMouseClicked(1))
	{
		ImGui::OpenPopup("selectNodePopUp");
	}

	if (ImGui::BeginPopup("selectNodePopUp"))
	{
		for (auto it = NodeFactory::instance().begin(); it != NodeFactory::instance().end(); it++)
		{
			if (ImGui::Button(it->first.c_str()))
			{
				auto newNode = it->second->cloneShared();

				const glm::vec2 nodePosition(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
				newNode->setPosition(nodePosition);

				m_allNodes.push_back(newNode);
			}
		}

		ImGui::EndPopup();
	}

	if (isDraggingNode() && m_draggedNode != nullptr)
	{
		glm::vec2 dragDelta(ImGui::GetMousePos().x - m_dragAnchorPos.x, ImGui::GetMousePos().y - m_dragAnchorPos.y);
		m_draggedNode->setPosition(dragDelta);
	}
	else if (m_isDraggingLink && m_draggedLink != nullptr)
	{
		if (m_draggedLink->input == nullptr && m_draggedLink->output != nullptr)
		{
			ImVec2 A = ImGui::GetMousePos();
			ImVec2 A_(A.x + 100.f, A.y);
			ImVec2 B(m_draggedLink->output->position.x, m_draggedLink->output->position.y);
			ImVec2 B_(B.x - 100.f, B.y);

			ImGui::GetWindowDrawList()->AddBezierCurve(A, A_, B_, B, ImColor(0.5f, 0.5f, 0.5f, 1.0f), 2);
		}
		else if (m_draggedLink->input != nullptr && m_draggedLink->output == nullptr)
		{
			ImVec2 A(m_draggedLink->input->position.x, m_draggedLink->input->position.y);
			ImVec2 A_(A.x + 100.f, A.y);
			ImVec2 B = ImGui::GetMousePos();
			ImVec2 B_(B.x - 100.f, B.y);

			ImGui::GetWindowDrawList()->AddBezierCurve(A, A_, B_, B, ImColor(0.5f, 0.5f, 0.5f, 1.0f), 2);
		}
	}

	if (ImGui::IsMouseReleased(0) && m_isDraggingNode)
	{
		setIsDraggingNode(false);
		setDraggedNode(nullptr);
	}

	if (ImGui::IsMouseClickedAnyButton() && m_isDraggingLink && m_canResetDragLink)
	{
		if (m_draggedLink != nullptr)
		{
			removeLink(m_draggedLink);
		}

		setIsDraggingLink(false);
		setDraggedLink(nullptr);
	}
	m_canResetDragLink = true;
}

bool NodeManager::isDraggingNode() const
{
	return m_isDraggingNode;
}

void NodeManager::setIsDraggingNode(bool state)
{
	m_isDraggingNode = state;
}

void NodeManager::setDragAnchorPos(const glm::vec2 & position)
{
	m_dragAnchorPos = position;
}

const glm::vec2 & NodeManager::getDragAnchorPos() const
{
	return m_dragAnchorPos;
}

Node * NodeManager::getDraggedNode() const
{
	return m_draggedNode;
}

void NodeManager::setDraggedNode(Node * node)
{
	m_draggedNode = node;
}

void NodeManager::setIsDraggingLink(bool state)
{
	m_isDraggingLink = state;
}

bool NodeManager::getIsDraggingLink() const
{
	return m_isDraggingLink;
}

void NodeManager::setDraggedLink(Link * link)
{
	m_draggedLink = link;
}

Link * NodeManager::getDraggedLink() const
{
	return m_draggedLink;
}

void NodeManager::setCanResetDragLink(bool state)
{
	m_canResetDragLink = state;
}

void NodeManager::addLink(std::shared_ptr<Link> link)
{
	m_allLinks.push_back(link);
}

void NodeManager::removeLink(Link * link)
{
	m_allLinks.erase(std::remove_if(m_allLinks.begin(), m_allLinks.end(), [link](const std::shared_ptr<Link>& item) { return item.get() == link; }), m_allLinks.end());
}

//// END : NodeManager
///////////////////////////////////////////

}