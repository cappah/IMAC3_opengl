#include "MaterialVisualScripting.h"

namespace MVS {

///////////////////////////////////////////
//// BEGIN : Link

void Link::drawUI()
{
	//TODO
}

//// END : Link
///////////////////////////////////////////

///////////////////////////////////////////
//// BEGIN : Node

void Node::drawUI()
{
	//TODO
}

//// END : Node
///////////////////////////////////////////

///////////////////////////////////////////
//// BEGIN : NodeManager

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
		std::shared_ptr<Link> newLink = std::make_shared<Link>();
		// Load
		newLink->load(root["links"][i]);
		Node* leftNode = getNode(newLink->leftNodeID);
		Node* rightNode = getNode(newLink->rightNodeID);
		newLink->leftNodePtr = leftNode;
		newLink->rightNodePtr = rightNode;
		// Add
		m_allLinks.push_back(newLink);
	}
}

void NodeManager::drawContent(Project& project, EditorModal* parentWindow)
{
	for (auto node : m_allNodes)
	{
		node->drawUI();
	}

	for (auto link : m_allLinks)
	{
		link->drawUI();
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
}

//// END : NodeManager
///////////////////////////////////////////

}