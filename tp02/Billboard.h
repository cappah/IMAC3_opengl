#pragma once

#include "Component.h"
#include "Mesh.h"
#include "Materials.h"
#include "Texture.h"

class Billboard : public Component
{
private:
	glm::vec3 m_translation;
	glm::vec2 m_scale;
	Mesh* m_quadMesh;
	MaterialBillboard* m_billboardMaterial;
	Texture* m_texture;
	std::string m_textureName;
	glm::vec4 m_color;

public:
	Billboard();
	~Billboard();

	void render(const glm::mat4& projection, const glm::mat4& view);

	void setTranslation(const glm::vec3& translation);
	void setScale(const glm::vec2& scale);
	void setColor(const glm::vec4& color);
	glm::vec3 getTranslation() const;
	glm::vec2 getScale() const;
	glm::vec4 getColor() const;

	virtual void applyTransform(const glm::vec3& translation, const glm::vec3& scale = glm::vec3(1, 1, 1), const glm::quat& rotation = glm::quat()) override;
	virtual void drawUI(Scene & scene) override;
	virtual void eraseFromScene(Scene & scene) override;
	virtual void addToScene(Scene & scene) override;
	virtual Component * clone(Entity * entity) override;

};

