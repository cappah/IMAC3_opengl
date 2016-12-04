#pragma once

#include <glm/glm.hpp>
#include "FrameBuffer.h"
#include "Texture.h"
#include "Mesh.h"
#include "Materials.h"
#include "ResourcePointer.h"

class Entity;
class Editor;
class Scene;

class Viewport
{
private:
	glm::vec2 m_position;
	glm::vec2 m_size;
	glm::vec2 m_renderSize;
	bool m_isHovered;

public:
	const glm::vec2& getPosition() const
	{
		return m_position;
	}

	const glm::vec2& getSize() const
	{
		return m_size;
	}

	const glm::vec2& getRenderSize() const
	{
		return m_renderSize;
	}

	void setPosition(const glm::vec2& position)
	{
		m_position = position;
	}

	void setSize(const glm::vec2& size)
	{
		m_size = size;
	}

	void setRenderSize(const glm::vec2& size)
	{
		m_renderSize = size;
	}

	void setIsHovered(bool state)
	{
		m_isHovered = state;
	}

	bool getIsHovered() const
	{
		return m_isHovered;
	}
};

/////////////////////////////////////////

class Inspector
{
private:
	char textValue[30];
	int intValue;
	float floatValue;
	glm::vec3 vector3Value;

	bool m_multipleEditing;
	Scene* m_currentScene;
	Editor* m_editorPtr;

public:
	Inspector(Editor* editorPtr);
	~Inspector();

	void setScene(Scene* currentScene);
	void drawUI();

	//void drawUI(const std::vector<Entity*>& entities);
	//void drawUI(const std::vector<PointLight*>& pointLights);
	//void drawUI(const std::vector<DirectionalLight*>& directionalLights);
	//void drawUI(const std::vector<SpotLight*>& spotLights);
	//void drawUI(const std::vector<MeshRenderer*>& meshRenderers);
	//void drawUI(const std::vector<Collider*>& colliders);
};

/////////////////////////////////////////

class SceneHierarchy
{
private:
	Scene* m_currentScene;
	Editor* m_editorPtr;
public:
	SceneHierarchy(Editor* editorPtr);
	void setScene(Scene* scene);
	Scene* getScene() const;
	void displayTreeEntityNode(Entity* entity, int &entityId, bool &setParenting, Entity*& parentToAttachSelected);
	void drawUI();
};

/////////////////////////////////////////

class DebugDrawRenderer
{
private:
	GlHelper::Framebuffer m_framebuffer;
	std::shared_ptr<Texture> m_texture;
	Mesh m_quadMesh;
	ResourcePtr<MaterialBlit> m_material;
	std::vector<std::string> m_outputNames;
	std::string m_currentOutputName;

public:
	DebugDrawRenderer();
	void drawTexture(GLuint textureId);
	void drawUI();
	void setCurrentOutputName(const std::string& outputName)
	{
		m_currentOutputName = outputName;
	}
	void drawOutputIfNeeded(const std::string& outputName, GLuint textureId)
	{
		if (std::find(m_outputNames.begin(), m_outputNames.end(), outputName) == m_outputNames.end())
		{
			m_outputNames.push_back(outputName);
		}

		if (m_currentOutputName == outputName)
		{
			drawTexture(textureId);
		}
	}
};


