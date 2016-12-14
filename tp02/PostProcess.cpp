#include "glew/glew.h"

#include "PostProcess.h"
#include "Materials.h"
#include "Factories.h"
#include "EditorTools.h"
#include "imgui/imgui.h"
#include "Camera.h"
#include "Lights.h"

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

	////////////////////// INIT FINAL TEXTURE ////////////////////////
	GlHelper::makeFloatColorTexture(m_finalTexture, 400, 400);
	m_finalTexture.initGL();

	////////////////////// INIT FINAL FRAMEBUFFER ////////////////////////
	m_finalFB.bind();
	m_finalFB.attachTexture(&m_finalTexture, GL_COLOR_ATTACHMENT0);
	m_finalFB.checkIntegrity();
	m_finalFB.unbind();

	////////////////////// INIT BLIT MATERIAL ////////////////////////
	m_materialBlit.init(*getProgramFactory().get("blit"));
}

void PostProcessManager::onViewportResized(float width, float height)
{
	m_finalFB.bind();
	m_finalFB.detachTexture(GL_COLOR_ATTACHMENT0);

	m_finalTexture.freeGL();
	GlHelper::makeFloatColorTexture(m_finalTexture, width, height);
	m_finalTexture.initGL();

	m_finalFB.attachTexture(&m_finalTexture, GL_COLOR_ATTACHMENT0);
	m_finalFB.checkIntegrity();
	m_finalFB.unbind();

	for (auto& operation : m_operationList)
	{
		operation.second->onViewportResized(width, height);
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

void PostProcessManager::render(const BaseCamera& camera, Texture& beautyColor, Texture& beautyHighValues, Texture& beautyDepth, Texture& gPassHightValues, const std::vector<PointLight*>& pointLights, DebugDrawRenderer* debugDrawer)
{
	//auto& it = camera.getPostProcessProxy().begin();
	//while (it != camera.getPostProcessProxy().end())
	//{
	//	m_operationList[(*it)->getOperationName()]->render(**it, camera, m_renderQuad, beautyColor, beautyHighValues, beautyDepth, gPassHightValues, pointLights, debugDrawer);
	//	it++;
	//}

	// Copy the beauty color texture to the final texture. The rest of the effects will be added to this texture.
	m_finalFB.bind();
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	m_materialBlit.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, beautyColor.glId);
	m_materialBlit.setUniformBlitTexture(0);
	m_renderQuad.draw();
	m_finalFB.unbind();

	// Add effects to final texture one by one.
	for (auto operation : m_operationList)
	{
		auto postProcessProxy = camera.getPostProcessProxy();
		auto operationData = postProcessProxy.getOperationData(operation.first);
		if (operationData != nullptr)
		{
			operation.second->render(*operationData, camera, m_finalFB, m_finalTexture, m_renderQuad, beautyColor, beautyHighValues, beautyDepth, gPassHightValues, pointLights, debugDrawer);
		}
	}
}

void PostProcessManager::renderSSAO(const BaseCamera& camera)
{
	//TODO SSAO
	auto operation = m_operationList["ssao"];
	auto postProcessProxy = camera.getPostProcessProxy();
	auto operationData = postProcessProxy.getOperationData("ssao");
	if (operationData != nullptr)
	{
		//operation->render(operationDatas, ...); //TODO SSAO
	}
}

void PostProcessManager::renderResultOnCamera(BaseCamera& camera)
{
	//assert(camera.getPostProcessProxy().getOperationCount() > 0);
	//auto it = camera.getPostProcessProxy().end();
	//it--;
	//const Texture* resultTexture = m_operationList[(*it)->getOperationName()]->getResult();
	camera.renderFrame(&m_finalTexture /*resultTexture*/);
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

PostProcessOperationData * PostProcessProxy::getOperationData(const std::string& operationName) const
{
	auto& found = std::find_if(m_operationDataList.begin(), m_operationDataList.end(), [&operationName](const std::shared_ptr<PostProcessOperationData>& item) { return item->getOperationName() == operationName; });
	if (found != m_operationDataList.end())
		return found->get();
	else
		return nullptr;
}

//// END : PostProcessManager
////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
//// BEGIN : BloomPostProcessOperation

REGISTER_POST_PROCESS(BloomPostProcessOperation, BloomPostProcessOperationData, "bloom")

BloomPostProcessOperationData::BloomPostProcessOperationData(const std::string& operationName) 
	: PostProcessOperationData(operationName)
	, m_blurStepCount(5)
	, m_gamma(2.0)
	, m_exposure(1.0)
{

}

void BloomPostProcessOperationData::drawUI()
{
	ImGui::InputInt("blur step count", &m_blurStepCount);
	ImGui::InputFloat("Exposure", &m_exposure);
	ImGui::InputFloat("Gamma", &m_gamma);
}

int BloomPostProcessOperationData::getBlurStepCount() const
{
	return m_blurStepCount;
}

float BloomPostProcessOperationData::getExposure() const
{
	return m_exposure;
}

float BloomPostProcessOperationData::getGamma() const
{
	return m_gamma;
}

///////////////////////

BloomPostProcessOperation::BloomPostProcessOperation(const std::string& operationName)
	: PostProcessOperation(operationName)
{
	m_materialBlur = std::make_shared<MaterialBlur>(*getProgramFactory().get("blur"));
	m_materialBloom = std::make_shared<MaterialBloom>(*getProgramFactory().get("bloom"));
	m_materialAdd = std::make_shared<MaterialAdd>(*getProgramFactory().get("add"));

	for (int i = 0; i < 2; i++)
	{
		GlHelper::makeFloatColorTexture(m_colorTextures[i], 400, 400);
		m_colorTextures[i].initGL();

		m_pingPongFB[i].bind();
		m_pingPongFB[i].attachTexture(&m_colorTextures[i], GL_COLOR_ATTACHMENT0);
		m_pingPongFB[i].checkIntegrity();
		m_pingPongFB[i].unbind();
	}

	GlHelper::makeFloatColorTexture(m_highValuesTexture, 400, 400);
	m_highValuesTexture.initGL();

	m_highValuesFB.bind();
	m_highValuesFB.attachTexture(&m_highValuesTexture, GL_COLOR_ATTACHMENT0);
	m_highValuesFB.checkIntegrity();
	m_highValuesFB.unbind();
}

void BloomPostProcessOperation::render(const PostProcessOperationData& operationData, const BaseCamera& camera, GlHelper::Framebuffer& finalFB, Texture& finalTexture, Mesh& renderQuad, Texture& beautyColor, Texture& beautyHighValues, Texture& beautyDepth, Texture& gPassHighValues, const std::vector<PointLight*>& pointLights, DebugDrawRenderer* debugDrawer)
{
	const BloomPostProcessOperationData& castedDatas = *static_cast<const BloomPostProcessOperationData*>(&operationData);
	const int blurStepCount = castedDatas.getBlurStepCount();
	const float gamma = castedDatas.getGamma();
	const float exposure = castedDatas.getExposure();

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);


	m_highValuesFB.bind();
	glClear(GL_COLOR_BUFFER_BIT);

	m_materialAdd->use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPassHighValues.glId);
	m_materialAdd->glUniform_Texture01(0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, beautyHighValues.glId);
	m_materialAdd->glUniform_Texture02(1);

	renderQuad.draw();

	m_highValuesFB.unbind();

	//// Begin blur pass
	bool horizontal = true;
	bool firstPass = true;
	int FBIndex = 1;
	int TexIndex = 0;
	m_materialBlur->use();

	for (int i = 0; i < 2; i++)
	{
		m_pingPongFB[i].bind();
		glClear(GL_COLOR_BUFFER_BIT);
		m_pingPongFB[i].unbind();
	}

	for (int i = 0; i < blurStepCount; i++)
	{
		m_pingPongFB[FBIndex].bind();
		//m_materialBlur->use();
		glClear(GL_COLOR_BUFFER_BIT);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, firstPass ? m_highValuesTexture.glId : m_colorTextures[TexIndex].glId);
			m_materialBlur->glUniform_passId(TexIndex);
			m_materialBlur->glUniform_Texture(0);

			renderQuad.draw();

		m_pingPongFB[FBIndex].unbind();

		horizontal = !horizontal;
		FBIndex = horizontal ? 1 : 0;
		TexIndex = horizontal ? 0 : 1;

		if(firstPass)
			firstPass = false;
	}

	//// End blur pass


	//// Begin bloom pass
	finalFB.bind();

		m_materialBloom->use();
		m_materialBloom->glUniform_Gamma(gamma);
		m_materialBloom->glUniform_Exposure(exposure);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_colorTextures[TexIndex].glId);
		m_materialBloom->glUniform_TextureBlur(0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, finalTexture.glId);
		m_materialBloom->glUniform_Texture(1);

		renderQuad.draw();

	finalFB.unbind();


	if (debugDrawer != nullptr)
	{
		debugDrawer->addSeparator();
		debugDrawer->drawOutputIfNeeded("bloom_highValues", m_highValuesTexture.glId);
		debugDrawer->drawOutputIfNeeded("bloom_blur", m_colorTextures[TexIndex].glId);
		debugDrawer->drawOutputIfNeeded("bloom_result", finalTexture.glId);
	}

	glEnable(GL_DEPTH_TEST);

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

	m_highValuesFB.bind();
	m_highValuesFB.detachTexture(GL_COLOR_ATTACHMENT0);

	m_highValuesTexture.freeGL();
	GlHelper::makeFloatColorTexture(m_highValuesTexture, width, height);
	m_highValuesTexture.initGL();

	m_highValuesFB.attachTexture(&m_highValuesTexture, GL_COLOR_ATTACHMENT0);
	m_highValuesFB.checkIntegrity();
	m_highValuesFB.unbind();
}

//const Texture* BloomPostProcessOperation::getResult() const
//{
//	return &m_finalTexture;
//}

//// END : BloomPostProcessOperation
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
//// BEGIN : FlaresPostProcessOperation

REGISTER_POST_PROCESS(FlaresPostProcessOperation, FlaresPostProcessOperationData, "flares")

FlaresPostProcessOperationData::FlaresPostProcessOperationData(const std::string & operationName)
	: PostProcessOperationData(operationName)
{
	m_materialFlares.init(*getProgramFactory().get("flares"));
}


void FlaresPostProcessOperationData::drawUI()
{
	m_materialFlares.drawUI();
}

const MaterialFlares & FlaresPostProcessOperationData::getMaterial() const
{
	return m_materialFlares;
}

FlaresPostProcessOperation::FlaresPostProcessOperation(const std::string & operationName)
	: PostProcessOperation(operationName)
{
	m_materialAdd = std::make_shared<MaterialAdd>(*getProgramFactory().get("add"));

	GlHelper::makeFloatColorTexture(m_flaresTexture, 400, 400);
	m_flaresTexture.initGL();

	m_flaresFB.bind();
	m_flaresFB.attachTexture(&m_flaresTexture, GL_COLOR_ATTACHMENT0);
	m_flaresFB.checkIntegrity();
	m_flaresFB.unbind();

	//GlHelper::makeFloatColorTexture(m_finalTexture, 400, 400);
	//m_finalTexture.initGL();

	//m_finalFB.bind();
	//m_finalFB.attachTexture(&m_finalTexture, GL_COLOR_ATTACHMENT0);
	//m_finalFB.checkIntegrity();
	//m_finalFB.unbind();

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_positionBuffer);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_positionBuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void FlaresPostProcessOperation::render(const PostProcessOperationData & operationData, const BaseCamera& camera, GlHelper::Framebuffer& finalFB, Texture& finalTexture, Mesh & renderQuad, Texture & beautyColor, Texture & beautyHighValues, Texture & beautyDepth, Texture & gPassHighValues, const std::vector<PointLight*>& pointLights, DebugDrawRenderer * debugDrawer)
{
	const FlaresPostProcessOperationData* operationDatas = static_cast<const FlaresPostProcessOperationData*>(&operationData);
	const MaterialFlares& materialFlares = operationDatas->getMaterial();


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	std::vector<glm::vec3> pointLightPositions;
	for (auto& light : pointLights)
	{
		pointLightPositions.push_back(light->position);
	}
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, pointLightPositions.size() * sizeof(float) * 3, &pointLightPositions[0], GL_DYNAMIC_DRAW);
	
	materialFlares.use();
	int texCount = 0;
	materialFlares.pushInternalsToGPU(texCount);
	materialFlares.glUniform_VP((camera.getProjectionMatrix() * camera.getViewMatrix()));

	glActiveTexture(GL_TEXTURE0 + texCount);
	glBindTexture(GL_TEXTURE_2D, beautyDepth.glId);
	materialFlares.glUniform_Depth(texCount);
	texCount++;

	m_flaresFB.bind();
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_POINTS, 0, pointLightPositions.size());
	m_flaresFB.unbind();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDisable(GL_BLEND);

	///////////////////////

	finalFB.bind();

		m_materialAdd->use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, finalTexture.glId);
		m_materialAdd->glUniform_Texture01(0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_flaresTexture.glId);
		m_materialAdd->glUniform_Texture02(1);

		renderQuad.draw();

	finalFB.unbind();

	///////////////////////

	if (debugDrawer != nullptr)
	{
		debugDrawer->addSeparator();
		debugDrawer->drawOutputIfNeeded("flares_flareTexture", m_flaresTexture.glId);
	}
}

void FlaresPostProcessOperation::onViewportResized(float width, float height)
{
	m_flaresFB.bind();
	m_flaresFB.detachTexture(GL_COLOR_ATTACHMENT0);

	m_flaresTexture.freeGL();
	GlHelper::makeFloatColorTexture(m_flaresTexture, width, height);
	m_flaresTexture.initGL();

	m_flaresFB.attachTexture(&m_flaresTexture, GL_COLOR_ATTACHMENT0);
	m_flaresFB.checkIntegrity();
	m_flaresFB.unbind();

	//////

	//m_finalFB.bind();
	//m_finalFB.detachTexture(GL_COLOR_ATTACHMENT0);

	//m_finalTexture.freeGL();
	//GlHelper::makeFloatColorTexture(m_finalTexture, width, height);
	//m_finalTexture.initGL();

	//m_finalFB.attachTexture(&m_finalTexture, GL_COLOR_ATTACHMENT0);
	//m_finalFB.checkIntegrity();
	//m_finalFB.unbind();
}

//const Texture * FlaresPostProcessOperation::getResult() const
//{
//	return &m_finalTexture;
//}

//// END : FlaresPostProcessOperation
////////////////////////////////////////////////////////////////