#include "glew/glew.h"

#include "PostProcess.h"
#include "Materials.h"
#include "Factories.h"
#include "EditorTools.h"
#include "imgui/imgui.h"
#include "Camera.h"

////////////////////////////////////////////////////////////////
//// BEGIN : PostProcessManager

PostProcessManager::PostProcessManager()
	: m_renderQuad(GL_TRIANGLES, (Mesh::USE_INDEX | Mesh::USE_VERTICES), 2)
{
	////////////////////// INIT QUAD MESH ////////////////////////
	m_renderQuad.triangleIndex = { 0, 1, 2, 2, 1, 3 };
	m_renderQuad.vertices = { -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0 };
	m_renderQuad.initGl();

	auto& it = PostProcessFactory::instance().begin();
	while (it != PostProcessFactory::instance().end())
	{
		m_operationList[it->first] = it->second->cloneShared(it->first);
		it++;
	}
}

void PostProcessManager::onViewportResized(float Width, float Height)
{
	for (auto& operation : m_operationList)
	{
		operation.second->onViewportResized(Width, Height);
	}
}

void PostProcessManager::resizeBlitQuad(const glm::vec4 & viewport)
{
	m_renderQuad.vertices = { viewport.x, viewport.y, viewport.x + viewport.z, viewport.y, viewport.x, viewport.y + viewport.w, viewport.x + viewport.z , viewport.y + viewport.w };
	// update in CG : 
	glBindBuffer(GL_ARRAY_BUFFER, m_renderQuad.vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, m_renderQuad.vertices.size() * sizeof(float), &(m_renderQuad.vertices)[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void PostProcessManager::render(const BaseCamera& camera, Texture& beautyColor, Texture& beautyHighValues, Texture& beautyDepth, DebugDrawRenderer* debugDrawer)
{
	auto& it = camera.getPostProcessProxy().begin();
	while (it != camera.getPostProcessProxy().end())
	{
		m_operationList[(*it)->getOperationName()]->render(**it, m_renderQuad, beautyColor, beautyHighValues, beautyDepth, debugDrawer);
		it++;
	}
}

void PostProcessManager::renderResultOnCamera(BaseCamera& camera)
{
	assert(camera.getPostProcessProxy().getOperationCount() > 0);
	auto it = camera.getPostProcessProxy().end();
	it--;
	const Texture* resultTexture = m_operationList[(*it)->getOperationName()]->getResult();
	camera.renderFrame(resultTexture);
}

int PostProcessManager::getOperationCount() const
{
	return m_operationList.size();
}

void PostProcessProxy::drawUI()
{
	ImGui::PushID(this);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 0.f));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0.f, 0.f));
	ImVec2 dropDownPos (ImGui::GetCursorPos().x + ImGui::GetWindowPos().x, ImGui::GetCursorPos().y + ImGui::GetWindowPos().y);
	if (ImGui::Button("add post process", ImVec2(ImGui::GetContentRegionAvailWidth(), 25)))
	{
		ImGui::OpenPopup("popUpAddOperation");
	}
	dropDownPos.y += 25;
	ImGui::SetNextWindowPos(ImVec2(dropDownPos.x + ImGui::GetStyle().WindowPadding.x, dropDownPos.y));
	ImGui::SetNextWindowContentWidth(ImGui::GetItemRectSize().x - ImGui::GetStyle().WindowPadding.x * 3.f );

	if (ImGui::BeginPopup("popUpAddOperation"))
	{
		auto& it = PostProcessDataFactory::instance().begin();
		while (it != PostProcessDataFactory::instance().end())
		{
			if (ImGui::Button(it->first.c_str(), ImVec2(ImGui::GetContentRegionAvailWidth(), 25)))
			{
				m_operationDataList.push_back(it->second->cloneShared(it->first));
				ImGui::CloseCurrentPopup();
			}
			it++;
		}
		ImGui::EndPopup();
	}

	ImGui::PopStyleVar(2);

	int index = 0;
	int removeIdx = -1;
	for (auto& operationData : m_operationDataList)
	{
		bool shouldRemove = false;
		ImGui::PushID(index);
		
			/*if (ImGui::Ext::removableTreeNode(operationData->getOperationName().c_str(), shouldRemove))
			{
				operationData->drawUI();
				ImGui::TreePop();
			}*/
			if(ImGui::Ext::collapsingLabelWithRemoveButton(operationData->getOperationName().c_str(), shouldRemove))
				operationData->drawUI();

			if (shouldRemove)
				removeIdx = index;

		ImGui::PopID();

		index++;
	}

	if (removeIdx >= 0)
	{
		m_operationDataList.erase(m_operationDataList.begin() + removeIdx);
	}

	ImGui::PopID();
}

int PostProcessProxy::getOperationCount() const
{
	return m_operationDataList.size();
}

//// END : PostProcessManager
////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
//// BEGIN : BloomPostProcessOperation

REGISTER_POST_PROCESS(BloomPostProcessOperation, BloomPostProcessOperationData, "bloom")

BloomPostProcessOperationData::BloomPostProcessOperationData(const std::string& operationName) 
	: PostProcessOperationData(operationName)
	, m_blurStepCount(5)
{

}

void BloomPostProcessOperationData::drawUI()
{
	ImGui::InputInt("blur step count", &m_blurStepCount);
}

int BloomPostProcessOperationData::getBlurStepCount() const
{
	return m_blurStepCount;
}

///////////////////////

BloomPostProcessOperation::BloomPostProcessOperation(const std::string& operationName)
	: PostProcessOperation(operationName)
{
	m_materialBlur = std::make_shared<MaterialBlur>(*getProgramFactory().get("blur"));
	m_materialBloom = std::make_shared<MaterialBloom>(*getProgramFactory().get("bloom"));

	for (int i = 0; i < 2; i++)
	{
		GlHelper::makeFloatColorTexture(m_colorTextures[i], 400, 400);
		m_colorTextures[i].initGL();

		m_pingPongFB[i].bind();
		m_pingPongFB[i].attachTexture(&m_colorTextures[i], GL_COLOR_ATTACHMENT0);
		m_pingPongFB[i].checkIntegrity();
		m_pingPongFB[i].unbind();
	}

	GlHelper::makeFloatColorTexture(m_finalTexture, 400, 400);
	m_finalTexture.initGL();

	m_finalFB.bind();
	m_finalFB.attachTexture(&m_finalTexture, GL_COLOR_ATTACHMENT0);
	m_finalFB.checkIntegrity();
	m_finalFB.unbind();
}

void BloomPostProcessOperation::render(const PostProcessOperationData& operationData, Mesh& renderQuad, Texture& beautyColor, Texture& beautyHighValues, Texture& beautyDepth, DebugDrawRenderer* debugDrawer)
{
	const BloomPostProcessOperationData& castedDatas = *static_cast<const BloomPostProcessOperationData*>(&operationData);
	const int blurStepCount = castedDatas.getBlurStepCount();

	//// Begin blur pass
	bool horizontal = true;
	bool firstPass = true;
	int FBIndex = 1;
	int TexIndex = 0;
	m_materialBlur->use();

	for (int i = 0; i < blurStepCount *2; i++)
	{
		m_pingPongFB[FBIndex].bind();
		//m_materialBlur->use();
		//glClear(GL_COLOR_BUFFER_BIT);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, firstPass ? beautyHighValues.glId : m_colorTextures[TexIndex].glId);
			m_materialBlur->glUniform_passId(TexIndex);
			m_materialBlur->glUniform_Texture(0);

			renderQuad.draw();

		m_pingPongFB[FBIndex].unbind();

		horizontal != horizontal;
		TexIndex = horizontal ? 1 : 0;
		FBIndex = horizontal ? 0 : 1;
		if(firstPass)
			firstPass = false;
	}

	if (debugDrawer != nullptr)
	{
		debugDrawer->addSeparator();
		debugDrawer->drawOutputIfNeeded("bloom_blur", m_colorTextures[1].glId);
	}

	//// End blur pass


	//// Begin bloom pass
	m_finalFB.bind();

		m_materialBloom->use();
	
		m_materialBloom->glUniform_exposure(2.f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_colorTextures[1].glId);
		m_materialBloom->glUniform_TextureBlur(0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, beautyColor.glId);
		m_materialBloom->glUniform_Texture(1);

		renderQuad.draw();

	m_finalFB.unbind();

	if (debugDrawer != nullptr)
	{
		debugDrawer->drawOutputIfNeeded("bloom_result", m_finalTexture.glId);
	}

	//// End bloom pass
}

void BloomPostProcessOperation::onViewportResized(float width, float height)
{
	for (int i = 0; i < 2; i++)
	{
		m_pingPongFB[i].bind();
		m_pingPongFB[i].detachTexture(GL_COLOR_ATTACHMENT0);

		m_colorTextures[i].freeGL();
		GlHelper::makeFloatColorTexture(m_colorTextures[i], width, height);
		m_colorTextures[i].initGL();

		m_pingPongFB[i].attachTexture(&m_colorTextures[i], GL_COLOR_ATTACHMENT0);
		m_pingPongFB[i].checkIntegrity();
		m_pingPongFB[i].unbind();
	}

	m_finalFB.bind();
	m_finalFB.detachTexture(GL_COLOR_ATTACHMENT0);

	m_finalTexture.freeGL();
	GlHelper::makeFloatColorTexture(m_finalTexture, width, height);
	m_finalTexture.initGL();

	m_finalFB.attachTexture(&m_finalTexture, GL_COLOR_ATTACHMENT0);
	m_finalFB.checkIntegrity();
	m_finalFB.unbind();
}

const Texture* BloomPostProcessOperation::getResult() const
{
	return &m_finalTexture;
}

//// END : BloomPostProcessOperation
////////////////////////////////////////////////////////////////