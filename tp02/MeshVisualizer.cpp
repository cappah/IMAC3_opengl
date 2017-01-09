#include "MeshVisualizer.h"
#include "Renderer.h"


MeshVisualizer::MeshVisualizer()
{
	GlHelper::makeColorTexture(m_renderTex, 400, 400);
	m_frameBuffer.bind();
	m_frameBuffer.attachTexture(&m_renderTex, GL_COLOR_ATTACHMENT0);
	m_frameBuffer.checkIntegrity();
	m_frameBuffer.unbind();
}

MeshVisualizer::~MeshVisualizer()
{
	for (int i = 0; i < m_pointLights.size(); i++)
		delete m_pointLights[i];
}

void MeshVisualizer::setMesh(Mesh * mesh)
{
	m_visualizedMesh = mesh->makeSharedSubMesh(0);

	//m_camera.clearRenderBatches();

	//auto renderBatch = m_materialInstance->makeSharedRenderBatch();
	//renderBatch->add(m_visualizedMesh.get(), m_materialInstance.get());

	//m_camera.addRenderBatch(pipeline, renderBatch);

	//m_pipelineType = pipeline;
}

void MeshVisualizer::setMaterial(std::shared_ptr<Material> material)
{
	m_materialInstance = material;
	Rendering::PipelineType newPipelineType = material->getPipelineType();

	m_camera.clearRenderBatches();
	auto renderBatch = m_materialInstance->makeSharedRenderBatch();
	renderBatch->add(m_visualizedMesh.get(), m_materialInstance.get());
	m_camera.addRenderBatch(newPipelineType, renderBatch);

	m_pipelineType = newPipelineType;

	//if (m_camera.getRenderBatches(m_pipelineType).size() == 0)
	//{
	//	setMesh(newPipelineType, getMeshFactory().getDefault("cube"));
	//}
	//else
	//{
	//	std::shared_ptr<IRenderBatch> renderBatch = m_camera.getRenderBatches(m_pipelineType).begin()->second;
	//	m_camera.clearRenderBatches(m_pipelineType);
	//	m_camera.addRenderBatch(newPipelineType, renderBatch);
	//}
	//m_pipelineType = newPipelineType;
}

void MeshVisualizer::render(Renderer& renderer)
{
	//TODO : refactor
	std::vector<DirectionalLight*> m_directionalLights;
	std::vector<SpotLight*> m_spotLights;

	renderer.render(m_camera, m_pointLights, m_directionalLights, m_spotLights, false, nullptr);
}

void MeshVisualizer::rotateCamera(float x, float y)
{
	m_cameraPhi += y;
	m_cameraTheta += x;

	glm::vec3 eye(glm::cos(m_cameraTheta) * glm::sin(m_cameraPhi), glm::sin(m_cameraTheta) * glm::cos(m_cameraPhi), glm::sin(m_cameraTheta));

	m_camera.lookAt(eye, m_cameraTarget);
}

void MeshVisualizer::panCamera(float x, float y)
{
	m_cameraTarget.x += x;
	m_cameraTarget.y += y;

	glm::vec3 eye(glm::cos(m_cameraTheta) * glm::sin(m_cameraPhi), glm::sin(m_cameraTheta) * glm::cos(m_cameraPhi), glm::sin(m_cameraTheta));

	m_camera.lookAt(eye, m_cameraTarget);
}

void MeshVisualizer::drawUI()
{
	const ImVec2 availContent = ImGui::GetContentRegionAvail();
	ImGui::ImageButton((void*)m_camera.getFinalFrame()->glId, ImVec2(availContent.x - 8, availContent.y - 8) /*ImVec2(100, 100)*/, ImVec2(1, 0), ImVec2(0, 1));
	m_isSelected = ImGui::IsItemActive();

	ImVec2 dragDelta = ImGui::GetMouseDragDelta(0);
	if (m_isSelected && (dragDelta.x * dragDelta.x + dragDelta.y * dragDelta.y > 0.01f))
	{
		if (ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL))
			panCamera(dragDelta.x, dragDelta.y);
		else
			rotateCamera(dragDelta.x, dragDelta.y);
		ImGui::ResetMouseDragDelta(0);
	}
}
