#pragma once

#include <assert.h>
#include <vector>
#include <sstream>
#include <memory>
#include <unordered_map>

#include "glm/common.hpp"

#include "ShaderParameters.h"
#include "EditorNodes.h"

namespace MVS {

enum NodeType {
	OPERATOR,
	PARAMETER,
	FUNCTION,
	FINAL,
};

enum ParameterType {
	FLOAT,
	FLOAT2,
	FLOAT3,
	FLOAT4,
	TEXTURE,
};

enum FlowType {
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

struct Input
{
	std::string name;
	Node* nodePtr;
	int outputIdx;
	FlowType desiredType;

	Input(const std::string& _name, FlowType _desiredType)
		: name(_name), desiredType(_desiredType)
	{}

	void resolveUndeterminedTypes(FlowType _desiredType)
	{
		assert(_desiredType != FlowType::UNDEFINED);
		if(desiredType == FlowType::UNDEFINED)
			desiredType = _desiredType;

		if (nodePtr != nullptr)
		{
			if (nodePtr->outputs[outputIdx]->outputType == FlowType::UNDEFINED)
				nodePtr->outputs[outputIdx]->outputType = _desiredType;

			nodePtr->resolveUndeterminedTypes();
		}
	}

	void compile(std::stringstream& stream, CompilationErrorCheck& errorCheck)
	{
		nodePtr->compile(errorCheck);
		formatAndOutputResult(stream, nodePtr->outputs[outputIdx], desiredType, errorCheck);
	}

	void linkToOutput(Node* _nodePtr, int _outputIdx)
	{
		nodePtr = _nodePtr;
		outputIdx = _outputIdx;
	}
};

struct Output
{
	std::string name;
	FlowType outputType;
	std::string valueStr;

	Output(const std::string& _name, FlowType _outputType)
		: name(_name)
		, outputType(_outputType)
	{}
};

#define INHERIT_FROM_NODE(RealNodeType)\
virtual std::shared_ptr<Node> cloneShared() override\
{\
	return std::make_shared<RealNodeType>(this);\
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

	Node()
		: type(NodeType::FUNCTION)
		, name("default")
	{
		nodeID = IDGenerator<MVS::Node>::instance().lockID();
	}

	Node(NodeType _type, const std::string& _name)
		: type(_type)
		, name(_name)
	{
		nodeID = IDGenerator<MVS::Node>::instance().lockID();
	}

	void resolveUndeterminedTypes()
	{
		for (auto input : inputs)
		{
			if(input->desiredType == FlowType::UNDEFINED && outputs.size() > 0)
				input->resolveUndeterminedTypes(outputs[0]->outputType);
			else if(input->desiredType != FlowType::UNDEFINED)
				input->resolveUndeterminedTypes(input->desiredType);
		}
	}

	virtual std::shared_ptr<Node> cloneShared() = 0;

	void save(Json::Value& root) const
	{
		root["name"] = name;
		root["type"] = (int)type;
		nodeID.save(root["id"]);
	}
	void load(const Json::Value& root)
	{
		//name = root["name"].asString();
		nodeID.load(root["id"]);
	}

	void drawUI();

	void setPosition(const glm::vec2& pos)
	{
		position = pos;
	}

	const glm::vec2& getPosition() const
	{
		return position;
	}
};

struct Link
{
	ID leftNodeID;
	ID rightNodeID;

	Node* leftNodePtr;
	Node* rightNodePtr;

	void save(Json::Value& root) const
	{
		leftNodeID.save(root["leftNodeID"]);
		rightNodeID.save(root["rightNodeID"]);
	}
	void load(const Json::Value& root)
	{
		leftNodeID.load(root["leftNodeID"]);
		rightNodeID.load(root["rightNodeID"]);
	}

	void drawUI();
};


struct FinalNode : public Node
{
	INHERIT_FROM_NODE(FinalNode)

	FinalNode()
		: Node(NodeType::FINAL, "result")
	{
		inputs.push_back(std::make_shared<Input>("Diffuse", FlowType::FLOAT3));
		inputs.push_back(std::make_shared<Input>("Specular", FlowType::FLOAT));
		inputs.push_back(std::make_shared<Input>("Emissive", FlowType::FLOAT3));
		inputs.push_back(std::make_shared<Input>("Normals", FlowType::FLOAT3));
		inputs.push_back(std::make_shared<Input>("SpecularPower", FlowType::FLOAT));
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

		stream << "float " << parameterName << " = " << shaderParameter.valueAsString() << ';';
	}
};

template<>
void ParameterNode<Texture>::defineParameter(std::stringstream& stream, CompilationErrorCheck& errorCheck)
{
	// Texture type is always uniform
	stream << "uniform ";

	stream << "sampler2D " << parameterName << ';';
}

//////////////////////////////////////////////////////////////
//// BEGIN : Parameters

template<>
ParameterNode<float>::ParameterNode()
	: BaseParameterNode("scalar")
	, shaderParameter(name, true, 0.f, EditorGUI::FieldDisplayType::DEFAULT)
{
	outputs.push_back(std::make_shared<Output>("x", FlowType::FLOAT));
}

template<>
void ParameterNode<float>::compile(CompilationErrorCheck& errorCheck)
{
	outputs[0]->valueStr = parameterName;
}

/////////////

template<>
ParameterNode<glm::vec2>::ParameterNode()
	: BaseParameterNode("vector2")
	, shaderParameter(name, true, glm::vec2(0, 0), EditorGUI::FieldDisplayType::DEFAULT)
{
	outputs.push_back(std::make_shared<Output>("rg", FlowType::FLOAT2));
	outputs.push_back(std::make_shared<Output>("r", FlowType::FLOAT));
	outputs.push_back(std::make_shared<Output>("g", FlowType::FLOAT));
}

template<>
void ParameterNode<glm::vec2>::compile(CompilationErrorCheck& errorCheck)
{
	outputs[0]->valueStr = parameterName;
	outputs[1]->valueStr = parameterName + ".r";
	outputs[2]->valueStr = parameterName + ".g";
}

/////////////

template<>
ParameterNode<glm::vec3>::ParameterNode()
	: BaseParameterNode("vector3")
	, shaderParameter(name, true, glm::vec3(0,0,0), EditorGUI::FieldDisplayType::DEFAULT)
{
	outputs.push_back(std::make_shared<Output>("rgb", FlowType::FLOAT3));
	outputs.push_back(std::make_shared<Output>("r", FlowType::FLOAT));
	outputs.push_back(std::make_shared<Output>("g", FlowType::FLOAT));
	outputs.push_back(std::make_shared<Output>("b", FlowType::FLOAT));
}

template<>
void ParameterNode<glm::vec3>::compile(CompilationErrorCheck& errorCheck)
{
	outputs[0]->valueStr = parameterName;
	outputs[1]->valueStr = parameterName + ".r";
	outputs[2]->valueStr = parameterName + ".g";
	outputs[3]->valueStr = parameterName + ".b";
}

/////////////

template<>
ParameterNode<glm::vec4>::ParameterNode()
	: BaseParameterNode("vector4")
	, shaderParameter(name, true, glm::vec4(0, 0, 0, 0), EditorGUI::FieldDisplayType::DEFAULT)
{
	outputs.push_back(std::make_shared<Output>("rgba", FlowType::FLOAT4));
	outputs.push_back(std::make_shared<Output>("r", FlowType::FLOAT));
	outputs.push_back(std::make_shared<Output>("g", FlowType::FLOAT));
	outputs.push_back(std::make_shared<Output>("b", FlowType::FLOAT));
	outputs.push_back(std::make_shared<Output>("a", FlowType::FLOAT));
}

template<>
void ParameterNode<glm::vec4>::compile(CompilationErrorCheck& errorCheck)
{
	outputs[0]->valueStr = parameterName;
	outputs[1]->valueStr = parameterName + ".r";
	outputs[2]->valueStr = parameterName + ".g";
	outputs[3]->valueStr = parameterName + ".b";
	outputs[4]->valueStr = parameterName + ".a";
}

/////////////

template<>
ParameterNode<Texture>::ParameterNode()
	: BaseParameterNode("texture")
	, shaderParameter(name, true, ResourcePtr<Texture>(), EditorGUI::FieldDisplayType::DEFAULT)
{
	inputs.push_back(std::make_shared<Input>("texCoords", FlowType::FLOAT2));

	outputs.push_back(std::make_shared<Output>("rgba", FlowType::FLOAT4));
	outputs.push_back(std::make_shared<Output>("r", FlowType::FLOAT));
	outputs.push_back(std::make_shared<Output>("g", FlowType::FLOAT));
	outputs.push_back(std::make_shared<Output>("b", FlowType::FLOAT));
	outputs.push_back(std::make_shared<Output>("a", FlowType::FLOAT));
}

template<>
void ParameterNode<Texture>::compile(CompilationErrorCheck& errorCheck)
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
		inputs.push_back(std::make_shared<Input>("a", FlowType::UNDEFINED));
		inputs.push_back(std::make_shared<Input>("b", FlowType::UNDEFINED));

		outputs.push_back(std::make_shared<Output>("r", FlowType::UNDEFINED));
	}
};

struct OperatorNodeMinus : public OperatorNode
{
	INHERIT_FROM_NODE(OperatorNodeMinus)

	OperatorNodeMinus()
		: OperatorNode("minus", "-")
	{
		inputs.push_back(std::make_shared<Input>("a", FlowType::UNDEFINED));
		inputs.push_back(std::make_shared<Input>("b", FlowType::UNDEFINED));

		outputs.push_back(std::make_shared<Output>("r", FlowType::UNDEFINED));
	}
};

struct OperatorNodeMultiply : public OperatorNode
{
	INHERIT_FROM_NODE(OperatorNodeMultiply)

	OperatorNodeMultiply()
		: OperatorNode("multiply", "*")
	{
		inputs.push_back(std::make_shared<Input>("a", FlowType::UNDEFINED));
		inputs.push_back(std::make_shared<Input>("b", FlowType::UNDEFINED));

		outputs.push_back(std::make_shared<Output>("r", FlowType::UNDEFINED));
	}
};

struct OperatorNodeDivide : public OperatorNode
{
	INHERIT_FROM_NODE(OperatorNodeDivide)

	OperatorNodeDivide()
		: OperatorNode("divide", "/")
	{
		inputs.push_back(std::make_shared<Input>("a", FlowType::UNDEFINED));
		inputs.push_back(std::make_shared<Input>("b", FlowType::UNDEFINED));

		outputs.push_back(std::make_shared<Output>("r", FlowType::UNDEFINED));
	}
};

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
		inputs.push_back(std::make_shared<Input>("a", FlowType::UNDEFINED));
		inputs.push_back(std::make_shared<Input>("b", FlowType::UNDEFINED));

		outputs.push_back(std::make_shared<Output>("r", FlowType::UNDEFINED));
	}
};

struct FunctionNodeCross : public FunctionNode
{
	INHERIT_FROM_NODE(FunctionNodeCross)

	FunctionNodeCross()
		: FunctionNode("cross", "cross")
	{
		inputs.push_back(std::make_shared<Input>("a", FlowType::FLOAT3));
		inputs.push_back(std::make_shared<Input>("b", FlowType::FLOAT3));

		outputs.push_back(std::make_shared<Output>("r", FlowType::FLOAT3));
	}
};

struct FunctionNodeNormalize : public FunctionNode
{
	INHERIT_FROM_NODE(FunctionNodeNormalize)

	FunctionNodeNormalize()
		: FunctionNode("normalize", "normalize")
	{
		inputs.push_back(std::make_shared<Input>("a", FlowType::UNDEFINED));

		outputs.push_back(std::make_shared<Output>("r", FlowType::UNDEFINED));
	}
};

struct FunctionNodeDistance : public FunctionNode
{
	INHERIT_FROM_NODE(FunctionNodeDistance)

	FunctionNodeDistance()
		: FunctionNode("distance", "distance")
	{
		inputs.push_back(std::make_shared<Input>("a", FlowType::UNDEFINED));
		inputs.push_back(std::make_shared<Input>("b", FlowType::UNDEFINED));

		outputs.push_back(std::make_shared<Output>("r", FlowType::UNDEFINED));
	}
};

struct FunctionNodeLength : public FunctionNode
{
	INHERIT_FROM_NODE(FunctionNodeLength)

	FunctionNodeLength()
		: FunctionNode("length", "length")
	{
		inputs.push_back(std::make_shared<Input>("a", FlowType::UNDEFINED));

		outputs.push_back(std::make_shared<Output>("r", FlowType::UNDEFINED));
	}
};

struct FunctionNodeMin : public FunctionNode
{
	INHERIT_FROM_NODE(FunctionNodeMin)

	FunctionNodeMin()
		: FunctionNode("minimum", "min")
	{
		inputs.push_back(std::make_shared<Input>("a", FlowType::UNDEFINED));
		inputs.push_back(std::make_shared<Input>("b", FlowType::UNDEFINED));

		outputs.push_back(std::make_shared<Output>("r", FlowType::UNDEFINED));
	}
};

struct FunctionNodeMax : public FunctionNode
{
	INHERIT_FROM_NODE(FunctionNodeMax)

	FunctionNodeMax()
		: FunctionNode("maximum", "max")
	{
		inputs.push_back(std::make_shared<Input>("a", FlowType::UNDEFINED));
		inputs.push_back(std::make_shared<Input>("b", FlowType::UNDEFINED));

		outputs.push_back(std::make_shared<Output>("r", FlowType::UNDEFINED));
	}
};

struct FunctionNodeMod : public FunctionNode
{
	INHERIT_FROM_NODE(FunctionNodeMod)

	FunctionNodeMod()
		: FunctionNode("modulo", "mod")
	{
		inputs.push_back(std::make_shared<Input>("a", FlowType::UNDEFINED));
		inputs.push_back(std::make_shared<Input>("b", FlowType::UNDEFINED));

		outputs.push_back(std::make_shared<Output>("r", FlowType::UNDEFINED));
	}
};

struct FunctionNodeCos : public FunctionNode
{
	INHERIT_FROM_NODE(FunctionNodeCos)

	FunctionNodeCos()
		: FunctionNode("cosine", "cos")
	{
		inputs.push_back(std::make_shared<Input>("a", FlowType::UNDEFINED));

		outputs.push_back(std::make_shared<Output>("r", FlowType::UNDEFINED));
	}
};

struct FunctionNodeSin : public FunctionNode
{
	INHERIT_FROM_NODE(FunctionNodeSin)

	FunctionNodeSin()
		: FunctionNode("sine", "sin")
	{
		inputs.push_back(std::make_shared<Input>("a", FlowType::UNDEFINED));

		outputs.push_back(std::make_shared<Output>("r", FlowType::UNDEFINED));
	}
};

struct FunctionNodeTan : public FunctionNode
{
	INHERIT_FROM_NODE(FunctionNodeTan)

	FunctionNodeTan()
		: FunctionNode("tangent", "tan")
	{
		inputs.push_back(std::make_shared<Input>("a", FlowType::UNDEFINED));

		outputs.push_back(std::make_shared<Output>("r", FlowType::UNDEFINED));
	}
};

//// END : Functions
//////////////////////////////////////////////////////////////

class NodeFactory : public ISingleton<NodeFactory>
{
	SINGLETON_IMPL(NodeFactory);

private:
	std::unordered_map<std::string, std::shared_ptr<Node>> m_container;

public:

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

	std::unordered_map<std::string, std::shared_ptr<Node>>::const_iterator& begin() const
	{
		return m_container.begin();
	}

	std::unordered_map<std::string, std::shared_ptr<Node>>::const_iterator& end() const
	{
		return m_container.end();
	}
};

#define REGISTER_NODE(NodeName, NodeType) static bool initialized##NodeType = NodeFactory::instance().registerNode(NodeName, std::make_shared<NodeType>());


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

public:
	void compile()
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
		}
	}

	Node* getNode(const ID& id) const;
	void save(Json::Value& root) const;
	void load(const Json::Value& root);
	void drawContent(Project& project, EditorModal* parentWindow) override;

private:

	void internalResolveUndeterminedTypes()
	{
		m_finalOutput->resolveUndeterminedTypes();
	}

	void internalDefineParameters(std::stringstream& compileStream, CompilationErrorCheck& errorCheck)
	{
		for (auto node : m_parameterNodes)
		{
			node->defineParameter(compileStream, errorCheck);
		}
	}

	void internalCompile(CompilationErrorCheck& errorCheck)
	{
		m_finalOutput->compile(errorCheck);
	}
};


}