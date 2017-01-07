#pragma once

#include <assert.h>
#include <vector>
#include <sstream>
#include <memory>
#include <unordered_map>

#include "glm/common.hpp"

#include "ShaderParameters.h"
#include "EditorNodes.h"

class ShaderProgram;

namespace MVS {

class NodeManager;
struct Node;
struct Link;
struct Input;
struct Output;

enum class NodeType {
	OPERATOR,
	PARAMETER,
	FUNCTION,
	FINAL,
};

enum class ParameterType {
	FLOAT,
	FLOAT2,
	FLOAT3,
	FLOAT4,
	TEXTURE,
};

enum class FlowType {
	UNDEFINED = 0,
	FLOAT = 1,
	FLOAT2,
	FLOAT3,
	FLOAT4,
};

struct CompilationErrorCheck
{
	bool compilationFailed;
	std::string errorMsg;
};

struct Input
{
	std::string name;
	Node* nodePtr;
	int outputIdx;
	FlowType desiredType;

	Node* parentNode;
	Link* link;
	glm::vec2 position;

	Input(Node* _parentNode, const std::string& _name, FlowType _desiredType);
	void resolveUndeterminedTypes(FlowType _desiredType);
	void compile(std::stringstream& stream, CompilationErrorCheck& errorCheck);
	void linkToOutput(Node* _nodePtr, int _outputIdx);
};

struct Output
{
	std::string name;
	FlowType outputType;
	std::string valueStr;

	Node* parentNode;
	Link* link;
	glm::vec2 position;

	Output(Node* _parentNode, const std::string& _name, FlowType _outputType);
};

void formatAndOutputResult(std::stringstream& stream, const Output& output, FlowType desiredType, CompilationErrorCheck& errorCheck);

#define INHERIT_FROM_NODE(RealNodeType)\
virtual std::shared_ptr<Node> cloneShared() override\
{\
	return std::make_shared<RealNodeType>(*this);\
}

struct Node 
{
	NodeType type;
	std::string name;
	ID nodeID;

	std::vector<std::shared_ptr<Input>> inputs;
	std::vector<std::shared_ptr<Output>> outputs;

	glm::vec2 position;

	virtual void compile(CompilationErrorCheck& errorCheck) = 0;

	Node();
	Node(NodeType _type, const std::string& _name);

	void resolveUndeterminedTypes();
	virtual std::shared_ptr<Node> cloneShared() = 0;
	
	void save(Json::Value& root) const;
	void load(const Json::Value& root);

	void drawUI(NodeManager& manager);
	void setPosition(const glm::vec2& pos);
	const glm::vec2& getPosition() const;
};

struct Link
{
	ID inputNodeID;
	int inputIdx;
	ID outputNodeID;
	int outputIdx;

	Output* input;
	Input* output;

	Link(Output* _input, Input* _output);

	void save(Json::Value& root) const;
	void load(const Json::Value& root);

	void drawUI(NodeManager& manager);
};

class NodeFactory : public ISingleton<NodeFactory>
{
	SINGLETON_IMPL(NodeFactory);

private:
	std::unordered_map<std::string, std::shared_ptr<Node>> m_container;

public:

	NodeFactory()
	{}

	bool registerNode(const std::string& nodeName, std::shared_ptr<Node> node)
	{
		m_container[nodeName] = node;
		return true;
	}

	std::shared_ptr<Node> getSharedNodeInstance(const std::string& nodeName)
	{
		assert(m_container.find(nodeName) != m_container.end());

		return m_container[nodeName]->cloneShared();
	}

	std::unordered_map<std::string, std::shared_ptr<Node>>::const_iterator begin() const
	{
		return m_container.begin();
	}

	std::unordered_map<std::string, std::shared_ptr<Node>>::const_iterator end() const
	{
		return m_container.end();
	}
};

#define REGISTER_NODE(NodeName, NodeType) static bool initialized##NodeType = NodeFactory::instance().registerNode(NodeName, std::make_shared<NodeType>());

struct FinalNode : public Node
{
	INHERIT_FROM_NODE(FinalNode)

	FinalNode()
		: Node(NodeType::FINAL, "result")
	{
		inputs.push_back(std::make_shared<Input>(this, "Diffuse", FlowType::FLOAT3));
		inputs.push_back(std::make_shared<Input>(this, "Specular", FlowType::FLOAT));
		inputs.push_back(std::make_shared<Input>(this, "Emissive", FlowType::FLOAT3));
		inputs.push_back(std::make_shared<Input>(this, "Normals", FlowType::FLOAT3));
		inputs.push_back(std::make_shared<Input>(this, "SpecularPower", FlowType::FLOAT));
	}

	void compile(CompilationErrorCheck& errorCheck)
	{
		std::stringstream paramDiffuseStream;
		inputs[0]->compile(paramDiffuseStream, errorCheck);

		std::stringstream paramSpecularStream;
		inputs[1]->compile(paramSpecularStream, errorCheck);

		std::stringstream paramEmissiveStream;
		inputs[2]->compile(paramEmissiveStream, errorCheck);

		std::stringstream paramNormalstextureStream;
		inputs[3]->compile(paramNormalstextureStream, errorCheck);

		std::stringstream paramSpecularPowerStream;
		inputs[4]->compile(paramSpecularPowerStream, errorCheck);


		std::stringstream nodeCompileResult;

		nodeCompileResult << "void computeShaderParameters(inout vec3 paramDiffuse, inout vec3 paramNormals, inout float paramSpecular, inout float paramSpecularPower, inout vec3 paramEmissive)";
		nodeCompileResult << '{';
		nodeCompileResult << "paramDiffuse = " << paramDiffuseStream.str();
		nodeCompileResult << "paramSpecular = " << paramSpecularStream.str();
		nodeCompileResult << "paramEmissive = " << paramEmissiveStream.str();
		nodeCompileResult << "paramNormalstexture = " << paramNormalstextureStream.str();
		nodeCompileResult << "paramSpecularPower = " << paramSpecularPowerStream.str();
		nodeCompileResult << '}';

		outputs[0]->valueStr = nodeCompileResult.str();
	}
};

struct FunctionNode : public Node
{
	INHERIT_FROM_NODE(FunctionNode)

	std::string functionStr;

	FunctionNode(const std::string& _nodeName, const std::string& _operatorStr)
		: Node(NodeType::OPERATOR, _nodeName)
		, functionStr(_operatorStr)
	{

	}

	virtual void compile(CompilationErrorCheck& errorCheck) override
	{
		// Print : functionStr(input02, input02, ...)
		std::stringstream nodeCompileResult;
		nodeCompileResult << functionStr;
		nodeCompileResult << "(";
		int idx = 0;
		for (auto input : inputs)
		{
			input->compile(nodeCompileResult, errorCheck);
			if (idx < inputs.size() - 1)
				nodeCompileResult << ',';
			idx++;
		}
		nodeCompileResult << ")";

		outputs[0]->valueStr = nodeCompileResult.str();
	}


};

struct OperatorNode : public Node
{
	INHERIT_FROM_NODE(OperatorNode)

	std::string operatorStr;

	OperatorNode(const std::string& _nodeName, const std::string& _operatorStr)
		: Node(NodeType::OPERATOR, _nodeName)
		, operatorStr(_operatorStr)
	{

	}

	virtual void compile(CompilationErrorCheck& errorCheck) override
	{
		// Print : ([...] operator [...])
		std::stringstream nodeCompileResult;
		nodeCompileResult << "(";
		int idx = 0;
		for (auto input : inputs)
		{
			input->compile(nodeCompileResult, errorCheck);
			if (idx < inputs.size() - 1)
				nodeCompileResult << operatorStr;
			idx++;
		}
		nodeCompileResult << ")";

		outputs[0]->valueStr = nodeCompileResult.str();
	}
};

struct BaseParameterNode : public Node
{
	ParameterType type;
	std::string parameterName;
	bool isUniform;

	BaseParameterNode(const std::string& _nodeName)
		: Node(NodeType::PARAMETER, _nodeName)
	{}

	virtual void defineParameter(std::stringstream& stream, CompilationErrorCheck& errorCheck) = 0;
};

template <typename T>
struct ParameterNode : public BaseParameterNode
{
	INHERIT_FROM_NODE(ParameterNode<T>)

	InternalShaderParameter<T, ShaderParameter::IsNotArray> shaderParameter;

	ParameterNode();
	virtual void compile(CompilationErrorCheck& errorCheck) override;
	virtual void defineParameter(std::stringstream& stream, CompilationErrorCheck& errorCheck) override
	{
		// Print [uniform] type name;
		if (isUniform)
		{
			stream << "uniform ";
		}

		stream << Utils::typeAsString<T>() << " " << parameterName << " = " << shaderParameter.valueAsString() << ';';
	}
};

template<>
inline void ParameterNode<Texture>::defineParameter(std::stringstream& stream, CompilationErrorCheck& errorCheck)
{
	// Texture type is always uniform
	stream << "uniform ";

	stream << "sampler2D " << parameterName << ';';
}

//////////////////////////////////////////////////////////////
//// BEGIN : Parameters

template<>
inline ParameterNode<float>::ParameterNode()
	: BaseParameterNode("scalar")
	, shaderParameter(name, true, 0.f, EditorGUI::FieldDisplayType::DEFAULT)
{
	outputs.push_back(std::make_shared<Output>(this, "x", FlowType::FLOAT));
}

template<>
inline void ParameterNode<float>::compile(CompilationErrorCheck& errorCheck)
{
	outputs[0]->valueStr = parameterName;
}

/////////////

template<>
inline ParameterNode<glm::vec2>::ParameterNode()
	: BaseParameterNode("vector2")
	, shaderParameter(name, true, glm::vec2(0, 0), EditorGUI::FieldDisplayType::DEFAULT)
{
	outputs.push_back(std::make_shared<Output>(this, "rg", FlowType::FLOAT2));
	outputs.push_back(std::make_shared<Output>(this, "r", FlowType::FLOAT));
	outputs.push_back(std::make_shared<Output>(this, "g", FlowType::FLOAT));
}

template<>
inline void ParameterNode<glm::vec2>::compile(CompilationErrorCheck& errorCheck)
{
	outputs[0]->valueStr = parameterName;
	outputs[1]->valueStr = parameterName + ".r";
	outputs[2]->valueStr = parameterName + ".g";
}

/////////////

template<>
inline ParameterNode<glm::vec3>::ParameterNode()
	: BaseParameterNode("vector3")
	, shaderParameter(name, true, glm::vec3(0,0,0), EditorGUI::FieldDisplayType::DEFAULT)
{
	outputs.push_back(std::make_shared<Output>(this, "rgb", FlowType::FLOAT3));
	outputs.push_back(std::make_shared<Output>(this, "r", FlowType::FLOAT));
	outputs.push_back(std::make_shared<Output>(this, "g", FlowType::FLOAT));
	outputs.push_back(std::make_shared<Output>(this, "b", FlowType::FLOAT));
}

template<>
inline void ParameterNode<glm::vec3>::compile(CompilationErrorCheck& errorCheck)
{
	outputs[0]->valueStr = parameterName;
	outputs[1]->valueStr = parameterName + ".r";
	outputs[2]->valueStr = parameterName + ".g";
	outputs[3]->valueStr = parameterName + ".b";
}

/////////////

template<>
inline ParameterNode<glm::vec4>::ParameterNode()
	: BaseParameterNode("vector4")
	, shaderParameter(name, true, glm::vec4(0, 0, 0, 0), EditorGUI::FieldDisplayType::DEFAULT)
{
	outputs.push_back(std::make_shared<Output>(this, "rgba", FlowType::FLOAT4));
	outputs.push_back(std::make_shared<Output>(this, "r", FlowType::FLOAT));
	outputs.push_back(std::make_shared<Output>(this, "g", FlowType::FLOAT));
	outputs.push_back(std::make_shared<Output>(this, "b", FlowType::FLOAT));
	outputs.push_back(std::make_shared<Output>(this, "a", FlowType::FLOAT));
}

template<>
inline void ParameterNode<glm::vec4>::compile(CompilationErrorCheck& errorCheck)
{
	outputs[0]->valueStr = parameterName;
	outputs[1]->valueStr = parameterName + ".r";
	outputs[2]->valueStr = parameterName + ".g";
	outputs[3]->valueStr = parameterName + ".b";
	outputs[4]->valueStr = parameterName + ".a";
}

/////////////

template<>
inline ParameterNode<Texture>::ParameterNode()
	: BaseParameterNode("texture")
	, shaderParameter(name, true, ResourcePtr<Texture>(), EditorGUI::FieldDisplayType::DEFAULT)
{
	inputs.push_back(std::make_shared<Input>(this, "texCoords", FlowType::FLOAT2));

	outputs.push_back(std::make_shared<Output>(this, "rgba", FlowType::FLOAT4));
	outputs.push_back(std::make_shared<Output>(this, "r", FlowType::FLOAT));
	outputs.push_back(std::make_shared<Output>(this, "g", FlowType::FLOAT));
	outputs.push_back(std::make_shared<Output>(this, "b", FlowType::FLOAT));
	outputs.push_back(std::make_shared<Output>(this, "a", FlowType::FLOAT));
}

template<>
inline void ParameterNode<Texture>::compile(CompilationErrorCheck& errorCheck)
{
	std::stringstream textureCoords;
	inputs[0]->compile(textureCoords, errorCheck);
	std::stringstream core;
	core << "texture(" << parameterName << "," << textureCoords.str() << ')';

	outputs[0]->valueStr = core.str() + ".rgba"; //texture(parameterName, texCoords).rgba
	outputs[1]->valueStr = core.str() + ".r";    //texture(parameterName, texCoords).r
	outputs[2]->valueStr = core.str() + ".g";    //texture(parameterName, texCoords).g
	outputs[3]->valueStr = core.str() + ".b";    //texture(parameterName, texCoords).b
	outputs[4]->valueStr = core.str() + ".a";    //texture(parameterName, texCoords).a
}

//// END : Parameters
//////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////
//// BEGIN : Operators

struct OperatorNodeAdd : public OperatorNode
{
	INHERIT_FROM_NODE(OperatorNodeAdd)

	OperatorNodeAdd()
		: OperatorNode("add", "+")
	{
		inputs.push_back(std::make_shared<Input>(this, "a", FlowType::UNDEFINED));
		inputs.push_back(std::make_shared<Input>(this, "b", FlowType::UNDEFINED));

		outputs.push_back(std::make_shared<Output>(this, "r", FlowType::UNDEFINED));
	}
};

REGISTER_NODE("add", OperatorNodeAdd)

struct OperatorNodeMinus : public OperatorNode
{
	INHERIT_FROM_NODE(OperatorNodeMinus)

	OperatorNodeMinus()
		: OperatorNode("minus", "-")
	{
		inputs.push_back(std::make_shared<Input>(this, "a", FlowType::UNDEFINED));
		inputs.push_back(std::make_shared<Input>(this, "b", FlowType::UNDEFINED));

		outputs.push_back(std::make_shared<Output>(this, "r", FlowType::UNDEFINED));
	}
};

REGISTER_NODE("minus", OperatorNodeMinus)

struct OperatorNodeMultiply : public OperatorNode
{
	INHERIT_FROM_NODE(OperatorNodeMultiply)

	OperatorNodeMultiply()
		: OperatorNode("multiply", "*")
	{
		inputs.push_back(std::make_shared<Input>(this, "a", FlowType::UNDEFINED));
		inputs.push_back(std::make_shared<Input>(this, "b", FlowType::UNDEFINED));

		outputs.push_back(std::make_shared<Output>(this, "r", FlowType::UNDEFINED));
	}
};

REGISTER_NODE("multiply", OperatorNodeMultiply)

struct OperatorNodeDivide : public OperatorNode
{
	INHERIT_FROM_NODE(OperatorNodeDivide)

	OperatorNodeDivide()
		: OperatorNode("divide", "/")
	{
		inputs.push_back(std::make_shared<Input>(this, "a", FlowType::UNDEFINED));
		inputs.push_back(std::make_shared<Input>(this, "b", FlowType::UNDEFINED));

		outputs.push_back(std::make_shared<Output>(this, "r", FlowType::UNDEFINED));
	}
};

REGISTER_NODE("divide", OperatorNodeDivide)

//// END : Operators
//////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////
//// BEGIN : Functions

struct FunctionNodeDot : public FunctionNode
{
	INHERIT_FROM_NODE(FunctionNodeDot)

	FunctionNodeDot()
		: FunctionNode("dot", "dot")
	{
		inputs.push_back(std::make_shared<Input>(this, "a", FlowType::UNDEFINED));
		inputs.push_back(std::make_shared<Input>(this, "b", FlowType::UNDEFINED));

		outputs.push_back(std::make_shared<Output>(this, "r", FlowType::UNDEFINED));
	}
};

REGISTER_NODE("dot", FunctionNodeDot)

struct FunctionNodeCross : public FunctionNode
{
	INHERIT_FROM_NODE(FunctionNodeCross)

	FunctionNodeCross()
		: FunctionNode("cross", "cross")
	{
		inputs.push_back(std::make_shared<Input>(this, "a", FlowType::FLOAT3));
		inputs.push_back(std::make_shared<Input>(this, "b", FlowType::FLOAT3));

		outputs.push_back(std::make_shared<Output>(this, "r", FlowType::FLOAT3));
	}
};

REGISTER_NODE("cross", FunctionNodeCross)

struct FunctionNodeNormalize : public FunctionNode
{
	INHERIT_FROM_NODE(FunctionNodeNormalize)

	FunctionNodeNormalize()
		: FunctionNode("normalize", "normalize")
	{
		inputs.push_back(std::make_shared<Input>(this, "a", FlowType::UNDEFINED));

		outputs.push_back(std::make_shared<Output>(this, "r", FlowType::UNDEFINED));
	}
};

REGISTER_NODE("normalize", FunctionNodeNormalize)

struct FunctionNodeDistance : public FunctionNode
{
	INHERIT_FROM_NODE(FunctionNodeDistance)

	FunctionNodeDistance()
		: FunctionNode("distance", "distance")
	{
		inputs.push_back(std::make_shared<Input>(this, "a", FlowType::UNDEFINED));
		inputs.push_back(std::make_shared<Input>(this, "b", FlowType::UNDEFINED));

		outputs.push_back(std::make_shared<Output>(this, "r", FlowType::UNDEFINED));
	}
};

REGISTER_NODE("distance", FunctionNodeDistance)

struct FunctionNodeLength : public FunctionNode
{
	INHERIT_FROM_NODE(FunctionNodeLength)

	FunctionNodeLength()
		: FunctionNode("length", "length")
	{
		inputs.push_back(std::make_shared<Input>(this, "a", FlowType::UNDEFINED));

		outputs.push_back(std::make_shared<Output>(this, "r", FlowType::UNDEFINED));
	}
};

REGISTER_NODE("length", FunctionNodeLength)

struct FunctionNodeMin : public FunctionNode
{
	INHERIT_FROM_NODE(FunctionNodeMin)

	FunctionNodeMin()
		: FunctionNode("minimum", "min")
	{
		inputs.push_back(std::make_shared<Input>(this, "a", FlowType::UNDEFINED));
		inputs.push_back(std::make_shared<Input>(this, "b", FlowType::UNDEFINED));

		outputs.push_back(std::make_shared<Output>(this, "r", FlowType::UNDEFINED));
	}
};

REGISTER_NODE("minimum", FunctionNodeMin)

struct FunctionNodeMax : public FunctionNode
{
	INHERIT_FROM_NODE(FunctionNodeMax)

	FunctionNodeMax()
		: FunctionNode("maximum", "max")
	{
		inputs.push_back(std::make_shared<Input>(this, "a", FlowType::UNDEFINED));
		inputs.push_back(std::make_shared<Input>(this, "b", FlowType::UNDEFINED));

		outputs.push_back(std::make_shared<Output>(this, "r", FlowType::UNDEFINED));
	}
};

REGISTER_NODE("maximum", FunctionNodeMax)

struct FunctionNodeMod : public FunctionNode
{
	INHERIT_FROM_NODE(FunctionNodeMod)

	FunctionNodeMod()
		: FunctionNode("modulo", "mod")
	{
		inputs.push_back(std::make_shared<Input>(this, "a", FlowType::UNDEFINED));
		inputs.push_back(std::make_shared<Input>(this, "b", FlowType::UNDEFINED));

		outputs.push_back(std::make_shared<Output>(this, "r", FlowType::UNDEFINED));
	}
};

REGISTER_NODE("modulo", FunctionNodeMod)

struct FunctionNodeCos : public FunctionNode
{
	INHERIT_FROM_NODE(FunctionNodeCos)

	FunctionNodeCos()
		: FunctionNode("cosine", "cos")
	{
		inputs.push_back(std::make_shared<Input>(this, "a", FlowType::UNDEFINED));

		outputs.push_back(std::make_shared<Output>(this, "r", FlowType::UNDEFINED));
	}
};

REGISTER_NODE("cosine", FunctionNodeCos)

struct FunctionNodeSin : public FunctionNode
{
	INHERIT_FROM_NODE(FunctionNodeSin)

	FunctionNodeSin()
		: FunctionNode("sine", "sin")
	{
		inputs.push_back(std::make_shared<Input>(this, "a", FlowType::UNDEFINED));

		outputs.push_back(std::make_shared<Output>(this, "r", FlowType::UNDEFINED));
	}
};

REGISTER_NODE("sine", FunctionNodeSin)

struct FunctionNodeTan : public FunctionNode
{
	INHERIT_FROM_NODE(FunctionNodeTan)

	FunctionNodeTan()
		: FunctionNode("tangent", "tan")
	{
		inputs.push_back(std::make_shared<Input>(this, "a", FlowType::UNDEFINED));

		outputs.push_back(std::make_shared<Output>(this, "r", FlowType::UNDEFINED));
	}
};

REGISTER_NODE("tangent", FunctionNodeTan)

//// END : Functions
//////////////////////////////////////////////////////////////

class NodeManager : public EditorFrame
{
private:
	Node* m_finalOutput;
	std::vector < std::shared_ptr<Node> > m_allNodes;
	std::vector< std::shared_ptr<Link> > m_allLinks;
	std::vector<BaseParameterNode*> m_parameterNodes;
	std::stringstream m_compileStream;
	std::string m_compileResult;
	bool m_lastCompilationSucceeded;

	ShaderProgram* m_programPtr;

	glm::vec2 m_dragAnchorPos;
	bool m_isDraggingNode;
	Node* m_draggedNode;
	bool m_isDraggingLink;
	Link* m_draggedLink;
	bool m_canResetDragLink;

public:
	NodeManager(ShaderProgram* programPtr);
	~NodeManager();

	void compile();

	Node* getNode(const ID& id) const;
	void save(Json::Value& root) const;
	void load(const Json::Value& root);
	void drawContent(Project& project, EditorModal* parentWindow) override;

	bool isDraggingNode() const;
	void setIsDraggingNode(bool state);
	void setDragAnchorPos(const glm::vec2& position);
	const glm::vec2& getDragAnchorPos() const;
	Node* getDraggedNode() const;
	void setDraggedNode(Node* node);
	void setIsDraggingLink(bool state);
	bool getIsDraggingLink() const;
	void setDraggedLink(Link* link);
	Link* getDraggedLink() const;
	void setCanResetDragLink(bool state);

	void addLink(std::shared_ptr<Link> link);
	void removeLink(Link* link);

private:

	void internalResolveUndeterminedTypes();
	void internalDefineParameters(std::stringstream& compileStream, CompilationErrorCheck& errorCheck);
	void internalCompile(CompilationErrorCheck& errorCheck);
};


}