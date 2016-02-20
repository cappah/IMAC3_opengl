#include "Billboard.h"
//forwards : 
#include "Factories.h"
#include "Scene.h"


Billboard::Billboard(): Component(ComponentType::BILLBOARD), m_translation(0,0,0), m_scale(1,1), m_textureName(""), m_color(1,1,1,1)
{
	m_quadMesh = MeshFactory::get().get("defaultQuad");
	m_billboardMaterial = static_cast<MaterialBillboard*>( MaterialFactory::get().get("billboard") );
	m_texture = TextureFactory::get().get("default");
	m_texture->initGL();
}


Billboard::~Billboard()
{
	m_texture->freeGL();
}

void Billboard::render(const glm::mat4 & projection, const glm::mat4 & view)
{
	glm::mat4 MVP = projection * view;
	glm::mat4 NormalMatrix = glm::transpose(glm::inverse(glm::mat4(1)));
	glm::vec3 CameraRight = glm::vec3(view[0][0], view[1][0], view[2][0]);
	glm::vec3 CameraUp = glm::vec3(view[0][1], view[1][1], view[2][1]);

	m_billboardMaterial->use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture->glId);

	m_billboardMaterial->setUniformMVP(MVP);
	m_billboardMaterial->setUniformCameraRight(CameraRight);
	m_billboardMaterial->setUniformCameraUp(CameraUp);
	m_billboardMaterial->setUniformScale(m_scale);
	m_billboardMaterial->setUniformTexture(0);
	m_billboardMaterial->setUniformTranslation(m_translation);
	m_billboardMaterial->setUniformColor(m_color);

	m_quadMesh->draw();
}

void Billboard::setTranslation(const glm::vec3 & translation)
{
	m_translation = translation;
}

void Billboard::setScale(const glm::vec2 & scale)
{
	m_scale = scale;
}

void Billboard::setColor(const glm::vec4 & color)
{
	m_color = color;
}

glm::vec3 Billboard::getTranslation() const
{
	return glm::vec3();
}

glm::vec2 Billboard::getScale() const
{
	return glm::vec2();
}

glm::vec4 Billboard::getColor() const
{
	return m_color;
}

void Billboard::applyTransform(const glm::vec3 & translation, const glm::vec3 & scale, const glm::quat & rotation)
{
	m_scale.x = scale.x;
	m_scale.y = scale.y;

	m_translation = translation;
}

void Billboard::drawUI(Scene & scene)
{
	if (ImGui::CollapsingHeader("billboard"))
	{
		char texName[20];
		int stringLength = std::min((int)m_textureName.size(), 20);
		m_textureName.copy(texName, stringLength);
		texName[stringLength] = '\0';
		if (ImGui::InputText("textureName", texName, 20))
		{
			m_textureName = texName;

			if (TextureFactory::get().contains(m_textureName))
			{
				m_texture->freeGL();
				m_texture = TextureFactory::get().get(m_textureName);
				m_texture->initGL();
			}
		}

		ImGui::ColorEdit4("Color", glm::value_ptr(m_color));
	}
}

void Billboard::eraseFromScene(Scene & scene)
{
	scene.erase(this);
}

Component* Billboard::clone(Entity* entity)
{
	Billboard* billboard = new Billboard(*this);

	billboard->attachToEntity(entity);

	return billboard;
}

void Billboard::addToScene(Scene& scene)
{
	scene.add(this);
}